#include <components/spatialmapcell_span_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <systems/spatialmap_system.h>
#include <utility.h>

namespace Utility {
void align_sprite_to(
    entt::registry& registry,
    entt::entity entity,
    Utility::SpriteAnchor alignment_anchor,
    glm::ivec2 world_position
)
{
    TransformComponent& transform { registry.get<TransformComponent>(entity) };

    if (alignment_anchor == Utility::SpriteAnchor::ORIGIN) {
        transform.position = world_position;
    }

    if (alignment_anchor == Utility::SpriteAnchor::SPRITE_ANCHOR) {
        const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };
        transform.position = world_position - sprite.sprite_definition->anchor;
    }

    SpatialMapSystem::update_entity(registry, entity);
}

bool AABB(const entt::registry& registry, entt::entity entity, const glm::ivec2 mouse_position)
{
    const TransformComponent& transform { registry.get<const TransformComponent>(entity) };
    const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };
    glm::ivec2 AA { transform.position };
    glm::ivec2 BB { AA + glm::ivec2 { sprite.sprite_definition->source_rect.w, sprite.sprite_definition->source_rect.h } };
    return (
        glm::all(glm::greaterThanEqual(mouse_position, AA))
        & glm::all(glm::lessThan(mouse_position, BB))
    );
}
}
