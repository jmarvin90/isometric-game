#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <rapidjson/document.h>

struct SpriteComponent {
    const SDL_Rect source_rect;
    SDL_Texture* texture;
    glm::ivec2 anchor;

    SpriteComponent(SDL_Rect source_rect, SDL_Texture* texture)
        : source_rect { source_rect }
        , texture { texture }
        , anchor { source_rect.w / 2,
                   source_rect.w / 4 }
    {
    }

    SpriteComponent(const rapidjson::Value& json_object, SDL_Texture* texture)
        : source_rect {
            json_object["origin_x"].GetInt(), //
            json_object["origin_y"].GetInt(), //
            json_object["width"].GetInt(), //
            json_object["height"].GetInt() //
        }
        , texture { texture }
        , anchor { json_object["anchor_x"].GetInt(), json_object["anchor_y"].GetInt() }
    {
    }
};

#endif