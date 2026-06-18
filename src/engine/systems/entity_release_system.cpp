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
#include <systems/walker_system.h>

namespace EntityReleaseSystem {

void update(entt::registry& registry)
{
    auto deletion_view { registry.view<EntityReleaseFlag>() };
    for (auto entity : deletion_view) {
        BuildingSystem::remove(registry, entity);
        SpatialMapSystem::remove_entity(registry, entity);
        SpatialMapSystem::remove_segment(registry, entity);
        GraphSystem::remove(registry, entity);
        WalkerSystem::remove(registry, entity);
    }
    registry.destroy(deletion_view.begin(), deletion_view.end());
}
}