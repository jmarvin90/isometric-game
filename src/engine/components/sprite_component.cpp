#include <components/sprite_component.h>
#include <nlohmann/json.hpp>

void from_json(const nlohmann::json& json, SpriteComponent& sprite)
{
    json.at("origin_x").get_to(sprite.source_rect.x);
    json.at("origin_y").get_to(sprite.source_rect.y);
    json.at("width").get_to(sprite.source_rect.w);
    json.at("height").get_to(sprite.source_rect.h);
    json.at("anchor_x").get_to(sprite.anchor.x);
    json.at("anchor_y").get_to(sprite.anchor.y);
}
