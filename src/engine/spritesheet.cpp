#include <utility>
#include <cstdlib>
#include <string>
#include <iostream>
#include <optional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <spdlog/spdlog.h>

#include "spritesheet.h"

bool operator==(const SDL_Rect &lhs, const SDL_Rect &rhs)
{
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.h == rhs.h &&
           lhs.w == rhs.w;
}

SpriteDefinition::SpriteDefinition(const rapidxml::xml_node<> *xml_definition)
    : texture_rect{
          std::atoi(xml_definition->first_attribute("x")->value()),
          std::atoi(xml_definition->first_attribute("y")->value()),
          std::atoi(xml_definition->first_attribute("width")->value()),
          std::atoi(xml_definition->first_attribute("height")->value())} {}

TileSpriteDefinition::TileSpriteDefinition(rapidxml::xml_node<> *xml_definition)
    : SpriteDefinition(xml_definition), connection{
                                            uint8_t(std::atoi(xml_definition->first_attribute("connection")->value()))} {}