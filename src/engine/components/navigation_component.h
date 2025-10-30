#ifndef NAVIGATIONCOMPONENT_H
#define NAVIGATIONCOMPONENT_H

#include <rapidjson/document.h>
#include <directions.h>

struct NavigationComponent {
    Direction::TDirection directions;
    NavigationComponent(int directions): directions {directions} {}

    NavigationComponent(const rapidjson::Value& json_object)
    : directions {json_object["directions"].GetInt()}
    {}
};

#endif