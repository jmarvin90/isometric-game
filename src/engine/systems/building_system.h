#ifndef BUILDINGSYSTEM_H
#define BUILDINGSYSTEM_H

#include <entt/entt.hpp>

namespace BuildingSystem {
void create(entt::registry& registry, entt::entity entity);
void update(entt::registry& registry);
void remove(entt::registry& registry, entt::entity entity);
}

#endif