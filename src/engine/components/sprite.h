#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "constants.h"

struct Sprite {
    // to be replaced with an asset identifier?
    SDL_Texture* texture;
    SDL_Rect source_rect;
    glm::vec2 offset;

    Sprite(
        SDL_Texture* texture,
        SDL_Rect source_rect
    ): 
        texture{texture}, 
        source_rect{source_rect},
        /* 
            TODO: determine how the offset should work.

            Here, we basically determine that the BOTTOM MIDDLE of the portion
            of the texture we're rendering should be aligned to the BOTTOM MIDDLE
            of the tile we're rendering it into.

            NOTE that the "middle" is offset to the right, for a positive difference
            (e.g. where the spite is smaller than a standard tile) in the case that 
            the width of the source rect is not evenly divisible by two.

            The logic used to do the offset (ceil division) will potentially have
            the inverse affect in the case it's used when the sprite is larger than
            a standard tile.

            Will leave for now since we don't have any scenarios where that's the
            case.
        */
        offset{
            ((constants::TILE_SIZE.x - source_rect.w) / 2)
            + ((constants::TILE_SIZE.x - source_rect.w) % 2 != 0),
            constants::TILE_SIZE.y - source_rect.h
        } {}

        Sprite(
            SDL_Texture* texture,
            SDL_Rect source_rect,
            glm::vec2 in_offset
        ): 
            texture{texture}, 
            source_rect{source_rect},
            /* 
                TODO: determine how the offset should work.
    
                Here, we basically determine that the BOTTOM MIDDLE of the portion
                of the texture we're rendering should be aligned to the BOTTOM MIDDLE
                of the tile we're rendering it into.
    
                NOTE that the "middle" is offset to the right, for a positive difference
                (e.g. where the spite is smaller than a standard tile) in the case that 
                the width of the source rect is not evenly divisible by two.
    
                The logic used to do the offset (ceil division) will potentially have
                the inverse affect in the case it's used when the sprite is larger than
                a standard tile.
    
                Will leave for now since we don't have any scenarios where that's the
                case.
            */
            offset{
                glm::vec2{
                    ((constants::TILE_SIZE.x - source_rect.w) / 2)
                    + ((constants::TILE_SIZE.x - source_rect.w) % 2 != 0),
                    constants::TILE_SIZE.y - source_rect.h
                 } - in_offset
            } {}
};

#endif