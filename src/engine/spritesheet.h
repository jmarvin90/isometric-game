#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL2/SDL.h>
#include <iso_utility.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <sprite.h>
#include <string>

// TODO - differentiate between a sprite definition and a component

struct SpriteSheet {
    std::unique_ptr<SDL_Texture, ISOUtility::SDLDestroyer> texture;
    std::unique_ptr<SDL_Texture, ISOUtility::SDLDestroyer> outline_texture;
    std::unordered_map<std::string, SpriteDefinition> sprites;

    SpriteSheet(
        const std::string spritesheet_path,
        const std::string atlas_path,
        std::unique_ptr<SDL_Renderer, ISOUtility::SDLDestroyer>& renderer
    );

    SpriteSheet(const SpriteSheet&) = delete;
    SpriteSheet& operator=(const SpriteSheet&) = delete;
    SpriteSheet(SpriteSheet&&) = default;
    SpriteSheet& operator=(SpriteSheet&&) = default;
};

#endif