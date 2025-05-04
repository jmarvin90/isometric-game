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

bool operator==(const SDL_Rect& lhs, const SDL_Rect& rhs);

class SpriteDefinition
{
public:
    SDL_Rect texture_rect;
    SpriteDefinition() = default;
    SpriteDefinition(const rapidxml::xml_node<>* xml_definition);
    // SpriteDefinition(SDL_Rect texture_rect): texture_rect{texture_rect} {};
    ~SpriteDefinition() = default;
    virtual void load_sprites_from_xml() = delete;
};

class TileSpriteDefinition : public SpriteDefinition
{
public:
    TileSpriteDefinition() = default;
    uint8_t connection;
    TileSpriteDefinition(rapidxml::xml_node<>* xml_definition);
    // TileSpriteDefinition(SDL_Rect texture_rect, uint8_t connection)
    //     : SpriteDefinition{texture_rect}
    //     , connection{connection} {};
};

template <typename T>
class SpriteSheet
{
    const std::string image_path;
    const std::string atlas_path;
    SDL_Texture* spritesheet;

public:
    std::unordered_map<std::string, const T> sprites;

    SpriteSheet(const std::string& image_path, const std::string& atlas_path, SDL_Renderer* renderer)
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
                T{ _iternode });
        }
    };

    SpriteSheet(const SpriteSheet&) = default;

    ~SpriteSheet()
    {
        spdlog::info("SpriteSheet destructor called.");
        SDL_DestroyTexture(spritesheet);
    }

    const T& get_sprite_definition(const std::string& sprite_name) const { return sprites.at(sprite_name); };

    // Not yet implemented/used
    const SDL_Rect& get_sprite_rect(const std::string& sprite_name) const
    {
        return sprites.at(sprite_name).texture_rect;
    };

    SDL_Texture* get_spritesheet_texture() const
    {
        return spritesheet;
    }

    // Perhaps doesn't need to be std::optional for now as some conditions on before
    // the lookup can give us confidence on whether we'll return a value or not
    const std::optional<std::string_view> reverse_lookup(const SDL_Rect& target_rect) const
    {
        auto it = std::find_if(
            std::begin(sprites),
            std::end(sprites),
            [&target_rect](auto&& p)
            { return p.second.texture_rect == target_rect; });

        if (it == std::end(sprites))
            return std::nullopt;

        return it->first;
    }
};

#endif