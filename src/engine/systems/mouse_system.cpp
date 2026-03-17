#include <components/grid_position_component.h>
#include <components/mouseover_component.h>
#include <components/render_offset_component.h>
#include <components/spatialmapcell_component.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <position.h>
#include <projection.h>
#include <sprite_component.h>
#include <systems/mouse_system.h>
#include <utility.h>

namespace MouseSystem {
void update(entt::registry& registry)
{
    MouseComponent& mouse { registry.ctx().get<MouseComponent>() };
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };

    mouse.screen_previous_position = mouse.screen_current_position;
    SDL_GetMouseState(&mouse.screen_current_position.x, &mouse.screen_current_position.y);
    mouse.moved_in_frame = (mouse.screen_previous_position != mouse.screen_current_position);

    if (mouse.moved_in_frame) {
        entt::entity best_entity { entt::null };
        int best_depth { -1 };

        glm::ivec2 mouse_world_position {
            Position::screen_to_world(mouse.screen_current_position, camera.position)
        };

        entt::entity spatialmap_cell_entity {
            registry.ctx().get<Grid<entt::entity, SpatialMapProjection>>().at_world(mouse_world_position)
        };

        const SpatialMapCellComponent* spatialmap_cell {
            registry.try_get<const SpatialMapCellComponent>(spatialmap_cell_entity)
        };

        if (spatialmap_cell_entity == entt::null || !spatialmap_cell)
            return;

        for (entt::entity entity : spatialmap_cell->entities) {
            if (!Utility::AABB(registry, entity, mouse_world_position))
                continue;

            const TransformComponent& transform { registry.get<const TransformComponent>(entity) };
            const RenderOffsetComponent* offset { registry.try_get<const RenderOffsetComponent>(entity) };
            const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };

            glm::vec2 position { offset ? transform.position + glm::vec2(offset->offset) : transform.position };

            if (!sprite.sprite_definition->spritemask.at_world(mouse_world_position, position))
                continue;

            if (transform.z_index > best_depth) {
                best_entity = entity;
                best_depth = transform.z_index;
            }
        }

        for (entt::entity entity : registry.view<const MouseOverComponent>()) {
            if (entity != best_entity)
                registry.remove<MouseOverComponent>(entity);
        }

        if (best_entity != entt::null && !registry.all_of<MouseOverComponent>(best_entity))
            registry.emplace<MouseOverComponent>(best_entity);
    }
}

void highlight([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity)
{
}

void remove_highlight([[maybe_unused]] entt::registry& registry, [[maybe_unused]] entt::entity entity)
{
}
}
