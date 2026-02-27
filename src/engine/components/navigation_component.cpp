#include <components/navigation_component.h>
#include <nlohmann/json.hpp>

void from_json(const nlohmann::json& json, NavigationComponent& navigation)
{
    if (json.contains("directions")) {
        navigation.directions = Direction::TDirection { json.at("directions") };
        navigation.is_junction = Direction::is_junction(json.at("directions"));
    }
}