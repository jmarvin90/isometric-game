#ifndef TWODGRID_H
#define TWODGRID_H

#include <components/grid_position_component.h>
#include <components/transform_component.h>

#include <entt/entt.hpp>/
#include <glm/glm.hpp>
#include <vector>

namespace {
template <typename Grid>
bool index_is_valid(const int index, const Grid& grid)
{
    return index >= 0 && index < int(grid.cells.size());
}

template <typename Grid>
bool position_is_valid(const glm::ivec2 grid_position, const Grid& grid)
{
    bool greater_than_zero { glm::all(glm::greaterThanEqual(grid_position, { 0, 0 })) };
    bool in_bounds { glm::all(glm::lessThan(grid_position, { grid.cells_per_row, grid.rows_per_grid })) };
    return greater_than_zero && in_bounds;
}

template <typename Grid>
int grid_position_to_index(const glm::ivec2 grid_position, const Grid& grid)
{
    return (grid_position.y * grid.cells_per_row) + grid_position.x;
}

template <typename Grid>
glm::ivec2 index_to_grid_position(const int index_position, const Grid& grid)
{
    if (index_position < grid.cells_per_row)
        return { index_position, 0 };

    glm::ivec2 output {
        index_position % grid.cells_per_row, //
        index_position / grid.cells_per_row
    };

    return output;
}

}

template <typename Projection>
class Grid {
private:
    entt::registry& registry;

public:
    std::vector<entt::entity> cells;
    glm::ivec2 cell_size;
    const int cells_per_row;
    const int rows_per_grid;
    glm::ivec2 area;

    Grid(entt::registry& registry, glm::ivec2 cell_size, int cells_per_row, int rows_per_grid)
        : registry { registry }
        , cell_size { cell_size }
        , cells_per_row { cells_per_row }
        , rows_per_grid { rows_per_grid }
        , area { cell_size * glm::ivec2 { cells_per_row, rows_per_grid } }
    {
        int total_cells { cells_per_row * rows_per_grid };
        cells.reserve(total_cells);
        for (int i = 0; i < total_cells; i++) {
            glm::ivec2 grid_position { index_to_grid_position(i, *this) };
            entt::entity cell { registry.create() };
            cells.push_back(cell);
            registry.emplace<GridPositionComponent>(cell, grid_position);
            registry.emplace<TransformComponent>(cell, Projection::grid_to_world(grid_position, *this), 0, 0.0);
        }
    }

    // TODO: understand why this causes 'entity not in set' errors
    ~Grid()
    {
        // for (auto cell : cells) {
        //     registry.destroy(cell);
        // }
    }

    // TODO - superfluous?
    entt::entity operator[](const int index_position) const
    {
        if (index_is_valid(index_position, *this))
            return entt::null;

        return cells.at(index_position);
    }

    entt::entity operator[](const glm::ivec2 grid_position) const
    {
        if (!position_is_valid(grid_position, *this))
            return entt::null;

        return cells.at(grid_position_to_index(grid_position, *this));
    }

    entt::entity at_world(const glm::ivec2 world_position) const
    {
        return (*this)[Projection::world_to_grid(world_position, *this)];
    }
};

#endif