#ifndef NAVIGATIONCOMPONENT_H
#define NAVIGATIONCOMPONENT_H

#include <directions.h>
#include <rapidjson/document.h>

#include <entt/entt.hpp>

struct NavigationComponent {
    Direction::TDirection directions;
    entt::entity segment_id;
    bool is_junction;

    NavigationComponent(int directions)
        : directions { static_cast<uint8_t>(directions) }
        , segment_id { entt::null }
        , is_junction { Direction::is_junction(this->directions) }
    {
    }

    NavigationComponent(const rapidjson::Value& json_object)
        : directions {
            (json_object.HasMember("directions") && json_object["directions"].IsInt())
                ? static_cast<uint8_t>(json_object["directions"].GetInt())
                : static_cast<uint8_t>(0)
        }
        , segment_id { entt::null }
        , is_junction { Direction::is_junction(this->directions) }
    {
    }
};

#endif