#ifndef JUNCTIONCOMPONENT_H
#define JUNCTIONCOMPONENT_H

#include <array>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

struct JunctionComponent {
    std::array<entt::entity, 4> connections;
    JunctionComponent() { connections.fill(entt::null); }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(JunctionComponent, connections)
};

#endif