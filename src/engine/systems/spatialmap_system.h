#ifndef SPATIALMAPSYSTEM_H
#define SPATIALMAPSYSTEM_H

#include <entt/entt.hpp>

namespace SpatialMapSystem {
void create_entity(entt::registry& registry, entt::entity entity);
void remove_entity(entt::registry& registry, entt::entity entity);

void create_segment(entt::registry& registry, entt::entity entity);
void remove_segment(entt::registry& registry, entt::entity entity);

void update(entt::registry& registry);
};

#endif