#ifndef POSITION_H
#define POSITION_H

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Position {

glm::ivec2 grid_to_world(
    const glm::ivec2 grid_position,
    const glm::ivec2 tile_centre,
    const glm::ivec2 tilemap_origin,
    const glm::mat2 conversion_matrix
);

glm::ivec2 screen_to_world(
    const glm::ivec2 screen_position,
    const glm::ivec2 camera_position
);

glm::ivec2 spatial_map_to_world(
    const glm::ivec2 spatial_map_cell_position,
    const glm::ivec2 spatial_map_cell_size
);

glm::ivec2 world_to_grid(
    const glm::ivec2 world_position,
    const glm::ivec2 tile_centre,
    const glm::ivec2 tilemap_origin,
    const glm::mat2 inverted_matrix
);

glm::ivec2 world_to_screen(
    const glm::ivec2 world_position,
    const glm::ivec2 camera_position
);

glm::ivec2 world_to_spatial_map(
    const glm::ivec2 world_position,
    const glm::ivec2 spatial_map_cell_size
);

}

#endif