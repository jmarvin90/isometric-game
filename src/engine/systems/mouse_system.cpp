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
    mouse.moved = (mouse.window_previous_position != mouse.window_current_position);

    const ScreenPositionComponent screen_position { mouse.window_current_position };
    const TileMapGridPositionComponent grid_position {
        Position::to_grid_position(
            Position::to_world_position(screen_position, registry.ctx().get<const CameraComponent>()),
            registry.ctx().get<const TileSpecComponent>(),
            registry.ctx().get<const TileMapComponent>()
        )
    };
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };

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