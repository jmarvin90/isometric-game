#include <components/grid_position_component.h>
#include <components/mouseover_component.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <position.h>
#include <projection.h>
#include <systems/mouse_system.h>

void MouseSystem::update(entt::registry& registry)
{
    MouseComponent& mouse { registry.ctx().get<MouseComponent>() };
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    const Grid<TileMapProjection>& tilemap { registry.ctx().get<const Grid<TileMapProjection>>() };

    mouse.window_previous_position = mouse.window_current_position;
    SDL_GetMouseState(&mouse.window_current_position.x, &mouse.window_current_position.y);
    mouse.moved_in_frame = (mouse.window_previous_position != mouse.window_current_position);

    glm::ivec2 mouse_world_position {
        Position::screen_to_world(mouse.window_current_position, camera.position)
    };

    entt::entity mouse_over_entity { tilemap.at_world(mouse_world_position) };

    if (mouse_over_entity == entt::null)
        return;

    if (registry.all_of<MouseOverComponent>(mouse_over_entity)) {
        registry.patch<MouseOverComponent>(
            mouse_over_entity, [](auto& moc) { moc.this_frame = true; }
        );
    } else {
        registry.emplace<MouseOverComponent>(mouse_over_entity, true, false);
    }
}