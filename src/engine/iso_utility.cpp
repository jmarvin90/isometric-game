#include <components/spatialmapcell_span_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <entt/entt.hpp>
#include <iso_utility.h>
#include <sprite.h>
#include <spritesheet.h>
#include <systems/spatialmap_system.h>

/*
    TODO - this is pretty redundant if it needs to be called
    after the sprite/transform have been emplaced because some
    signals invoked by emplacement require the transform to
    be accurate
*/
namespace ISOUtility {
void align_sprite_to(
    entt::registry& registry,
    entt::entity entity,
    [[maybe_unused]] TransformComponent& transform,
    const SpriteComponent& sprite,
    SpriteAnchor alignment_anchor,
    glm::ivec2 world_position
)
{
    glm::vec2 new_position {};

    if (alignment_anchor == SpriteAnchor::ORIGIN) {
        new_position = world_position;
    }

    if (alignment_anchor == SpriteAnchor::SPRITE_ANCHOR) {
        new_position = world_position - sprite.sprite_definition->anchor;
    }

    registry.patch<TransformComponent>(
        entity, [new_position](auto& transform) {
            transform.position = new_position;
        }
    );
}

bool AABB(
    const glm::vec2& entity_position,
    const SpriteComponent& sprite,
    const glm::ivec2 query_position
)
{
    glm::ivec2 AA { entity_position };
    glm::ivec2 BB {
        AA
        + glm::ivec2 {
            sprite.sprite_definition->source_rect.w,
            sprite.sprite_definition->source_rect.h }
    };

    return (
        glm::all(glm::greaterThanEqual(query_position, AA))
        && glm::all(glm::lessThan(query_position, BB))
    );
}
}
