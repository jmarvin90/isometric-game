#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>
#include <glm/glm.hpp>

#include "constants.h"

class TileMap;

class Tile {
    entt::registry& registry;
    const glm::ivec2 grid_position;
    TileMap* tilemap;
    char tile_connection_bitmask {0};
    Tile* connections[4] {};
    entt::entity entity;
    std::vector<entt::entity> building_levels;

    public:
        Tile(entt::registry& registry, const glm::ivec2 grid_position, TileMap* tilemap);
        ~Tile();
        
        // Don't need to be const if we're returning a copy
        glm::ivec2 world_position() const;
        glm::ivec2 get_grid_position() const { return grid_position; }

        entt::entity add_building_level(SDL_Texture* texture, const SDL_Rect sprite_rect);
        entt::entity get_entity() const { return entity; }

        // Awaiting definition
        entt::entity topmost_building_level() const { return building_levels.back(); }
        void remove_building_level();

        // Not sure on the use of an out-paramter here; what's the alternative?
        // ... Can be pretty confident that caller has allocated 5 positions,
        // but not 100%
        void get_tile_iso_points(
            SDL_Point* point_array, const glm::ivec2& camera_position
        ) const;

        char get_connection_bitmask() const { return tile_connection_bitmask; }
        void set_connection_bitmask(char connection_bitmask) { tile_connection_bitmask = connection_bitmask;}
};

class TileMap {
    std::vector<Tile> tilemap;
    
    public: 
        TileMap(entt::registry& registry);
        ~TileMap();

        Tile* selected_tile {nullptr};

        Tile& operator[](const glm::ivec2 position);
        glm::ivec2 grid_to_pixel(glm::ivec2 grid_pos);
};

#endif