#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "constants.h"

struct Sprite {
    SDL_Texture* texture;
    int height_px;
    int width_px;
    glm::vec2 offset;
    Sprite(SDL_Texture* texture):
        texture{texture} {
            SDL_QueryTexture(texture, NULL, NULL, &width_px, &height_px);
            offset = {0, constants::TILE_HEIGHT - height_px};
        }
};

#endif