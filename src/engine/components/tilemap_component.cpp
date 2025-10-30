#include <components/tilemap_component.h> 
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <components/highlight_component.h>
#include <components/navigation_component.h>

#include <spritesheet.h>
#include <position.h>

#include <optional>


TileMapComponent::TileMapComponent(entt::registry& registry, const int tiles_per_row)
: tiles_per_row {tiles_per_row}
, n_tiles {tiles_per_row * tiles_per_row}
, highlighted_tile {std::nullopt}
{
    tiles.reserve(n_tiles);
    const TileSpecComponent& tilespec {registry.ctx().get<TileSpecComponent>()};
    area = tilespec.iso_area * tiles_per_row;
    origin_px = glm::ivec2{(area / 2).x - tilespec.centre.x, 0};
}

std::optional<Tile> TileMapComponent::operator[](const glm::ivec2 grid_position) {
    if (!GridPosition(grid_position).is_valid(*this)) {
        return std::nullopt;
    }
    return tiles.at((grid_position.y * tiles_per_row) + grid_position.x);
}

std::optional<Tile> TileMapComponent::operator[](const glm::ivec2 grid_position) const {
    if (!GridPosition(grid_position).is_valid(*this)) {
        return std::nullopt;
    }
    return tiles.at((grid_position.y * tiles_per_row) + grid_position.x);
}