#include <algorithm>
#include <array>
#include <components/grid_position_component.h>
#include <components/navigation_component.h>
#include <components/segment_manager_component.h>
#include <directions.h>
#include <grid.h>
#include <projection.h>
#include <systems/tilemap_system.h>
#include <vector>

namespace {

std::vector<entt::entity> scan(const entt::registry& registry, entt::entity origin, Direction::TDirection direction)
{
    const Grid<entt::entity, TileMapProjection>& tilemap {
        registry.ctx().get<const Grid<entt::entity, TileMapProjection>>()
    };
    glm::ivec2 direction_vector { Direction::direction_vectors[direction] };
    Direction::TDirection reverse { Direction::reverse(direction) };

    entt::entity current { origin };
    std::vector<entt::entity> output { current };

    const NavigationComponent* current_nav { &registry.get<const NavigationComponent>(current) };
    glm::ivec2 current_position { registry.get<const GridPositionComponent>(current).position };

    while (true) {
        glm::ivec2 next_position { current_position + direction_vector };
        entt::entity next { tilemap[next_position] };

        // TODO - again, shouldn't happen - should trigger assertion in Grid
        if (next == entt::null)
            return output;

        const NavigationComponent* next_nav { registry.try_get<const NavigationComponent>(next) };

        if (!next_nav || (current != origin && current_nav->is_junction)) {
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
} // namespace

namespace TileMapSystem {

void connect(entt::registry& registry, entt::entity entity)
{
    SegmentManagerComponent& seg_manager { registry.ctx().get<SegmentManagerComponent>() };
    const NavigationComponent& current_nav { registry.get<const NavigationComponent>(entity) };
    std::array<std::vector<entt::entity>, 4> connections;

    for (
        Direction::TDirection direction { Direction::TDirection::NORTH };
        direction != Direction::TDirection::NO_DIRECTION;
        direction = direction >> 1) {
        connections[Direction::index_position(direction)] = scan(registry, entity, direction);
    }

    if (current_nav.is_junction) {
        for (
            Direction::TDirection direction { Direction::TDirection::NORTH };
            direction != Direction::TDirection::NO_DIRECTION;
            direction = direction >> 1) {
            if (connections[Direction::index_position(direction)].size() > 1) {
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
                seg_manager.construct_queue.emplace_back(segment, direction);
            }
        }
    }
}

void disconnect(entt::registry& registry, entt::entity entity)
{
    const NavigationComponent& nav { registry.get<NavigationComponent>(entity) };
    if (nav.segment_id == entt::null)
        return;
}
}