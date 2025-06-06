#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <unordered_map>
#include <optional>
#include <string>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <spdlog/spdlog.h>
#include <rapidxml/rapidxml_utils.hpp>
#include <rapidxml/rapidxml_print.hpp>

#include "components/sprite.h"

// TODO: std::string& -> std::string_view

bool operator==(const SDL_Rect& lhs, const SDL_Rect& rhs);

struct Sprite;

class SpriteSheet
{
    const std::string image_path;
    const std::string atlas_path;
    SDL_Texture* spritesheet;

public:
    std::unordered_map<std::string, const Sprite> sprites;

    SpriteSheet(const std::string& image_path, const std::string& atlas_path, SDL_Renderer* renderer);
    SpriteSheet(const SpriteSheet&) = default;
    ~SpriteSheet();

    const Sprite* get_sprite_definition(std::string sprite_name) const;
    SDL_Texture* get_spritesheet_texture() const;
    void get_sprites_of_type(const uint8_t sprite_type, std::vector<std::pair<std::string, const Sprite*>> out_sprites) const;
    
    // TODO: dubious; string pointer? string reference? string_view?
    std::string get_sprite_name(const Sprite* target_sprite) const;
};

#endif