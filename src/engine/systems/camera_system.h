#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <spdlog/spdlog.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

class CameraSystem {
public:
    static void update(entt::registry& registry,
        const SDL_DisplayMode& display_mode);
};

#endif