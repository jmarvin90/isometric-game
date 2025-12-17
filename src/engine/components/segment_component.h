#ifndef SEGMENTCOMPONENT_H
#define SEGMENTCOMPONENT_H

#include <entt/entt.hpp>

struct SegmentComponent {
    entt::entity start;
    entt::entity end;
};

#endif