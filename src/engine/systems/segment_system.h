#ifndef SEGMENTSYSTEM_H
#define SEGMENTSYSTEM_H

#include <entt/entt.hpp>

namespace SegmentSystem {
void connect(entt::registry& registry, entt::entity entity);
void disconnect(entt::registry& registry, entt::entity entity);
void update(entt::registry& registry);
};

#endif