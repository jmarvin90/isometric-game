#include <components/spatialmapcell_span_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <iso_utility.h>
#include <sprite.h>
#include <spritesheet.h>
#include <systems/spatialmap_system.h>
#include <entt/entt.hpp>

namespace ISOUtility {
void align_sprite_to(
    entt::registry& registry,
    entt::entity entity,
    [[maybe_unused]] TransformComponent& transform,
    const SpriteComponent& sprite,
    ISOUtility::SpriteAnchor alignment_anchor,
    glm::ivec2 world_position
)
{
    glm::vec2 new_position {};

    if (alignment_anchor == ISOUtility::SpriteAnchor::ORIGIN) {
        new_position = world_position;
    }

    if (alignment_anchor == ISOUtility::SpriteAnchor::SPRITE_ANCHOR) {
        new_position = world_position - sprite.sprite_definition->anchor;
    }

    registry.patch<TransformComponent>(
        entity, [new_position](auto& transform) {
            transform.position = new_position;
        }
    );
}

bool AABB(
    const TransformComponent& transform,
    const SpriteComponent& sprite,
    const glm::ivec2 position
)
{
    glm::ivec2 AA { transform.position };
    glm::ivec2 BB {
        AA
        + glm::ivec2 {
            sprite.sprite_definition->source_rect.w,
            sprite.sprite_definition->source_rect.h }
    };

    return (
        glm::all(glm::greaterThanEqual(position, AA))
        & glm::all(glm::lessThan(position, BB))
    );
}
}
