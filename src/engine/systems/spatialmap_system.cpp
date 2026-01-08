#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <components/segment_component.h>
#include <components/spatialmap_component.h>
#include <components/spatialmapcell_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <entt/entt.hpp>
#include <position.h>
#include <systems/spatialmap_system.h>
#include <vector>

namespace {
// TODO - potentially a component member function
SpatialMapCellComponent& get_or_create_cell(entt::registry& registry, int cell_number)
{
    SpatialMapComponent& spatial_map { registry.ctx().get<SpatialMapComponent>() };
    entt::entity cell { spatial_map[cell_number] };

    if (cell != entt::null) {
        return registry.get<SpatialMapCellComponent>(cell);
    } else {
        cell = registry.create();

        glm::ivec2 spatial_map_world_pos {
            SpatialMapGridPosition::from_cell_number(spatial_map, cell_number).to_world_position(spatial_map)
        };

        return registry.emplace<SpatialMapCellComponent>(
            cell,
            SDL_Rect {
                spatial_map_world_pos.x,
                spatial_map_world_pos.y,
                spatial_map.cell_size.x,
                spatial_map.cell_size.y }
        );
    }
}

/*
    TODO: I need to come back to this because it doesn't consider any offsets for e.g.
    the centre of a tile
*/
std::vector<SpatialMapCellComponent*> intersected_segments(
    [[maybe_unused]] entt::registry& registry,
    [[maybe_unused]] const SegmentComponent& segment
)
{
    [[maybe_unused]] SpatialMapComponent& spatial_map { registry.ctx().get<SpatialMapComponent>() };
    [[maybe_unused]] const TransformComponent& segment_start { registry.get<const TransformComponent>(segment.start) };
    [[maybe_unused]] const TransformComponent& segment_end { registry.get<const TransformComponent>(segment.end) };
    std::vector<SpatialMapCellComponent*> output;
    return output;

    // output.push_back(
    //     &registry.get<SpatialMapCellComponent>(
    //         spatial_map[WorldPosition(segment_start.position).to_spatial_map_cell(spatial_map)]
    //     )
    // );

    // output.push_back(
    //     &registry.get<SpatialMapCellComponent>(
    //         spatial_map[WorldPosition(segment_end.position).to_spatial_map_cell(spatial_map)]
    //     )
    // );
}

} // namespace

void SpatialMapSystem::emplace_entity(entt::registry& registry, entt::entity entity)
{
    const TransformComponent* transform { registry.try_get<const TransformComponent>(entity) };

    // TODO - raise a problem if the transform is missing
    if (!transform)
        return;

    int cell { WorldPosition(transform->position).to_spatial_map_cell(registry) };
    SpatialMapCellComponent& cell_component { get_or_create_cell(registry, cell) };
    cell_component.entities.emplace_back(entity);
}

void SpatialMapSystem::remove_entity(entt::registry& registry, entt::entity entity)
{
    const TransformComponent* transform { registry.try_get<const TransformComponent>(entity) };

    // TODO - raise a problem if the transform is missing
    if (!transform)
        return;

    int cell { WorldPosition(transform->position).to_spatial_map_cell(registry) };
    SpatialMapCellComponent& cell_component { get_or_create_cell(registry, cell) };
    cell_component.entities.erase(
        std::remove_if(
            cell_component.segments.begin(),
            cell_component.segments.end(),
            [entity](const entt::entity comparator) {
                return comparator == entity;
            }
        )
    );
}

void SpatialMapSystem::emplace_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<SegmentComponent>(entity) };
    std::vector<SpatialMapCellComponent*> cells { intersected_segments(registry, segment) };
    for (auto cell : cells) {
        cell->segments.emplace_back(entity);
    }
}

void SpatialMapSystem::remove_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<SegmentComponent>(entity) };
    std::vector<SpatialMapCellComponent*> cells { intersected_segments(registry, segment) };
    [[maybe_unused]] int number_of_segments { static_cast<int>(cells.size()) };
    for (auto cell : cells) {
        cell->segments.erase(
            std::remove_if(
                cell->segments.begin(),
                cell->segments.end(),
                [entity](const entt::entity comparator) {
                    return comparator == entity;
                }
            )
        );
    }
}