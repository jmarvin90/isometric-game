#include <components/spatialmapcell_span_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <position.h>
#include <systems/spatialmap_system.h>
#include <utility.h>

namespace Utility {
void align_sprite_to(
    entt::registry& registry,
    entt::entity entity,
    Utility::SpriteAnchor alignment_anchor,
    WorldPosition position
)
{
    TransformComponent& transform { registry.get<TransformComponent>(entity) };

    if (alignment_anchor == Utility::SpriteAnchor::ORIGIN) {
        transform.position = position.position;
    }

    if (alignment_anchor == Utility::SpriteAnchor::SPRITE_ANCHOR) {
        const SpriteComponent& sprite { registry.get<const SpriteComponent>(entity) };
        transform.position = position.position - sprite.anchor;
    }

    SpatialMapSystem::update_entity(registry, entity);
}
}