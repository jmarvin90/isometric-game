#ifndef TILEMAPSYSTEM_H
#define TILEMAPSYSTEM_H

#include <components/mouse_component.h>
#include <components/transform_component.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <optional>

namespace TileMapSystem {
void emplace_tiles(entt::registry& registry);
void connect(entt::registry& registry, entt::entity entity);
void disconnect(entt::registry& registry, entt::entity entity);
};

#endif