#ifndef BUILDINGPAIRCOMPONENT_H
#define BUILDINGPAIRCOMPONENT_H

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

struct BuildingPairComponent {
    entt::entity paired_with;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(BuildingPairComponent, paired_with)
};

#endif