#ifndef SPRITE_H
#define SPRITE_H

#include <rapidjson/document.h>
#include <SDL2/SDL.h>

struct Sprite {
    const SDL_Rect source_rect;
    SDL_Texture* texture;

    Sprite(SDL_Rect source_rect, SDL_Texture* texture)
    : source_rect {source_rect}
    , texture {texture}
    {}

    Sprite(const rapidjson::Value& json_object, SDL_Texture* texture)
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