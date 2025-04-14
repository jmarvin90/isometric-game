#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <unordered_map>
#include <optional>

#include <SDL2/SDL.h>
#include <string>


class SpriteDefinition {
    public:
        SDL_Rect texture_rect;
        char connection;
        SpriteDefinition() = default;
        SpriteDefinition(SDL_Rect texture_rect, char connection): texture_rect{texture_rect}, connection{connection} {};
        ~SpriteDefinition() = default;
};

class SpriteSheet {
    const std::string image_path;
    const std::string atlas_path;
    SDL_Texture* spritesheet;

    public:
        std::unordered_map<std::string, const SpriteDefinition> sprites;

        SpriteSheet(const std::string& image_path,  const std::string& atlas_path, SDL_Renderer* renderer);
        SpriteSheet(const SpriteSheet&) = default;
        ~SpriteSheet();

        const SpriteDefinition& get_sprite_definition(const std::string& sprite_name) const;
        
        // Not yet implemented/used
        const SDL_Rect& get_sprite_rect(const std::string& sprite_name) const;
        SDL_Texture* get_spritesheet_texture() const;

        // Perhaps doesn't need to be std::optional for now as some conditions on before
        // the lookup can give us confidence on whether we'll return a value or not
        const std::optional<std::string_view> reverse_lookup(const SDL_Rect& target_rect) const;
};

#endif