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

} // namespace

namespace Pathfinding {

// TODO - or entt::entity
void path_between(
    const entt::registry& registry,
    entt::entity from_tile,
    entt::entity to_tile,
    std::vector<entt::entity>& path
)
{
    if (from_tile == to_tile)
        return;

    const GridPositionComponent& from_tile_position {
        registry.get<const GridPositionComponent>(from_tile)
    };

    const GridPositionComponent& to_tile_position {
        registry.get<const GridPositionComponent>(to_tile)
    };

    std::priority_queue<PathStep, std::vector<PathStep>, Compare> frontier;
    std::unordered_map<entt::entity, entt::entity> came_from;
    frontier.push({ from_tile, 0 });

    while (!frontier.empty()) {
        PathStep current { frontier.top() };
        frontier.pop();

        if (
            current.tile == to_tile
            || is_adjacent_to(from_tile_position.position, to_tile_position.position)
        )
            break;

        const JunctionComponent* junction_component {
            registry.try_get<const JunctionComponent>(current.tile)
        };

        std::vector<entt::entity> pending;

        bool has_required_components {
            registry.any_of<SegmentMemberComponent, JunctionComponent>(current.tile)
        };

        assert(has_required_components);

        if (!junction_component) {
            const SegmentComponent* segment_component {
                &registry.get<const SegmentComponent>(
                    registry.get<SegmentMemberComponent>(current.tile).segment
                )
            };
            pending.push_back(segment_component->origin);
            pending.push_back(segment_component->termination);
        } else {
            for (auto segment : junction_component->connections) {
                if (segment == entt::null)
                    continue;

                const SegmentComponent& segment_component {
                    registry.get<const SegmentComponent>(segment)
                };

                for (auto tile : segment_component.entities) {
                    if (
                        tile == to_tile
                        || is_adjacent_to(
                            to_tile_position.position,
                            registry.get<const GridPositionComponent>(tile).position
                        )
                    ) {
                        pending.push_back(to_tile);
                        break;
                    }
                }

                if (
                    std::find(
                        segment_component.entities.begin(),
                        segment_component.entities.end(),
                        to_tile
                    )
                    != segment_component.entities.end()
                ) {
                    pending.push_back(to_tile);
                    break;
                }

                if (segment_component.origin == current.tile) {
                    pending.push_back(segment_component.termination);
                } else if (segment_component.termination == current.tile) {
                    pending.push_back(segment_component.origin);
                } else {
                    //
                }
            }
        }

        for (auto next_tile : pending) {
            if (came_from.find(next_tile) != came_from.end())
                continue;

            const GridPositionComponent& next_tile_position {
                registry.get<const GridPositionComponent>(next_tile)
            };

            frontier.push(
                { next_tile,
                  heuristic(next_tile_position.position, to_tile_position.position) }
            );

            came_from.emplace(next_tile, current.tile);
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