#include <components/building_pair_component.h>
#include <components/connectivity_component.h>
#include <components/flags.h>
#include <components/origin_component.h>
#include <components/path_component.h>
#include <components/render_offset_component.h>
#include <components/road_access_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <components/velocity_component.h>
#include <components/walker_component.h>
#include <constants.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <pathfinding.h>
#include <projection.h>
#include <spritesheet.h>
#include <systems/walker_system.h>

namespace WalkerSystem {
void create(
    entt::registry& registry,
    entt::entity origin_building_entity,
    const std::vector<PathSegment>& expanded_path,
    const SpriteSheet& spritesheet
)
{
    entt::entity walker_entity { registry.create() };
    registry.emplace<PathComponent>(walker_entity, expanded_path);
    registry.emplace<WalkerComponent>(origin_building_entity, walker_entity);
    registry.emplace<OriginComponent>(walker_entity, origin_building_entity);

    const SpriteComponent& sprite_component {
        registry.emplace<SpriteComponent>(
            walker_entity,
            &spritesheet.sprites.at(
                Constants::WALKER_DIRECTIONS.at(expanded_path.front().direction)
            )
        )
    };

    registry.emplace<TransformComponent>(
        walker_entity,
        expanded_path.front().start,
        1,
        0.0
    );

    registry.emplace<RenderOffsetComponent>(
        walker_entity,
        -sprite_component.sprite_definition->anchor
    );

    registry.emplace<VelocityComponent>(
        walker_entity,
        Direction::isometric_direction_vectors.at(expanded_path.front().direction),
        60 // TODO - should be encoded in the sprite definition JSON
    );
}

void remove(
    entt::registry& registry,
    entt::entity walker
)
{
    if (!registry.all_of<OriginComponent>(walker))
        return;

    entt::entity origin {
        registry.get<const OriginComponent>(walker).origin
    };

    registry.remove<WalkerComponent>(origin);
}

void update(entt::registry& registry)
{
    auto pending {
        registry.view<
            BuildingPairComponent,
            RoadAccessComponent,
            SenderFlag>(entt::exclude<WalkerComponent>)
    };

    if (pending.begin() == pending.end())
        return;

    using TileMapType = Grid<entt::entity, TileMapProjection>;
    const TileMapType tilemap { registry.ctx().get<const TileMapType>() };
    std::vector<entt::entity> path {};

    for (auto [building_entity, building_pair, road_access] : pending.each()) {
        path.clear();

        // TODO: idiomatic/repeatable way to get sprite position from entity
        entt::entity target_tile {
            tilemap.at_world(
                glm::ivec2 { registry.get<const TransformComponent>(building_pair.paired_with).position }
                + registry.get<const SpriteComponent>(building_pair.paired_with).sprite_definition->anchor
            )
        };

        // Get the first viable path, even if not the best
        for (entt::entity access_point : road_access.road_access_points) {
            Pathfinding::path_between(
                registry,
                access_point,
                target_tile,
                path
            );
            if (!path.empty())
                break;
        }

        if (path.empty())
            continue;

        std::vector<PathSegment> expanded_path {};
        Pathfinding::expand_path(registry, path, expanded_path);
        const SpriteSheet& spritesheet { registry.ctx().get<const SpriteSheet>() };
        create(registry, building_entity, expanded_path, spritesheet);
    }
}
}