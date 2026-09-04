#ifndef SPATIALMAPCELLSPANCOMPONENT_H
#define SPATIALMAPCELLSPANCOMPONENT_H

#include <glm/glm.hpp>
#include <nlohmann/json.hpp>

struct SpatialMapCellSpanComponent {
    glm::ivec2 AA;
    glm::ivec2 BB;
    bool operator!=(const SpatialMapCellSpanComponent& other) const
    {
        return AA != other.AA || BB != other.BB;
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SpatialMapCellSpanComponent, AA, BB);
};

#endif