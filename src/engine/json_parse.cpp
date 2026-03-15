#include <SDL2/SDL.h>
#include <components/sprite_component.h>
#include <glm/glm.hpp>
#include <json_parse.h>
#include <nlohmann/json.hpp>

namespace glm {
void to_json(nlohmann::json& j, const glm::ivec2& P)
{
    j = { { "x", P.x }, { "y", P.y } };
};

void to_json(nlohmann::json& j, const glm::vec2& P)
{
    j = { { "x", P.x }, { "y", P.y } };
}

void from_json(const nlohmann::json& j, glm::ivec2& P)
{
    P.x = j.at("x").get<int>();
    P.y = j.at("y").get<int>();
}

void from_json(const nlohmann::json& j, glm::vec2& P)
{
    P.x = j.at("x").get<double>();
    P.y = j.at("y").get<double>();
}
}

void to_json(nlohmann::json& j, const SDL_Rect& P)
{
    j = { { "x", P.x }, { "y", P.y }, { "h", P.h }, { "w", P.w } };
}

void from_json(const nlohmann::json& j, SDL_Rect& P)
{
    P.x = j.at("x").get<int>();
    P.y = j.at("y").get<int>();
    P.h = j.at("h").get<int>();
    P.w = j.at("w").get<int>();
}

void from_json(const nlohmann::json& json, SpriteComponent& sprite)
{
    // json.get_to(sprite);
    json["source_rect"].get_to(sprite.source_rect);
    // // json.at("x").get_to(sprite.source_rect.x);
    // // json.at("y").get_to(sprite.source_rect.y);
    // // json.at("w").get_to(sprite.source_rect.w);
    // // json.at("h").get_to(sprite.source_rect.h);
    json["anchor"].get_to(sprite.anchor);
    json["name"].get_to(sprite.name);
    // // json.at("anchor_x").get_to(sprite.anchor.x);
    // // json.at("anchor_y").get_to(sprite.anchor.y);
}