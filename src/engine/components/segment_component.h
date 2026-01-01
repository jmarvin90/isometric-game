#ifndef SEGMENTCOMPONENT_H
#define SEGMENTCOMPONENT_H

#include <directions.h>
#include <vector>
#include <entt/entt.hpp>

struct SegmentComponent {
    entt::entity start;
    entt::entity end;
    Direction::TDirection direction;
    std::vector<entt::entity> entities;
    SegmentComponent(std::vector<entt::entity> _entities, Direction::TDirection direction)
    : start { entt::null }
    , end { entt::null }
    , direction {direction}
    , entities {}
    {
        size_t size {_entities.size()};

        for (auto entity: _entities) {
            if (entity == _entities.front()) {
                start = entity;
            } 
            
            if (entity == _entities.back()) {
                end = entity;
            } 
            
            if (entity != _entities.front() && entity != _entities.back()) {
                entities.push_back(entity);
            }
        }
    }
};

#endif