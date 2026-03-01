#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <components/sprite_component.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <spdlog/spdlog.h>
#include <spritesheet.h>

void from_json(const nlohmann::json& json, SpriteComponent& sprite)
{
    json.at("origin_x").get_to(sprite.source_rect.x);
    json.at("origin_y").get_to(sprite.source_rect.y);
    json.at("width").get_to(sprite.source_rect.w);
    json.at("height").get_to(sprite.source_rect.h);
    json.at("anchor_x").get_to(sprite.anchor.x);
    json.at("anchor_y").get_to(sprite.anchor.y);
}

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
            navigation.try_emplace(
                json_object["name"], NavigationComponent { json_object["directions"] }
            );
        }
    }
}