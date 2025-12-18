#ifndef SEGMENTCOMPONENT_H
#define SEGMENTCOMPONENT_H

#include <directions.h>

#include <entt/entt.hpp>

struct SegmentComponent {
    entt::entity start;
    entt::entity end;
    Direction::TDirection direction;
};

#endif