#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <utility.h>

struct SpriteComponent {
    SDL_Rect source_rect;
    SDL_Texture* texture;
    glm::ivec2 anchor;

    SpriteComponent()
        : source_rect {}
        , texture { nullptr }
        , anchor {}
    {
    }

    SpriteComponent(
        SDL_Rect source_rect,
        SDL_Texture* texture
    )
        : source_rect { source_rect }
        , texture { texture }
        , anchor { source_rect.w / 2,
                   source_rect.w / 4 }
    {
    }
};

void from_json(const nlohmann::json& json, SpriteComponent& sprite);

#endif