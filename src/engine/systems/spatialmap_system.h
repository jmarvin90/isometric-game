#ifndef SPATIALMAPSYSTEM_H
#define SPATIALMAPSYSTEM_H

#include <entt/entt.hpp>

namespace SpatialMapSystem {
void emplace_entity(entt::registry& registry, entt::entity entity);
void remove_entity(entt::registry& registry, entt::entity entity);
void emplace_segment(entt::registry& registry, entt::entity entity);
void remove_segment(entt::registry& registry, entt::entity entity);
void update_entity(entt::registry& registry, entt::entity entity);
void update_on_load(entt::registry& registry);
};

#endif