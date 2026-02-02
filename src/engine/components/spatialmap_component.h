#ifndef SPATIALMAPCOMPONENT_H
#define SPATIALMAPCOMPONENT_H

#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <unordered_map>

struct SpatialMapComponent {
    int divisor;

    int n_per_row;
    int n_cells;

    glm::ivec2 cell_size;

    std::unordered_map<int, entt::entity> map;

    SpatialMapComponent(
        const TileSpecComponent& tile_spec,
        const TileMapComponent& tile_map,
        const int divisor
    )
        : divisor { divisor }
        , n_per_row { tile_map.n_per_row / divisor }
        , n_cells { n_per_row * n_per_row }
        , cell_size { tile_spec.iso_area * divisor }
    {
        assert(tile_map.n_tiles % divisor == 0);
    }

    entt::entity operator[](const int cell_number) const;
    entt::entity operator[](const glm::ivec2 grid_position) const;
};

#endif