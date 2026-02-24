#ifndef SEGMENTCOMPONENT_H
#define SEGMENTCOMPONENT_H

#include <directions.h>
#include <entt/entt.hpp>
#include <vector>

struct Interval {

    Interval(
        entt::entity origin,
        entt::entity termination,
        Direction::TDirection direction,
        int length
    )

    {
    }
};

struct SegmentComponent {
    entt::entity origin;
    entt::entity termination;
    Direction::TDirection direction;
    std::vector<entt::entity> entities;
    int length;

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
        , length { entities.size() }
    {
    }

    SegmentComponent(std::vector<entt::entity>& _entities, Direction::TDirection direction)
        : origin { _entities.front() }
        , termination { _entities.back() }
        , direction { direction }
        , entities {}
        , length {}
    {
        for (auto entity : _entities) {
            if (entity != _entities.front() && entity != _entities.back()) {
                entities.push_back(entity);
            }
        }
        length = entities.size();
    }

    bool operator<(const SegmentComponent& comparator) const { return length < comparator.length; }
};

#endif