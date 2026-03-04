#ifndef JSONPARSE_H
#define JSONPARSE_H

#include <SDL2/SDL.h>
#include <components/sprite_component.h>
#include <glm/glm.hpp>
#include <grid.h>
#include <nlohmann/json.hpp>

namespace glm {
void to_json(nlohmann::json& j, const glm::ivec2& P);
void to_json(nlohmann::json& j, const glm::vec2& P);

void from_json(const nlohmann::json& j, glm::ivec2& P);
void from_json(const nlohmann::json& j, glm::vec2& P);
}

void from_json(const nlohmann::json& j, SDL_Rect& P);
void to_json(nlohmann::json& j, const SDL_Rect& P);

void from_json(const nlohmann::json& json, SpriteComponent& sprite);

template <typename Projection>
void to_json(nlohmann::json& j, const Grid<Projection>& grid)
{
    j["cells"] = grid.cells;
    j["cell_size"] = grid.cell_size;
    j["grid_dimensions"] = grid.grid_dimensions;
}

#endif
