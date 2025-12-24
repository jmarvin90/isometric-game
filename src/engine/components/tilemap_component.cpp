#include <components/highlight_component.h>
#include <components/navigation_component.h>
#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <position.h>
#include <spritesheet.h>

#include <optional>

#include <entt/entt.hpp>

TileMapComponent::TileMapComponent(entt::registry& registry, const int tiles_per_row)
    : tiles_per_row { tiles_per_row }
    , n_tiles { tiles_per_row * tiles_per_row }
    , highlighted_tile { entt::null }
{
    tiles.reserve(n_tiles);

    const TileSpecComponent& tilespec { registry.ctx().get<TileSpecComponent>() };
    area = tilespec.iso_area * tiles_per_row;
    origin_px = glm::ivec2 { (area / 2).x - tilespec.centre.x, 0 };
}

entt::entity TileMapComponent::operator[](const glm::ivec2 grid_position) const
{
    int index_pos { (grid_position.y * tiles_per_row) + grid_position.x };

    if (!GridPosition(grid_position).is_valid(*this) || static_cast<int>(tiles.size()) <= index_pos) {
        return entt::null;
    }

    return tiles.at(index_pos);
}