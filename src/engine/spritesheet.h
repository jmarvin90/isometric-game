#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <unordered_map>
#include <SDL2/SDL.h>
#include <string>


class SpriteSheet {
    const std::string image_path;
    const std::string atlas_path;

    public:
        /* TODO: the attributes perhaps should be private! */
        std::unordered_map<std::string, SDL_Rect> sprites;
        SDL_Texture* spritesheet;

        SpriteSheet(const std::string& image_path,  const std::string& atlas_path, SDL_Renderer* renderer);
        
        // Not yet implemented/used
        const SDL_Rect get_sprite_rect(const std::string& sprite_name) const;
};

#endif