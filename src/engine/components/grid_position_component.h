#ifndef GRIDPOSITIONCOMPONENT_H
#define GRIDPOSITIONCOMPONENT_H

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

struct GridPositionComponent {
    glm::ivec2 position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GridPositionComponent, position)
};

#endif