#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <string>
#include <SDL2/SDL.h>

class SpriteSheet {
    SDL_Texture* spritesheet;

    public:
        SpriteSheet(
            const std::string spritesheet_path,
            const std::string atlas_path,
            SDL_Renderer* renderer
        );
        SpriteSheet(const SpriteSheet&) = delete;
        ~SpriteSheet();
};

#endif