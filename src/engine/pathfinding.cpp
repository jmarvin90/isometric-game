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
            == segment_component.entities.end()
        )
            return segment;
    }
    return entt::null;
}

struct Compare {
    bool operator()(const SegmentComponent* lhs, const SegmentComponent* rhs)
    {
        return lhs->length < rhs->length;
    }
};

} // namespace

namespace Pathfinding {

// TODO - or entt::entity
std::vector<const SegmentComponent*> path_between(
    const entt::registry& registry,
    entt::entity from_tile,
    entt::entity to_tile,
    std::vector<const SegmentComponent*>& path
)
{
    if (from_tile == to_tile)
        return std::vector<const SegmentComponent*> {};

    entt::entity start_segment { get_segment(registry, from_tile) };
    entt::entity end_segment { get_segment(registry, to_tile) };

    if (start_segment == entt::null || end_segment == entt::null)
        return std::vector<const SegmentComponent*> {};

    std::priority_queue<const SegmentComponent*, std::vector<const SegmentComponent*>, Compare> frontier;
    const SegmentComponent& start_segment_component { registry.get<const SegmentComponent>(start_segment) };

    // TODO - determine if I want to return a single segment in case the start and the end are on the same seg
    if (start_segment == end_segment) {
        path.push_back(&start_segment_component);
        return;
    }

    const SegmentComponent& end_segment_component { registry.get<const SegmentComponent>(end_segment) };

    frontier.push(&start_segment_component);

    std::unordered_map<const SegmentComponent*, const SegmentComponent*> came_from;
    came_from.emplace(&start_segment_component, nullptr);

    while (!frontier.empty()) {
        const SegmentComponent* current_segment_component { frontier.top() };
        frontier.pop();

        if (current_segment_component == &end_segment_component)
            break;

        for (
            const JunctionComponent& junction : {
                registry.get<const JunctionComponent>(current_segment_component->origin),
                registry.get<const JunctionComponent>(current_segment_component->termination) }
        ) {
            for (entt::entity next_segment : junction.connections) {
                if (next_segment == entt::null)
                    continue;
                const SegmentComponent* next_segment_component { &registry.get<const SegmentComponent>(next_segment) };
                if (came_from.find(next_segment_component) == came_from.end()) {
                    frontier.push(next_segment_component);
                    came_from.emplace(next_segment_component, current_segment_component);
                }
            }
        }
    }

    const SegmentComponent* current { &end_segment_component };

    if (came_from.find(current) == came_from.end()) {
        return;
    }

    while (current != &start_segment_component) {
        path.push_back(current);
        current = came_from.at(current);
    }
    path.push_back(&start_segment_component);
    std::reverse(path.begin(), path.end());
}
} // namespace