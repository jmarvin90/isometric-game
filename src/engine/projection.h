#ifndef PROJECTION_H
#define PROJECTION_H

#include <glm/glm.hpp>

namespace {
glm::mat2 projection_matrix(const glm::ivec2 cell_size)
{
    return glm::mat2 {
        cell_size.x / 2.0f,
        cell_size.y / 2.0f,
        -cell_size.x / 2.0f,
        cell_size.y / 2.0f
    };
}
}

struct TileMapProjection {
    template <typename Grid>
    static glm::ivec2 grid_to_world(const glm::ivec2 grid_position, const Grid& grid)
    {
        int grid_width { grid.cell_size.x * grid.cells_per_row };
        glm::ivec2 tilemap_origin { (grid_width / 2) - (grid.cell_size.x / 2), 0 };
        glm::ivec2 movement { projection_matrix(grid.cell_size) * grid_position };
        return movement + tilemap_origin;
    }

    template <typename Grid>
    static glm::ivec2 world_to_grid(const glm::ivec2 world_position, const Grid& grid)
    {
        int grid_width { grid.cell_size.x * grid.cells_per_row };
        glm::ivec2 tile_centre { grid.cell_size / 2 };
        glm::ivec2 tilemap_origin { (grid_width / 2) - (grid.cell_size.x / 2), 0 };
        glm::ivec2 world_pos_adjusted { world_position - tile_centre };
        glm::ivec2 centred_world_pos { world_pos_adjusted - tilemap_origin };
        return glm::round(glm::inverse(projection_matrix(grid.cell_size)) * centred_world_pos);
    }
};

struct SpatialMapProjection {
    template <typename Grid>
    static glm::ivec2 grid_to_world(const glm::ivec2 grid_position, const Grid& grid)
    {
        return grid_position * grid.cell_size;
    }

    template <typename Grid>
    static glm::ivec2 world_to_grid(const glm::ivec2 world_position, const Grid& grid)
    {
        return world_position / grid.cell_size;
    }
};

#endif