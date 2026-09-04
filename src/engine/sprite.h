#ifndef SPRITEDEFINITION_H
#define SPRITEDEFINITION_H

#include <SDL2/SDL.h>
#include <directions.h>
#include <glm/glm.hpp>
#include <grid.h>
#include <nlohmann/json.hpp>
#include <projection.h>
#include <string>
#include <unordered_map>

enum class SpriteType {
    TILE = 0,
    BUILDING_SENDER = 1,
    BUILDING_RECEIVER = 2,
    WALKER = 3,
    FURNITURE = 4
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