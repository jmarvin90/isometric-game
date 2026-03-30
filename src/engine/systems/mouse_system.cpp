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

namespace {
entt::entity get_hovered_entity(const entt::registry& registry, const MouseComponent& mouse, const CameraComponent& camera)
{
    entt::entity spatialmap_cell_entity {
        registry.ctx().get<Grid<entt::entity, SpatialMapProjection>>().at_world(mouse.world_position)
    };

    const SpatialMapCellComponent* spatialmap_cell {
        registry.try_get<const SpatialMapCellComponent>(spatialmap_cell_entity)
    };

    if (spatialmap_cell_entity == entt::null || !spatialmap_cell)
        return;

    entt::entity best_entity { entt::null };
    int best_depth { -1 };
    int best_y { -1 };

    for (entt::entity entity : spatialmap_cell->entities) {
        if (!Utility::AABB(registry, entity, mouse.world_position))
            continue;

        const TransformComponent& transform { registry.get<const TransformComponent>(entity) };
        const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };

        if (!sprite.sprite_definition->spritemask.at_world(mouse.world_position, transform.position))
            continue;

        if (
            transform.z_index > best_depth
            || (transform.z_index == best_depth && transform.position.y > best_y)
        ) {
            best_entity = entity;
            best_depth = transform.z_index;
            best_y = transform.position.y;
        }
    }

    return best_entity;
}
}

namespace MouseSystem {
void update(entt::registry& registry)
{
    MouseComponent& mouse { registry.ctx().get<MouseComponent>() };
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };

    glm::ivec2 current_mouse_screen_pos {};
    SDL_GetMouseState(&current_mouse_screen_pos.x, &current_mouse_screen_pos.y);
    mouse.moved_in_frame = (mouse.screen_position != current_mouse_screen_pos);

    if (mouse.moved_in_frame) {
        mouse.screen_position = current_mouse_screen_pos;

        mouse.world_position = Position::screen_to_world(
            mouse.screen_position, camera.position
        );

        entt::entity hovered_entity { get_hovered_entity(registry, mouse, camera) };

        for (entt::entity entity : registry.view<const MouseOverComponent>()) {
            if (entity != hovered_entity)
                registry.remove<MouseOverComponent>(entity);
        }

        if (hovered_entity != entt::null)
            registry.emplace_or_replace<MouseOverComponent>(hovered_entity);
    }
}
}