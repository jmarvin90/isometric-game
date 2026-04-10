#ifndef GRAPHSYSTEM_H
#define GRAPHSYSTEM_H

#include <entt/entt.hpp>

namespace GraphSystem {
void tile_update(entt::registry& registry, entt::entity entity);
void update(entt::registry& registry);
};

#endif