#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include <components/segment_component.h>
#include <components/spatialmapcell_component.h>
#include <components/spatialmapcell_span_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <position.h>
#include <projection.h>
#include <systems/spatialmap_system.h>
#include <vector>

namespace {

std::vector<entt::entity> intersected_segments(
    entt::registry& registry,
    const SegmentComponent& segment
)
{
    Grid<entt::entity, SpatialMapProjection>& spatial_map { registry.ctx().get<Grid<entt::entity, SpatialMapProjection>>() };
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

    std::vector<entt::entity> output;

    while (true) {
        entt::entity spatial_map_cell { spatial_map[chunk] };

        if (!registry.all_of<SpatialMapCellComponent>(spatial_map_cell))
            registry.emplace<SpatialMapCellComponent>(spatial_map_cell);

        output.push_back(spatial_map_cell);

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

SpatialMapCellSpanComponent spanned_cells(entt::registry& registry, entt::entity entity)
{
    // TODO: will return invalid cells if an entity spans the edge of the map
    const TransformComponent& transform { registry.get<const TransformComponent>(entity) };
    const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };
    const Grid<entt::entity, SpatialMapProjection>& spatial_map { registry.ctx().get<Grid<entt::entity, SpatialMapProjection>>() };

    glm::ivec2 AA { transform.position };
    glm::ivec2 BB { AA + glm::ivec2 { sprite.sprite_definition->source_rect.w, sprite.sprite_definition->source_rect.h } };

    return {
        SpatialMapProjection::world_to_grid(AA, spatial_map),
        SpatialMapProjection::world_to_grid(BB, spatial_map),
    };
}

} // namespace

namespace SpatialMapSystem {

void update_entity(entt::registry& registry, entt::entity entity)
{
    SpatialMapCellSpanComponent& current_span { registry.get<SpatialMapCellSpanComponent>(entity) };
    const SpatialMapCellSpanComponent new_span { spanned_cells(registry, entity) };

    if (current_span != new_span) {
        remove_entity(registry, entity);
        emplace_entity(registry, entity);
        current_span = new_span;
    }
}

void emplace_entity(entt::registry& registry, entt::entity entity)
{
    const Grid<entt::entity, SpatialMapProjection>& spatial_map {
        registry.ctx().get<const Grid<entt::entity, SpatialMapProjection>>()
    };

    const SpatialMapCellSpanComponent& cell_span {
        registry.emplace_or_replace<SpatialMapCellSpanComponent>(
            entity, spanned_cells(registry, entity)
        )
    };

    for (int x = cell_span.AA.x; x <= cell_span.BB.x; x++) {
        for (int y = cell_span.AA.y; y <= cell_span.BB.y; y++) {
            SpatialMapCellComponent& cell {
                registry.get_or_emplace<SpatialMapCellComponent>(spatial_map[{ x, y }])
            };
            cell.entities.emplace_back(entity);
        }
    }
}

void remove_entity(entt::registry& registry, entt::entity entity)
{
    const Grid<entt::entity, SpatialMapProjection>& spatial_map {
        registry.ctx().get<const Grid<entt::entity, SpatialMapProjection>>()
    };

    const SpatialMapCellSpanComponent& cell_span {
        registry.get<const SpatialMapCellSpanComponent>(entity)
    };

    for (int x = cell_span.AA.x; x <= cell_span.BB.x; x++) {
        for (int y = cell_span.AA.y; y <= cell_span.BB.y; y++) {

            SpatialMapCellComponent* cell {
                registry.try_get<SpatialMapCellComponent>(spatial_map[{ x, y }])
            };

            if (!cell)
                continue;

            cell->entities.erase(
                std::remove(
                    cell->entities.begin(),
                    cell->entities.end(),
                    entity
                )
            );
        }
    }
    registry.remove<SpatialMapCellSpanComponent>(entity);
}

void emplace_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<SegmentComponent>(entity) };
    std::vector<entt::entity> cells { intersected_segments(registry, segment) };
    for (auto cell : cells) {
        registry.get<SpatialMapCellComponent>(cell).segments.emplace_back(entity);
    }
}

void remove_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<SegmentComponent>(entity) };
    std::vector<entt::entity> cells { intersected_segments(registry, segment) };
    for (auto cell : cells) {
        SpatialMapCellComponent& cell_component {
            registry.get<SpatialMapCellComponent>(cell)
        };
        cell_component.segments.erase(
            std::remove(
                cell_component.segments.begin(),
                cell_component.segments.end(),
                entity
            )
        );
    }
}
}