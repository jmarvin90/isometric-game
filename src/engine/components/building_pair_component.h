#ifndef BUILDINGPAIRCOMPONENT_H
#define BUILDINGPAIRCOMPONENT_H

#include <entt/entt.hpp>

struct BuildingPairComponent {
    entt::entity paired_with;
};

#endif