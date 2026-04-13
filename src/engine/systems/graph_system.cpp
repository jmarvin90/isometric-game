#include <algorithm>
#include <components/connectivity_component.h>
#include <components/flags.h>
#include <components/junction_component.h>
#include <components/segment_component.h>
#include <components/segment_member_component.h>
#include <components/transform_component.h>
#include <directions.h>
#include <grid.h>
#include <iterator>
#include <projection.h>
#include <systems/graph_system.h>

#include <entt/entt.hpp>

namespace {
bool reciprocate(
    const entt::registry& registry,
    entt::entity lhs, 
    entt::entity rhs,
    Direction::TDirection direction
)
{
    const ConnectivityComponent& lhs_component { registry.get<const ConnectivityComponent>(lhs) };
    const ConnectivityComponent* rhs_component { registry.try_get<const ConnectivityComponent>(rhs) };

    if (!rhs_component)
        return false;

    return (
        Direction::any(lhs_component.directions & direction)
        && Direction::any(rhs_component->directions & Direction::reverse(direction))
    );
}

std::vector<entt::entity> get_segment_from(
    const entt::registry& registry,
    entt::entity tile, 
    Direction::TDirection direction
) {
    const Grid<entt::entity, TileMapProjection>& tilemap {
        registry.ctx().get<const Grid<entt::entity, TileMapProjection>>()
    };

    std::vector<entt::entity> output;
    entt::entity current_entity { tile };
    glm::ivec2 current_position { registry.get<GridPositionComponent>(current_entity).position };

    while (true) {
        output.push_back(current_entity);

        if (current_entity != tile && registry.all_of<JunctionComponent>(current_entity))
            break;

        glm::ivec2 next_position { current_position + Direction::direction_vectors[direction] };
        entt::entity next_entity { tilemap[next_position] };

        if (
            next_entity == entt::null
            || !reciprocate(registry, current_entity, next_entity, direction)
        )
            break;

        current_entity = next_entity;
        current_position = next_position;
    }

    return output;
}

void junction_populate(entt::registry& registry, entt::entity junction_tile)
{
    const ConnectivityComponent& connectivity { 
        registry.get<const ConnectivityComponent>(junction_tile)
    };

    for (
        auto remaining { Direction::to_underlying(connectivity.directions)};
        remaining;
    ) {
        auto dir { remaining & -remaining };
        remaining ^= dir;
        auto direction { Direction::TDirection(dir) };

        std::vector<entt::entity> segment {
            get_segment_from(registry, junction_tile, direction)
        };

        if (segment.size() == 1)
            continue;

        entt::entity segment_entity { registry.create() };

        SegmentComponent& segment_component { 
            registry.emplace<SegmentComponent>(segment_entity, segment, direction)
        };

        JunctionComponent& origin { 
            registry.get_or_emplace<JunctionComponent>(segment_component.origin)
        };

        JunctionComponent& termination {
            registry.get_or_emplace<JunctionComponent>(segment_component.termination)
        };

        origin.connections[Direction::index_position(direction)] = segment_entity;
        termination.connections[Direction::index_position(Direction::reverse(direction))] = segment_entity;
    }
}

void graph_release(entt::registry& registry) {
    // Delete all the segments
    auto segments { registry.view<SegmentComponent>() };
    if (!segments.empty())
        registry.destroy(segments.begin(), segments.end());

    // Delete all segment memberships
    registry.clear<SegmentMemberComponent>();

    // Delete all the junctions
    registry.clear<JunctionComponent>();
}

void graph_compute(entt::registry& registry) {
    auto connecting_tiles { registry.view<ConnectivityComponent>()};
    for (auto [entity, connectivity]: connecting_tiles.each()) {
        if (connectivity.is_junction) {
            junction_populate(registry, entity);
        }
    }
}
}

namespace GraphSystem {
void update(entt::registry& registry, [[maybe_unused]] entt::entity entity)
{
    graph_release(registry);
    graph_compute(registry);
}

void emplace_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    for (auto member: segment.entities) {
        registry.emplace_or_replace<SegmentMemberComponent>(member, entity);
    }
}

void remove_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    for (auto member: segment.entities) {
        registry.remove<SegmentMemberComponent>(member);
    }
}
}