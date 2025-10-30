#ifndef TILEMAPCOMPONENT_H
#define TILEMAPCOMPONENT_H

#include <vector>
#include <entt/entt.hpp>
#include <optional>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

struct Tile {
    entt::entity tile_entity;
    std::optional<entt::entity> building_entity;
    Tile(entt::registry& registry)
    : tile_entity {registry.create()}
    , building_entity {std::nullopt}
    {}
    bool operator==(const Tile& comparator) const {
        return (
            tile_entity == comparator.tile_entity &&
            building_entity == comparator.building_entity
        );
    }
};


struct TileMapComponent {
    const int tiles_per_row;
    const int n_tiles;
    std::vector<Tile> tiles;
    glm::ivec2 area;
    glm::ivec2 origin_px;
    std::optional<Tile> highlighted_tile;
    std::unordered_map<entt::entity, std::array<entt::entity, 4>> network;

    TileMapComponent(const TileMapComponent&) = delete;
    TileMapComponent(TileMapComponent&&) = default;
    TileMapComponent(entt::registry& registry, const int tiles_per_row);
    std::optional<Tile> operator[](const glm::ivec2 grid_position) const;
};

#endif