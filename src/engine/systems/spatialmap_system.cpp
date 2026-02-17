#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <components/debug_component.h>
#include <components/segment_component.h>
#include <components/spatialmap_component.h>
#include <components/spatialmapcell_component.h>
#include <components/spatialmapcell_span_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <position.h>
#include <systems/spatialmap_system.h>
#include <vector>

namespace {
// TODO - potentially a component member function;
// TODO - be aware of pointer invalidation; perhaps return the entity ID instead
SpatialMapCellComponent* get_or_create_cell(entt::registry& registry, const glm::ivec2 grid_position)
{
    SpatialMapComponent& spatial_map { registry.ctx().get<SpatialMapComponent>() };
    entt::entity cell { spatial_map[grid_position] };

    if (cell != entt::null) {
        return &registry.get<SpatialMapCellComponent>(cell);
    } else {
        cell = registry.create();
        spatial_map.emplace_at(grid_position, cell);

        glm::ivec2 spatial_map_world_position { Position::to_world_position(grid_position, spatial_map) };

        return &registry.emplace<SpatialMapCellComponent>(
            cell,
            SDL_Rect {
                spatial_map_world_position.x,
                spatial_map_world_position.y,
                spatial_map.cell_size.x,
                spatial_map.cell_size.y }
        );
    }
}

/*
    TODO: I need to come back to this because it doesn't consider any offsets for e.g.
    the centre of a tile
    TODO - potentially a std::vector<entt::entity>
*/
std::vector<SpatialMapCellComponent*> intersected_segments(
    entt::registry& registry,
    const SegmentComponent& segment
)
{
    SpatialMapComponent& spatial_map { registry.ctx().get<SpatialMapComponent>() };
    const TransformComponent& segment_start { registry.get<const TransformComponent>(segment.origin) };
    const TransformComponent& segment_end { registry.get<const TransformComponent>(segment.termination) };

    glm::vec2 start { segment_start.position / glm::vec2 { spatial_map.cell_size } };
    glm::vec2 end { segment_end.position / glm::vec2 { spatial_map.cell_size } };
    glm::vec2 delta { end - start };

    glm::vec2 chunk { glm::floor(start) };
    glm::vec2 chunk_end { glm::floor(end) };
    glm::vec2 step { Direction::to_direction_vector(delta) };

    glm::vec2 tDelta {
        (delta.x != 0) ? std::abs(1.0f / delta.x) : std::numeric_limits<float>::infinity(),
        (delta.y != 0) ? std::abs(1.0f / delta.y) : std::numeric_limits<float>::infinity()
    };

    glm::vec2 next_boundary { step.x > 0 ? chunk.x + 1 : chunk.x, step.y > 0 ? chunk.y + 1 : chunk.y };
    glm::vec2 tMax { (next_boundary - start) / delta };

    std::vector<SpatialMapCellComponent*> output;

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

glm::ivec2 to_spatial_map_grid_position(const glm::ivec2 world_position, const SpatialMapComponent& spatial_map)
{
    assert(spatial_map.cell_size.x > 0 && spatial_map.cell_size.y > 0);
    return world_position / spatial_map.cell_size;
}

SpatialMapCellSpanComponent spanned_cells(entt::registry& registry, entt::entity entity)
{
    const TransformComponent& transform { registry.get<const TransformComponent>(entity) };
    const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };

    glm::ivec2 AA { transform.position };
    glm::ivec2 BB { AA + glm::ivec2 { sprite.source_rect.w, sprite.source_rect.h } };

    return {
        to_spatial_map_grid_position(AA, spatial_map),
        to_spatial_map_grid_position(BB, spatial_map)
    };
}

} // namespace

void SpatialMapSystem::update_entity(entt::registry& registry, entt::entity entity)
{
    SpatialMapCellSpanComponent& current_span { registry.get<SpatialMapCellSpanComponent>(entity) };
    const SpatialMapCellSpanComponent new_span { spanned_cells(registry, entity) };

    if (current_span != new_span) {
        SpatialMapSystem::remove_entity(registry, entity);
        SpatialMapSystem::emplace_entity(registry, entity);
        current_span = new_span;
    }
}

void SpatialMapSystem::emplace_entity(entt::registry& registry, entt::entity entity)
{
    const SpatialMapCellSpanComponent& cell_span {
        registry.emplace<SpatialMapCellSpanComponent>(entity, spanned_cells(registry, entity))
    };
    for (int x = cell_span.AA.x; x <= cell_span.BB.x; x++) {
        for (int y = cell_span.AA.y; y <= cell_span.BB.y; y++) {
            SpatialMapCellComponent* cell { get_or_create_cell(registry, { x, y }) };
            cell->entities.emplace_back(entity);
        }
    }
}

void SpatialMapSystem::remove_entity(entt::registry& registry, entt::entity entity)
{
    SpatialMapCellSpanComponent& cell_span { registry.get<SpatialMapCellSpanComponent>(entity) };
    for (int x = cell_span.AA.x; x <= cell_span.BB.x; x++) {
        for (int y = cell_span.AA.y; y <= cell_span.BB.y; y++) {
            SpatialMapCellComponent* cell { get_or_create_cell(registry, { x, y }) };
            std::remove_if(
                cell->entities.begin(),
                cell->entities.end(),
                [entity](const entt::entity comparator) {
                    return comparator == entity;
                }
            );
        }
    }
    registry.remove<SpatialMapCellSpanComponent>(entity);
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