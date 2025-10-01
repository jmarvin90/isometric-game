#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include "spritesheet.h"
#include "constants.h"
#include "utils.h"

struct Sprite
{
    // SDL_Texture* texture;
    SpriteSheet* spritesheet;
    SDL_Rect source_rect;
    uint8_t sprite_type{ 0 };
    glm::vec2 offset{ 0, 0 };
    uint8_t connection{ 0 };
    bool has_vertical { 0 };
    uint8_t direction{ 0 };

    Sprite(const Sprite& in_sprite) = default;

    Sprite(
        // SDL_Texture* texture,
        SpriteSheet* spritesheet,
        const SDL_Rect source_rect,
        const glm::vec2 in_offset = { 0, 0 }
    )
        : spritesheet { spritesheet }
        // : texture{ texture }
        , source_rect{ source_rect }
        , offset{ in_offset }
    {
    }

    Sprite(SpriteSheet* spritesheet, const rapidxml::xml_node<>* xml_definition)
        : spritesheet { spritesheet }
        // : texture{ texture }
        , source_rect{
            std::atoi(xml_definition->first_attribute("x")->value()),
            std::atoi(xml_definition->first_attribute("y")->value()),
            std::atoi(xml_definition->first_attribute("width")->value()),
            std::atoi(xml_definition->first_attribute("height")->value()) }
        , sprite_type{
            static_cast<uint8_t>(
                xml_definition->first_attribute("sprite_type") ? 
                std::atoi(xml_definition->first_attribute("sprite_type")->value())
                : 0 
            )}
        , connection {
            static_cast<uint8_t>(
                xml_definition->first_attribute("connection") ?
                std::atoi(xml_definition->first_attribute("connection")->value())
                : 0
            )}
        , has_vertical {
            static_cast<bool>(
                xml_definition->first_attribute("has_vertical") ?
                std::atoi(xml_definition->first_attribute("has_vertical")->value())
                : 0
            )}
        , direction {
            static_cast<uint8_t>(
                xml_definition->first_attribute("direction") ?
                std::atoi(xml_definition->first_attribute("direction")->value())
                : 0
            )}
    {
    }
};

#endif