#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <spdlog/spdlog.h>
#include <spritesheet.h>

#include <fstream>
#include <optional>

SpriteSheet::SpriteSheet(const std::string spritesheet_path, const std::string atlas_path, SDL_Renderer* renderer)
{
    SDL_Surface* surface { IMG_Load(spritesheet_path.c_str()) };
    if (!surface) {
        spdlog::info("Could not load texture from path: " + spritesheet_path);
    };

    spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if (!spritesheet) {
        spdlog::info("Could not load texture from surface using image: " + spritesheet_path);
    }

    SDL_FreeSurface(surface);

    std::ifstream input { atlas_path };
    rapidjson::IStreamWrapper read { input };
    rapidjson::Document my_document;
    my_document.ParseStream(read);

    for (const auto& json_object : my_document.GetArray()) {
        sprites.try_emplace(
            json_object["name"].GetString(), //
            SpriteComponent { json_object, spritesheet }, //
            json_object.HasMember("directions")
                ? std::make_optional<NavigationComponent>(json_object)
                : std::nullopt
        );
    }
}

const SpriteSheetEntry& SpriteSheet::get(const std::string name) const { return sprites.at(name); }

SpriteSheet::~SpriteSheet() { SDL_DestroyTexture(spritesheet); }
