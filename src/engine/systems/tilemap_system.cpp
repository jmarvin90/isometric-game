#include <components/grid_position_component.h>
#include <components/highlight_component.h>
#include <components/junction_component.h>
#include <components/navigation_component.h>
#include <components/segment_component.h>
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
#include <vector>

namespace {

    std::vector<entt::entity> scan(
        [[maybe_unused]] const entt::registry& registry,
        [[maybe_unused]] entt::entity origin,
        [[maybe_unused]] Direction::TDirection direction)
    {
        const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
        glm::ivec2 direction_vector { Direction::direction_vectors[direction] };
        entt::entity current { origin };
        Direction::TDirection reverse_direction { Direction::reverse_direction(direction) };

        std::vector<entt::entity> output { current };

        while (true) {
            glm::ivec2 current_position { registry.get<const GridPositionComponent>(current).grid_position };
            glm::ivec2 next_position { current_position + direction_vector };

            entt::entity next { tilemap[next_position] };

            if (next == entt::null) {
                return output;
            }

            const NavigationComponent& current_nav { registry.get<const NavigationComponent>(current) };
            const NavigationComponent* next_nav { registry.try_get<const NavigationComponent>(next) };

            if (!next_nav || (current != origin && Direction::is_junction(current_nav.directions))) {
                return output;
            }

            bool current_can_connect_forward { Direction::any(current_nav.directions & direction) };
            bool next_can_connect_back { Direction::any(next_nav->directions & reverse_direction) };

            if (!current_can_connect_forward || !next_can_connect_back) {
                return output;
            }

            current = next;
            output.emplace_back(current);
        }
    }

    void apply_highlight(entt::registry& registry,
        const entt::entity tile,
        int factor = 1)
    {
        TransformComponent& transform { registry.get<TransformComponent>(tile) };
        transform.position.y -= (30 * factor);
        transform.z_index += factor;
    }

} // namespace

void TileMapSystem::update(entt::registry& registry, const bool debug_mode)
{
    auto& tilemap = registry.ctx().get<TileMapComponent>();
    const auto& mouse = registry.ctx().get<const MouseComponent>();

    if (!debug_mode) {
        if (tilemap.highlighted_tile != entt::null) {
            apply_highlight(registry, tilemap.highlighted_tile, -1);
            tilemap.highlighted_tile = entt::null;
        }
        return;
    }

    if (!mouse.moved) {
        return;
    }

    const GridPosition grid_position {
        ScreenPosition(mouse.window_current_position).to_grid_position(registry)
    };

    entt::entity new_tile = tilemap[grid_position];

    if (new_tile == tilemap.highlighted_tile) {
        return;
    }

    if (tilemap.highlighted_tile != entt::null) {
        apply_highlight(registry, tilemap.highlighted_tile, -1);
    }

    if (new_tile != entt::null) {
        apply_highlight(registry, new_tile);
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

void _create_junction(
    entt::registry& registry,
    entt::entity junction_id,
    entt::entity segment_id,
    Direction::TDirection direction)
{
    JunctionComponent* current_junction { registry.try_get<JunctionComponent>(junction_id) };

    if (current_junction) {
        entt::entity current_seg { current_junction->connections[Direction::index_position(direction)] };
        if (current_seg != entt::null) {
            registry.destroy(current_seg);
        }
    } else {
        current_junction = &registry.emplace<JunctionComponent>(junction_id);
    }

    current_junction->connections[Direction::index_position(direction)] = segment_id;
}

void _create_segment(
    [[maybe_unused]] entt::registry& registry,
    [[maybe_unused]] std::vector<entt::entity> segment,
    [[maybe_unused]] Direction::TDirection direction)
{
    if (segment.front() == segment.back() || segment.empty()) {
        return;
    }

    entt::entity segment_id { registry.create() };
    [[maybe_unused]] SegmentComponent& seg { registry.emplace<SegmentComponent>(segment_id, segment.front(), segment.back()) };

    if (segment.empty())
        return;

    for (entt::entity entity : segment) {
        if (entity == segment.back()) {
            _create_junction(registry, entity, segment_id, Direction::reverse_direction(direction));
        } else if (entity == segment.front()) {
            _create_junction(registry, entity, segment_id, direction);
        } else {
            NavigationComponent* nav { registry.try_get<NavigationComponent>(entity) };
            nav->segment_id = segment_id;
        }
    }
}

void TileMapSystem::connect(
    [[maybe_unused]] entt::registry& registry,
    [[maybe_unused]] entt::entity entity)
{
    const NavigationComponent& current_nav { registry.get<const NavigationComponent>(entity) };
    bool is_junction { Direction::is_junction(current_nav.directions) };

    [[maybe_unused]] const GridPositionComponent gridpos { registry.get<const GridPositionComponent>(entity) };

    std::array<std::vector<entt::entity>, 4> connections;

    for (
        Direction::TDirection direction { Direction::TDirection::NORTH };
        direction != Direction::TDirection::NO_DIRECTION;
        direction = direction >> 1) {
        connections[Direction::index_position(direction)] = scan(registry, entity, direction);
    }

    if (is_junction) {
        for (
            Direction::TDirection direction { Direction::TDirection::NORTH };
            direction != Direction::TDirection::NO_DIRECTION;
            direction = direction >> 1) {
            _create_segment(registry, connections[Direction::index_position(direction)], direction);
        }
    } else {
        for (auto direction : { Direction::TDirection::NORTH, Direction::TDirection::WEST }) {
            Direction::TDirection opposite { direction >> 2 };
            auto& left { connections[Direction::index_position(direction)] };
            auto& right { connections[Direction::index_position(opposite)] };

            // TODO: make this better; _create_segment assumes vector size > 1
            if (left.size() == 1 && right.size() == 1)
                continue;

            if (right.size() == 1)
                _create_segment(registry, left, direction);
            else if (left.size() == 1)
                _create_segment(registry, left, direction);
            else {
                left.insert(left.end(), right.begin(), right.end());
                _create_segment(registry, left, direction);
            }
        }
    }
}