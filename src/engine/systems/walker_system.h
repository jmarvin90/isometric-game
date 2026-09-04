#ifndef WALKERSYSTEM_H
#define WALKERSYSTEM_H

#include <components/path_component.h>
#include <entt/entt.hpp>
#include <spritesheet.h>

namespace WalkerSystem {
void create(
    entt::registry& registry,
    entt::entity origin_building_entity,
    const std::vector<PathSegment>& expanded_path,
    const SpriteSheet& spritesheet
);
void remove(entt::registry& registry, entt::entity walker);
void update(entt::registry& registry);
}

#endif