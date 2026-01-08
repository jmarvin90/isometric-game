#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/spatialmap_component.h>
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <constants.h>
#include <position.h>
#include <spdlog/spdlog.h>

bool IPosition::_in_min_bounds() const { return position.x >= 0 && position.y >= 0; }
/*




*/
const WorldPosition ScreenPosition::to_world_position(const entt::registry& registry) const
{
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    return WorldPosition { (position + camera.position()) - constants::SCENE_BORDER_PX };
}

const GridPosition ScreenPosition::to_grid_position(const entt::registry& registry) const
{
    return to_world_position(registry).to_grid_position(registry);
}

bool ScreenPosition::is_valid(const SDL_DisplayMode& display_mode) const
{
    return (_in_min_bounds() && (position.x < display_mode.w && position.y < display_mode.h));
}
/*




*/
const ScreenPosition WorldPosition::to_screen_position(const CameraComponent& camera) const
{
    return ScreenPosition { (position - camera.position()) + constants::SCENE_BORDER_PX };
}

const ScreenPosition WorldPosition::to_screen_position(const entt::registry& registry) const
{
    return to_screen_position(registry.ctx().get<const CameraComponent>());
}

const glm::vec2 WorldPosition::to_grid_gross(const entt::registry& registry) const
{
    const TileSpecComponent& tile_spec { registry.ctx().get<TileSpecComponent>() };
    const TileMapComponent& tilemap { registry.ctx().get<TileMapComponent>() };
    const glm::ivec2 world_pos_adjusted { position - tile_spec.centre };
    const glm::ivec2 centred_world_pos { world_pos_adjusted - tilemap.origin_px };
    return glm::vec2 { tile_spec.matrix_inverted * centred_world_pos };
}

const GridPosition WorldPosition::to_grid_position(const entt::registry& registry) const
{
    const glm::vec2 gross_position { to_grid_gross(registry) };
    return GridPosition(glm::ivec2 { std::round(gross_position.x), std::round(gross_position.y) });
}
/*




*/
// TODO: check this works as intended
int WorldPosition::to_spatial_map_cell(const SpatialMapComponent& spatial_map) const
{
    glm::ivec2 cell { position / spatial_map.cell_size };
    return (cell.y * spatial_map.cells_per_row) + cell.x;
}

int WorldPosition::to_spatial_map_cell(const entt::registry& registry) const
{
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };
    return to_spatial_map_cell(spatial_map);
}

WorldPosition::WorldPosition(const entt::registry& registry, const entt::entity entity)
    : IPosition(registry.try_get<const TransformComponent>(entity)->position)
{
}

bool WorldPosition::is_valid(const TileMapComponent& tilemap) const
{
    return (_in_min_bounds() && (position.x < tilemap.area.x && position.y < tilemap.area.y));
}

bool WorldPosition::is_valid(const entt::registry& registry) const
{
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    return is_valid(tilemap);
}
/*




*/
GridPosition::GridPosition(const entt::registry& registry, const int tile_n)
{
    const TileMapComponent& tilemap { registry.ctx().get<TileMapComponent>() };
    if (tile_n < tilemap.tiles_per_row) {
        position = { tile_n, 0 };
    } else {
        position = { tile_n % tilemap.tiles_per_row, tile_n / tilemap.tiles_per_row };
    }
}

const WorldPosition GridPosition::to_world_position(const entt::registry& registry) const
{
    const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    const glm::ivec2 movement { tilespec.matrix * position };
    const glm::ivec2 world_pos_gross { (movement + tilemap.origin_px) - tilespec.centre };
    return WorldPosition { world_pos_gross + tilespec.centre };
}

const SpatialMapGridPosition GridPosition::to_spatial_map_position(const SpatialMapComponent& spatial_map) const
{
    glm::ivec2 output {
        position.x > spatial_map.divisor ? position.x / spatial_map.divisor : 0,
        position.y > spatial_map.divisor ? position.y / spatial_map.divisor : 0
    };
    return SpatialMapGridPosition(output);
}

const SpatialMapGridPosition GridPosition::to_spatial_map_position(const entt::registry& registry) const
{
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };
    return to_spatial_map_position(spatial_map);
}

bool GridPosition::is_valid(const TileMapComponent& tilemap) const
{
    return (_in_min_bounds() && (position.x < tilemap.tiles_per_row && position.y < tilemap.tiles_per_row));
}

bool GridPosition::is_valid(const entt::registry& registry) const
{
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    return is_valid(tilemap);
}
/*




*/
SpatialMapGridPosition SpatialMapGridPosition::from_cell_number(
    const SpatialMapComponent& spatial_map, const int cell_number
)
{
    if (cell_number < spatial_map.cells_per_row) {
        return SpatialMapGridPosition({ cell_number, 0 });
    } else {
        return SpatialMapGridPosition(
            { cell_number % spatial_map.cells_per_row, cell_number / spatial_map.cells_per_row }
        );
    }
}

SpatialMapGridPosition SpatialMapGridPosition::from_cell_number(const entt::registry& registry, const int cell_number)
{
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };
    return SpatialMapGridPosition::from_cell_number(spatial_map, cell_number);
}

int SpatialMapGridPosition::to_spatial_map_cell(const entt::registry& registry) const
{
    // TODO - feels dubious converting to world position then back to spatial map cell
    return to_world_position(registry).to_spatial_map_cell(registry);
}

WorldPosition SpatialMapGridPosition::to_world_position(const SpatialMapComponent& spatial_map) const
{
    return WorldPosition(position * spatial_map.cell_size);
}

WorldPosition SpatialMapGridPosition::to_world_position(const entt::registry& registry) const
{
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };
    return WorldPosition(position * spatial_map.cell_size);
}

// TODO: basically identical to GridPosition::is_valid();
bool SpatialMapGridPosition::is_valid(const SpatialMapComponent& spatial_map) const
{
    return (_in_min_bounds() && (position.x < spatial_map.cells_per_row && position.y < spatial_map.cells_per_row));
}

bool SpatialMapGridPosition::is_valid(const entt::registry& registry) const
{
    const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent&>() };
    return is_valid(spatial_map);
};
