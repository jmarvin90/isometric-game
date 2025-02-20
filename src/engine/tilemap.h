#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>
#include <glm/glm.hpp>

#include "constants.h"

class Tile {
    entt::registry& registry;
    const glm::ivec2 grid_position;
    entt::entity entity;
    std::vector<entt::entity> building_levels;

    public:
        Tile(entt::registry& registry, const glm::ivec2 grid_position);
        const glm::ivec2 world_position() const;
        entt::entity add_building_level(SDL_Texture* texture, SDL_Rect sprite_rect);
        entt::entity get_entity() { return entity; }
        entt::entity topmost_building_level() { return building_levels.back(); }
};

class TileMap {
    std::vector<Tile> tilemap;
    
    public: 
        TileMap(entt::registry& registry);
        ~TileMap();
        Tile& at(int x, int y);
        glm::ivec2 grid_to_pixel(glm::ivec2 grid_pos);
};

#endif