#ifndef SEGMENTMANAGERCOMPONENT_H
#define SEGMENTMANAGERCOMPONENT_H

#include <components/segment_component.h>
#include <entt/entt.hpp>
#include <vector>

struct SegmentManagerComponent {
    std::vector<SegmentComponent> construct_queue;
    std::vector<entt::entity> destruct_queue;
};

#endif