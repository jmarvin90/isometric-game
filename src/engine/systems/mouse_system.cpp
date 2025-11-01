#include <systems/mouse_system.h>

void MouseSystem::update(entt::registry& registry)
{
    MouseComponent& mouse { registry.ctx().get<MouseComponent>() };

    mouse.window_previous_position = mouse.window_current_position;

    SDL_GetMouseState(&mouse.window_current_position.x,
        &mouse.window_current_position.y);

    mouse.moved = (mouse.window_previous_position != mouse.window_current_position);
}