#ifndef PATHFINDING_H
#define PATHFINDING_H

#include <components/segment_component.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <vector>

namespace Pathfinding {

struct PathSegment {
    glm::ivec2 start;
    glm::ivec2 end;
    Direction::TDirection direction;
    PathSegment(glm::ivec2 start, glm::ivec2 end, Direction::TDirection direction)
        : start { start }
        , end { end }
        , direction { direction }
    {
    }
};

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