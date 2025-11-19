#include <components/grid_position_component.h>
#include <components/highlight_component.h>
#include <components/navigation_component.h>
#include <components/sprite_component.h>
#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <directions.h>
#include <position.h>
#include <spdlog/spdlog.h>
#include <spritesheet.h>
#include <systems/tilemap_system.h>

#include <array>
#include <optional>

namespace {

    struct TileScanIterator {
        entt::registry& registry;
        TileMapComponent& tilemap;
        glm::ivec2 pos;
        Direction::TDirection direction;
        std::optional<entt::entity> current;

        TileScanIterator() = delete;
        TileScanIterator(entt::registry& registry,
            glm::ivec2 pos,
            Direction::TDirection direction)
            : registry { registry }
            , tilemap { registry.ctx().get<TileMapComponent>() }
            , pos { pos }
            , direction { direction }
            , current { tilemap[pos] }
        {
        }

        entt::entity operator*() { return current.value(); };
        // const Tile* operator->() const { return &current->value(); }

        TileScanIterator& operator++()
        {
            pos += Direction::directions[Direction::index_position(direction)].vec;
            current = tilemap[pos];
            return *this;
        }

        TileScanIterator& operator--()
        {
            Direction::TDirection reversed { Direction::reverse_direction(direction) };
            pos += Direction::directions[Direction::index_position(reversed)].vec;
            current = tilemap[pos];
            return *this;
        }

        bool operator!=(std::nullopt_t) const { return current.has_value(); }
    };

    struct TileScan {
        entt::registry& registry;
        glm::ivec2 start;
        Direction::TDirection direction;

        TileScan() = delete;
        TileScan(entt::registry& registry,
            const glm::ivec2 start,
            const Direction::TDirection direction)
            : registry { registry }
            , start { start }
            , direction { direction }
        {
        }

        TileScanIterator begin()
        {
            return TileScanIterator(registry, start, direction);
        }
        std::nullopt_t end() { return std::nullopt; }
    };

    void apply_highlight(entt::registry& registry,
        const entt::entity tile,
        int factor = 1)
    {
        TransformComponent& transform { registry.get<TransformComponent>(tile) };
        transform.position.y -= (30 * factor);
        transform.z_index += factor;
    }

    [[maybe_unused]] std::optional<entt::entity> scan(entt::registry& registry,
        glm::ivec2 from_position,
        Direction::TDirection direction)
    {
        const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
        const Direction::TDirection reverse { Direction::reverse_direction(direction) };

        std::optional<entt::entity> current_tile { tilemap[from_position] };

        if (!current_tile)
            return std::nullopt;

        NavigationComponent current_nav { registry.get<const NavigationComponent>(current_tile.value()) };

        for ([[maybe_unused]] entt::entity tile : TileScan(registry, from_position, direction)) {
            if (tile == current_tile)
                continue;

            NavigationComponent next_nav { registry.get<const NavigationComponent>(tile) };

            [[maybe_unused]] const GridPositionComponent next_pos { registry.get<const GridPositionComponent>(tile) };

            bool curr_can_connect_forward { Direction::any(current_nav.directions & direction) };
            bool next_can_connect_back { Direction::any(next_nav.directions & reverse) };

            if (!curr_can_connect_forward || !next_can_connect_back) {
                return current_tile;
            }

            current_nav = next_nav;
            current_tile = tile;

            [[maybe_unused]] bool is_junction { Direction::is_junction(next_nav.directions) };
            if (is_junction) {
                return current_tile;
            }
        }

        return current_tile;
    }

} // namespace

void TileMapSystem::update(entt::registry& registry, const bool debug_mode)
{
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
        ScreenPosition(mouse.window_current_position).to_grid_position(registry)
    };

    std::optional<entt::entity> new_tile = tilemap[grid_position];

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

void TileMapSystem::emplace_tiles(entt::registry& registry)
{
    TileMapComponent& tilemap { registry.ctx().get<TileMapComponent>() };
    const TileSpecComponent& tilespec {
        registry.ctx().get<const TileSpecComponent>()
    };
    const SpriteSheet& spritesheet { registry.ctx().get<const SpriteSheet&>() };

    for (int i = 0; i < tilemap.n_tiles; i++) {
        entt::entity tile { tilemap.tiles.emplace_back(registry.create()) };

        const GridPosition grid_position { registry, i };
        const glm::ivec2 world_position { grid_position.to_world_position(registry) };

        registry.emplace<TransformComponent>(tile, world_position, 0, 0.0);

        registry.emplace<HighlightComponent>(tile, SDL_Color { 0, 0, 255, 255 }, tilespec.iso_points());

        registry.emplace<GridPositionComponent>(tile, grid_position);

        std::string tile_handle {};

        std::vector<glm::ivec2> ew_positions {
            { 1, 1 },
            { 2, 1 },
            { 3, 1 },
            { 5, 1 },
            { 6, 1 },
            { 7, 1 }
        };

        std::vector<glm::ivec2> nesw_positions {
            { 4, 1 }
        };

        std::vector<glm::ivec2> ns_positions {
            { 4, 2 },
            { 4, 3 },
            { 4, 4 },
            { 4, 5 },
            { 4, 6 },
            { 4, 7 }
        };

        glm::ivec2 vec_position { grid_position };

        if (
            (
                (vec_position.x >= 0 && vec_position.x <= 3) || (vec_position.x > 4 && vec_position.x <= 7))
            && vec_position.y == 1) {
            tile_handle = "grass_ew";
        } else if (vec_position == glm::ivec2 { 4, 1 }) {
            tile_handle = "gass_nesw";
        } else if (
            vec_position.x == 4 && (vec_position.y >= 2 && vec_position.y <= 7)) {
            tile_handle = "grass_ns";
        } else {
            tile_handle = "grass";
        }

        registry.emplace<SpriteComponent>(tile, spritesheet.get(tile_handle).first);
        registry.emplace<NavigationComponent>(tile, spritesheet.get(tile_handle).second);
    }
}