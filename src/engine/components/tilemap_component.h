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

    Tile(const Tile&) = delete;
    Tile(Tile&&) = default;
    ~Tile() = default;
    std::vector<entt::entity> entities() const;
};


struct TileMapComponent {
    const int tiles_per_row;
    const int n_tiles;
    std::vector<Tile> tiles;
    glm::ivec2 area;
    glm::ivec2 origin_px;
    Tile* highlighted_tile;

    TileMapComponent(const TileMapComponent&) = delete;
    TileMapComponent(TileMapComponent&&) = default;
    TileMapComponent(entt::registry& registry, const int tiles_per_row);
    Tile* operator[](const glm::ivec2 grid_position);
    void emplace_tiles(entt::registry& registry);
};

#endif