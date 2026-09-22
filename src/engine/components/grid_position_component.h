#ifndef GRIDPOSITIONCOMPONENT_H
#define GRIDPOSITIONCOMPONENT_H

#include <glm/glm.hpp>
#include <glm_json.h>
#include <nlohmann/json.hpp>

namespace glm {

void to_json(nlohmann::json& j, const glm::ivec2& p);

void from_json(
    const nlohmann::json& j,
    glm::ivec2& p
);

}

struct GridPositionComponent {
    glm::ivec2 position;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(GridPositionComponent, position)
};

#endif