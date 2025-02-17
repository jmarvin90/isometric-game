#include <utility>
#include <cstdlib>
#include <string>
#include <iostream>

#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <spdlog/spdlog.h>

#include "spritesheet.h"

SpriteSheet::SpriteSheet(
    const std::string& image_path, 
    const std::string& atlas_path,
    SDL_Renderer* renderer
): 
    image_path{image_path}, 
    atlas_path{atlas_path} 
{
    spdlog::info("Creating spritesheet for " + image_path);

    // Load the texture
    SDL_Surface* surface {IMG_Load(image_path.c_str())};
    if (!surface) {
        spdlog::info(
            "Could not load texture from path: " + 
            image_path
        );
    }

    spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if (!spritesheet) {
        spdlog::info(
            "Could not load texture from surface using image: " +
            image_path
        );
    }
    
    SDL_FreeSurface(surface);

    // Load the xml Atlas file
    rapidxml::file<> xml_file{atlas_path.c_str()};
    rapidxml::xml_document<> xml_doc;
    xml_doc.parse<0>(xml_file.data());
    rapidxml::xml_node<>* node = xml_doc.first_node();              // TextureAtlas

    // For each SubTexture node, create an entry in sprites
    for (
        rapidxml::xml_node<>* _iternode = node->first_node();       // SubTexture
        _iternode; 
        _iternode = _iternode->next_sibling()
    ) {
        /* 
            TODO: investigate why it's not possible (whether it should be possible)
            to do the emplace without having to use SDL_Rect here. 
            What are the implications? What's the difference to try_emplace?
        */

        sprites.emplace(
            _iternode->first_attribute("name")->value(),
            SDL_Rect{
                std::atoi(_iternode->first_attribute("x")->value()),
                std::atoi(_iternode->first_attribute("y")->value()),
                std::atoi(_iternode->first_attribute("width")->value()),
                std::atoi(_iternode->first_attribute("height")->value())
            }
        );
    }    
}

SpriteSheet::~SpriteSheet() {
    spdlog::info("SpriteSheet destructor called.");
    SDL_DestroyTexture(spritesheet);
}

SDL_Rect& SpriteSheet::get_sprite_rect(const std::string& sprite_name) {
    return sprites.at(sprite_name);
}

SDL_Texture* SpriteSheet::get_spritesheet_texture() {
    return spritesheet;
}