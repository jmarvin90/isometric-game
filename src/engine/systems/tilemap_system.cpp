#include <systems/tilemap_system.h>

#include <spritesheet.h>
#include <position.h>

#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <components/highlight_component.h>
#include <components/navigation_component.h>
#include <components/sprite_component.h>
#include <components/grid_position_component.h>
#include <directions.h>

#include <optional>

namespace {
    std::vector<entt::entity> get_tile_entities(const Tile tile) {
        if (tile.building_entity) {
            return std::vector<entt::entity>{
                tile.tile_entity,
                tile.building_entity.value()
            };
        }
        return std::vector<entt::entity>{tile.tile_entity};
    }

    void apply_highlight(entt::registry& registry, const Tile tile, int factor=1) {
        for (entt::entity entity: get_tile_entities(tile)) {
            TransformComponent& transform {registry.get<TransformComponent>(entity)};
            transform.position.y -= (30 * factor);
            transform.z_index += factor;
        }
    }
}

namespace TileMap {
    const std::array<std::optional<Tile>, 4> neighbours(
        const TileMapComponent& tilemap,
        const glm::ivec2 grid_position
    ) {
        std::array<std::optional<Tile>, 4> output {};
        output.fill(std::nullopt);

        for (Direction::DirectionInfo direction: Direction::directions) {
            GridPosition proposed {grid_position + direction.vec};
            if (tilemap[proposed]) {
                output[static_cast<int>(direction.direction)] = tilemap[proposed].value();
            }
        }
        return output;
    }
};

void TileMapSystem::update(entt::registry& registry, const bool debug_mode) {
    auto& tilemap = registry.ctx().get<TileMapComponent>();
    const auto& mouse = registry.ctx().get<const MouseComponent>();

    if (!debug_mode) {
        if (tilemap.highlighted_tile) {
            apply_highlight(registry, tilemap.highlighted_tile.value(), -1);
            tilemap.highlighted_tile = std::nullopt;
        }
        return;
    }

    if (!mouse.moved) {
        return;
    }

    const GridPosition grid_position {
        ScreenPosition(mouse.window_current_position)
        .to_grid_position(registry)
    };

    std::optional<Tile> new_tile = tilemap[grid_position];

    if (new_tile == tilemap.highlighted_tile) {
        return;
    }

    if (tilemap.highlighted_tile) {
        apply_highlight(registry, tilemap.highlighted_tile.value(), -1);
    }

    if (new_tile) {
        apply_highlight(registry, new_tile.value());
    }

    tilemap.highlighted_tile = new_tile;
}

void TileMapSystem::emplace_tiles(entt::registry& registry) {
    TileMapComponent& tilemap {registry.ctx().get<TileMapComponent>()};
    const TileSpecComponent& tilespec {registry.ctx().get<const TileSpecComponent>()};
    const SpriteSheet& spritesheet {registry.ctx().get<const SpriteSheet&>()};

    for (int i=0; i<tilemap.n_tiles; i++) {
        Tile& tile {tilemap.tiles.emplace_back(registry)};

        const GridPosition grid_position {registry, i};
        const glm::ivec2 world_position {grid_position.to_world_position(registry)};
        
        registry.emplace<TransformComponent>(tile.tile_entity, world_position, 0, 0.0);
        
        registry.emplace<HighlightComponent>(
            tile.tile_entity,
            SDL_Color{0, 0, 255, 255},
            tilespec.iso_points()
        );
        
        registry.emplace<GridPositionComponent>(tile.tile_entity, grid_position);

        std::string tile_handle {};

        if (glm::ivec2(grid_position).y == 1) {
            tile_handle = "grass_ew";
        } else {
            tile_handle = "grass";
        }

        registry.emplace<SpriteComponent>(
            tile.tile_entity,
            spritesheet.get(tile_handle).first
        );
        registry.emplace<NavigationComponent>(
            tile.tile_entity,
            spritesheet.get(tile_handle).second
        );
    }
}