#include <tilemap.h>
#include <glm/glm.hpp>
#include <optional>
#include <spdlog/spdlog.h>
#include <tilespec.h>
#include <components/transform.h>
#include <components/highlight.h>
#include <position.h>
#include <spritesheet.h>


bool Tile::has(const entt::entity entity) const {
    return (
        entity == tile_entity ||
        (
            building_entity.has_value() && 
            entity == building_entity.value()
        )
    );
}

TileMap::~TileMap() {
    for (const Tile& tile: m_tiles) {
        m_registry.destroy(tile.tile_entity);
        if (tile.building_entity) {
            m_registry.destroy(tile.building_entity.value());
        }
    }
}

TileMap::TileMap(
    entt::registry& registry,
    const int n_tiles,
    const int tile_width,
    const int tile_depth,
    const SpriteSheet& spritesheet
)
: m_registry {registry}
, m_n_tiles {n_tiles}
, m_tile_spec {tile_width, tile_depth}
, m_highlighted_tile {nullptr}
{
    const int n_tiles_total {n_tiles * n_tiles};
    
    // Reserve the correct amount of memory for the size of map
    m_tiles.reserve(n_tiles_total);

    // Calculate the grid position for each slot in the vector
    for (int cell=0; cell<n_tiles_total; cell++) {
        Tile& tile {m_tiles.emplace_back(registry)};
        const GridPosition grid_position {(*this), cell};
        const glm::ivec2 world_pos {grid_position.to_world_position()};
        registry.emplace<Transform>(tile.tile_entity, world_pos, 0, 0.0);
        registry.emplace<Highlight>(tile.tile_entity, SDL_Color{0, 0, 255, 255}, m_tile_spec.iso_points());
        
        // TODO: to be deleted anyway
        if (glm::ivec2(grid_position).y == 1) {
            registry.emplace<Sprite>(tile.tile_entity, spritesheet.get("grass_ew"));
        } else {
            registry.emplace<Sprite>(tile.tile_entity, spritesheet.get("grass"));
        }
    }
}

const glm::ivec2 TileMap::area() const {
    return m_tile_spec.iso_area * m_n_tiles;
}

const glm::ivec2 TileMap::origin_px() const {
    return {(area().x / 2) - (m_tile_spec.centre().x), 0};
}

Tile* TileMap::operator[](const glm::ivec2 grid_position) {
    if (GridPosition(*this, grid_position).is_valid()) {
        return &m_tiles.at((grid_position.y * m_n_tiles) + grid_position.x);
    }
    return nullptr;
}

void TileMap::highlight_tile(const glm::ivec2 grid_position) {
    const Tile* tile {(*this)[grid_position]};
    if (tile) {
        m_highlighted_tile = tile;
    } else {
        m_highlighted_tile = nullptr;
    }
}

const Tile* TileMap::highlighted_tile() const {
    return m_highlighted_tile;
}