#ifndef SPRITEDEFINITION_H
#define SPRITEDEFINITION_H

#include <directions.h>
#include <grid.h>
#include <iso_utility.h>
#include <nlohmann/json.hpp>
#include <projection.h>
#include <string>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct SpriteDefinition {
    std::string name;
    SDL_Rect source_rect;
    ISOUtility::SpriteType sprite_type;
    glm::ivec2 anchor;
    Direction::TDirection directions;
    Grid<bool, SpriteMaskProjection> spritemask;
    SpriteDefinition(nlohmann::json input, SDL_Surface* surface);
};

#endif