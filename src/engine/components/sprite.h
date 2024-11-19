#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>

#include "constants.h"

struct Sprite {
    SDL_Texture* texture;
    int height_px;
    int width_px;
    int vertical_offset_px;
    int horizontal_offset_px;
    Sprite(SDL_Texture* texture):
        texture{texture} {
            SDL_QueryTexture(texture, NULL, NULL, &width_px, &height_px);
            vertical_offset_px = constants::TILE_HEIGHT - height_px;
            horizontal_offset_px = 0;
            // horizontal_offset_px = constants::TILE_WIDTH - width_px;
        }
};

struct TerrainSprite: public Sprite {   
};

struct VerticalSprite: public Sprite {
};

#endif