#include <utility>
#include <cstdlib>
#include <string>
#include <iostream>
#include <optional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <spdlog/spdlog.h>

#include "spritesheet.h"
#include "components/sprite.h"

bool operator==(const SDL_Rect& lhs, const SDL_Rect& rhs)
{
    return lhs.x == rhs.x &&
        lhs.y == rhs.y &&
        lhs.h == rhs.h &&
        lhs.w == rhs.w;
}

SpriteSheet::SpriteSheet(const std::string& image_path, const std::string& atlas_path, SDL_Renderer* renderer)
    : image_path{ image_path }, atlas_path{ atlas_path }
{
    spdlog::info("Creating spritesheet for " + image_path);

    // Load the texture
    SDL_Surface* surface{ IMG_Load(image_path.c_str()) };
    if (!surface)
    {
        spdlog::info(
            "Could not load texture from path: " +
            image_path);
    }

    spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if (!spritesheet)
    {
        spdlog::info(
            "Could not load texture from surface using image: " +
            image_path);
    }

    SDL_FreeSurface(surface);

    // Load the xml Atlas file
    rapidxml::file<> xml_file{ atlas_path.c_str() };
    rapidxml::xml_document<> xml_doc;
    xml_doc.parse<0>(xml_file.data());
    rapidxml::xml_node<>* node = xml_doc.first_node(); // TextureAtlas

    // For each SubTexture node, create an entry in sprites
    for (
        rapidxml::xml_node<>* _iternode = node->first_node(); // SubTexture
        _iternode;
        _iternode = _iternode->next_sibling())
    {
        /*
            TODO: investigate why it's not possible (whether it should be possible)
            to do the emplace without having to use SDL_Rect here.
            What are the implications? What's the difference to try_emplace?
        */

        sprites.emplace(
            _iternode->first_attribute("name")->value(),
            Sprite{ spritesheet, _iternode }
        );
    }
}

SpriteSheet::~SpriteSheet()
{
    spdlog::info("SpriteSheet destructor called.");
    SDL_DestroyTexture(spritesheet);
}

const Sprite* SpriteSheet::get_sprite_definition(std::string sprite_name) const {
    auto result { sprites.find(sprite_name) };
    return (result != sprites.end() ? & result->second : nullptr);
}

SDL_Texture* SpriteSheet::get_spritesheet_texture() const {
    return spritesheet;
}

void SpriteSheet::get_sprites_of_type(const uint8_t sprite_type, std::vector<std::pair<std::string, const Sprite*>> out_sprites) const {
    for (const auto& [name, sprite]: sprites) {
        if (sprite.sprite_type == sprite_type) {
            out_sprites.push_back(std::pair<std::string, const Sprite*>(name, &sprite));
        }
    }
}

std::string SpriteSheet::get_sprite_name(const Sprite* target_sprite) const {
    for (const auto& [name, sprite]: sprites) {
        if (&sprite == target_sprite) {
            return name;
        }
    }
    // TODO: better than this!
    return std::string {};
}
