#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "constants.h"

struct Sprite {
    // to be replaced with an asset identifier?
    SDL_Texture* texture;
    const SDL_Rect& source_rect;
    const glm::vec2 offset;

    Sprite(
        SDL_Texture* texture,
        const SDL_Rect& source_rect
    ): 
        texture{texture}, 
        source_rect{source_rect},
        offset{
            constants::TILE_SIZE.x - source_rect.w, 
            constants::TILE_SIZE.y - source_rect.h
        } {}
};

#endif