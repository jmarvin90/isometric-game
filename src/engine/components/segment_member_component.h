#ifndef SEGMENTMEMBERCOMPONENT_H
#define SEGMENTMEMBERCOMPONENT_H

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

struct SegmentMemberComponent {
    entt::entity segment;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SegmentMemberComponent, segment)
};

#endif