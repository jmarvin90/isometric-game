#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include <entt/entt.hpp>

namespace MovementSystem {
void update(entt::registry& registry, const float delta_time);
}

#endif