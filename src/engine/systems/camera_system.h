#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <components/camera_component.h>
#include <components/mouse_component.h>

#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <entt/entt.hpp>
#include <SDL2/SDL.h>


class CameraSystem {
    public:
        static void update(entt::registry& registry, const SDL_DisplayMode& display_mode) {
            const MouseComponent& mouse {registry.ctx().get<const MouseComponent>()};
            CameraComponent& camera {registry.ctx().get<CameraComponent>()};
            const TileMapComponent& tilemap {registry.ctx().get<const TileMapComponent>()};

            const glm::ivec2 screen_border_px {150, 150};

            if (
                mouse.window_current_position.x < screen_border_px.x &&
                camera.position.x > 0
            ) {
                camera.position.x -= 10;
            }

            if (
                mouse.window_current_position.y < screen_border_px.y &&
                camera.position.y > 0
            ) {
                camera.position.y -= 10;
            }

            if (
                (display_mode.w - mouse.window_current_position.x) < screen_border_px.x &&
                (camera.position.x + display_mode.w) < (tilemap.area.x + (screen_border_px.x * 2))
            ) {
                camera.position.x += 10;
            }

            if (
                (display_mode.h - mouse.window_current_position.y) < screen_border_px.y &&
                (camera.position.y + display_mode.h) < (tilemap.area.y + (screen_border_px.y * 2))
            ) {
                camera.position.y += 10;
            }
        }
};

#endif