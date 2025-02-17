#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <unordered_map>
#include <SDL2/SDL.h>
#include <string>


class SpriteSheet {
    std::string image_path;
    std::string atlas_path;

    /* TODO: the attributes perhaps should be private! */
    std::unordered_map<std::string, SDL_Rect> sprites;
    SDL_Texture* spritesheet;

    public:
        SpriteSheet(const std::string& image_path,  const std::string& atlas_path, SDL_Renderer* renderer);
        SpriteSheet(const SpriteSheet&) = default;
        ~SpriteSheet();
        
        // Not yet implemented/used
        SDL_Rect& get_sprite_rect(const std::string& sprite_name);
        SDL_Texture* get_spritesheet_texture();
};

#endif