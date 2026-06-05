#include <systems/walker_system.h>

#include <components/building_pair_component.h>
#include <components/connectivity_component.h>
#include <components/flags.h>
#include <components/path_component.h>
#include <components/road_access_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <pathfinding.h>
#include <projection.h>

namespace {
}

namespace WalkerSystem {
void update(entt::registry& registry)
{
    [[maybe_unused]] auto pending {
        registry.view<
            TransformComponent,
            SpriteComponent,
            BuildingPairComponent,
            RoadAccessComponent,
            SenderFlag>(entt::exclude<HasWalkerFlag>)
    };

    using TileMapType = Grid<entt::entity, TileMapProjection>;
    const TileMapType tilemap { registry.ctx().get<const TileMapType>() };

    for (auto [entity, transform, sprite, building_pair, road_access] : pending.each()) {
        std::vector<entt::entity> path {};

        entt::entity target_tile {
            tilemap.at_world(
                glm::ivec2 { registry.get<const TransformComponent>(building_pair.paired_with).position }
                + registry.get<const SpriteComponent>(building_pair.paired_with).sprite_definition->anchor
            )
        };

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

        entt::entity walker_entity { registry.create() };
        registry.emplace<PathComponent>(walker_entity);
        registry.emplace<HasWalkerFlag>(entity);
    }
}
}