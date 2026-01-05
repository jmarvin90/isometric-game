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

namespace {

/*
    TODO: I need to come back to this because it doesn't consider any offsets for e.g.
    the centre of a tile
*/
std::vector<SpatialMapCellComponent&> intersected_segments(
    entt::registry& registry,
    const SegmentComponent& segment
)
{
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };
    WorldPosition segment_start { registry.get<const TransformComponent>(segment.start).position };
    WorldPosition segment_end { registry.get<const TransformComponent>(segment.end).position };

    for (
        int cell = segment_start.to_spatial_map_cell(registry);
        cell <= segment_end.to_spatial_map_cell(registry);
        cell++
    ) {
        entt::entity segment_cell { spatial_map[cell] };
        if (segment_cell == entt::null)
            continue;

        const SpatialMapCellComponent& cell_component {
            registry.get<const SpatialMapCellComponent>(segment_cell)
        };

        SDL_Point start = segment_start;
        SDL_Point end = segment_end;

        if (
            SDL_IntersectRectAndLine(
                &cell_component.cell,
                &start.x,
                &start.y,
                &end.x,
                &end.y
            )
        ) {
        }
    }
    // glm::ivec2 start_cell { segment_start.to_spatial_map_cell(registry) };
    // glm::ivec2 end_cell { segment_end.to_spatial_map_cell(registry) };
}

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

void SpatialMapSystem::emplace_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<SegmentComponent>(entity) };
    std::vector<glm::ivec2> segments { intersected_segments(registry, segment) };

    for (auto segment : segments) {
        int cell { SpatialMapGridPosition(segment).to_spatial_map_cell(registry) };
        SpatialMapCellComponent& cell_component { get_or_create_cell(registry, cell) };
        cell_component.segments.emplace_back(entity);
    }
}

void SpatialMapSystem::remove_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<SegmentComponent>(entity) };
    SpatialMapComponent& spatial_map { registry.ctx().get<SpatialMapComponent>() };
    std::vector<glm::ivec2> segments { intersected_segments(registry, segment) };
    [[maybe_unused]] int number_of_segments { static_cast<int>(segments.size()) };
    for (auto segment : segments) {
        int cell { SpatialMapGridPosition(segment).to_spatial_map_cell(registry) };
        SpatialMapCellComponent& cell_component { registry.get<SpatialMapCellComponent>(spatial_map[cell]) };
        cell_component.segments.erase(
            std::remove_if(
                cell_component.segments.begin(),
                cell_component.segments.end(),
                [entity](const entt::entity comparator) {
                    return comparator == entity;
                }
            )
        );
    }
}