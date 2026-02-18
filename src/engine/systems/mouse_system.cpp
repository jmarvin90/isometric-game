#include <components/mouseover_component.h>
#include <components/screen_position_component.h>
#include <components/tilemap_component.h>
#include <components/tilemap_grid_position_component.h>
#include <components/tilespec_component.h>
#include <entt/entt.hpp>
#include <position.h>
#include <systems/mouse_system.h>

void MouseSystem::update(entt::registry& registry)
{
    MouseComponent& mouse { registry.ctx().get<MouseComponent>() };
    mouse.window_previous_position = mouse.window_current_position;
    SDL_GetMouseState(&mouse.window_current_position.x, &mouse.window_current_position.y);
    mouse.moved_in_frame = (mouse.window_previous_position != mouse.window_current_position);

    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };

    const TileMapGridPositionComponent grid_position {
        Position::world_to_grid(
            Position::screen_to_world(mouse.window_current_position, camera.position()),
            tilespec.centre,
            tilemap.origin_px,
            tilespec.matrix_inverted
        )
    };

    if (tilemap[grid_position.position] != entt::null) {
        if (registry.all_of<MouseOverComponent>(tilemap[grid_position.position])) {
            registry.patch<MouseOverComponent>(
                tilemap[grid_position.position],
                [](auto& moc) { moc.this_frame = true; }
            );
        } else {
            registry.emplace<MouseOverComponent>(tilemap[grid_position.position], true, false);
        }
    }
}