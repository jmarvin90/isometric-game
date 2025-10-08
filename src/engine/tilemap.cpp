#include <tilemap.h>
#include <glm/glm.hpp>
#include <optional>
#include <spdlog/spdlog.h>

Tile::Tile(entt::registry& registry, const TileMap& tilemap, const int tile_n)
: m_entity {registry.create()}
, m_tilemap {tilemap}
, tile_n {tile_n}
{
    // Create the transform component for each tile on construction
    registry.emplace<Transform>(
        m_entity,
        origin_px(),
        0,
        0.0
    );
}

Tile::~Tile() {
    m_tilemap.registry.destroy(m_entity);
}

void Tile::iso_sdl_points(std::array<SDL_Point, 5>& iso_sdl_points, const glm::ivec2& offset) {
    const glm::ivec2 centre {centre_px() + offset};
    const glm::ivec2 origin {origin_px() + offset};
    const glm::ivec2 diff {centre - origin};
    iso_sdl_points[0] = SDL_Point {centre.x, origin.y};
    iso_sdl_points[1] = SDL_Point {centre.x + diff.x, centre.y};
    iso_sdl_points[2] = SDL_Point {centre.x, centre.y + diff.y};
    iso_sdl_points[3] = SDL_Point {origin.x, centre.y};
    iso_sdl_points[4] = SDL_Point {centre.x, origin.y};
}

const glm::ivec2& Tile::grid_pos() {
    if (m_grid_pos) {
        return m_grid_pos.value();
    }

    m_grid_pos = (
        tile_n < m_tilemap.m_n_tiles 
        ? glm::ivec2(tile_n, 0) 
        : glm::ivec2(tile_n % m_tilemap.m_n_tiles, tile_n / m_tilemap.m_n_tiles)
    );
    return m_grid_pos.value();
}

const glm::ivec2& Tile::origin_px() {
    if (m_origin_px) {
        return m_origin_px.value();
    }

    const glm::ivec2 movement {m_tilemap.mat * grid_pos()};
    m_origin_px.emplace(movement + m_tilemap.origin_px);
    return m_origin_px.value();
}

const glm::ivec2& Tile::centre_px() {
    if (m_centre_px) {
        return m_centre_px.value();
    }
    
    m_centre_px.emplace(origin_px() + m_tilemap.tile_spec / 2);
    return m_centre_px.value();
}

TileMap::TileMap(entt::registry& registry, const int n_tiles, const glm::ivec2 tile_spec)
: registry {registry}
, m_n_tiles {n_tiles}
, tile_spec {tile_spec}
, tilemap_area {tile_spec * n_tiles}
, origin_px {(tilemap_area.x / 2) - (tile_spec.x / 2), 0}
, mat {tile_spec.x / 2.0f, tile_spec.y / 2.0f, -tile_spec.x / 2.0f, tile_spec.y / 2.0f}
, mat_inv {glm::inverse(mat)}
{
    const int n_tiles_total {n_tiles * n_tiles};
    
    // Reserve the correct amount of memory for the size of map
    // TODO: check if requires tile to be copy constructible?
    tiles.reserve(n_tiles_total);

    // Calculate the grid position for each slot in the vector
    for (int cell=0; cell<n_tiles_total; cell++) {
        tiles.emplace_back(registry, *this, cell);
    }

    spdlog::info(
        "Tilemap origin: " + 
        std::to_string(origin_px.x) + ", " + std::to_string(origin_px.y)
    );
}

Tile* TileMap::operator[](const glm::ivec2 world_position) {
    const glm::ivec2 grid_position {world_px_to_grid(world_position)};

    if (
        grid_position.x > 0 && grid_position.y > 0 &&
        grid_position.x < m_n_tiles && grid_position.y < m_n_tiles
    ) {
        return &tiles.at((grid_position.y * m_n_tiles) + grid_position.x);
    }

    return nullptr;
}

// Consider deprecating or returning const Tile&
const glm::ivec2 TileMap::world_px_to_grid(const glm::ivec2 world_pos) const {
    const glm::ivec2 world_pos_adjusted {world_pos - (tile_spec / 2)};
    const glm::ivec2 centred_world_pos {world_pos_adjusted - origin_px};
    const glm::ivec2 grid_pos_gross {mat_inv * centred_world_pos};
    return glm::ivec2(std::round(grid_pos_gross.x), std::round(grid_pos_gross.y));
}

// Consider deprecating
const glm::ivec2 TileMap::grid_to_world_px(const glm::ivec2 grid_pos) const {
    const glm::ivec2 movement {mat * grid_pos};
    const glm::ivec2 world_pos {movement + origin_px};
    return world_pos + (tile_spec / 2);
}