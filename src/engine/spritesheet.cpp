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
std::vector<bool> get_mask(const SDL_Surface* surface, const SDL_Rect rect)
{
    std::vector<bool> output;
    int n_pixels { rect.w * rect.h };
    output.reserve(n_pixels);
    uint32_t* pixels { static_cast<uint32_t*>(surface->pixels) };
    int start_pixel { (rect.y * surface->w) + rect.x };
    uint8_t r, g, b, a;
    for (int y = 0; y < rect.h; y++) {
        for (int x = 0; x < rect.w; x++) {
            int current_pixel { start_pixel + (surface->w * y) + x };
            uint32_t pixel { pixels[start_pixel + current_pixel] };
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);
            output.push_back((r + g + b) != 0 && a != 0);
        }
    }
    return output;
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
    , spritemask { get_mask(surface, source_rect), //
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

    texture = std::unique_ptr<SDL_Texture, Utility::SDLDestroyer>(
        SDL_CreateTextureFromSurface(renderer.get(), surface.get())
    );

    if (!texture) {
        spdlog::info("Could not load texture from surface using image: " + spritesheet_path);
    };

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