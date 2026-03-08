#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <archive.h>
#include <components/sprite_component.h>
#include <fstream>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <spritesheet.h>

SpriteSheet::SpriteSheet(
    const std::string spritesheet_path,
    const std::string atlas_path,
    std::unique_ptr<SDL_Renderer, Utility::SDLDestroyer>& renderer
)
{
    SDL_Surface* surface { IMG_Load(spritesheet_path.c_str()) };

    if (!surface) {
        spdlog::info("Could not load texture from path: " + spritesheet_path);
    };

    texture = std::unique_ptr<SDL_Texture, Utility::SDLDestroyer>(
        SDL_CreateTextureFromSurface(renderer.get(), surface)
    );

    if (!texture) {
        spdlog::info("Could not load texture from surface using image: " + spritesheet_path);
    }

    SDL_FreeSurface(surface);

    std::ifstream input { atlas_path };
    nlohmann::json data = nlohmann::json::parse(input);

    for (const auto& json_object : data) {
        auto emplacement_result { sprites.try_emplace(json_object["name"]) };
        emplacement_result.first->second = json_object.get<SpriteComponent>();
        if (json_object.contains("directions")) {
            int directions { json_object.at("directions") };
            navigation.try_emplace(
                json_object["name"], NavigationComponent { directions }
            );
        }
    }
}