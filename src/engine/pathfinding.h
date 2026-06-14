#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <components/path_component.h>
#include <components/segment_component.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <vector>

struct PathSegment;

namespace Pathfinding {

void path_between(
    const entt::registry& registry,
    entt::entity from_tile,
    entt::entity to_tile,
    std::vector<entt::entity>& path
);

void expand_path(
    entt::registry& registry,
    const std::vector<entt::entity>& path,
    std::vector<PathSegment>& expanded_path
);

} // namespace

#endif