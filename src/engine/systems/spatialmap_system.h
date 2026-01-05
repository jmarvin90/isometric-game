#ifndef SPATIALMAPSYSTEM_H
#define SPATIALMAPSYSTEM_H

#include <entt/entt.hpp>

class SpatialMapSystem {

public:
    static void emplace_entity(entt::registry& registry, entt::entity entity);
    static void emplace_segment(entt::registry& registry, entt::entity entity);
    static void remove_segment(entt::registry& registry, entt::entity entity);
};

#endif