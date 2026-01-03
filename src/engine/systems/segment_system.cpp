#include <components/grid_position_component.h>
#include <components/junction_component.h>
#include <components/navigation_component.h>
#include <components/segment_component.h>
#include <components/transform_component.h>
#include <components/segment_manager_component.h>
#include <directions.h>
#include <position.h>
#include <systems/segment_system.h>

#include <entt/entt.hpp>

namespace {
    void junction_emplace(
        entt::registry& registry, 
        entt::entity junction_entity, 
        entt::entity segment_entity,
        Direction::TDirection direction
    ) {
        JunctionComponent* junct;

        if (!registry.all_of<JunctionComponent>(junction_entity)) {
            junct = &registry.emplace<JunctionComponent>(junction_entity);
        } else {
            junct = &registry.get<JunctionComponent>(junction_entity);
            entt::entity current_segment { junct->connections[Direction::index_position(direction)] };
            if (current_segment != entt::null) {
                SegmentManagerComponent& seg_manager { registry.ctx().get<SegmentManagerComponent>() };
                seg_manager.destruct_queue.push_back(current_segment);
            }
        }
        junct->connections[Direction::index_position(direction)] = segment_entity;
    }

    void junction_displace(
        entt::registry& registry,
        entt::entity junction_entity,
        entt::entity segment_entity,
        Direction::TDirection direction
    ) {
        JunctionComponent& junction { registry.get<JunctionComponent>(junction_entity) };
        entt::entity current_segment { junction.connections[Direction::index_position(direction)] };
        if (current_segment == entt::null || current_segment != segment_entity) 
            return;
        junction.connections[Direction::index_position(direction)] = entt::null;
        if (
            std::all_of(
                junction.connections.begin(), 
                junction.connections.end(), 
                [](entt::entity i) { return i == entt::null; }
            )
        ) {
            registry.remove<JunctionComponent>(junction_entity);
        }
    }
}

void SegmentSystem::connect(entt::registry& registry, entt::entity entity) {
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    junction_emplace(registry, segment.start, entity, segment.direction);
    junction_emplace(registry, segment.end, entity, Direction::reverse(segment.direction));
}

void SegmentSystem::disconnect(entt::registry& registry, entt::entity entity) {
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    junction_displace(registry, segment.start, entity, segment.direction);
    junction_displace(registry, segment.end, entity, Direction::reverse(segment.direction));
}

void SegmentSystem::update(entt::registry& registry) {
    SegmentManagerComponent& seg_manager { registry.ctx().get<SegmentManagerComponent>() };

    registry.destroy(seg_manager.destruct_queue.begin(), seg_manager.destruct_queue.end());
    seg_manager.destruct_queue.clear();

    for (auto segment: seg_manager.construct_queue) {
        entt::entity segment_entity { registry.create() };
        registry.emplace<SegmentComponent>(
            segment_entity, 
            segment.start,
            segment.end,
            segment.direction,
            segment.entities
        );
    }
    seg_manager.construct_queue.clear();
}