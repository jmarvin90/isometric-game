#ifndef ROADACCESSCOMPONENT_H
#define ROADACCESSCOMPONENT_H

#include <entt/entt.hpp>
#include <vector>

struct RoadAccessComponent {
    std::vector<entt::entity> road_access_points;
};

#endif