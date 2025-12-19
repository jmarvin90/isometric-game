#ifndef TILEMAPSYSTEM_H
#define TILEMAPSYSTEM_H

#include <components/mouse_component.h>
#include <components/tilemap_component.h>
#include <components/transform_component.h>
#include <position.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <optional>

class TileMapSystem {
public:
    static void update(entt::registry& registry, const bool debug_mode);
    static void emplace_tiles(entt::registry& registry);
    static void connect(entt::registry& registry, entt::entity entity);
    static void disconnect(entt::registry& registry, entt::entity entity);
};

#endif