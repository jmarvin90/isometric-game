#ifndef SEGMENTCOMPONENT_H
#define SEGMENTCOMPONENT_H

#include <algorithm>
#include <directions.h>
#include <entt/entt.hpp>
#include <vector>

struct SegmentComponent {
    entt::entity origin;
    entt::entity termination;
    Direction::TDirection direction;
    std::vector<entt::entity> entities;

    SegmentComponent(const SegmentComponent&) = default;
    SegmentComponent(SegmentComponent&&) = default;
    SegmentComponent& operator=(const SegmentComponent&) = default;
    SegmentComponent& operator=(SegmentComponent&&) = default;

    SegmentComponent(
        entt::entity origin,
        entt::entity termination,
        Direction::TDirection direction,
        std::vector<entt::entity> entities
    )
        : origin { origin }
        , termination { termination }
        , direction { direction }
        , entities { entities }
    {
    }

    SegmentComponent(std::vector<entt::entity>& _entities, Direction::TDirection direction)
        : origin { _entities.front() }
        , termination { _entities.back() }
        , direction { direction }
        , entities {}
    {
        std::copy_if(
            _entities.begin(),
            _entities.end(),
            std::back_inserter(entities),
            [_entities](entt::entity x) {
                return x != _entities.front() && x != _entities.back();
            }
        );
    }

    bool operator<(const SegmentComponent& comparator) const
    {
        return entities.size() < comparator.entities.size();
    }
};

#endif