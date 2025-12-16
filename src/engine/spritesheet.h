#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL2/SDL.h>
#include <components/navigation_component.h>
#include <components/sprite_component.h>

#include <string>
#include <unordered_map>

using TileDef = std::pair<SpriteComponent, NavigationComponent>;

class SpriteSheet {
    SDL_Texture* spritesheet;
    std::unordered_map<std::string, TileDef> sprites;

public:
    SpriteSheet(const std::string spritesheet_path,
        const std::string atlas_path,
        SDL_Renderer* renderer);
    ~SpriteSheet();
    const TileDef& get(const std::string name) const;

    SpriteSheet(const SpriteSheet&) = delete;
    SpriteSheet& operator=(const SpriteSheet&) = delete;
    SpriteSheet(SpriteSheet&&) = default;
    SpriteSheet& operator=(SpriteSheet&&) = default;
};

#endif