#include <components/grid_position_component.h>
#include <components/mouseover_component.h>
#include <components/spatialmapcell_component.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <position.h>
#include <projection.h>
#include <sprite_component.h>
#include <systems/mouse_system.h>
#include <utility.h>

namespace {
}

namespace MouseSystem {
void update(entt::registry& registry)
{
    MouseComponent& mouse { registry.ctx().get<MouseComponent>() };
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };

    mouse.screen_previous_position = mouse.screen_current_position;
    SDL_GetMouseState(&mouse.screen_current_position.x, &mouse.screen_current_position.y);
    mouse.moved_in_frame = (mouse.screen_previous_position != mouse.screen_current_position);

    if (mouse.moved_in_frame) {

        registry.clear<MouseOverComponent>();

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
            if (Utility::AABB(registry, entity, mouse_world_position)) {
                continue;
            }
        }
    }
}
}
