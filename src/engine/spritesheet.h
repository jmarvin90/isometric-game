#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <string>
#include <SDL2/SDL.h>
#include <unordered_map>

#include <components/sprite_component.h>
#include <components/navigation_component.h>

using TileDef = std::pair<SpriteComponent, NavigationComponent>;

class SpriteSheet {
    SDL_Texture* spritesheet;
    std::unordered_map<std::string, TileDef> sprites;

    public:
        SpriteSheet(
            const std::string spritesheet_path,
            const std::string atlas_path,
            SDL_Renderer* renderer
        );
        SpriteSheet(const SpriteSheet&) = delete;
        ~SpriteSheet();
        const TileDef& get(const std::string name) const;
};

#endif