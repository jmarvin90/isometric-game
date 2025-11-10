#ifndef CONNECTIONSCOMPONENT_H
#define CONNECTIONSCOMPONENT_H

#include <array>
#include <entt/entt.hpp>
#include <optional>

struct ConnectionsComponent {
    std::array<std::optional<entt::entity>, 4> connections;
};

#endif