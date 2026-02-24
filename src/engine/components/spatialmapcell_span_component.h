#ifndef SPATIALMAPCELLSPANCOMPONENT_H
#define SPATIALMAPCELLSPANCOMPONENT_H

#include <glm/glm.hpp>

struct SpatialMapCellSpanComponent {
    glm::ivec2 AA;
    glm::ivec2 BB;
    bool operator!=(const SpatialMapCellSpanComponent& other) const
    {
        return AA != other.AA || BB != other.BB;
    }
};

#endif