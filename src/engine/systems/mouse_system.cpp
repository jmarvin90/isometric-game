#include <components/mouseover_component.h>
#include <components/tilemap_component.h>
#include <entt/entt.hpp>
#include <position.h>
#include <systems/mouse_system.h>

void MouseSystem::update(entt::registry& registry)
{
    MouseComponent& mouse { registry.ctx().get<MouseComponent>() };
    mouse.window_previous_position = mouse.window_current_position;
    SDL_GetMouseState(&mouse.window_current_position.x, &mouse.window_current_position.y);
    mouse.moved = (mouse.window_previous_position != mouse.window_current_position);
    const GridPosition grid_position { ScreenPosition(mouse.window_current_position).to_grid_position(registry) };
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    if (tilemap[grid_position] != entt::null) {
        if (registry.all_of<MouseOverComponent>(tilemap[grid_position])) {
            registry.patch<MouseOverComponent>(
                tilemap[grid_position],
                [](auto& moc) { moc.this_frame = true; }
            );
        } else {
            registry.emplace<MouseOverComponent>(tilemap[grid_position], true, false);
        }
    }
}