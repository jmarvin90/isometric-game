#include <components/transform_component.h>
#include <components/velocity_component.h>
#include <systems/movement_system.h>

namespace MovementSystem {
void update(entt::registry& registry, const float delta_time)
{
    auto entities { registry.view<VelocityComponent>() };
    for (auto [entity, velocity] : entities.each()) {
        registry.patch<TransformComponent>(
            entity,
            [velocity, delta_time](auto& transform) {
                transform.position += (velocity.velocity * delta_time);
            }
        );
    }
}
}