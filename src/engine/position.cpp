#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <position.h>
#include <spdlog/spdlog.h>

const WorldPosition ScreenPosition::to_world_position(
    const entt::registry& registry) const
{
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };

    // TODO: Replace with scene description
    const glm::ivec2 scene_border_px { 150, 150 };
    return WorldPosition { (position + camera.position) - scene_border_px };
}

const GridPosition ScreenPosition::to_grid_position(
    const entt::registry& registry) const
{
    return to_world_position(registry).to_grid_position(registry);
}

const ScreenPosition WorldPosition::to_screen_position(
    const entt::registry& registry) const
{
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };

    // TODO: Replace with scene description
    const glm::ivec2 scene_border_px { 150, 150 };
    return ScreenPosition { (position - camera.position) + scene_border_px };
}

const glm::vec2 WorldPosition::to_grid_gross(const entt::registry& registry) const
{
    const TileSpecComponent& tile_spec { registry.ctx().get<TileSpecComponent>() };
    const TileMapComponent& tilemap { registry.ctx().get<TileMapComponent>() };

    const glm::ivec2 world_pos_adjusted { position - tile_spec.centre };
    const glm::ivec2 centred_world_pos { world_pos_adjusted - tilemap.origin_px };
    return glm::vec2 { tile_spec.matrix_inverted * centred_world_pos };
}

const GridPosition WorldPosition::to_grid_position(
    const entt::registry& registry) const
{
    const glm::vec2 gross_position { to_grid_gross(registry) };
    return GridPosition(
        glm::ivec2 { std::round(gross_position.x), std::round(gross_position.y) });
}

GridPosition::GridPosition(const entt::registry& registry, const int tile_n)
{
    const TileMapComponent& tilemap { registry.ctx().get<TileMapComponent>() };

    if (tile_n < tilemap.tiles_per_row) {
        position = { tile_n, 0 };
    } else {
        position = { tile_n % tilemap.tiles_per_row, tile_n / tilemap.tiles_per_row };
    }
}

WorldPosition::WorldPosition(const entt::registry& registry, const entt::entity entity)
    : IPosition(registry.try_get<const TransformComponent>(entity)->position)
{
}

// TODO - why not const
const WorldPosition GridPosition::to_world_position(
    const entt::registry& registry) const
{
    const TileSpecComponent& tilespec {
        registry.ctx().get<const TileSpecComponent>()
    };
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };

    const glm::ivec2 movement { tilespec.matrix * position };

    const glm::ivec2 world_pos_gross { (movement + tilemap.origin_px) - tilespec.centre };

    return WorldPosition { world_pos_gross + tilespec.centre };
}

bool IPosition::_in_min_bounds() const
{
    return position.x >= 0 && position.y >= 0;
}

bool ScreenPosition::is_valid(const SDL_DisplayMode& display_mode) const
{
    return (_in_min_bounds() && (position.x < display_mode.w && position.y < display_mode.h));
}

bool GridPosition::is_valid(const TileMapComponent& tilemap) const
{
    return (_in_min_bounds() && (position.x < tilemap.tiles_per_row && position.y < tilemap.tiles_per_row));
}

bool GridPosition::is_valid(const entt::registry& registry) const
{
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    return (_in_min_bounds() && (position.x < tilemap.tiles_per_row && position.y < tilemap.tiles_per_row));
}

bool WorldPosition::is_valid(const entt::registry& registry) const
{
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    return (_in_min_bounds() && (position.x < tilemap.area.x && position.y < tilemap.area.y));
}