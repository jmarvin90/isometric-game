#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <json_parse.h>
#include <nlohmann/json.hpp>
#include <sprite.h>
#include <spritemask.h>
#include <string>

SpriteDefinition::SpriteDefinition(nlohmann::json input, SDL_Surface* surface)
    : name { input["name"].get<std::string>() }
    , source_rect { input["source_rect"].get<SDL_Rect>() }
    , sprite_type { input["type"].get<SpriteType>() }
    , anchor { input["anchor"].get<glm::ivec2>() }
    , directions {
        input.contains("directions")
            ? Direction::TDirection(input["directions"].get<uint8_t>())
            : Direction::TDirection::NO_DIRECTION
    }
    , spritemask { SpriteMask::get_mask(surface, source_rect, name), //
                   glm::ivec2 { 1, 1 }, //
                   glm::ivec2 { source_rect.w, source_rect.h } }
{
}