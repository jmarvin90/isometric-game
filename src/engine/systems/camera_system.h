#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <components/camera_component.h>
#include <components/mouse_component.h>

#include <entt/entt.hpp>
#include <SDL2/SDL.h>

class CameraSystem {
    public:
        static void update(entt::registry& registry) {
            const MouseComponent& mouse {registry.ctx().get<const MouseComponent>()};
            CameraComponent& camera {registry.ctx().get<CameraComponent>()};
        }
};

#endif