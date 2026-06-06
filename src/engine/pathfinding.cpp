#include <SDL2/SDL.h>
#include <algorithm>
#include <components/grid_position_component.h>
#include <components/junction_component.h>
#include <components/segment_component.h>
#include <components/segment_member_component.h>
#include <components/spatialmapcell_component.h>
#include <components/transform_component.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <grid.h>
#include <pathfinding.h>
#include <projection.h>
#include <queue>
#include <unordered_map>
#include <vector>

namespace {

bool is_adjacent_to(glm::ivec2 from, glm::ivec2 to)
{
    glm::ivec2 diff = glm::abs(from - to);
    return diff.x + diff.y == 1;
}

bool is_goal(const entt::registry& registry, entt::entity goal, entt::entity comparator)
{
    return (
        goal == comparator
        || is_adjacent_to(
            registry.get<const GridPositionComponent>(goal).position,
            registry.get<const GridPositionComponent>(comparator).position
        )
    );
}

struct PathStep {
    entt::entity tile;
    int priority;
};

struct Compare {
    bool operator()(const PathStep& lhs, const PathStep& rhs)
    {
        return lhs.priority > rhs.priority;
    }
};

int heuristic(const glm::ivec2& lhs, const glm::ivec2& rhs)
{
    glm::ivec2 delta { glm::abs(rhs - lhs) };
    return delta.x + delta.y;
}

void advance(
    const entt::registry& registry,
    std::priority_queue<PathStep, std::vector<PathStep>, Compare>& frontier,
    std::unordered_map<entt::entity, entt::entity>& came_from,
    entt::entity current_tile,
    entt::entity query_tile,
    entt::entity target_tile
)
{
    if (came_from.find(query_tile) != came_from.end())
        return;

    int heuristic_val {
        heuristic(
            registry.get<const GridPositionComponent>(query_tile).position,
            registry.get<const GridPositionComponent>(target_tile).position
        )
    };

    frontier.push({ query_tile, heuristic_val });
    came_from.emplace(query_tile, current_tile);
}

void query_segment(
    const entt::registry& registry,
    const SegmentComponent& segment,
    std::priority_queue<PathStep, std::vector<PathStep>, Compare>& frontier,
    std::unordered_map<entt::entity, entt::entity>& came_from,
    entt::entity target_tile,
    entt::entity current_tile
)
{
    auto it {
        std::find_if(
            segment.entities.begin(),
            segment.entities.end(),
            [&registry, target_tile](entt::entity entity) {
                return is_goal(registry, target_tile, entity);
            }
        )
    };

    if (it != segment.entities.end()) {
        advance(registry, frontier, came_from, current_tile, *it, target_tile);
    } else {
        for (auto entity : { segment.origin, segment.termination }) {
            advance(registry, frontier, came_from, current_tile, entity, target_tile);
        }
    }
}

} // namespace

namespace Pathfinding {

void path_between(
    const entt::registry& registry,
    entt::entity from_tile,
    entt::entity to_tile,
    std::vector<entt::entity>& path
)
{
    if (from_tile == to_tile)
        return;

    std::priority_queue<PathStep, std::vector<PathStep>, Compare> frontier;
    std::unordered_map<entt::entity, entt::entity> came_from;
    frontier.push({ from_tile, 0 });

    while (!frontier.empty()) {
        PathStep current { frontier.top() };
        frontier.pop();

        assert(
            (
                registry.any_of<SegmentMemberComponent, JunctionComponent>(current.tile) //
                && !registry.all_of<SegmentMemberComponent, JunctionComponent>(current.tile) //
            )
        );

        if (is_goal(registry, to_tile, current.tile)) {
            came_from.emplace(to_tile, current.tile);
            break;
        }

        if (registry.all_of<SegmentMemberComponent>(current.tile)) {
            query_segment(
                registry,
                registry.get<SegmentComponent>(registry.get<SegmentMemberComponent>(current.tile).segment),
                frontier,
                came_from,
                to_tile,
                current.tile
            );
        }

        if (registry.all_of<JunctionComponent>(current.tile)) {
            for (auto segment_entity : registry.get<JunctionComponent>(current.tile).connections) {
                if (segment_entity == entt::null)
                    continue;

                query_segment(
                    registry,
                    registry.get<SegmentComponent>(segment_entity),
                    frontier,
                    came_from,
                    to_tile,
                    current.tile
                );
            }
        }
    }

    if (came_from.find(to_tile) == came_from.end())
        return;

    for (
        entt::entity current = to_tile;
        current != from_tile;
        current = came_from.at(current)
    ) {
        path.push_back(current);
    }

    path.push_back(from_tile);
    std::reverse(path.begin(), path.end());
}

} // namespace