#ifndef SPRITE_H
#define SPRITE_H

#include <rapidjson/document.h>
#include <SDL2/SDL.h>

struct SpriteComponent {
    const SDL_Rect source_rect;
    SDL_Texture* texture;

    SpriteComponent(SDL_Rect source_rect, SDL_Texture* texture)
    : source_rect {source_rect}
    , texture {texture}
    {}

    SpriteComponent(const rapidjson::Value& json_object, SDL_Texture* texture)
    : source_rect {
        json_object["origin_x"].GetInt(),
        json_object["origin_y"].GetInt(),
        json_object["width"].GetInt(),
        json_object["height"].GetInt()
    }
    , texture {texture}
    {}
};

#endif