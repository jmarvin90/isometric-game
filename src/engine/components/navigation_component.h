#ifndef NAVIGATIONCOMPONENT_H
#define NAVIGATIONCOMPONENT_H

#include <directions.h>
#include <rapidjson/document.h>

struct NavigationComponent {
    Direction::TDirection directions;
    NavigationComponent(int directions)
        : directions { static_cast<uint8_t>(directions) }
    {
    }

    NavigationComponent(const rapidjson::Value& json_object)
        : directions { (json_object.HasMember("directions") && json_object["directions"].IsInt())
                ? static_cast<uint8_t>(json_object["directions"].GetInt())
                : static_cast<uint8_t>(0) }
    {
    }
};

#endif