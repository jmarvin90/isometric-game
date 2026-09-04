#include <components/building_pair_component.h>
#include <components/flags.h>
#include <components/junction_component.h>
#include <components/road_access_component.h>
#include <components/segment_member_component.h>
#include <components/sprite_component.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <position.h>
#include <projection.h>
#include <sprite.h>
#include <systems/building_system.h>

namespace {

std::vector<entt::entity> get_access_points(
    entt::registry& registry,
    entt::entity entity
)
{
    glm::ivec2 position {
        glm::ivec2 { registry.get<const TransformComponent>(entity).position }
        + registry.get<const SpriteComponent>(entity).sprite_definition->anchor
    };

    using TileMapType = Grid<entt::entity, TileMapProjection>;
    const TileMapType tilemap { registry.ctx().get<const TileMapType>() };
    glm::ivec2 grid_position { TileMapProjection::world_to_grid(position, tilemap) };
    std::vector<entt::entity> output {};

    for (auto direction : Direction::EachDirectionIn { Direction::TDirection::ALL_CARDINAL_DIRECTIONS }) {
        glm::ivec2 candidate_grid_position {
            grid_position + Direction::direction_vectors.at(direction)
        };

        if (!tilemap.position_is_valid(candidate_grid_position))
            continue;

        entt::entity candidate { tilemap[candidate_grid_position] };

        if (
            candidate != entt::null
            && registry.any_of<JunctionComponent, SegmentMemberComponent>(candidate)
        ) {
            output.emplace_back(candidate);
        }
    }

    return output;
}

void unpair(entt::registry& registry, entt::entity entity)
{
    if (!registry.all_of<BuildingPairComponent>(entity))
        return;

    const BuildingPairComponent& pair {
        registry.get<const BuildingPairComponent>(entity)
    };

    for (auto paired_entity : { entity, pair.paired_with }) {
        registry.remove<BuildingPairComponent>(paired_entity);
    }
}
}

namespace BuildingSystem {

void create(entt::registry& registry, entt::entity entity)
{
    remove(registry, entity);

    const SpriteComponent& sprite {
        registry.get<const SpriteComponent>(entity)
    };

    // TODO: copied mulitple times, emplacement condition duplicated
    std::vector<entt::entity> access_points;

    switch (sprite.sprite_definition->sprite_type) {
    case SpriteType::BUILDING_SENDER:
        access_points = get_access_points(registry, entity);
        registry.emplace<SenderFlag>(entity);
        if (access_points.size() != 0)
            registry.emplace_or_replace<RoadAccessComponent>(
                entity, access_points
            );
        break;
    case SpriteType::BUILDING_RECEIVER:
        access_points = get_access_points(registry, entity);
        registry.emplace<ReceiverFlag>(entity);
        if (access_points.size() != 0)
            registry.emplace_or_replace<RoadAccessComponent>(
                entity, access_points
            );
        break;
    default:
        break;
    }
}

void update(entt::registry& registry)
{
    auto unpaired_senders {
        registry.view<SenderFlag>(entt::exclude<BuildingPairComponent>)
    };

    auto unpaired_receivers {
        registry.view<ReceiverFlag>(entt::exclude<BuildingPairComponent>)
    };

    auto sendersIt { unpaired_senders.begin() };
    auto receiversIt { unpaired_receivers.begin() };

    while (
        sendersIt != unpaired_senders.end()
        && receiversIt != unpaired_receivers.end()
    ) {
        entt::entity sender { *sendersIt++ };
        entt::entity receiver { *receiversIt++ };
        registry.emplace<BuildingPairComponent>(sender, receiver);
        registry.emplace<BuildingPairComponent>(receiver, sender);
    }
}

void remove(entt::registry& registry, entt::entity entity)
{
    if (!registry.any_of<SenderFlag, ReceiverFlag, RoadAccessComponent>(entity))
        return;

    unpair(registry, entity);
    registry.remove<SenderFlag, ReceiverFlag, RoadAccessComponent>(entity);
}

}