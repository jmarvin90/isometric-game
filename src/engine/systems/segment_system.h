#ifndef SEGMENTSYSTEM_H
#define SEGMENTSYSTEM_H

#include <entt/entt.hpp>

class SegmentSystem {
    public:
    static void connect(entt::registry& registry, entt::entity entity);
    static void disconnect(entt::registry& registry, entt::entity entity);
    static void update(entt::registry& registry);
};

#endif