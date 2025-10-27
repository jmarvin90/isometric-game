#ifndef MOUSESYSTEM_H
#define MOUSESYSTEM_H

#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <systems/position_system.h>

#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>


class MouseSystem {
    public:
        static void update(entt::registry& registry) {
            MouseComponent& mouse {registry.ctx().get<MouseComponent>()};
            const CameraComponent& camera {registry.ctx().get<const CameraComponent>()};

            mouse.window_previous_position = mouse.window_current_position;
            
            SDL_GetMouseState(
                &mouse.window_current_position.x,
                &mouse.window_current_position.y
            );

            mouse.moved = (
                mouse.window_previous_position !=
                mouse.window_current_position
            );
        }
};

#endif