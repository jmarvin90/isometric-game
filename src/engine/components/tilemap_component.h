#ifndef TILEMAPCOMPONENT_H
#define TILEMAPCOMPONENT_H

#include <components/tilespec_component.h>

#include <spdlog/spdlog.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <optional>
#include <vector>

struct TileMapComponent {
    const int n_per_row;
    const int n_tiles;
    std::vector<entt::entity> tiles;
    glm::ivec2 area;
    glm::ivec2 origin_px;

    TileMapComponent(const TileMapComponent&) = delete;
    TileMapComponent(TileMapComponent&&) = default;
    TileMapComponent(const TileSpecComponent& tile_spec, const int n_per_row);
    TileMapComponent(entt::registry& registry, const int n_per_row);
    entt::entity operator[](const glm::ivec2 grid_position) const;
};

#endif