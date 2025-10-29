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
        static void update(entt::registry& registry, const SDL_DisplayMode& display_mode);
};

#endif