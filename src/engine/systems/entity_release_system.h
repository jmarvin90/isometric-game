#ifndef ENTITYRELEASESYSTEM_H
#define ENTITYRELEASESYSTEM_H

#include <entt/entt.hpp>

namespace EntityReleaseSystem {
void flag_delete(entt::registry& registry, entt::entity entity);
void update(entt::registry& registry);
}

#endif