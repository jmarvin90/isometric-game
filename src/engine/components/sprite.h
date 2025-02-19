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
        /* 
            TODO: determine how the offset should work.

            Here, we basically determine that the BOTTOM MIDDLE of the portion
            of the texture we're rendering should be aligned to the BOTTOM MIDDLE
            of the tile we're rendering it into.
        */
        offset{
            (constants::TILE_SIZE.x - source_rect.w) / 2, 
            constants::TILE_SIZE.y - source_rect.h
        } {}
};

#endif