#include <tilemap.h>
#include <glm/glm.hpp>
#include <optional>
#include <spdlog/spdlog.h>
#include <tilespec.h>
#include <components/transform.h>
#include <components/highlight.h>
#include <position.h>
#include <spritesheet.h>


glm::ivec2 tile_n_to_grid_pos(const int tile_n, const int n_tiles) {
    if (tile_n < n_tiles) {
        return {tile_n, 0};
    }
    return {tile_n % n_tiles, tile_n / n_tiles};
}

TileMap::~TileMap() {
    for (const entt::entity entity: m_tiles) {
        m_registry.destroy(entity);
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
, m_highlighted_tile {}
{
    const int n_tiles_total {n_tiles * n_tiles};
    
    // Reserve the correct amount of memory for the size of map
    // TODO: check if requires tile to be copy constructible?
    m_tiles.reserve(n_tiles_total);

    // Calculate the grid position for each slot in the vector
    for (int cell=0; cell<n_tiles_total; cell++) {
        entt::entity entity {m_tiles.emplace_back(registry.create())};
        const glm::ivec2 grid_pos {tile_n_to_grid_pos(cell, m_n_tiles)};
        const glm::ivec2 world_pos {GridPosition{(*this), grid_pos}.to_world_position()};
        registry.emplace<Transform>(entity, world_pos, 0, 0.0);
        registry.emplace<Highlight>(entity, SDL_Color{0, 0, 255, 255}, m_tile_spec.iso_points());
        
        if (grid_pos.y == 1) {
            registry.emplace<Sprite>(entity, spritesheet.get("grass_ew"));
        } else {
            registry.emplace<Sprite>(entity, spritesheet.get("grass"));
        }
    }
}

const glm::ivec2 TileMap::area() const {
    return m_tile_spec.iso_area * m_n_tiles;
}

const glm::ivec2 TileMap::origin_px() const {
    return {(area().x / 2) - (m_tile_spec.centre().x), 0};
}

std::optional<entt::entity> TileMap::operator[](const glm::ivec2 grid_position) const {
    if (
        grid_position.x >= 0 && grid_position.y >= 0 &&
        grid_position.x < m_n_tiles && grid_position.y < m_n_tiles
    ) {
        return m_tiles.at((grid_position.y * m_n_tiles) + grid_position.x);
    }
    return std::nullopt;
}

void TileMap::highlight_tile(const glm::ivec2 grid_pos) {
    std::optional<entt::entity> tile {(*this)[grid_pos]};
    if (tile) {
        m_highlighted_tile = tile.value();
    } else {
        m_highlighted_tile.reset();
    }
}

const std::optional<entt::entity> TileMap::highlighted_tile() const {
    return m_highlighted_tile;
}