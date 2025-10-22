#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <string>
#include <SDL2/SDL.h>
#include <unordered_map>
#include <components/sprite.h>

class SpriteSheet {
    SDL_Texture* spritesheet;
    std::unordered_map<std::string, Sprite> sprites;


    public:
        SpriteSheet(
            const std::string spritesheet_path,
            const std::string atlas_path,
            SDL_Renderer* renderer
        );
        SpriteSheet(const SpriteSheet&) = delete;
        ~SpriteSheet();
        const Sprite& get(const std::string name) const;
};

#endif