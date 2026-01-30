#ifndef SEGMENTCOMPONENT_H
#define SEGMENTCOMPONENT_H

#include <directions.h>
#include <entt/entt.hpp>
#include <vector>

struct Interval {
    entt::entity origin;
    entt::entity termination;
    Direction::TDirection direction;
    int length;
    Interval(
        entt::entity origin,
        entt::entity termination,
        Direction::TDirection direction,
        int length
    )
        : origin { origin }
        , termination { termination }
        , direction { direction }
        , length { length }
    {
    }
    bool operator<(const Interval& comparator) const { return length < comparator.length; }
};

struct SegmentComponent : public Interval {
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
        : Interval(origin, termination, direction, entities.size())
        , entities { entities }
    {
    }

    SegmentComponent(std::vector<entt::entity>& _entities, Direction::TDirection direction)
        : Interval(_entities.front(), _entities.back(), direction, _entities.size())
        , entities {}
    {
        for (auto entity : _entities) {
            if (entity != _entities.front() && entity != _entities.back()) {
                entities.push_back(entity);
            }
        }
    }
};

#endif