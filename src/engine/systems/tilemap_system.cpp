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
#include <spdlog/spdlog.h>

namespace {

    struct TileScanIterator {
        const entt::registry& registry;
        glm::ivec2 pos;
        Direction::TDirection direction;
        std::optional<Tile> current;
        
        TileScanIterator() = delete;
        TileScanIterator(
            const entt::registry& registry,
            glm::ivec2 pos,
            Direction::TDirection direction
        )
        : registry {registry}
        , pos {pos}
        , direction {direction}
        , current {registry.ctx().get<TileMapComponent>()[pos]}
        {}

        Tile* operator*() { return &current.value(); };
        // const Tile* operator->() const { return &current->value(); }

        TileScanIterator& operator++() {
            pos += Direction::directions[int(direction)].vec;
            current = registry.ctx().get<TileMapComponent>()[pos];
            return *this;
        }

        TileScanIterator& operator--() {
            Direction::TDirection reversed {Direction::reverse_direction(direction)};
            pos += Direction::directions[int(reversed)].vec;
            current = registry.ctx().get<TileMapComponent>()[pos];
            return *this;
        }

        bool operator!=(std::nullopt_t) const {
            return current.has_value();
        }
    };

    struct TileScan {
        const entt::registry& registry;
        glm::ivec2 start;
        Direction::TDirection direction;

        TileScan() = delete;
        TileScan(
            const entt::registry& registry,
            const glm::ivec2 start,
            const Direction::TDirection direction
        )
        : registry {registry}
        , start {start}
        , direction {direction}
        {}

        TileScanIterator begin() { return TileScanIterator(registry, start, direction);}
        std::nullopt_t end() { return std::nullopt; }
    };

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

    [[maybe_unused]] const std::array<std::optional<Tile>, 4> neighbours(
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

    [[maybe_unused]] std::optional<Tile> scan(
        entt::registry& registry,
        glm::ivec2 from_position,
        Direction::TDirection direction
    ) {
        const TileMapComponent& tilemap {registry.ctx().get<const TileMapComponent>()};

        std::optional<Tile> current_tile {tilemap[from_position]}; 
        if (!current_tile) return std::nullopt;

        NavigationComponent* current_nav {
            registry.try_get<NavigationComponent>(current_tile->tile_entity)
        };

        const Direction::TDirection reverse{Direction::reverse_direction(direction)};

        for ([[maybe_unused]] Tile* tile: TileScan(registry, from_position, direction)) {
            NavigationComponent* next_nav {
                registry.try_get<NavigationComponent>(tile->tile_entity)
            };

            bool can_connect_forward {Direction::any(current_nav->directions & direction)};
            bool can_connect_back {Direction::any(next_nav->directions & reverse)};
            bool is_junction {__builtin_popcount(Direction::to_underlying(current_nav->directions)) > 2};

            if (!can_connect_forward || !can_connect_back || is_junction) {
                return current_tile;
            }

            current_nav = next_nav;
            current_tile = *tile;
        }

        return current_tile;
    }
}

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