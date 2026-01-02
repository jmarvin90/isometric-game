#ifndef JUNCTIONSYSTEM_H
#define JUNCTIONSYSTEM_H

#include <entt/entt.hpp>

class JunctionSystem {
public:
    static void on_update(entt::registry& registry, entt::entity entity);
};

#endif