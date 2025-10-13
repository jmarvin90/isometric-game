#include <tilemap.h>
#include <glm/glm.hpp>
#include <optional>
#include <spdlog/spdlog.h>
#include <tilespec.h>
#include <components/transform.h>
#include <components/highlight.h>

// To delete at some stage
std::vector<SDL_Point> iso_sdl_points(
    const glm::ivec2 tile_spec 
) {
    const glm::ivec2 centre {tile_spec / 2};
    std::vector<SDL_Point> points {};
    points.push_back(SDL_Point {centre.x, 0});
    points.push_back(SDL_Point {tile_spec.x, centre.y});
    points.push_back(SDL_Point {centre.x, tile_spec.y});
    points.push_back(SDL_Point {0, centre.y});
    points.push_back(SDL_Point {centre.x, 0});
    return points;
}

glm::ivec2 tile_n_to_grid_pos(const int tile_n, const int n_tiles) {
    if (tile_n < n_tiles) {
        return {tile_n, 0};
    }
    return {tile_n % n_tiles, tile_n / n_tiles};
}

TileMap::~TileMap() {
    for (const entt::entity entity: tiles) {
        registry.destroy(entity);
    }
}

TileMap::TileMap(entt::registry& registry, const int n_tiles, const TileSpec& tile_spec)
: registry {registry}
, m_n_tiles {n_tiles}
, tile_spec {tile_spec}
, m_highlighted_tile {}
{
    const int n_tiles_total {n_tiles * n_tiles};
    
    // Reserve the correct amount of memory for the size of map
    // TODO: check if requires tile to be copy constructible?
    tiles.reserve(n_tiles_total);

    // Calculate the grid position for each slot in the vector
    for (int cell=0; cell<n_tiles_total; cell++) {
        entt::entity entity {tiles.emplace_back(registry.create())};
        const glm::ivec2 grid_pos {tile_n_to_grid_pos(cell, m_n_tiles)};
        const glm::ivec2 world_pos {grid_to_world_px(grid_pos)};
        registry.emplace<Transform>(entity, world_pos, 0, 0.0);
        registry.emplace<Highlight>(entity, SDL_Color{0, 0, 255, 255}, iso_sdl_points(tile_spec.size));
    }
}

const glm::ivec2 TileMap::area() const {
    return tile_spec.size * m_n_tiles;
}

const glm::ivec2 TileMap::origin_px() const {
    return {(area().x / 2) - (tile_spec.size.x / 2), 0};
}

std::optional<entt::entity> TileMap::operator[](const glm::ivec2 grid_position) const {
    if (
        grid_position.x > 0 && grid_position.y > 0 &&
        grid_position.x < area().x && grid_position.y < area().y
    ) {
        return tiles.at((grid_position.y * m_n_tiles) + grid_position.x);
    }
    return std::nullopt;
}

void TileMap::highlight_tile(const glm::ivec2 grid_pos) {
    spdlog::info(
        "Highlighting tile at " + 
        std::to_string(grid_pos.x) + ", " +
        std::to_string(grid_pos.y)
    ); 

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

// Consider deprecating or returning const Tile&
const glm::ivec2 TileMap::world_px_to_grid(const glm::ivec2 world_pos) const {
    const glm::ivec2 world_pos_adjusted {world_pos - (tile_spec.size / 2)};
    const glm::ivec2 centred_world_pos {world_pos_adjusted - origin_px()};
    const glm::ivec2 grid_pos_gross {tile_spec.matrix_inverted * centred_world_pos};
    return glm::ivec2(std::round(grid_pos_gross.x), std::round(grid_pos_gross.y));
}

// Consider deprecating
const glm::ivec2 TileMap::grid_to_world_px(const glm::ivec2 grid_pos) const {
    const glm::ivec2 movement {tile_spec.matrix * grid_pos};
    const glm::ivec2 world_pos {movement + origin_px()};
    return world_pos + (tile_spec.size / 2);
}