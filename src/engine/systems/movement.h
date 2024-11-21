#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <entt/entt.hpp>
#include <string>

#include "rigid_body.h"
#include "transform.h"
#include "mousemap.h"
#include "constants.h"

bool is_out_of_bounds(const Transform& transform, const MouseMap& mousemap) {
    const glm::ivec2 grid_position{mousemap.pixel_to_grid(transform.position)};
    return (
        grid_position.x < 0 || grid_position.y < 0 ||
        grid_position.x > constants::MAP_SIZE_N_TILES -1 || grid_position.y > constants::MAP_SIZE_N_TILES -1
    );
}

void apply_velocity(
    Transform& transform, 
    const RigidBody& rigid_body, 
    const double delta_time
) {
    transform.position.x += (rigid_body.velocity.x * delta_time);
    transform.position.y += (rigid_body.velocity.y * delta_time);
}

void movement_update(entt::registry& registry, const MouseMap& mousemap, const double delta_time) {
    auto entities_in_motion {registry.view<RigidBody, Transform>()};
    for (auto entity: entities_in_motion) {
        Transform& transform {entities_in_motion.get<Transform>(entity)};
        const RigidBody& rigid_body {entities_in_motion.get<RigidBody>(entity)};

        if (!is_out_of_bounds(transform, mousemap)) {
            apply_velocity(transform, rigid_body, delta_time);
        } else {
            registry.destroy(entity);
        }
    }
}


#endif