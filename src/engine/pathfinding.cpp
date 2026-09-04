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
#include <optional>
#include <pathfinding.h>
#include <projection.h>
#include <queue>
#include <unordered_map>
#include <vector>

namespace {

bool is_adjacent_to(glm::vec2 from, glm::vec2 to)
{
    glm::vec2 diff = glm::abs(from - to);
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

int heuristic(const glm::vec2& lhs, const glm::vec2& rhs)
{
    glm::vec2 delta { glm::abs(rhs - lhs) };
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

    // Advance to target tile if it's the goal, else via the origin/termination
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
    if (
        from_tile == to_tile
        || !registry.any_of<JunctionComponent, SegmentMemberComponent>(from_tile)
    )
        return;

    std::priority_queue<PathStep, std::vector<PathStep>, Compare> frontier;
    std::unordered_map<entt::entity, entt::entity> came_from;

    if (registry.all_of<SegmentMemberComponent>(from_tile)) {
        query_segment(
            registry,
            registry.get<SegmentComponent>(registry.get<SegmentMemberComponent>(from_tile).segment),
            frontier,
            came_from,
            to_tile,
            from_tile
        );
    } else {
        frontier.push({ from_tile, 0 });
    }

    while (!frontier.empty()) {
        PathStep current { frontier.top() };
        frontier.pop();

        if (is_goal(registry, to_tile, current.tile)) {
            came_from.emplace(to_tile, current.tile);
            break;
        }

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

    if (came_from.find(to_tile) == came_from.end())
        return;

    for (
        entt::entity current = came_from.at(to_tile);
        current != from_tile;
        current = came_from.at(current)
    ) {
        path.push_back(current);
    }

    path.push_back(from_tile);
    std::reverse(path.begin(), path.end());
}

void expand_path(
    entt::registry& registry,
    const std::vector<entt::entity>& path,
    std::vector<PathSegment>& expanded_path
)
{
    Direction::TDirection incoming_direction { Direction::TDirection::NO_DIRECTION };
    glm::ivec2 current_grid_position { registry.get<const GridPositionComponent>(path.front()).position };
    glm::ivec2 current_transform_abs { registry.get<const TransformComponent>(path.front()).position };

    for (size_t index = 0; index < path.size() - 1; index++) {
        const glm::ivec2 next_grid_position { registry.get<const GridPositionComponent>(path.at(index + 1)).position };
        const glm::ivec2 next_transform_abs { registry.get<const TransformComponent>(path.at(index + 1)).position };

        Direction::TDirection outgoing_direction {
            Direction::from_vector(next_grid_position - current_grid_position)
        };

        Direction::TDirection reverse_outgoing_direction {
            Direction::reverse(outgoing_direction)
        };

        glm::vec2 outgoing_exit {
            current_transform_abs
            + Constants::ROAD_GATES.at(index_position(outgoing_direction)).exit
        };

        // Add a step to traverse a junction
        if (index != 0 && registry.all_of<JunctionComponent>(path.at(index))) {
            expanded_path.emplace_back(
                current_transform_abs + Constants::ROAD_GATES.at(index_position(Direction::reverse(incoming_direction))).entry,
                outgoing_exit,
                incoming_direction | outgoing_direction
            );
        }

        expanded_path.emplace_back(
            outgoing_exit,
            next_transform_abs + Constants::ROAD_GATES.at(index_position(reverse_outgoing_direction)).entry,
            outgoing_direction
        );

        incoming_direction = outgoing_direction;
        current_grid_position = next_grid_position;
        current_transform_abs = next_transform_abs;
    }
    [[maybe_unused]] const GridPositionComponent& last_pos { registry.get<const GridPositionComponent>(path.back()) };
}

} // namespace