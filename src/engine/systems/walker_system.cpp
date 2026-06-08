#include <components/building_pair_component.h>
#include <components/connectivity_component.h>
#include <components/flags.h>
#include <components/path_component.h>
#include <components/road_access_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <components/velocity_component.h>
#include <constants.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <pathfinding.h>
#include <projection.h>
#include <spritesheet.h>
#include <systems/walker_system.h>

#include <spdlog/spdlog.h>

namespace {
void create_walkers(entt::registry& registry)
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
    std::vector<entt::entity> path {};

    for (auto [entity, transform, sprite, building_pair, road_access] : pending.each()) {
        path.clear();

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

        entt::entity walker_entity { registry.create() };
        registry.emplace<PathComponent>(walker_entity, path);
        registry.emplace<HasWalkerFlag>(entity);
    }
}

[[maybe_unused]] Direction::TDirection direction_between(
    const entt::registry& registry,
    entt::entity start,
    entt::entity end
)
{
    glm::ivec2 dir {
        registry.get<const GridPositionComponent>(start).position
        - registry.get<const GridPositionComponent>(end).position
    };

    return Direction::vector_directions.at(
        Direction::to_direction_vector(dir)
    );
}

// void advance(entt::registry& registry, entt::entity entity)
// {
//     PathComponent& path { registry.get<PathComponent>(entity) };
//     entt::entity current { path.path[path.current++] };

//     if (path.path.size() == path.current) {
//     }

//     entt::entity next { path.path[path.current] };
// }
}

namespace WalkerSystem {
void update(entt::registry& registry)
{
    create_walkers(registry);
}
}