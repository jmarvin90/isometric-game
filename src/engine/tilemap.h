#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>
#include <glm/glm.hpp>

#include "constants.h"

class TileMap;

class Tile
{
    entt::registry &registry;
    glm::ivec2 grid_position;
    TileMap *tilemap;
    entt::entity entity;
    std::vector<entt::entity> building_levels;

protected:
    uint8_t m_tile_connection_bitmask{0};

public:
    Tile(entt::registry &registry, const glm::ivec2 grid_position, TileMap *const tilemap);
    ~Tile();

    // Don't need to be const if we're returning a copy
    glm::ivec2 world_position() const;
    glm::ivec2 get_grid_position() const { return grid_position; }

    entt::entity add_building_level(SDL_Texture *texture, const SDL_Rect sprite_rect);
    entt::entity get_entity() const { return entity; }

    // Awaiting definition
    entt::entity topmost_building_level() const { return building_levels.back(); }
    void remove_building_level();

    // Not sure on the use of an out-paramter here; what's the alternative?
    // ... Can be pretty confident that caller has allocated 5 positions,
    // but not 100%
    void get_tile_iso_points(
        SDL_Point *point_array, const glm::ivec2 &camera_position) const;

    uint8_t get_connection_bitmask() const { return m_tile_connection_bitmask; }
    void set_connection_bitmask(const uint8_t connection_bitmask);

    friend class TileMap;
};

class TileMap
{
    // TODO: think about const Tile* const
    std::vector<Tile> tilemap;
    const Tile *scan(const glm::ivec2 from, const uint8_t direction) const;

public:
    // TODO: the necessaries to make private
    std::unordered_map<const Tile *, std::array<const Tile *, 4>> graph{};

    TileMap(entt::registry &registry);
    ~TileMap();

    Tile *selected_tile{nullptr};

    Tile &operator[](const glm::ivec2 position);
    const Tile &operator[](const glm::ivec2 position) const;

    glm::ivec2 grid_to_pixel(glm::ivec2 grid_pos);

    void disconnect(const Tile *tile, const uint8_t direction);

    void connect(
        const Tile *origin, const Tile *termination, const uint8_t direction);

    void get_path_between(
        const glm::ivec2 point_a, const glm::ivec2 point_b, std::vector<glm::ivec2> &path);

    friend class Tile;
};

#endif