#ifndef GRAPHSYSTEM_H
#define GRAPHSYSTEM_H

#include <entt/entt.hpp>

namespace GraphSystem {
void create(entt::registry& registry, entt::entity entity);
void update(entt::registry& registry);
void remove(entt::registry& registry, entt::entity entity);
};

#endif