#include <systems/movement_system.h>

#include <components/flags.h>
#include <components/path_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <components/velocity_component.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

namespace MovementSystem {
void update(entt::registry& registry, const float delta_time)
{
    auto view { registry.view<TransformComponent, SpriteComponent, VelocityComponent, PathComponent>() };
    for (auto [entity, transform, sprite, velocity, path] : view.each()) {
        float budget { velocity.speed * delta_time };

        while (budget > 0 && size_t(path.current) != path.path.size() - 1) {
            const PathSegment& current_segment { path.path.at(path.current) };
            const glm::vec2 remaining { glm::vec2 { current_segment.end } - transform.position };
            const float dist_to_target = glm::length(remaining);

            if (budget > dist_to_target) {
                // If we need to turn;
                registry.patch<TransformComponent>(
                    entity,
                    [current_segment, remaining](auto& transform) {
                        transform.position = glm::vec2 { current_segment.end };
                    }
                );
                budget -= dist_to_target;
                path.current++;
                registry.patch<VelocityComponent>(
                    entity,
                    [path](auto& vel) {
                        vel.direction_vector = Direction::isometric_direction_vectors.at(
                            path.path.at(path.current).direction
                        );
                    }
                );
                continue;
            } else {
                glm::vec2 movement {
                    budget //
                    * glm::vec2( //
                        Direction::isometric_direction_vectors.at( //
                            current_segment.direction //
                        ) //
                    )
                };
                registry.patch<TransformComponent>(
                    entity,
                    [movement](auto& transform) {
                        transform.position += movement;
                    }
                );
                budget = 0;
            }
        }
    }
}
}