#ifndef JUNCTIONCOMPONENT_H
#define JUNCTIONCOMPONENT_H

#include <entt/entt.hpp>
#include <optional>
#include <array>

struct JunctionComponent {
    std::array<std::optional<entt::entity>, 4> connections;
};

#endif