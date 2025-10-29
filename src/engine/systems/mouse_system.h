#ifndef MOUSESYSTEM_H
#define MOUSESYSTEM_H

#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <position.h>

#include <entt/entt.hpp>
#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>


class MouseSystem {
    public:
        static void update(entt::registry& registry);
};

#endif