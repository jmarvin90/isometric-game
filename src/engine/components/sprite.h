#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

#include "constants.h"

struct Sprite {
    // to be replaced with an asset identifier?
    SDL_Texture* texture;
    SDL_Rect& source_rect;
    glm::vec2 offset;
};

#endif