#include <components/spatialmap_component.h>
#include <components/spatialmapcell_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <entt/entt.hpp>
#include <position.h>
#include <systems/spatialmap_system.h>

void SpatialMapSystem::register_entity(entt::registry& registry, entt::entity entity)
{
    const TransformComponent* transform { registry.try_get<const TransformComponent>(entity) };

    if (!transform)
        return;

    const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };
    SpatialMapComponent& spatial_map { registry.ctx().get<SpatialMapComponent>() };

    glm::ivec2 top_left { transform->position };
    glm::ivec2 top_right { top_left + glm::ivec2 { sprite.source_rect.w, 0 } };
    glm::ivec2 bottom_left { top_left + glm::ivec2 { 0, sprite.source_rect.h } };
    glm::ivec2 bottom_right { bottom_left + glm::ivec2 { sprite.source_rect.w, 0 } };

    for (glm::ivec2 pos : { top_left, top_right, bottom_left, bottom_right }) {
        int cell { WorldPosition(pos).to_spatial_map_cell(registry) };
        SpatialMapCellComponent* cell_component;

        entt::entity cell_entity { spatial_map[cell] };
        if (cell_entity == entt::null) {
            cell_entity = registry.create();

            glm::ivec2 cell_pos;
            if (cell < spatial_map.cells_per_row) {
                cell_pos = { cell, 0 };
            } else {
                cell_pos = { cell % spatial_map.cells_per_row, cell / spatial_map.cells_per_row };
            }

            // TODO: replace with position.h alternative
            glm::ivec2 world_pos { cell_pos * spatial_map.cell_size };

            cell_component = &registry.emplace<SpatialMapCellComponent>(cell_entity, SDL_Rect { world_pos.x, world_pos.y, spatial_map.cell_size.x, spatial_map.cell_size.y });
        } else {
            cell_component = &registry.get<SpatialMapCellComponent>(cell_entity);
        }
        cell_component->entities.emplace_back(entity);
    }
}