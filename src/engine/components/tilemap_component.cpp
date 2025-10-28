#include <components/tilemap_component.h> 
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <components/highlight_component.h>

#include <spritesheet.h>
#include <position.h>

std::vector<entt::entity> Tile::entities() const {
    if (building_entity) {
        return std::vector<entt::entity>{tile_entity, building_entity.value()};
    } else {
        return std::vector<entt::entity>{tile_entity};
    }
} 

TileMapComponent::TileMapComponent(entt::registry& registry, const int tiles_per_row)
: tiles_per_row {tiles_per_row}
, n_tiles {tiles_per_row * tiles_per_row}
, highlighted_tile {nullptr}
{
    tiles.reserve(n_tiles);
    const TileSpecComponent& tilespec {registry.ctx().get<TileSpecComponent>()};
    area = tilespec.iso_area * tiles_per_row;
    origin_px = glm::ivec2{(area / 2).x - tilespec.centre.x, 0};
}

Tile* TileMapComponent::operator[](const glm::ivec2 grid_position) {
    if (!GridPosition(grid_position).is_valid(*this)) {
        return nullptr;
    }
    return &tiles.at((grid_position.y * tiles_per_row) + grid_position.x);
}

void TileMapComponent::emplace_tiles(entt::registry& registry) {
    const TileSpecComponent& tilespec {registry.ctx().get<const TileSpecComponent>()};
    const SpriteSheet& spritesheet {registry.ctx().get<const SpriteSheet&>()};

    for (int i=0; i<n_tiles; i++) {
        Tile& tile {tiles.emplace_back(registry)};

        const GridPosition grid_position {registry, i};
        const glm::ivec2 world_position {grid_position.to_world_position(registry)};
        
        registry.emplace<TransformComponent>(tile.tile_entity, world_position, 0, 0.0);
        registry.emplace<HighlightComponent>(tile.tile_entity, SDL_Color{0, 0, 255, 255}, tilespec.iso_points());
        // registry.emplace<TileComponent>(tile.tile_entity);

        if (glm::ivec2(grid_position).y == 1) {
            registry.emplace<SpriteComponent>(tile.tile_entity, spritesheet.get("grass_ew"));
        } else {
            registry.emplace<SpriteComponent>(tile.tile_entity, spritesheet.get("grass"));
        }
    }
}