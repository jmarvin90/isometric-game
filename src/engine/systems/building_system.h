#ifndef BUILDINGSYSTEM_H
#define BUILDINGSYSTEM_H

#include <entt/entt.hpp>

namespace BuildingSystem {
    void tag(entt::registry& registry, entt::entity entity);
    void untag(entt::registry& registry, entt::entity entity);
    void update(entt::registry& registry);
}

#endif