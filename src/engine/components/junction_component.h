#ifndef JUNCTIONCOMPONENT_H
#define JUNCTIONCOMPONENT_H

#include <array>
#include <entt/entt.hpp>
#include <optional>

struct JunctionComponent {
    std::array<entt::entity, 4> connections;
    JunctionComponent() { connections.fill(entt::null); }
};

#endif