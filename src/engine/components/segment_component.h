#ifndef SEGMENTCOMPONENT_H
#define SEGMENTCOMPONENT_H

#include <directions.h>
#include <entt/entt.hpp>
#include <vector>

struct SegmentComponent {
    entt::entity start;
    entt::entity end;
    Direction::TDirection direction;
    std::vector<entt::entity> entities;

    SegmentComponent(const SegmentComponent&) = default;
    SegmentComponent(SegmentComponent&&) = default;
    SegmentComponent& operator=(const SegmentComponent&) = default;
    SegmentComponent& operator=(SegmentComponent&&) = default;

    SegmentComponent(
        entt::entity start,
        entt::entity end,
        Direction::TDirection direction,
        std::vector<entt::entity> entities
    )
        : start { start }
        , end { end }
        , direction { direction }
        , entities { entities }
    {
    }

    SegmentComponent(std::vector<entt::entity> _entities, Direction::TDirection direction)
        : start { _entities.front() }
        , end { _entities.back() }
        , direction { direction }
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