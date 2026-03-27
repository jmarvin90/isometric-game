#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <archive.h>
#include <components/sprite_component.h>
#include <fstream>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spritesheet.h>

namespace {

bool pixel_is_opaque(const SDL_Surface* surface, int x, int y)
{
    uint32_t pixel { static_cast<uint32_t*>(surface->pixels)[y * surface->w + x] };
    uint8_t r, g, b, a;
    SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
    return a == 255; // define an opacity threshold here, e.g. for shadows
}

bool is_highlight_pixel(const SDL_Surface* surface, int x, int y)
{
    // If the current pixel is opaque, and
    // - on the edge of the image; or
    // - the either previous or the next one is not opaque
    bool current_pixel_is_opaque { pixel_is_opaque(surface, x, y) };
    bool is_on_x_edge { x == 0 || x == surface->w - 1 };
    bool is_on_y_edge { y == 0 || y == surface->h - 1 };

    if (is_on_x_edge || is_on_y_edge)
        return current_pixel_is_opaque;

    bool adjacent_is_transparent { false };
    for (int y_diff = -1; y_diff <= 1 && !adjacent_is_transparent; y_diff++) {
        for (int x_diff = -1; x_diff <= 1 && !adjacent_is_transparent; x_diff++) {
            if (x_diff == 0 || y_diff == 0)
                continue;
            adjacent_is_transparent = !pixel_is_opaque(surface, x + x_diff, y + y_diff);
        }
    }
    return current_pixel_is_opaque && adjacent_is_transparent;
}

std::vector<bool> get_mask(
    const SDL_Surface* surface,
    const SDL_Rect rect,
    [[maybe_unused]] std::string name // helpful in debug, remove later
)
{
    std::vector<bool> output;
    int n_pixels { rect.w * rect.h };
    output.reserve(n_pixels);
    for (int rect_y = 0; rect_y < rect.h; rect_y++) {
        for (int rect_x = 0; rect_x < rect.w; rect_x++) {
            output.push_back(pixel_is_opaque(surface, rect.x + rect_x, rect.y + rect_y));
        }
    }
    return output;
}

void set_outline_texture(SDL_Texture* texture, SDL_Surface* surface)
{
    std::vector<uint32_t> outline_pixels(surface->w * surface->h, 0);
    for (int y = 0; y < surface->h; y++) {
        for (int x = 0; x < surface->w; x++) {
            if (!is_highlight_pixel(surface, x, y))
                continue;
            outline_pixels[y * surface->w + x] = 0xFF0000FF; // red
        }
    }

    SDL_UpdateTexture(
        texture,
        nullptr,
        outline_pixels.data(),
        surface->w * sizeof(uint32_t)
    );
}
}

SpriteDefinition::SpriteDefinition(nlohmann::json input, SDL_Surface* surface)
    : name { input["name"].get<std::string>() }
    , source_rect { input["source_rect"].get<SDL_Rect>() }
    , anchor { input["anchor"].get<glm::ivec2>() }
    , directions {
        input.contains("directions")
            ? Direction::TDirection(input["directions"].get<uint8_t>())
            : Direction::TDirection::NO_DIRECTION
    }
    , spritemask { get_mask(surface, source_rect, name), //
                   glm::ivec2 { 1, 1 }, //
                   glm::ivec2 { source_rect.w, source_rect.h } }
{
}

SpriteSheet::SpriteSheet(
    const std::string spritesheet_path,
    const std::string atlas_path,
    std::unique_ptr<SDL_Renderer, Utility::SDLDestroyer>& renderer
)
{
    std::unique_ptr<SDL_Surface, Utility::SDLDestroyer> surface { IMG_Load(spritesheet_path.c_str()) };

    if (!surface) {
        spdlog::info("Could not load texture from path: " + spritesheet_path);
    };

    texture
        = std::unique_ptr<SDL_Texture, Utility::SDLDestroyer>(
            SDL_CreateTextureFromSurface(renderer.get(), surface.get())
        );

    outline_texture = std::unique_ptr<SDL_Texture, Utility::SDLDestroyer>(
        SDL_CreateTexture(
            renderer.get(),
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_STATIC,
            surface->w,
            surface->h
        )
    );

    SDL_SetTextureBlendMode(outline_texture.get(), SDL_BLENDMODE_BLEND);

    if (!texture || !outline_texture) {
        spdlog::info("Could not load texture from surface using image: " + spritesheet_path);
    };

    set_outline_texture(outline_texture.get(), surface.get());

    std::ifstream input { atlas_path };
    nlohmann::json data = nlohmann::json::parse(input);

    for (const auto& json_object : data) {
        auto emplacement_result {
            sprites.try_emplace(
                json_object["name"],
                SpriteDefinition(json_object, surface.get())
            )
        };

        assert(emplacement_result.second);
    }
}