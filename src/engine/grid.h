#ifndef TWODGRID_H
#define TWODGRID_H

#include <components/grid_position_component.h>
#include <components/transform_component.h>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace {
template <typename Grid>
bool index_is_valid(const int index, const Grid& grid)
{
    return index >= 0 && index < int(grid.cells.size());
}

template <typename Grid>
int grid_position_to_index(const glm::ivec2 grid_position, const Grid& grid)
{
    return (grid_position.y * grid.grid_dimensions.x) + grid_position.x;
}

template <typename Grid>
glm::ivec2 index_to_grid_position(const int index_position, const Grid& grid)
{
    if (index_position < grid.grid_dimensions.x)
        return { index_position, 0 };

    glm::ivec2 output {
        index_position % grid.grid_dimensions.x, //
        index_position / grid.grid_dimensions.x
    };

    return output;
}

}

template <typename StoredType, typename Projection>
class Grid {
private:
    // entt::registry& registry;

public:
    std::vector<StoredType> cells;
    glm::ivec2 cell_size;
    glm::ivec2 grid_dimensions;
    glm::ivec2 area;

    Grid(
        std::vector<StoredType> cells,
        glm::ivec2 cell_size,
        glm::ivec2 grid_dimensions
    )
        : cells { cells }
        , cell_size { cell_size }
        , grid_dimensions { grid_dimensions }
        , area { cell_size * grid_dimensions }
    {
    }

    Grid()
        : cells {}
        , cell_size {}
        , grid_dimensions {}
        , area {}
    {
    }

    ~Grid()
    {
    }

    bool position_is_valid(const glm::ivec2 grid_position) const
    {
        bool greater_than_zero { glm::all(glm::greaterThanEqual(grid_position, { 0, 0 })) };
        bool in_bounds { glm::all(glm::lessThan(grid_position, grid_dimensions)) };
        return greater_than_zero && in_bounds;
    }

    // TODO - superfluous?
    StoredType operator[](const int index_position) const
    {
        assert(index_is_valid(index_position, *this));
        return cells.at(index_position);
    }

    StoredType operator[](const glm::ivec2 grid_position) const
    {
        assert(position_is_valid(grid_position));
        return cells.at(grid_position_to_index(grid_position, *this));
    }

    StoredType at_world(const glm::ivec2 world_position) const
    {
        return (*this)[Projection::world_to_grid(world_position, *this)];
    }

    StoredType at_world(const glm::ivec2 world_position, const glm::ivec2 sprite_position) const
    {
        return (*this)[Projection::world_to_grid(world_position, sprite_position)];
    }
};

#endif