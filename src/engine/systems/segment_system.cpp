#include <systems/segment_system.h>
#include <components/junction_component.h>
#include <components/segment_component.h>
#include <components/navigation_component.h>
#include <components/transform_component.h>
#include <components/grid_position_component.h>
#include <directions.h>
#include <position.h>

#include <entt/entt.hpp>

namespace {
void emplace_segment_at_junction(
    entt::registry& registry, entt::entity junction_id, entt::entity segment_id, Direction::TDirection direction
)
{
    JunctionComponent* junction { registry.try_get<JunctionComponent>(junction_id) };
    if (junction && junction->connections[Direction::index_position(direction)] != entt::null) {
        // SegmentSystem::disconnect(registry, junction->connections[Direction::index_position(direction)]);
        registry.destroy(junction->connections[Direction::index_position(direction)]);
    }
    if (!junction) {
        junction = &registry.emplace<JunctionComponent>(junction_id);
    }
    junction->connections[Direction::index_position(direction)] = segment_id;
}

void remove_segment_from_junction(
    entt::registry& registry, entt::entity junction_id, entt::entity segment_id, Direction::TDirection direction
)
{
    JunctionComponent& junction { registry.get<JunctionComponent>(junction_id) };

    entt::entity& current_segment_connection { 
        junction.connections[Direction::index_position(direction)] 
    };

    if (current_segment_connection == segment_id) {
        current_segment_connection = entt::null;
    }
    if (
        std::all_of(
            junction.connections.begin(),
            junction.connections.end(),
            [](entt::entity i) {
                return i == entt::null;
            }
        )
    ) {
        registry.remove<JunctionComponent>(junction_id);
    }
}
}

void SegmentSystem::connect(entt::registry& registry, entt::entity entity) {
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity)};
    glm::ivec2 start {registry.get<GridPositionComponent>(segment.start).grid_position};
    glm::ivec2 end {registry.get<GridPositionComponent>(segment.end).grid_position};
    emplace_segment_at_junction(registry, segment.start, entity, segment.direction);
    emplace_segment_at_junction(registry, segment.end, entity, Direction::reverse(segment.direction));
}

void SegmentSystem::disconnect(entt::registry& registry, entt::entity entity) {
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    glm::ivec2 start {registry.get<GridPositionComponent>(segment.start).grid_position};
    glm::ivec2 end {registry.get<GridPositionComponent>(segment.end).grid_position};
    remove_segment_from_junction(registry, segment.start, entity, segment.direction);
    remove_segment_from_junction(registry, segment.end, entity, Direction::reverse(segment.direction));
    for (auto entity: segment.entities) {
        NavigationComponent& nav { registry.get<NavigationComponent>(entity)};
        nav.segment_id = entt::null;
    }
}