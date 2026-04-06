#ifndef NAVIGATIONCOMPONENT_H
#define NAVIGATIONCOMPONENT_H

#include <directions.h>
#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

struct NavigationComponent {
    Direction::TDirection directions;
    entt::entity segment_id;
    bool is_junction;

    NavigationComponent()
        : directions { Direction::TDirection::NO_DIRECTION }
        , segment_id { entt::null }
        , is_junction { false }
    {
    }

    NavigationComponent(Direction::TDirection directions)
        : directions { directions }
        , segment_id { entt::null }
        , is_junction { Direction::is_junction(this->directions) }
    {
    }

    NavigationComponent(int directions)
        : directions { static_cast<uint8_t>(directions) }
        , segment_id { entt::null }
        , is_junction { Direction::is_junction(this->directions) }
    {
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(NavigationComponent, directions, segment_id, is_junction)
};

#endif