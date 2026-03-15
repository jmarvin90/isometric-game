#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL2/SDL.h>
#include <components/navigation_component.h>
#include <components/sprite_component.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility.h>

struct SpriteSheet {
public:
    std::unique_ptr<SDL_Texture, Utility::SDLDestroyer> texture;
    std::unordered_map<std::string, SpriteComponent> sprites;
    std::unordered_map<std::string, NavigationComponent> navigation;

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