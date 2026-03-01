#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <utility.h>

struct SpriteComponent {
    SDL_Rect source_rect;
    glm::ivec2 anchor;

    SpriteComponent()
        : source_rect { 0, 0, 0, 0 }
        , anchor { 0, 0 }
    {
    }

    SpriteComponent(
        SDL_Rect source_rect
    )
        : source_rect { source_rect }
        , anchor { source_rect.w / 2,
                   source_rect.w / 4 }
    {
    }
};

#endif