#include <components/connections_component.h>
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
    std::optional<Tile> current;

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

    Tile* operator*() { return &current.value(); };
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

std::vector<entt::entity> get_tile_entities(const Tile tile)
{
    if (tile.building_entity) {
        return std::vector<entt::entity> { tile.tile_entity,
            tile.building_entity.value() };
    }
    return std::vector<entt::entity> { tile.tile_entity };
}

void apply_highlight(entt::registry& registry,
    const Tile tile,
    int factor = 1)
{
    for (entt::entity entity : get_tile_entities(tile)) {
        TransformComponent& transform { registry.get<TransformComponent>(entity) };
        transform.position.y -= (30 * factor);
        transform.z_index += factor;
    }
}

[[maybe_unused]] const std::array<std::optional<Tile>, 4> neighbours(
    const TileMapComponent& tilemap,
    const glm::ivec2 grid_position)
{
    std::array<std::optional<Tile>, 4> output {};
    output.fill(std::nullopt);

    for (Direction::DirectionInfo direction : Direction::directions) {
        GridPosition proposed { grid_position + direction.vec };
        if (tilemap[proposed]) {
            output[static_cast<int>(direction.direction)] = tilemap[proposed].value();
        }
    }
    return output;
}

[[maybe_unused]] std::optional<Tile> scan(entt::registry& registry,
    glm::ivec2 from_position,
    Direction::TDirection direction)
{
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };

    std::optional<Tile> current_tile { tilemap[from_position] };
    if (!current_tile)
        return std::nullopt;

    NavigationComponent* current_nav {
        registry.try_get<NavigationComponent>(current_tile->tile_entity)
    };

    const Direction::TDirection reverse { Direction::reverse_direction(direction) };

    for ([[maybe_unused]] Tile* tile :
        TileScan(registry, from_position, direction)) {
        NavigationComponent* next_nav {
            registry.try_get<NavigationComponent>(tile->tile_entity)
        };
        ConnectionsComponent* next_conns {
            registry.try_get<ConnectionsComponent>(tile->tile_entity)
        };

        // TODO: if this sets the only non-null dir to null, we should remove the
        // component; a means should be created by which components can be
        // deleted in case all of their directions are null
        if (next_conns) {
            next_conns->connections[Direction::index_position(direction)] = std::nullopt;
        }

        bool curr_can_connect_forward {
            Direction::any(current_nav->directions & direction)
        };
        bool next_can_connect_back { Direction::any(next_nav->directions & reverse) };
        bool is_junction { Direction::is_junction(current_nav->directions) };

        if (!curr_can_connect_forward || !next_can_connect_back || is_junction) {
            return current_tile;
        }

        current_nav = next_nav;
        current_tile = *tile;
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

void TileMapSystem::emplace_tiles(entt::registry& registry)
{
    TileMapComponent& tilemap { registry.ctx().get<TileMapComponent>() };
    const TileSpecComponent& tilespec {
        registry.ctx().get<const TileSpecComponent>()
    };
    const SpriteSheet& spritesheet { registry.ctx().get<const SpriteSheet&>() };

    for (int i = 0; i < tilemap.n_tiles; i++) {
        Tile& tile { tilemap.tiles.emplace_back(registry) };

        const GridPosition grid_position { registry, i };
        const glm::ivec2 world_position { grid_position.to_world_position(registry) };

        registry.emplace<TransformComponent>(tile.tile_entity, world_position, 0,
            0.0);

        registry.emplace<HighlightComponent>(
            tile.tile_entity, SDL_Color { 0, 0, 255, 255 }, tilespec.iso_points());

        registry.emplace<GridPositionComponent>(tile.tile_entity, grid_position);

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

        registry.emplace<SpriteComponent>(tile.tile_entity,
            spritesheet.get(tile_handle).first);
        registry.emplace<NavigationComponent>(tile.tile_entity,
            spritesheet.get(tile_handle).second);
    }
}

void TileMapSystem::connect(entt::registry& registry, entt::entity entity)
{
    const GridPositionComponent& grid_position {
        registry.get<const GridPositionComponent>(entity)
    };
    const NavigationComponent& nav {
        registry.get<const NavigationComponent>(entity)
    };

    for (uint8_t direction = 1; direction <= 8; direction = direction << 1) {
        Direction::TDirection dir { direction };
        Direction::TDirection reverse { Direction::reverse_direction(dir) };

        if (!Direction::any(dir & nav.directions))
            continue;

        std::optional<Tile> terminating_tile {
            scan(registry, grid_position.grid_position, dir)
        };
        const GridPositionComponent& terminating_tile_pos {
            registry.get<const GridPositionComponent>(
                terminating_tile->tile_entity)
        };

        ConnectionsComponent* connections {
            registry.try_get<ConnectionsComponent>(terminating_tile->tile_entity)
        };
        std::optional<Tile> reverse_terminating_tile {
            scan(registry, terminating_tile_pos.grid_position, reverse)
        };
        ConnectionsComponent* reverse_connections {
            registry.try_get<ConnectionsComponent>(
                reverse_terminating_tile->tile_entity)
        };

        if (terminating_tile == reverse_terminating_tile)
            continue;

        if (!connections) {
            connections = &registry.emplace<ConnectionsComponent>(
                terminating_tile->tile_entity);
        }

        if (!reverse_connections) {
            reverse_connections = &registry.emplace<ConnectionsComponent>(
                reverse_terminating_tile->tile_entity);
        }

        connections->connections[Direction::index_position(reverse)] = reverse_terminating_tile->tile_entity;
        reverse_connections->connections[Direction::index_position(dir)] = terminating_tile->tile_entity;
    }
}