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

Direction::TDirection resolved_directions(
    entt::registry& registry,
    Direction::TDirection unresolved_directions,
    glm::ivec2 grid_position
)
{
    const Grid<entt::entity, TileMapProjection>& tilemap {
        registry.ctx().get<const Grid<entt::entity, TileMapProjection>>()
    };

    Direction::TDirection resolved_directions { Direction::TDirection::NO_DIRECTION };

    for (
        auto direction : Direction::EachDirectionIn { unresolved_directions }) {
        glm::ivec2 step { Direction::direction_vectors[direction] };
        entt::entity next { tilemap[grid_position + step] };
        if (next != entt::null && reciprocate(registry, tilemap[grid_position], next, direction))
            resolved_directions = resolved_directions | direction;
    }
    return resolved_directions;
}

void identify_junctions(entt::registry& registry)
{
    auto connectivity_view { registry.view<ConnectivityComponent, GridPositionComponent>() };

    for (auto [entity, connectivity, grid_position] : connectivity_view.each()) {
        bool junction_in_theory { connectivity.is_junction };

        auto resolved { resolved_directions(registry, connectivity.directions, grid_position.position) };
        bool junction_in_practice { Direction::is_junction(resolved) };

        if (junction_in_theory || junction_in_practice) {
            registry.emplace<JunctionComponent>(entity);
        }
    }
}

std::vector<entt::entity> get_segment_from(
    const entt::registry& registry,
    entt::entity tile,
    Direction::TDirection direction
)
{
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

void junction_populate(
    entt::registry& registry,
    entt::entity junction_tile,
    const ConnectivityComponent& connectivity,
    const JunctionComponent& junction
)
{
    for (
        auto direction : Direction::EachDirectionIn { connectivity.directions }) {

        // If the segment has already been created from another junction
        if (
            junction.connections[Direction::index_position(direction)] != entt::null
        )
            continue;

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
            registry.get<JunctionComponent>(segment_component.origin)
        };

        JunctionComponent& termination {
            registry.get<JunctionComponent>(segment_component.termination)
        };

        origin.connections[Direction::index_position(direction)] = segment_entity;
        termination.connections[Direction::index_position(Direction::reverse(direction))] = segment_entity;
    }
}

void graph_release(entt::registry& registry)
{
    // Delete all the segments
    auto segments { registry.view<SegmentComponent>() };

    // TODO - investigate why commented alternative segfaults
    for (auto entity : segments)
        registry.destroy(entity);
    // registry.destroy(segments.begin(), segments.end());

    // Delete all segment memberships & junctions
    registry.clear<SegmentMemberComponent>();
    registry.clear<JunctionComponent>();
}

void graph_compute(entt::registry& registry)
{
    auto junctions_view {
        registry.view<JunctionComponent, ConnectivityComponent>()
    };
    for (auto [entity, junction, connectivity] : junctions_view.each()) {
        junction_populate(registry, entity, connectivity, junction);
    }
}
}

namespace GraphSystem {
void update(entt::registry& registry, [[maybe_unused]] entt::entity entity)
{
    graph_release(registry);
    identify_junctions(registry);
    graph_compute(registry);
}

void emplace_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    for (auto member : segment.entities) {
        registry.emplace_or_replace<SegmentMemberComponent>(member, entity);
    }
}

void remove_segment(entt::registry& registry, entt::entity entity)
{
    const SegmentComponent& segment { registry.get<const SegmentComponent>(entity) };
    for (auto member : segment.entities) {
        registry.remove<SegmentMemberComponent>(member);
    }
}
}