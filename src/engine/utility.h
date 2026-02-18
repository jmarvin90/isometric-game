#ifndef UTILITY_H
#define UTILITY_H

#include <entt/entt.hpp>
#include <glm/glm.hpp>

namespace Utility {
enum class SpriteAnchor {
    SPRITE_ANCHOR = 1,
    ORIGIN = 2
};

void align_sprite_to(
    entt::registry& registry,
    entt::entity entity,
    SpriteAnchor alignment_anchor,
    glm::ivec2 position
);
} // namespace

#endif