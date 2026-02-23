#include <SDL2/SDL.h>
#include <constants.h>
#include <position.h>

glm::vec2 to_grid_gross(
    const glm::ivec2 world_position,
    const glm::ivec2 tile_centre,
    const glm::ivec2 tilemap_origin,
    const glm::mat2 inverted_matrix
)
{
    const glm::ivec2 world_pos_adjusted { world_position - tile_centre };
    const glm::ivec2 centred_world_pos { world_pos_adjusted - tilemap_origin };
    return glm::vec2 { inverted_matrix * centred_world_pos };
}

namespace Position {

glm::ivec2 grid_to_world(
    const glm::ivec2 grid_position,
    [[maybe_unused]] const glm::ivec2 tile_centre,
    const glm::ivec2 tilemap_origin,
    const glm::mat2 conversion_matrix
)
{
    const glm::ivec2 movement { conversion_matrix * grid_position };
    const glm::ivec2 world_pos_gross { movement + tilemap_origin };
    return world_pos_gross;
}

glm::ivec2 screen_to_world(
    const glm::ivec2 screen_position,
    const glm::ivec2 camera_position
)
{
    return (screen_position + camera_position) - constants::SCENE_BORDER_PX;
}

glm::ivec2 spatial_map_to_world(
    const glm::ivec2 spatial_map_cell_position,
    const glm::ivec2 spatial_map_cell_size
)
{
    return spatial_map_cell_position * spatial_map_cell_size;
}

glm::ivec2 world_to_grid(
    const glm::ivec2 world_position,
    const glm::ivec2 tile_centre,
    const glm::ivec2 tilemap_origin,
    const glm::mat2 inverted_matrix
)
{
    const glm::vec2 gross_position {
        to_grid_gross(
            world_position,
            tile_centre,
            tilemap_origin,
            inverted_matrix
        )
    };

    return glm::round(gross_position);
}

glm::ivec2 world_to_screen(
    const glm::ivec2 world_position,
    const glm::ivec2 camera_position
)
{
    return (world_position - camera_position) + constants::SCENE_BORDER_PX;
}

glm::ivec2 world_to_spatial_map(
    const glm::ivec2 world_position,
    const glm::ivec2 spatial_map_cell_size
)
{
    return world_position / spatial_map_cell_size;
}

} // namespace