#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <utility.h>

struct SpriteComponent {
    std::string name;
    SDL_Rect source_rect;
    glm::ivec2 anchor;

    SpriteComponent()
        : name { "" }
        , source_rect { 0, 0, 0, 0 }
        , anchor { 0, 0 }
    {
    }

    SpriteComponent(
        std::string name,
        SDL_Rect source_rect
    )
        : name { name }
        , source_rect { source_rect }
        , anchor { source_rect.w / 2,
                   source_rect.w / 4 }
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpriteComponent, name, source_rect, anchor);
};

#endif