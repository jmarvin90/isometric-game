#include <components/building_pair_component.h>
#include <components/flags.h>
#include <components/grid_position_component.h>
#include <components/origin_component.h>
#include <components/path_component.h>
#include <components/segment_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <components/velocity_component.h>
#include <components/walker_component.h>
#include <entt/entt.hpp>
#include <systems/building_system.h>
#include <systems/entity_release_system.h>
#include <systems/graph_system.h>
#include <systems/spatialmap_system.h>

namespace EntityReleaseSystem {

void update(entt::registry& registry)
{
    auto deletion_view { registry.view<EntityReleaseFlag>() };
    for (auto entity : deletion_view) {
        if (registry.any_of<SenderFlag, ReceiverFlag, BuildingPairComponent>(entity)) {
            BuildingSystem::untag(registry, entity);
        }

        if (registry.any_of<SpriteComponent>(entity)) {
            SpatialMapSystem::remove_entity(registry, entity);
        }

        if (registry.any_of<SegmentComponent>(entity)) {
            SpatialMapSystem::remove_segment(registry, entity);
            GraphSystem::remove_segment(registry, entity);
        }

        if (registry.any_of<PathComponent, OriginComponent>(entity)) {
            registry.remove<WalkerComponent>(
                registry.get<OriginComponent>(entity).origin
            );
        }
    }
    registry.destroy(deletion_view.begin(), deletion_view.end());
}
}