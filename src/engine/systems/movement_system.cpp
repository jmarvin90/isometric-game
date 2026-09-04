#include <components/flags.h>
#include <components/path_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <components/velocity_component.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <spritesheet.h>
#include <systems/movement_system.h>

namespace {
void advance(
    entt::registry& registry,
    entt::entity entity,
    const SpriteSheet& spritesheet,
    float& budget,
    TransformComponent& transform,
    PathComponent& path
)
{
    const PathSegment& current_segment { path.path.at(path.current) };
    const glm::vec2 remaining { glm::vec2 { current_segment.end } - transform.position };
    const float dist_to_target = glm::length(remaining);
    const glm::vec2 movement_vector {
        Direction::isometric_direction_vectors.at(current_segment.direction)
    };
    glm::vec2 movement {};

    if (budget > dist_to_target) {
        // If we can afford to, move to the end of the current segment
        movement = remaining;
        budget -= dist_to_target;
        path.current++;
    } else {
        // Otherwise, move as far as we can afford along the current segment
        movement = budget * glm::normalize(movement_vector);
        budget = 0;
    }

    // Apply the movement to the transform
    registry.patch<TransformComponent>(
        entity,
        [&current_segment, &remaining, &movement](auto& transform) {
            transform.position += movement;
        }
    );

    // If we're at the end of the path or we have no budget left, exit
    if (path.current == path.path.size() || budget == 0)
        return;

    // Otherwise, point in the next direction
    registry.patch<VelocityComponent>(
        entity,
        [path](auto& vel) {
            vel.direction_vector = Direction::isometric_direction_vectors.at(
                path.path.at(path.current).direction
            );
        }
    );

    registry.patch<SpriteComponent>(
        entity,
        [&spritesheet, &path](auto& sprite) {
            const std::string& walker {
                Constants::WALKER_DIRECTIONS.at(
                    path.path.at(path.current).direction
                )
            };
            sprite.sprite_definition = &spritesheet.sprites.at(walker);
        }
    );
}
}

namespace MovementSystem {
void update(entt::registry& registry, const float delta_time)
{
    const SpriteSheet& spritesheet { registry.ctx().get<const SpriteSheet>() };
    auto view { registry.view<TransformComponent, VelocityComponent, PathComponent>() };

    for (auto [entity, transform, velocity, path] : view.each()) {

        if (path.current >= path.path.size()) {
            registry.emplace_or_replace<EntityReleaseFlag>(entity);
            continue;
        }

        float budget { velocity.speed * delta_time };
        while (budget > 0 && path.current != path.path.size()) {
            advance(registry, entity, spritesheet, budget, transform, path);
        }
    }
}
}