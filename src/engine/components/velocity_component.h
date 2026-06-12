#ifndef VELOCITYCOMPONENT_H
#define VELOCITYCOMPONENT_H

#include <glm/glm.hpp>

struct VelocityComponent {
    glm::ivec2 direction_vector;
    float speed;
};

#endif