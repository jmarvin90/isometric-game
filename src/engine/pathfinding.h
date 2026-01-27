#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <components/segment_component.h>
#include <directions.h>
#include <entt/entt.hpp>

namespace Pathfinding {

std::vector<const SegmentComponent*> path_between(
    const entt::registry& registry, entt::entity from_tile, [[maybe_unused]] entt::entity to_tile
);
} // namespace

#endif