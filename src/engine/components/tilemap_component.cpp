#include <components/highlight_component.h>
#include <components/navigation_component.h>
#include <components/tilemap_component.h>
#include <components/tilemap_grid_position_component.h>
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <glm/glm.hpp>

#include <entt/entt.hpp>

namespace {
// TODO: same as spatial map equivalent
bool is_valid(const TileMapGridPositionComponent grid_position, const TileMapComponent& tilemap)
{
    return (
        glm::all(glm::greaterThanEqual(grid_position.position, glm::ivec2 { 0, 0 })) && //
        glm::all(glm::lessThan(grid_position.position, glm::ivec2 { tilemap.n_per_row, tilemap.n_per_row }))
    );
}
}

TileMapComponent::TileMapComponent(const TileSpecComponent& tile_spec, const int n_per_row)
    : n_per_row { n_per_row }
    , n_tiles { n_per_row * n_per_row }
    , area { tile_spec.iso_area * n_per_row }
    , origin_px { (area / 2).x - tile_spec.centre.x, 0 }
{
    tiles.reserve(n_tiles);
}

entt::entity TileMapComponent::operator[](const glm::ivec2 grid_position) const
{
    int index_pos { (grid_position.y * n_per_row) + grid_position.x };

    if (
        !is_valid(TileMapGridPositionComponent { grid_position }, *this) || //
        static_cast<int>(tiles.size()) <= index_pos
    ) {
        return entt::null;
    }

    return tiles.at(index_pos);
}