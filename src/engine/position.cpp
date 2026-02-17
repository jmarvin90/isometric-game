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

glm::vec2 to_grid_gross(const glm::ivec2 world_position, const TileSpecComponent& tilespec, const TileMapComponent& tilemap)
{
    const glm::ivec2 world_pos_adjusted { world_position - tilespec.centre };
    const glm::ivec2 centred_world_pos { world_pos_adjusted - tilemap.origin_px };
    return glm::vec2 { tilespec.matrix_inverted * centred_world_pos };
}

namespace Position {

glm::ivec2 to_world_position(const glm::ivec2 grid_position, const TileSpecComponent& tilespec, const TileMapComponent& tilemap)
{
    const glm::ivec2 movement { tilespec.matrix * grid_position };
    const glm::ivec2 world_pos_gross { (movement + tilemap.origin_px) - tilespec.centre };
    return world_pos_gross + tilespec.centre;
}

glm::ivec2 to_world_position(const glm::ivec2 screen_position, const CameraComponent& camera)
{
    return (screen_position + camera.position()) - constants::SCENE_BORDER_PX;
}

glm::ivec2 to_world_position(const glm::ivec2 spatial_map_cell_position, const SpatialMapComponent& spatial_map)
{
    return spatial_map_cell_position * spatial_map.cell_size;
}

glm::ivec2 to_grid_position(const glm::ivec2 world_position, const TileSpecComponent& tilespec, const TileMapComponent& tilemap)
{
    const glm::vec2 gross_position { to_grid_gross(world_position, tilespec, tilemap) };
    return glm::round(gross_position);
}

glm::ivec2 to_screen_position(const glm::ivec2 world_position, const CameraComponent& camera)
{
    return (world_position - camera.position()) + constants::SCENE_BORDER_PX;
}

} // namespace