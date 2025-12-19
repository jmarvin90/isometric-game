#ifndef TILEMAPCOMPONENT_H
#define TILEMAPCOMPONENT_H

#include <spdlog/spdlog.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

struct TileMapComponent {
    const int tiles_per_row;
    const int n_tiles;
    std::vector<entt::entity> tiles;
    glm::ivec2 area;
    glm::ivec2 origin_px;
    entt::entity highlighted_tile;

    TileMapComponent(const TileMapComponent&) = delete;
    TileMapComponent(TileMapComponent&&) = default;
    TileMapComponent(entt::registry& registry, const int tiles_per_row);
    entt::entity operator[](const glm::ivec2 grid_position) const;
};

#endif