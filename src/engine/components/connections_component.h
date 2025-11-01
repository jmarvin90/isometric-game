#ifndef CONNECTIONSCOMPONENT_H
#define CONNECTIONSCOMPONENT_H

#include <entt/entt.hpp>
#include <array>
#include <optional>

struct ConnectionsComponent {
    std::array<std::optional<entt::entity>, 4> connections;
};

#endif