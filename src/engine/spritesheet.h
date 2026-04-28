#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL2/SDL.h>
#include <components/connectivity_component.h>
#include <components/sprite_component.h>
#include <grid.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <projection.h>
#include <string>
#include <unordered_map>
#include <utility.h>

// TODO - differentiate between a sprite definition and a component

namespace {
std::vector<bool> get_mask(const SDL_Surface* texture, const SDL_Rect rect);
}

struct SpriteDefinition {
    std::string name;
    SDL_Rect source_rect;
    Utility::SpriteType sprite_type;
    glm::ivec2 anchor;
    Direction::TDirection directions;
    Grid<bool, SpriteMaskProjection> spritemask;
    SpriteDefinition(nlohmann::json input, SDL_Surface* surface);
};

struct SpriteSheet {
    std::unique_ptr<SDL_Texture, Utility::SDLDestroyer> texture;
    std::unique_ptr<SDL_Texture, Utility::SDLDestroyer> outline_texture;
    std::unordered_map<std::string, SpriteDefinition> sprites;

    SpriteSheet(
        const std::string spritesheet_path,
        const std::string atlas_path,
        std::unique_ptr<SDL_Renderer, Utility::SDLDestroyer>& renderer
    );

    SpriteSheet(const SpriteSheet&) = delete;
    SpriteSheet& operator=(const SpriteSheet&) = delete;
    SpriteSheet(SpriteSheet&&) = default;
    SpriteSheet& operator=(SpriteSheet&&) = default;
};

#endif