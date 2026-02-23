#ifndef SPATIALMAPCOMPONENT_H
#define SPATIALMAPCOMPONENT_H

#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <grid.h>
#include <projection.h>
#include <unordered_map>

struct SpatialMapComponent {
    int divisor;

    int n_per_row;
    int n_cells;

    glm::ivec2 cell_size;

    std::unordered_map<int, entt::entity> map;

    SpatialMapComponent(
        const TileSpecComponent& tile_spec,
        const Grid<TileMapProjection>& tile_map,
        const int divisor
    )
        : divisor { divisor }
        , n_per_row { tile_map.cells_per_row / divisor }
        , n_cells { n_per_row * n_per_row }
        , cell_size { tile_spec.iso_area * divisor }
    {
        assert((tile_map.cells_per_row * tile_map.rows_per_grid) % divisor == 0);
    }

    entt::entity operator[](const int cell_number) const;
    entt::entity operator[](const glm::ivec2 grid_position) const;
    entt::entity operator[](const TransformComponent& transform) const;
    void emplace_at(const glm::ivec2 grid_position, entt::entity entity);
};

#endif