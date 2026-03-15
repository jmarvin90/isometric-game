#include <components/grid_position_component.h>
#include <components/mouseover_component.h>
#include <components/spatialmapcell_component.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <position.h>
#include <projection.h>
#include <sprite_component.h>
#include <systems/mouse_system.h>

namespace {
// TODO - surely I'm doing this somewhere else
bool aabb(const entt::registry& registry, entt::entity entity, const glm::ivec2 mouse_position)
{
    const TransformComponent& transform { registry.get<const TransformComponent>(entity) };
    const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };
    glm::ivec2 AA { transform.position };
    glm::ivec2 BB { AA + glm::ivec2 { sprite.source_rect.w, sprite.source_rect.h } };
    return (
        glm::all(glm::greaterThanEqual(mouse_position, AA))
        & glm::all(glm::lessThan(mouse_position, BB))
    );
}
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
            registry.ctx().get<Grid<SpatialMapProjection>>().at_world(mouse_world_position)
        };

        const SpatialMapCellComponent* spatialmap_cell {
            registry.try_get<const SpatialMapCellComponent>(spatialmap_cell_entity)
        };

        if (spatialmap_cell_entity == entt::null || !spatialmap_cell)
            return;

        for (entt::entity entity : spatialmap_cell->entities) {
            if (aabb(registry, entity, mouse_world_position)) {
                continue;
            }
        }
    }
}
}
