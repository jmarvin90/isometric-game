#include <camera_component.h>
#include <components/grid_position_component.h>
#include <components/render_offset_component.h>
#include <components/spatialmapcell_component.h>
#include <entt/entt.hpp>
#include <flags.h>
#include <grid.h>
#include <iso_utility.h>
#include <mouse_component.h>
#include <position.h>
#include <projection.h>
#include <sprite.h>
#include <sprite_component.h>
#include <spritesheet.h>
#include <systems/mouse_system.h>
#include <transform_component.h>

namespace {
entt::entity get_hovered_entity(
    const entt::registry& registry, const MouseComponent& mouse
)
{
    entt::entity spatialmap_cell_entity {
        registry.ctx().get<Grid<entt::entity, SpatialMapProjection>>().at_world(mouse.world_position)
    };

    const SpatialMapCellComponent* spatialmap_cell {
        registry.try_get<const SpatialMapCellComponent>(spatialmap_cell_entity)
    };

    if (spatialmap_cell_entity == entt::null || !spatialmap_cell)
        return entt::null;

    entt::entity best_entity { entt::null };
    int best_depth { -1 };
    int best_y { -1 };

    for (entt::entity entity : spatialmap_cell->entities) {
        const TransformComponent& transform {
            registry.get<const TransformComponent>(entity)
        };

        const SpriteComponent& sprite {
            registry.get<const SpriteComponent>(entity)
        };

        const RenderOffsetComponent* offset {
            registry.try_get<const RenderOffsetComponent>(entity)
        };

        glm::vec2 abs_position {
            offset ? transform.position + offset->render_offset : transform.position
        };

        if (!ISOUtility::AABB(abs_position, sprite, mouse.world_position))
            continue;

        if (
            !sprite.sprite_definition->spritemask.at_world(
                mouse.world_position, abs_position
            )
        )
            continue;

        if (
            transform.z_index > best_depth
            || (transform.z_index == best_depth && transform.position.y > best_y)
        ) {
            best_entity = entity;
            best_depth = transform.z_index;
            best_y = abs_position.y;
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
        registry.clear<HighlightedFlag>();
        mouse.screen_position = current_mouse_screen_pos;

        mouse.world_position = Position::screen_to_world(
            mouse.screen_position, camera.position
        );

        entt::entity hovered_entity { get_hovered_entity(registry, mouse) };
        if (hovered_entity != entt::null)
            registry.emplace_or_replace<HighlightedFlag>(
                get_hovered_entity(registry, mouse)
            );
    }
}

void select_entity(entt::registry& registry)
{
    const MouseComponent& mouse { registry.ctx().get<const MouseComponent>() };
    registry.clear<SelectedFlag>();
    registry.emplace_or_replace<SelectedFlag>(get_hovered_entity(registry, mouse));
}
}