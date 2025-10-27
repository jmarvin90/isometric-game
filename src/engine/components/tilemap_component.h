#ifndef TILEMAPCOMPONENT_H
#define TILEMAPCOMPONENT_h

#include <vector>
#include <entt/entt.hpp>
#include <optional>
#include <spdlog/spdlog.h>

#include <components/sprite_component.h>
#include <components/tile_component.h>
#include <components/transform_component.h>
#include <components/tilespec_component.h>
#include <components/highlight_component.h>
#include <components/tile_component.h>

#include <position.h>
#include <spritesheet.h>

struct Tile {
    entt::entity tile_entity;
    std::optional<entt::entity> building_entity;
    Tile(entt::registry& registry)
    : tile_entity {registry.create()}
    , building_entity {std::nullopt}
    {}

    Tile(const Tile&) = delete;
    Tile(Tile&&) = default;
    ~Tile() = default;
};


struct TileMapComponent {

    const int tiles_per_row;
    const int n_tiles;
    std::vector<Tile> tiles;

    TileMapComponent(const TileMapComponent&) = delete;
    TileMapComponent(TileMapComponent&&) = default;
    TileMapComponent(entt::registry& registry, const int tiles_per_row)
    : tiles_per_row {tiles_per_row}
    , n_tiles {tiles_per_row * tiles_per_row} 
    {
        tiles.reserve(n_tiles);
    }

    const glm::ivec2 area(entt::registry& registry) const {
        const TileSpecComponent& tilespec {registry.ctx().get<TileSpecComponent>()};
        return tilespec.iso_area * tiles_per_row;
    }

    const glm::ivec2 origin_px(entt::registry& registry) const {
        const TileSpecComponent& tilespec {registry.ctx().get<TileSpecComponent>()};
        const glm::ivec2 midline {area(registry) / 2};
        return {midline.x - tilespec.centre().x, 0}; 
    }

    void emplace_tiles(entt::registry& registry) {
        const TileSpecComponent& tilespec {registry.ctx().get<const TileSpecComponent>()};
        const SpriteSheet& spritesheet {registry.ctx().get<const SpriteSheet&>()};

        for (int i=0; i<n_tiles; i++) {
            Tile& tile {tiles.emplace_back(registry)};

            const GridPosition grid_position {registry, i};
            const glm::ivec2 world_position {grid_position.to_world_position(registry)};

            spdlog::info(
                std::to_string(world_position.x) + "," + std::to_string(world_position.y)
            );
            
            registry.emplace<TransformComponent>(tile.tile_entity, world_position, 0, 0.0);
            registry.emplace<HighlightComponent>(tile.tile_entity, SDL_Color{0, 0, 255, 255}, tilespec.iso_points());
            registry.emplace<TileComponent>(tile.tile_entity);

            if (glm::ivec2(grid_position).y == 1) {
                registry.emplace<SpriteComponent>(tile.tile_entity, spritesheet.get("grass_ew"));
            } else {
                registry.emplace<SpriteComponent>(tile.tile_entity, spritesheet.get("grass"));
            }
        }
    }
};

#endif