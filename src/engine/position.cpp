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
    const glm::ivec2 world_position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
)
{
    const glm::ivec2 world_pos_adjusted { world_position - tilespec.centre };
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

glm::ivec2 to_world_position(
    const TileMapGridPositionComponent& position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
)
{
    const glm::ivec2 movement { tilespec.matrix * position.position };
    const glm::ivec2 world_pos_gross { (movement + tilemap.origin_px) - tilespec.centre };
    return world_pos_gross + tilespec.centre;
}

glm::ivec2 to_world_position(const ScreenPositionComponent& position, const CameraComponent& camera)
{
    return (position.position + camera.position()) - constants::SCENE_BORDER_PX;
}

glm::ivec2 to_world_position(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map)
{
    return position.position * spatial_map.cell_size;
}

TileMapGridPositionComponent to_grid_position(
    const glm::ivec2 world_position,
    const TileSpecComponent& tilespec,
    const TileMapComponent& tilemap
)
{
    const glm::vec2 gross_position { to_grid_gross(world_position, tilespec, tilemap) };
    return TileMapGridPositionComponent {
        { std::round(gross_position.x), std::round(gross_position.y) }
    };
}

/*



*/

ScreenPositionComponent to_screen_position(const glm::ivec2 world_position, const CameraComponent& camera)
{
    return ScreenPositionComponent { (world_position - camera.position()) + constants::SCENE_BORDER_PX };
}

/*



*/

// int to_spatial_map_cell(const WorldPosition& position, const SpatialMapComponent& spatial_map)
// {
//     glm::ivec2 cell { position.position / spatial_map.cell_size };
//     return (cell.y * spatial_map.n_per_row) + cell.x;
// }

// int to_spatial_map_cell(const SpatialMapGridPosition& position, const SpatialMapComponent& spatial_map)
// {
//     return (position.position.y * spatial_map.n_per_row) + position.position.x;
// }

/*



*/

// SpatialMapGridPosition from_cell_number(const SpatialMapComponent& spatial_map, const int cell_number)
// {
//     if (cell_number < spatial_map.n_per_row) {
//         return SpatialMapGridPosition { { cell_number, 0 } };
//     } else {
//         return SpatialMapGridPosition {
//             { cell_number % spatial_map.n_per_row, cell_number / spatial_map.n_per_row }
//         };
//     }
// }

/*



*/

bool is_valid(const TileMapGridPositionComponent& position, const TileMapComponent& tilemap)
{
    return generic_valid(position, tilemap);
}

bool is_valid(const glm::ivec2 world_position, const TileMapComponent& tilemap)
{
    return (
        _in_min_bounds<glm::ivec2>(world_position) && glm::all(glm::lessThan(world_position, tilemap.area))
    );
}

bool is_valid(const ScreenPositionComponent& position, const SDL_DisplayMode& display_mode)
{
    return (
        _in_min_bounds<glm::ivec2>(position.position) && //
        glm::all(glm::lessThan(position.position, glm::ivec2 { display_mode.w, display_mode.h }))
    );
}

} // namespace