#include <components/grid_position_component.h>
#include <components/highlight_component.h>
#include <components/junction_component.h>
#include <components/mouseover_component.h>
#include <components/navigation_component.h>
#include <components/pending_destruction_component.h>
#include <components/segment_component.h>
#include <components/sprite_component.h>
#include <components/tilemap_component.h>
#include <components/tilespec_component.h>
#include <components/segment_manager_component.h>
#include <directions.h>
#include <position.h>
#include <spdlog/spdlog.h>
#include <spritesheet.h>
#include <systems/tilemap_system.h>

#include <algorithm>
#include <array>
#include <vector>

namespace {

std::vector<entt::entity> scan(const entt::registry& registry, entt::entity origin, Direction::TDirection direction)
{
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };
    glm::ivec2 direction_vector { Direction::direction_vectors[direction] };
    Direction::TDirection reverse { Direction::reverse(direction) };

    entt::entity current { origin };
    std::vector<entt::entity> output { current };

    const NavigationComponent* current_nav { &registry.get<const NavigationComponent>(current) };
    glm::ivec2 current_position { registry.get<const GridPositionComponent>(current).grid_position };

    while (true) {
        glm::ivec2 next_position { current_position + direction_vector };

        entt::entity next { tilemap[next_position] };

        if (next == entt::null) {
            return output;
        }

        const NavigationComponent* next_nav { registry.try_get<const NavigationComponent>(next) };

        if (!next_nav || (current != origin && Direction::is_junction(current_nav->directions))) {
            return output;
        }

        bool current_can_connect_forward { Direction::any(current_nav->directions & direction) };
        bool next_can_connect_back { Direction::any(next_nav->directions & reverse) };

        if (!current_can_connect_forward || !next_can_connect_back) {
            return output;
        }

        current = next;
        current_nav = next_nav;
        current_position = next_position;
        output.emplace_back(current);
    }
}

// TODO - not really a tilemap system issue
[[maybe_unused]] void apply_highlight(entt::registry& registry, const entt::entity tile, int factor = 1)
{
    TransformComponent& transform { registry.get<TransformComponent>(tile) };
    transform.position.y -= (30 * factor);
    transform.z_index += factor;
}

} // namespace

void TileMapSystem::update(
    [[maybe_unused]] entt::registry& registry,
    [[maybe_unused]] const bool debug_mode
)
{
    auto components { registry.view<MouseOverComponent>() };

    for (auto [entity, component] : components.each()) {
        if (component.this_frame && component.previous_frame) {
            component.this_frame = false;
            continue;
        }

        if (component.previous_frame) {
            apply_highlight(registry, entity, -1);
            registry.remove<MouseOverComponent>(entity);
        }

        if (component.this_frame && debug_mode) {
            apply_highlight(registry, entity, 1);
            component.this_frame = false;
            component.previous_frame = true;
        }
    }
}

void TileMapSystem::emplace_tiles(entt::registry& registry)
{
    TileMapComponent& tilemap { registry.ctx().get<TileMapComponent>() };
    const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };
    const SpriteSheet& spritesheet { registry.ctx().get<const SpriteSheet&>() };

    for (int i = 0; i < tilemap.n_tiles; i++) {
        entt::entity tile { tilemap.tiles.emplace_back(registry.create()) };

        const GridPosition grid_position { registry, i };
        const glm::ivec2 world_position { grid_position.to_world_position(registry) };

        registry.emplace<TransformComponent>(tile, world_position, 0, 0.0);
        registry.emplace<HighlightComponent>(tile, SDL_Color { 0, 0, 255, 255 }, tilespec.iso_points());
        registry.emplace<GridPositionComponent>(tile, grid_position);

        std::string tile_handle {};

        std::vector<glm::ivec2> ew_positions { { 1, 1 }, { 2, 1 }, { 3, 1 }, { 5, 1 }, { 6, 1 }, { 7, 1 } };

        std::vector<glm::ivec2> nesw_positions { { 4, 1 } };

        std::vector<glm::ivec2> ns_positions { { 4, 2 }, { 4, 3 }, { 4, 4 }, { 4, 5 }, { 4, 6 }, { 4, 7 } };

        glm::ivec2 vec_position { grid_position };

        if (((vec_position.x >= 0 && vec_position.x <= 3) || (vec_position.x > 4 && vec_position.x <= 7))
            && vec_position.y == 1) {
            tile_handle = "grass_ew";
        } else if (vec_position == glm::ivec2 { 4, 1 }) {
            tile_handle = "gass_nesw";
        } else if (vec_position.x == 4 && (vec_position.y >= 2 && vec_position.y <= 7)) {
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
    SegmentManagerComponent& seg_manager { registry.ctx().get<SegmentManagerComponent>() };
    const NavigationComponent& current_nav { registry.get<const NavigationComponent>(entity) };
    bool is_junction { Direction::is_junction(current_nav.directions) };
    std::array<std::vector<entt::entity>, 4> connections;

    for (
        Direction::TDirection direction { Direction::TDirection::NORTH };
        direction != Direction::TDirection::NO_DIRECTION;
        direction = direction >> 1
    ) {
        connections[Direction::index_position(direction)] = scan(registry, entity, direction);
    }

    if (is_junction) {
        for (
            Direction::TDirection direction { Direction::TDirection::NORTH };
            direction != Direction::TDirection::NO_DIRECTION;
            direction = direction >> 1
        ) {
            if (connections[Direction::index_position(direction)].size() > 1) {
                entt::entity segment_entity { registry.create() };
                seg_manager.construct_queue.emplace_back(
                    connections[Direction::index_position(direction)],
                    direction
                );
            }
        }
    } else {
        for (auto direction : { Direction::TDirection::NORTH, Direction::TDirection::WEST }) {
            Direction::TDirection opposite { direction >> 2 };
            auto& left { connections[Direction::index_position(direction)] };
            auto& right { connections[Direction::index_position(opposite)] };
            if (left.size() + right.size() > 2) {
                std::vector<entt::entity> segment;
                segment.reserve(left.size() + right.size() - 1);
                segment.insert(segment.end(), left.begin(), left.end());
                segment.insert(segment.end(), right.begin() + 1, right.end());
                entt::entity segment_entity { registry.create() };
                seg_manager.construct_queue.emplace_back(segment, direction);
            }
        }
    }
}

void TileMapSystem::disconnect(entt::registry& registry, entt::entity entity)
{
    const NavigationComponent& nav { registry.get<NavigationComponent>(entity) };
    if (nav.segment_id == entt::null)
        return;
}