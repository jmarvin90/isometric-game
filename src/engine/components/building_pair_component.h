#ifndef BUILDINGPAIRCOMPONENT_H
#define BUILDINGPAIRCOMPONENT_H

#include <entt/entt.hpp>

struct BuildingPairComponent {
    entt::entity sender;
    entt::entity receiver;
};

#endif