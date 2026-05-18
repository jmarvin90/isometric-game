#ifndef SPRITEDEFINITION_H
#define SPRITEDEFINITION_H

#include <directions.h>
#include <grid.h>
#include <nlohmann/json.hpp>
#include <projection.h>
#include <string>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

enum class SpriteType {
    TILE = 0,
    BUILDING = 1,
    WALKER = 2
};

enum class SpriteAnchor {
    SPRITE_ANCHOR = 1,
    ORIGIN = 2
};

struct SpriteDefinition {
    std::string name;
    SDL_Rect source_rect;
    SpriteType sprite_type;
    glm::ivec2 anchor;
    Direction::TDirection directions;
    Grid<bool, SpriteMaskProjection> spritemask;
    SpriteDefinition(nlohmann::json input, SDL_Surface* surface);
};

#endif