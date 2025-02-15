#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <unordered_map>
#include <SDL2/SDL.h>
#include <string>


class SpriteSheet {
    const std::string image_path;
    const std::string atlas_path;
    std::unordered_map<std::string, SDL_Rect> sprites;
    SDL_Texture* spritesheet;

    public:
        SpriteSheet(const std::string& image_path,  const std::string& atlas_path, SDL_Renderer* renderer);
        const SDL_Rect get_sprite_rect(const std::string& sprite_name) const;
};

#endif