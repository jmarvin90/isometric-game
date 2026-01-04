#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <components/camera_component.h>
#include <constants.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>

struct TransformComponent {
    glm::vec2 position;
    int z_index;
    double rotation;
};

#endif