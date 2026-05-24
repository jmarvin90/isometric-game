#ifndef GRAPHSYSTEM_H
#define GRAPHSYSTEM_H

#include <entt/entt.hpp>

namespace GraphSystem {
void flag_change(entt::registry& registry, entt::entity entity);
void update(entt::registry& registry);
void emplace_segment(entt::registry& registry, entt::entity entity);
void remove_segment(entt::registry& registry, entt::entity entity);
};

#endif