#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <entt/entt.hpp>

#include "rigid_body.h"
#include "transform.h"

void apply_velocity(
    Transform& transform, 
    const RigidBody& rigid_body, 
    const double delta_time
) {
    transform.position.x += (rigid_body.velocity.x * delta_time);
    transform.position.y += (rigid_body.velocity.y * delta_time);
}


#endif