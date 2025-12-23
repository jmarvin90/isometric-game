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
    int cell { WorldPosition(transform->position).to_spatial_map_cell(registry) };
    SpatialMapCellComponent* cell_component;

    entt::entity cell_entity { spatial_map[cell] };

    if (cell_entity == entt::null) {
        cell_entity = registry.create();

        glm::ivec2 spatial_map_world_pos { 
            SpatialMapGridPosition::from_cell_number(spatial_map, cell).to_world_position(spatial_map) 
        };

        cell_component = &registry.emplace<SpatialMapCellComponent>(
            cell_entity, 
            SDL_Rect { 
                spatial_map_world_pos.x,
                spatial_map_world_pos.y,
                spatial_map.cell_size.x,
                spatial_map.cell_size.y 
            }
        );
    } else {
        cell_component = &registry.get<SpatialMapCellComponent>(cell_entity);
    }
    cell_component->entities.emplace_back(entity);
}