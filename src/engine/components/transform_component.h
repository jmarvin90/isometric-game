#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <constants.h>
#include <components/camera_component.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

struct TransformComponent {
    glm::vec2 position;
    int z_index;
    double rotation;
};

#endif