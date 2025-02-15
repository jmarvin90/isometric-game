#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "constants.h"

struct Sprite {
    // to be replaced with an asset identifier?
    SDL_Texture* texture;

    // to be replaced by an SDLRect, which includes the x/y of the frame in the source texture?
    int height_px;
    int width_px;

    glm::vec2 offset;

    // Lawd knows what happens here
    Sprite(SDL_Texture* texture):
        texture{texture} {
            SDL_QueryTexture(texture, NULL, NULL, &width_px, &height_px);
            offset = {0, constants::TILE_SIZE.y - height_px};
        }
};

#endif