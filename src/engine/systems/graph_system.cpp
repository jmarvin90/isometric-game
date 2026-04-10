#include <components/connectivity_component.h>
#include <components/connectivity_update_flag.h>
#include <components/junction_component.h>
#include <components/segment_component.h>
#include <components/segment_delete_flag.h>
#include <components/segment_member_component.h>
#include <components/transform_component.h>
#include <directions.h>
#include <grid.h>
#include <projection.h>
#include <systems/graph_system.h>

#include <entt/entt.hpp>

namespace {
// Get the furthest tile entity away from origin in a specified direction
entt::entity unidirectional_scan(const entt::registry& registry, entt::entity origin_tile, Direction::TDirection direction)
{
    const Grid<entt::entity, TileMapProjection>& tilemap { registry.ctx().get<const Grid<entt::entity, TileMapProjection>>() };
    const glm::ivec2 direction_vector { Direction::direction_vectors[direction] };
    glm::ivec2 current_position { registry.get<const GridPositionComponent>(origin_tile).position };

    for (
        glm::ivec2 next_position { current_position + direction_vector };
        tilemap[next_position] != entt::null;
        next_position += direction_vector //
    ) {
        auto& current_connectivity { registry.get<const ConnectivityComponent>(tilemap[current_position]) };
        auto& next_connectivity { registry.get<const ConnectivityComponent>(tilemap[next_position]) };
        if (!Direction::any(current_connectivity.directions & Direction::reverse(next_connectivity.directions)))
            break;
        current_position = next_position;
    }

    return tilemap[current_position];
}

// Get an array with the scan results in all relevant directions
std::array<entt::entity, 4> multidirectional_scan(const entt::registry& registry, entt::entity origin_tile, Direction::TDirection directions)
{
    std::array<entt::entity, 4> output;
    output.fill(entt::null);

    auto remaining { Direction::to_underlying(directions) };

    while (remaining) {
        auto direction { remaining & -remaining };
        remaining ^= direction;
        Direction::TDirection current_direction { static_cast<Direction::TDirection>(direction) };
        output[Direction::index_position(current_direction)] = unidirectional_scan(registry, origin_tile, current_direction);
    }

    return output;
}
}

namespace GraphSystem {

void tile_update(entt::registry& registry, entt::entity entity)
{
    registry.emplace_or_replace<ConnectivityUpdateFlag>(entity);
}

void update(entt::registry& registry)
{
    for (auto [entity, segment_membership] : registry.view<ConnectivityUpdateFlag, SegmentMemberComponent>().each()) {
        registry.emplace_or_replace<SegmentDeleteFlag>(segment_membership.segment);
        registry.remove<SegmentMemberComponent>(entity);
    }

    for (
        auto [entity, connectivity_component] //
        : registry.view<ConnectivityComponent, ConnectivityUpdateFlag>(entt::exclude<SegmentMemberComponent>).each() //
    ) {
        std::array<entt::entity, 4> connections { multidirectional_scan(registry, entity, connectivity_component.directions) };
    }
}

}