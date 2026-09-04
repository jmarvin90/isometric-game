#ifndef SPATIALMAPCELLCOMPONENT_H
#define SPATIALMAPCELLCOMPONENT_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>
#include <vector>

struct SpatialMapCellComponent {
    std::vector<entt::entity> entities;
    std::vector<entt::entity> segments;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpatialMapCellComponent, entities, segments);
};

#endif