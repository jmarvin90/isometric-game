#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>
#include <glm/glm.hpp>

class TileMap {
    std::vector<std::vector<entt::entity>> tilemap;
    
    public:
        TileMap(entt::registry& registry);
        ~TileMap();
        entt::entity at(const int x, const int y) const;
        glm::ivec2 grid_to_pixel(const int x, const int y) const;
};

#endif