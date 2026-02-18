#ifndef MOUSESYSTEM_H
#define MOUSESYSTEM_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <spdlog/spdlog.h>

#include <entt/entt.hpp>

class MouseSystem {
public:
    static void update(entt::registry& registry);
};

#endif