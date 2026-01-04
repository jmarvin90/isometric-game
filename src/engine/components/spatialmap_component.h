#ifndef SPATIALMAPCOMPONENT_H
#define SPATIALMAPCOMPONENT_H

#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <unordered_map>

struct SpatialMapComponent {
    int cell_tile_width;
    glm::ivec2 cell_size;
    int cells_per_row;
    int n_rows;
    std::unordered_map<int, entt::entity> map;

    SpatialMapComponent(const entt::registry& registry, int cell_tile_width)
        : cell_tile_width { cell_tile_width }
        , cell_size { registry.ctx().get<const TileSpecComponent>().iso_area * cell_tile_width }
    {
        const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
        cells_per_row = tilemap.area.x / cell_size.x;
        n_rows = tilemap.area.y / cell_size.y;
    }

    entt::entity operator[](const int cell_number) const;
};

#endif