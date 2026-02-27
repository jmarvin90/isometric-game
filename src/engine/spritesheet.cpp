#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <optional>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
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

    spritesheet = std::unique_ptr<SDL_Texture, Utility::SDLDestroyer>(
        SDL_CreateTextureFromSurface(renderer.get(), surface)
    );

    if (!spritesheet) {
        spdlog::info("Could not load texture from surface using image: " + spritesheet_path);
    }

    SDL_FreeSurface(surface);

    std::ifstream input { atlas_path };
    nlohmann::json data = nlohmann::json::parse(input);

    // TODO - this is an untidy way of doing it that creates (potentially multiple) copies
    for (const auto& json_object : data) {
        const std::string sprite_name { json_object["name"] };
        SpriteComponent sprite_component { json_object.get<SpriteComponent>() };
        sprite_component.texture = spritesheet.get();

        sprites.try_emplace(
            sprite_name,
            sprite_component,
            json_object.contains("directions")
                ? std::make_optional<NavigationComponent>(json_object.get<NavigationComponent>())
                : std::nullopt
        );
    }
}

const SpriteSheetEntry& SpriteSheet::get(const std::string name) const { return sprites.at(name); }
