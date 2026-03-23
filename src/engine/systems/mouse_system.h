#ifndef MOUSESYSTEM_H
#define MOUSESYSTEM_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <spdlog/spdlog.h>

#include <entt/entt.hpp>

namespace MouseSystem {
void update(entt::registry& registry);
void highlight(entt::registry& registry, entt::entity entity);
void remove_highlight(entt::registry& registry, entt::entity entity);
};

#endif