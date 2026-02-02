#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <components/segment_component.h>
#include <directions.h>
#include <entt/entt.hpp>

namespace Pathfinding {

void path_between(
    const entt::registry& registry,
    entt::entity from_tile,
    entt::entity to_tile,
    std::vector<entt::entity>& path
);
} // namespace

#endif