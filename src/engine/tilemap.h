#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>
#include <glm/glm.hpp>
#include <optional>

#include "constants.h"

class Tile {
    entt::registry& registry;
    const glm::ivec2 grid_position;
    entt::entity entity;
    std::vector<entt::entity> building_levels;

    public:
        Tile(entt::registry& registry, const glm::ivec2 grid_position);
        ~Tile();
        
        // Don't need to be const if we're returning a copy
        glm::ivec2 world_position() const;
        glm::ivec2 get_grid_position() const { return grid_position; }

        entt::entity add_building_level(SDL_Texture* texture, const SDL_Rect sprite_rect);
        entt::entity get_entity() { return entity; }

        // Awaiting definition
        entt::entity topmost_building_level() { return building_levels.back(); }
        void remove_building_level();
};

class TileMap {
    std::vector<Tile> tilemap;
    
    public: 
        TileMap(entt::registry& registry);
        ~TileMap();

        Tile* selected_tile {nullptr};

        Tile& at(const glm::ivec2 position);
        glm::ivec2 grid_to_pixel(glm::ivec2 grid_pos);
};

#endif