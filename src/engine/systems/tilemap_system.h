#ifndef TILEMAPSYSTEM_H
#define TILEMAPSYSTEM_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include <position.h>
#include <components/transform_component.h>
#include <components/tilemap_component.h>
#include <components/mouse_component.h>

#include <optional>

namespace TileMap {
    const std::array<std::optional<Tile>, 4> neighbours(
        const TileMapComponent& tilemap,
        const glm::ivec2 grid_position
    );
};

class TileMapSystem {
    public:
        static void update(entt::registry& registry, const bool debug_mode);
        static void emplace_tiles(entt::registry& registry);
};

#endif