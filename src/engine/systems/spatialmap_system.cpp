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
SpatialMapCellComponent* get_or_create_cell(entt::registry& registry, glm::ivec2 grid_position)
{
    SpatialMapComponent& spatial_map { registry.ctx().get<SpatialMapComponent>() };
    entt::entity cell { spatial_map[grid_position] };

    if (cell != entt::null) {
        return &registry.get<SpatialMapCellComponent>(cell);
    } else {
        cell = registry.create();
        int cell_number = SpatialMapGridPosition(grid_position).to_spatial_map_cell(spatial_map);

        [[maybe_unused]] bool check { spatial_map.map.find(cell_number) == spatial_map.map.end() };
        spatial_map.map[cell_number] = cell;
        assert(spatial_map.map[cell_number] == cell);

        glm::ivec2 spatial_map_world_pos {
            SpatialMapGridPosition(grid_position).to_world_position(spatial_map)
        };

        return &registry.emplace<SpatialMapCellComponent>(
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

    glm::vec2 start { glm::vec2 { segment_start.position } / glm::vec2 { spatial_map.cell_size } };
    glm::vec2 end { glm::vec2 { segment_end.position } / glm::vec2 { spatial_map.cell_size } };
    glm::vec2 delta { end - start };

    glm::vec2 chunk { glm::floor(start) };
    glm::vec2 chunk_end { glm::floor(end) };
    glm::vec2 step { delta.x != 0 ? std::copysign(1, delta.x) : 0, delta.y != 0 ? std::copysign(1, delta.y) : 0 };

    glm::vec2 tDelta {
        (delta.x != 0) ? std::abs(1.0f / delta.x) : std::numeric_limits<float>::infinity(),
        (delta.y != 0) ? std::abs(1.0f / delta.y) : std::numeric_limits<float>::infinity()
    };

    glm::vec2 next_boundary { step.x > 0 ? chunk.x + 1 : chunk.x, step.y > 0 ? chunk.y + 1 : chunk.y };
    glm::vec2 tMax { (next_boundary - start) / delta };

    while (true) {
        output.push_back(get_or_create_cell(registry, chunk));

        if (chunk == chunk_end)
            break;

        if (tMax.x < tMax.y) {
            chunk.x += step.x;
            tMax.x += tDelta.x;
        } else {
            chunk.y += step.y;
            tMax.y += tDelta.y;
        }
    }

    return output;
}
} // namespace

void SpatialMapSystem::emplace_entity(entt::registry& registry, entt::entity entity)
{
    const TransformComponent* transform { registry.try_get<const TransformComponent>(entity) };

    // TODO - raise a problem if the transform is missing
    if (!transform)
        return;

    glm::ivec2 cell_position { WorldPosition(transform->position).to_spatial_map_position(registry) };
    SpatialMapCellComponent* cell_component { get_or_create_cell(registry, cell_position) };
    cell_component->entities.emplace_back(entity);
}

void SpatialMapSystem::remove_entity(entt::registry& registry, entt::entity entity)
{
    const TransformComponent* transform { registry.try_get<const TransformComponent>(entity) };

    // TODO - raise a problem if the transform is missing
    if (!transform)
        return;

    glm::ivec2 cell { WorldPosition(transform->position).to_spatial_map_position(registry) };
    SpatialMapCellComponent* cell_component { get_or_create_cell(registry, cell) };
    cell_component->entities.erase(
        std::remove_if(
            cell_component->segments.begin(),
            cell_component->segments.end(),
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