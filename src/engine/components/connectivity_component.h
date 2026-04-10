#ifndef CONNECTIVITYCOMPONENT_H
#define CONNECTIVITYCOMPONENT_H

#include <directions.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

struct ConnectivityComponent {
    Direction::TDirection directions;
    bool is_junction;

    ConnectivityComponent()
        : directions { Direction::TDirection::NO_DIRECTION }
        , is_junction { false }
    {
    }

    ConnectivityComponent(Direction::TDirection directions)
        : directions { directions }
        , is_junction { Direction::is_junction(this->directions) }
    {
    }

    ConnectivityComponent(int directions)
        : directions { static_cast<uint8_t>(directions) }
        , is_junction { Direction::is_junction(this->directions) }
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ConnectivityComponent, directions, is_junction)
};

#endif