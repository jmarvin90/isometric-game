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

std::vector<entt::entity> get_entities_between(
    const entt::registry& registry,
    glm::ivec2 origin_position,
    glm::ivec2 termination_position
)
{
    /*
        Get all of the tile entities between two positions.

        Possibly:
            - redundant, in case the scan function returns a vector of entities
              instead;
            - Upgradable into some sort of LERP function

        Ultimately:
            - Get delta between origin and termination
            - Get the step change via glm::sign
            - For the length of the delta, step through the tilemap using the
              step change
            - Append the entity from the given position in the tilemap to the
              output & return
    */
}

entt::entity unidirectional_scan(
    const entt::registry& registry,
    entt::entity origin_tile,
    Direction::TDirection direction
)
{
    /*
        Get (all of the) connected tile entities from a given origin/direction.
        Based on a for loop which continues until the next position's
        entity is entt::null, or the current and next position's connectivity
        isn't reciprocal.

        TODO: determine whether this should return the last entity in the chain,
        or the whole chain
    */
}

std::array<entt::entity, 4> multidirectional_scan(
    const entt::registry& registry,
    entt::entity origin_tile,
    Direction::TDirection directions
)
{
    /*

    Return an array which encodes the detail of viable connections in any
    direction.

    May need to be updated in case the unidirectional scan returns a vector
    instead of just a single entity.

    The bitwise logic is otherwise pretty nice.

    std::array<entt::entity, 4> output;
    output.fill(entt::null);

    auto remaining { Direction::to_underlying(directions) };

    while (remaining) {
        auto direction { remaining & -remaining };
        remaining ^= direction;
        Direction::TDirection current_direction {
            static_cast<Direction::TDirection>(direction)
        };
        output[Direction::index_position(current_direction)] = unidirectional_scan(registry, origin_tile, current_direction);
    }

    return output;
    */
}

void junction_displace(
    entt::registry& registry,
    entt::entity junction_entity,
    Direction::TDirection direction
)
{
    // Should still have junctions, probably
    assert(registry.all_of<JunctionComponent>(junction_entity));

    JunctionComponent& junction { registry.get<JunctionComponent>(junction_entity) };
    entt::entity& displaced { junction.connections[Direction::index_position(direction)] };

    // Set the connection val to null
    if (displaced != entt::null)
        displaced = entt::null;

    // If all of the vals are null, flag the component for deletion
    // TODO: perhaps this flagging shouldn't take place here, because
    // I might want to displace something then immediately emplace?
    if (
        std::all_of(
            junction.connections.begin(),
            junction.connections.end(),
            [](entt::entity i) { return i == entt::null; }
        )
    ) {
        registry.emplace_or_replace<JunctionDeleteFlag>(junction_entity);
    }
}

void junction_emplace(
    entt::registry& registry,
    entt::entity junction_entity,
    entt::entity segment_entity,
    Direction::TDirection direction
)
{
    /*
        Emplace a segment at a junction.

        Specifically:
            - Fetch a junction component if one exists; or
            - Create a junction component otherwise;
            - Populate the component's connectivity array at the appropriate
              index with the segment entity ID

        TODO: determine whether this function should mark stale segments being
        displaced for deletion [IMPORTANT!]
    */
}

void segment_create(
    entt::registry& registry,
    entt::entity origin,
    entt::entity termination,
    Direction::TDirection direction
)
{
    /*
        Create a segment.

        Anticipate that this method will be called from a batch process, where
        every updated tile will have any existing connectivity removed, and
        new connectivity established.

        Likely to involve:
         - Checking that origin and termination aren't the same (early out)
         - Getting all of the entities between origin and termination
         - Creating a segment entity
         - For intermediary nodes, populating segment membership components
         - For origin and termination, populating the new segment into
           the relevant junction components
    */
}
}

namespace GraphSystem {

void tile_update(entt::registry& registry, entt::entity entity)
{
    registry.emplace_or_replace<ConnectivityUpdateFlag>(entity);

    JunctionComponent* junction { registry.try_get<JunctionComponent>(entity) };
    SegmentMemberComponent* segment_member { registry.try_get<SegmentMemberComponent>(entity) };

    // If the entity is on a segment, flag the segment for deletion
    if (segment_member) {
        registry.emplace_or_replace<SegmentDeleteFlag>(segment_member->segment);
        const SegmentComponent& segment { registry.get<const SegmentComponent>(segment_member->segment) };

        // assume the entity is in the segment
        auto current_it { std::find(segment.entities.begin(), segment.entities.end(), entity) };
        assert(current_it != segment.entities.end());

        // if there's a previous entry
        if (current_it != segment.entities.begin()) {
            registry.emplace_or_replace<ConnectivityUpdateFlag>(*std::prev(current_it, 1));
        }

        // if there's a next entry
        if (std::next(current_it, 1) != segment.entities.end()) {
            registry.emplace_or_replace<ConnectivityUpdateFlag>(*std::next(current_it, 1));
        }
    }

    // If the entity is a junction, flag the junction for deletion
    if (junction) {
        registry.emplace_or_replace<JunctionDeleteFlag>(entity);
    }
}

void update(entt::registry& registry)
{
    // Destroy flagged junctions - flagging remaining segments for deletion
    auto junction_deletes { registry.view<JunctionComponent, JunctionDeleteFlag>() };
    for (auto [entity, junction] : junction_deletes.each()) {
        for (auto segment_entity : junction.connections) {
            if (segment_entity == entt::null)
                continue;
            registry.emplace_or_replace<SegmentDeleteFlag>(segment_entity);
        }
    }
    for (auto entity : junction_deletes) {
        registry.remove<JunctionComponent>(entity);
    }
    registry.clear<JunctionDeleteFlag>();

    // Remove reference to flagged segments from junctions; delete flagged segments
    auto segment_deletes { registry.view<SegmentComponent, SegmentDeleteFlag>() };
    for (auto [entity, segment] : segment_deletes.each()) {
        junction_displace(registry, segment.origin, segment.direction);
        junction_displace(registry, segment.termination, Direction::reverse(segment.direction));
    }
    registry.destroy(segment_deletes.begin(), segment_deletes.end());

    // Create and emplace new segments into junctions
    auto connectivity_updates {
        registry.view<ConnectivityComponent, ConnectivityUpdateFlag>()
    };
    for (auto [entity, connectivity] : connectivity_updates.each()) {
    }

    /*
        ConnectivityUpdateFlag:
            - Scan in all directions per ConnectivityComponent
            - Create all new relevant segments, including placement into
                junctions and assignment of segment membership
            - [Important!] Clear old segments that have been displaced
            - Clear the ConnectivityUpdateFlag storage
    */
}

}