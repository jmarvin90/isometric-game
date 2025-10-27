#include <position.h>

#include <components/camera_component.h>
#include <components/tilespec_component.h>
#include <components/tilemap_component.h>

#include <spdlog/spdlog.h>

const WorldPosition ScreenPosition::to_world_position(entt::registry& registry) const {
    const CameraComponent& camera {registry.ctx().get<const CameraComponent>()};
    
    // Replace with scene description
    const glm::ivec2 scene_border_px {150, 150};
    return WorldPosition{(position + camera.position) - scene_border_px}; 
}

const ScreenPosition WorldPosition::to_screen_position(entt::registry& registry) const {
    const CameraComponent& camera {registry.ctx().get<const CameraComponent>()};

    const glm::ivec2 scene_border_px {150, 150};
    return ScreenPosition{(position - camera.position) + scene_border_px};
}

const glm::vec2 WorldPosition::to_grid_gross(entt::registry& registry) const {
    const TileSpecComponent& tile_spec {registry.ctx().get<TileSpecComponent>()};
    const TileMapComponent& tilemap {registry.ctx().get<TileMapComponent>()};

    const glm::ivec2 world_pos_adjusted {position - tile_spec.centre()};
    const glm::ivec2 centred_world_pos {world_pos_adjusted - tilemap.origin_px(registry)};
    return glm::vec2 {tile_spec.matrix_inverted * centred_world_pos};
}

const GridPosition WorldPosition::to_grid_position(entt::registry& registry) const {
    const glm::vec2 gross_position {to_grid_gross(registry)};
    return GridPosition(
        glm::ivec2{
            std::round(gross_position.x),
            std::round(gross_position.y)
        }
    );
}

GridPosition::GridPosition(entt::registry& registry, const int tile_n) {
    const TileMapComponent& tilemap {registry.ctx().get<TileMapComponent>()};

    if (tile_n < tilemap.tiles_per_row) {
        position = {tile_n, 0};
    } else {
        position = {
            tile_n % tilemap.tiles_per_row,
            tile_n / tilemap.tiles_per_row
        };
    }
}

const WorldPosition GridPosition::to_world_position(entt::registry& registry) const {
    const TileSpecComponent& tilespec {registry.ctx().get<const TileSpecComponent>()};
    const TileMapComponent& tilemap {registry.ctx().get<const TileMapComponent>()};

    const glm::ivec2 movement {tilespec.matrix * position};

    const glm::ivec2 world_pos_gross {
        (movement + tilemap.origin_px(registry)) - 
        tilespec.centre()
    };

    return WorldPosition{world_pos_gross + tilespec.centre()};
}