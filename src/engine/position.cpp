#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/screen_position_component.h>
#include <components/spatialmap_component.h>
#include <components/tilemap_grid_position_component.h>
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <constants.h>
#include <position.h>
#include <spdlog/spdlog.h>

glm::vec2 to_grid_gross(
    const WorldPosition& position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
)
{
    const glm::ivec2 world_pos_adjusted { position.position - tilespec.centre };
    const glm::ivec2 centred_world_pos { world_pos_adjusted - tilemap.origin_px };
    return glm::vec2 { tilespec.matrix_inverted * centred_world_pos };
}

template <typename T, typename M>
bool generic_valid(const T& position, const M& map)
{
    return (
        _in_min_bounds<glm::ivec2>(position.position) && //
        (
            position.position.x < map.n_per_row && //
            position.position.y < map.n_per_row //
        )
    );
}

/*



*/

namespace Position {

WorldPosition to_world_position(
    const TileMapGridPositionComponent& position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
)
{
    const glm::ivec2 movement { tilespec.matrix * position.position };
    const glm::ivec2 world_pos_gross { (movement + tilemap.origin_px) - tilespec.centre };
    return WorldPosition { world_pos_gross + tilespec.centre };
}

WorldPosition to_world_position(const ScreenPositionComponent& position, const CameraComponent& camera)
{
    return WorldPosition { (position.position + camera.position()) - constants::SCENE_BORDER_PX };
}

WorldPosition to_world_position(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map)
{
    return WorldPosition { position.position * spatial_map.cell_size };
}

// WorldPosition to_world_position(const SpatialMapGridPosition& position, const entt::registry& registry)
// {
//     const SpatialMapComponent& spatial_map { registry.ctx().get<const SpatialMapComponent>() };
//     return to_world_position(position, spatial_map);
// }

/*



*/

TileMapGridPositionComponent to_grid_position(
    const WorldPosition& position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
)
{
    const glm::vec2 gross_position { to_grid_gross(position, tilespec, tilemap) };
    return TileMapGridPositionComponent {
        { std::round(gross_position.x), std::round(gross_position.y) }
    };
}

/*



*/

TileMapGridPositionComponent from_tile_number(const TileMapComponent& tilemap, const int tile_n)
{
    if (tile_n < tilemap.n_per_row) {
        return TileMapGridPositionComponent { { tile_n, 0 } };
    } else {
        return TileMapGridPositionComponent { { tile_n % tilemap.n_per_row, tile_n / tilemap.n_per_row } };
    }
}

/*



*/

ScreenPositionComponent to_screen_position(const WorldPosition& position, const CameraComponent& camera)
{
    return ScreenPositionComponent { (position.position - camera.position()) + constants::SCENE_BORDER_PX };
}

/*



*/

SpatialMapGridPosition to_spatial_map_grid_position(const WorldPosition& position, const SpatialMapComponent& spatial_map)
{
    assert(spatial_map.cell_size.x > 0 && spatial_map.cell_size.y > 0);
    return SpatialMapGridPosition { position.position / spatial_map.cell_size };
}

/*



*/

int to_spatial_map_cell(const WorldPosition& position, const SpatialMapComponent& spatial_map)
{
    glm::ivec2 cell { position.position / spatial_map.cell_size };
    return (cell.y * spatial_map.n_per_row) + cell.x;
}

int to_spatial_map_cell(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map)
{
    return (position.position.y * spatial_map.n_per_row) + position.position.x;
}

/*



*/

SpatialMapGridPosition from_cell_number(const SpatialMapComponent& spatial_map, const int cell_number)
{
    if (cell_number < spatial_map.n_per_row) {
        return SpatialMapGridPosition { { cell_number, 0 } };
    } else {
        return SpatialMapGridPosition {
            { cell_number % spatial_map.n_per_row, cell_number / spatial_map.n_per_row }
        };
    }
}

/*



*/

bool is_valid(const TileMapGridPositionComponent& position, const TileMapComponent& tilemap)
{
    return generic_valid(position, tilemap);
}

bool is_valid(const WorldPosition& position, const TileMapComponent& tilemap)
{
    return (
        _in_min_bounds<glm::ivec2>(position.position) //
        && (position.position.x < tilemap.area.x && //
            position.position.y < tilemap.area.y //
        )
    );
}

// TODO: basically identical to GridPosition::is_valid();
bool is_valid(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map)
{
    return generic_valid(position, spatial_map);
}

bool is_valid(const ScreenPositionComponent& position, const SDL_DisplayMode& display_mode)
{
    return (
        _in_min_bounds<glm::ivec2>(position.position) && //
        (position.position.x < display_mode.w && position.position.y < display_mode.h)
    );
}

} // namespace