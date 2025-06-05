#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <unordered_map>
#include <optional>
#include <string>

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

    const Sprite& get_sprite_definition(const std::string& sprite_name) const;
    const SDL_Rect& get_sprite_rect(const std::string& sprite_name) const;
    SDL_Texture* get_spritesheet_texture() const;

    // Perhaps doesn't need to be std::optional for now as some conditions on before
    // the lookup can give us confidence on whether we'll return a value or not
    const std::optional<std::string_view> reverse_lookup(const SDL_Rect& target_rect) const;
};

#endif