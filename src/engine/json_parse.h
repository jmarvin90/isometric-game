#ifndef JSONPARSE_H
#define JSONPARSE_H

#include <SDL2/SDL.h>
#include <components/sprite_component.h>
#include <glm/glm.hpp>
#include <grid.h>
#include <nlohmann/json.hpp>
#include <spritesheet.h>

namespace glm {
void to_json(nlohmann::json& j, const glm::ivec2& P);
void to_json(nlohmann::json& j, const glm::vec2& P);

void from_json(const nlohmann::json& j, glm::ivec2& P);
void from_json(const nlohmann::json& j, glm::vec2& P);
}

void from_json(const nlohmann::json& j, SDL_Rect& P);
void to_json(nlohmann::json& j, const SDL_Rect& P);

// void from_json(const nlohmann::json& json, SpriteComponent& sprite);
void from_json(const nlohmann::json& json, SpriteDefinition& sprite);

template <typename StoredType, typename Projection>
void to_json(nlohmann::json& j, const Grid<StoredType, Projection>& grid)
{
    j = nlohmann::json {
        { "cells", grid.cells },
        { "cell_size", grid.cell_size },
        { "grid_dimensions", grid.grid_dimensions }
    };
}

template <typename StoredType, typename Projection>
void from_json(const nlohmann::json& j, Grid<StoredType, Projection>& grid)
{
    j.at("cells").get_to(grid.cells);
    j.at("cell_size").get_to(grid.cell_size);
    j.at("grid_dimensions").get_to(grid.grid_dimensions);
    grid.area = grid.cell_size * grid.grid_dimensions;
}

#endif
