#include <components/grid_position_component.h>
#include <components/junction_component.h>
#include <components/navigation_component.h>
#include <components/segment_component.h>
#include <components/transform_component.h>
#include <directions.h>
#include <position.h>
#include <systems/segment_system.h>

#include <entt/entt.hpp>

namespace {
void emplace_segment_at_junction(
    entt::registry& registry,
    entt::entity junction_id,
    entt::entity segment_id,
    Direction::TDirection direction
)
{
    JunctionComponent* junct;
    if (!registry.all_of<JunctionComponent>(junction_id)) {
        junct = &registry.emplace<JunctionComponent>(junction_id);
    } else {
        junct = &registry.get<JunctionComponent>(junction_id);
        entt::entity current_segment_id {
            junct->connections[Direction::index_position(direction)]
        };
        if (current_segment_id != entt::null) {
            registry.destroy(current_segment_id);
        }
    }
    junct->connections[Direction::index_position(direction)] = segment_id;
}

[[maybe_unused]] void remove_segment_from_junction(
    entt::registry& registry,
    entt::entity junction_id,
    Direction::TDirection direction
)
{
    auto& junction { registry.get<JunctionComponent>(junction_id) };
    junction.connections[Direction::index_position(direction)] = entt::null;
    // TODO: how to clean up emtpy junctions?
}
}

void SegmentSystem::connect(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    glm::ivec2 start { registry.get<GridPositionComponent>(segment.start).grid_position };
    glm::ivec2 end { registry.get<GridPositionComponent>(segment.end).grid_position };
    emplace_segment_at_junction(registry, segment.start, entity, segment.direction);
    emplace_segment_at_junction(registry, segment.end, entity, Direction::reverse(segment.direction));
}

void SegmentSystem::disconnect(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    glm::ivec2 start { registry.get<GridPositionComponent>(segment.start).grid_position };
    glm::ivec2 end { registry.get<GridPositionComponent>(segment.end).grid_position };
    remove_segment_from_junction(registry, segment.start, segment.direction);
    remove_segment_from_junction(registry, segment.end, Direction::reverse(segment.direction));
    for (auto entity : segment.entities) {
        registry.get<NavigationComponent>(entity).segment_id = entt::null;
    }
}