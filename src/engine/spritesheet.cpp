#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <memory>
#include <spdlog/spdlog.h>
#include <sprite.h>
#include <spritemask.h>
#include <spritesheet.h>
#include <string>
#include <iso_utility.h>

SpriteSheet::SpriteSheet(
    const std::string spritesheet_path,
    const std::string atlas_path,
    std::unique_ptr<SDL_Renderer, ISOUtility::SDLDestroyer>& renderer
)
{
    std::unique_ptr<SDL_Surface, ISOUtility::SDLDestroyer> surface { 
        IMG_Load(spritesheet_path.c_str()) 
    };

    if (!surface) {
        spdlog::info("Could not load texture from path: " + spritesheet_path);
    };

    texture
        = std::unique_ptr<SDL_Texture, ISOUtility::SDLDestroyer>(
            SDL_CreateTextureFromSurface(renderer.get(), surface.get())
        );

    outline_texture = std::unique_ptr<SDL_Texture, ISOUtility::SDLDestroyer>(
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

    SpriteMask::set_outline_texture(outline_texture.get(), surface.get());

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