#ifndef PATHCOMPONENT_H
#define PATHCOMPONENT_H

#include <entt/entt.hpp>
#include <vector>

struct PathComponent {
    std::vector<entt::entity> path;
    int current { 0 };
};

#endif