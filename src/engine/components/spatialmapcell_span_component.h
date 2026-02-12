#ifndef SPATIALMAPCELLSPANCOMPONENT_H
#define SPATIALMAPCELLSPANCOMPONENT_H

#include <position.h>

struct SpatialMapCellSpanComponent {
    SpatialMapGridPosition AA;
    SpatialMapGridPosition BB;
    bool operator!=(const SpatialMapCellSpanComponent& other) const
    {
        return AA.position != other.AA.position || BB.position != other.BB.position;
    }
};

#endif