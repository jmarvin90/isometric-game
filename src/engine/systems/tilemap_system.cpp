#include <components/grid_position_component.h>
#include <components/highlight_component.h>
#include <components/junction_component.h>
#include <components/navigation_component.h>
#include <components/sprite_component.h>
#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <directions.h>
#include <position.h>
#include <spdlog/spdlog.h>
#include <spritesheet.h>
#include <systems/tilemap_system.h>

#include <algorithm>
#include <array>
#include <optional>

namespace {

    struct TileScanIterator {
        entt::registry& registry;
        TileMapComponent& tilemap;
        glm::ivec2 start_pos;
        glm::ivec2 pos;
        Direction::TDirection direction;
        Direction::TDirection reverse_direction;
        glm::ivec2 direction_vec;
        std::optional<entt::entity> current;

        TileScanIterator() = delete;
        TileScanIterator(entt::registry& registry,
            glm::ivec2 pos,
            Direction::TDirection direction)
            : registry { registry }
            , tilemap { registry.ctx().get<TileMapComponent>() }
            , start_pos { pos }
            , pos { pos }
            , direction { direction }
            , reverse_direction { Direction::reverse_direction(direction) }
            , direction_vec { Direction::directions[Direction::index_position(direction)].vec }
            , current { tilemap[pos] }
        {
        }

        entt::entity operator*() { return current.value(); };
        // const Tile* operator->() const { return &current->value(); }

        TileScanIterator& operator++()
        {
            glm::ivec2 next_pos { pos + direction_vec };
            std::optional<entt::entity> next_tile { tilemap[next_pos] };

            if (!next_tile) {
                current = std::nullopt;
                return *this;
            }

            const NavigationComponent* current_nav_component { registry.try_get<const NavigationComponent>(current.value()) };
            const NavigationComponent* next_nav_component { registry.try_get<const NavigationComponent>(next_tile.value()) };

            if (
                !current_nav_component || !next_nav_component || (pos != start_pos && Direction::is_junction(current_nav_component->directions))) {
                current = std::nullopt;
                return *this;
            }

            bool current_can_connect_forward { Direction::any(current_nav_component->directions & direction) };
            bool next_can_connect_back { Direction::any(next_nav_component->directions & reverse_direction) };

            if (!current_can_connect_forward || !next_can_connect_back) {
                current = std::nullopt;
            } else {
                current = next_tile;
                pos = next_pos;
            }

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

    [[maybe_unused]] void _disconnect(entt::registry& registry, entt::entity entity, const Direction::TDirection direction)
    {
        const Direction::TDirection reverse_direction { Direction::reverse_direction(direction) };

        auto junctions = registry.view<JunctionComponent>();
        for (auto [connected_entity, junction] : junctions.each()) {
            auto& connection { junction.connections[Direction::index_position(reverse_direction)] };

            if (connection && connection == entity) {
                connection = std::nullopt;
            }

            bool no_values = std::none_of(
                junction.connections.begin(),
                junction.connections.end(),
                [](auto const& opt) { return opt.has_value(); });

            if (no_values)
                registry.remove<JunctionComponent>(connected_entity);
        }
    }

    void _connect(
        entt::registry& registry,
        const entt::entity from_entity,
        const entt::entity to_entity,
        Direction::TDirection direction)
    {
        _disconnect(registry, to_entity, direction);
        JunctionComponent* junction { registry.try_get<JunctionComponent>(from_entity) };

        if (!junction) {
            junction = &registry.emplace<JunctionComponent>(from_entity);
        }

        junction->connections[Direction::index_position(direction)] = to_entity;
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

        // TODO: check for copies
        const SpriteSheetEntry& spritedef { spritesheet.get(tile_handle) };
        registry.emplace<SpriteComponent>(tile, spritedef.sprite_definition);
        if (spritedef.navigation_definition) {
            registry.emplace<NavigationComponent>(tile, spritedef.navigation_definition.value());
        }
    }
}

void TileMapSystem::connect(entt::registry& registry, entt::entity entity)
{
    auto [nav, pos] { registry.get<NavigationComponent, GridPositionComponent>(entity) };
    bool is_junction { Direction::is_junction(nav.directions) };

    std::array<std::optional<entt::entity>, 4> positions {};
    positions.fill(std::nullopt);

    for (
        Direction::TDirection direction { Direction::TDirection::NORTH };
        direction != Direction::TDirection::NO_DIRECTION;
        direction = direction >> 1) {
        for (auto next_pos : TileScan(registry, pos.grid_position, direction)) {
            positions[Direction::index_position(direction)] = next_pos;
        }
    }

    for (
        Direction::TDirection direction { Direction::TDirection::NORTH };
        direction != Direction::TDirection::NO_DIRECTION;
        direction = direction >> 1) {
        Direction::TDirection reverse_direction { Direction::reverse_direction(direction) };

        const auto& query_pos { positions[Direction::index_position(direction)] };
        const auto& reversed_query_pos { positions[Direction::index_position(reverse_direction)] };

        if (!query_pos || query_pos.value() == entity)
            continue;

        if (is_junction) {
            spdlog::info("Connecting entities {} and {}", int(entity), int(query_pos.value()));
            _connect(registry, entity, query_pos.value(), direction);
        } else {
            spdlog::info("Connecting entities {} and {}", int(query_pos.value()), int(reversed_query_pos.value()));
            _connect(registry, query_pos.value(), reversed_query_pos.value(), direction);
        }
    }
}