#ifndef TILEMAP_H
#define TILEMAP_H

#include <entt/entt.hpp>
#include <vector>

class TileMap {
    std::vector<std::vector<entt::entity>> tilemap;

    public:
        entt::entity at(const int x, const int y);
        TileMap(entt::registry& registry);
        ~TileMap();
};

#endif