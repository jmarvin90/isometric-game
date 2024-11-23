#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/glm.hpp>

struct Transform{
    glm::vec2 position;
    int z_index;
    double rotation;
};

#endif