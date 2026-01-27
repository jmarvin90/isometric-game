#include <algorithm>
#include <components/junction_component.h>
#include <components/spatialmapcell_component.h>
#include <components/tilemap_grid_position_component.h>
#include <glm/glm.hpp>
#include <pathfinding.h>
#include <position.h>
#include <queue>
#include <unordered_map>
#include <vector>

namespace {

entt::entity get_segment(const entt::registry& registry, entt::entity tile)
{
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };
    // TODO - am fetching this multiple times?
    const TileMapGridPositionComponent* grid_position { registry.try_get<const TileMapGridPositionComponent>(tile) };

    if (!grid_position)
        return entt::null;

    SpatialMapGridPosition spatial_map_position { Position::to_spatial_map_grid_position(*grid_position, registry) };
    entt::entity spatial_map_cell_entity { spatial_map[spatial_map_position.position] };

    // TODO: this shouldn't happen, really
    if (spatial_map_cell_entity == entt::null)
        return entt::null;

    const SpatialMapCellComponent& spatial_map_cell_component {
        registry.get<const SpatialMapCellComponent>(spatial_map_cell_entity)
    };

    for (auto segment : spatial_map_cell_component.segments) {
        const SegmentComponent& segment_component { registry.get<const SegmentComponent>(segment) };
        if (
            std::find(
                segment_component.entities.begin(), //
                segment_component.entities.end(), //
                tile
            )
            != segment_component.entities.end()
        )
            return segment;
    }
    return entt::null;
}

struct PathStep {
    entt::entity segment;
    int priority;
};

struct Compare {
    bool operator()(const PathStep& lhs, const PathStep& rhs)
    {
        return lhs.priority > rhs.priority;
    }
};

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

    entt::entity start_segment { get_segment(registry, from_tile) };
    entt::entity end_segment { get_segment(registry, to_tile) };

    if (start_segment == entt::null || end_segment == entt::null)
        return;

    /*
        TODO - determine if I want to return a single segment
        in case the start and the end are on the same seg
    */
    if (start_segment == end_segment) {
        path.push_back(start_segment);
        return;
    }

    std::priority_queue<PathStep, std::vector<PathStep>, Compare> frontier;
    std::unordered_map<const entt::entity, const entt::entity> came_from;

    frontier.push({ start_segment, 0 });
    came_from.emplace(start_segment, entt::null);

    while (!frontier.empty()) {
        const PathStep current_segment { frontier.top() };
        frontier.pop();

        if (current_segment.segment == end_segment)
            break;

        const SegmentComponent& current_segment_component { registry.get<const SegmentComponent>(current_segment.segment) };

        for (
            entt::entity junction_entity : {
                current_segment_component.origin,
                current_segment_component.termination }
        ) {
            const JunctionComponent& junction_component { registry.get<const JunctionComponent>(junction_entity) };
            for (entt::entity next_segment : junction_component.connections) {

                if (next_segment == entt::null)
                    continue;
                if (came_from.find(next_segment) != came_from.end())
                    continue;

                const SegmentComponent& next_segment_component { registry.get<const SegmentComponent>(next_segment) };
                frontier.push({ next_segment, next_segment_component.length });
                came_from.emplace(next_segment, current_segment.segment);
            }
        }
    }

    entt::entity current { end_segment };

    if (came_from.find(current) == came_from.end()) {
        return;
    }

    while (current != start_segment) {
        path.push_back(current);
        current = came_from.at(current);
    }
    path.push_back(start_segment);
    std::reverse(path.begin(), path.end());
}
} // namespace