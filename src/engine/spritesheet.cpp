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

void copy_sprite_vertical_component(
    SDL_Surface*& input_surface,
    SDL_Surface*& vertical_components_surface,
    SDL_Surface*& mousemap_surface,
    const Sprite* target_sprite,
    const int height_delta
) {
    if (!vertical_components_surface) {
        vertical_components_surface = SDL_CreateRGBSurface(
            0,
            input_surface->w,
            input_surface->h,
            32,
            input_surface->format->Rmask,
            input_surface->format->Gmask,
            input_surface->format->Bmask,
            input_surface->format->Amask
        );
    }

    if (!mousemap_surface) {
        mousemap_surface = IMG_Load(constants::MOUSE_MAP_PNG_PATH.c_str());
    }

    // [[maybe_unused]]SDL_Rect top_portion {
    //     target_sprite->source_rect.x, target_sprite->source_rect.y,
    //     target_sprite->source_rect.w, height_delta
    // };

    [[maybe_unused]]SDL_Rect top_portion {
        target_sprite->source_rect.x, target_sprite->source_rect.y,
        target_sprite->source_rect.w, height_delta
    };

    SDL_BlitSurface(
        input_surface, 
        &top_portion,
        vertical_components_surface,
        &top_portion
    );

    SDL_LockSurface(input_surface);
    SDL_LockSurface(vertical_components_surface);
    SDL_LockSurface(mousemap_surface);

    [[maybe_unused]] Uint32* source_pixels {(Uint32*)input_surface->pixels};
    [[maybe_unused]] Uint32* target_pixels {(Uint32*)vertical_components_surface->pixels};
    [[maybe_unused]] Uint32* mousemap_pixels {(Uint32*)mousemap_surface->pixels};

    [[maybe_unused]] int source_pitch {input_surface->pitch / 4};
    [[maybe_unused]] int target_pitch {vertical_components_surface->pitch / 4};
    [[maybe_unused]] int mousemap_pitch {mousemap_surface->pitch / 4};

    for (int y = 0; y < constants::TILE_SIZE.y / 2; y++) {
        for (int x = 0; x < constants::TILE_SIZE.x; x++) {

            int x_pixel {target_sprite->source_rect.x + x};

            int y_pixel {
                target_sprite->source_rect.y +
                top_portion.h + 
                y
            };

            [[maybe_unused]] Uint32* source_pixel {&source_pixels[y_pixel * source_pitch + x_pixel]};
            [[maybe_unused]] Uint32* target_pixel {&target_pixels[y_pixel * target_pitch + x_pixel]};
            [[maybe_unused]] Uint32* mousemap_pixel {&mousemap_pixels[y * mousemap_pitch + x]};

            SDL_Color source_pixel_colour {0x00, 0x00, 0x00, SDL_ALPHA_TRANSPARENT};
            SDL_Color mousemap_pixel_colour {0x00, 0x00, 0x00, SDL_ALPHA_TRANSPARENT};

            SDL_GetRGBA(
                *source_pixel, 
                input_surface->format,
                &source_pixel_colour.r,
                &source_pixel_colour.g,
                &source_pixel_colour.b,
                &source_pixel_colour.a
            );

            SDL_GetRGBA(
                *mousemap_pixel, 
                mousemap_surface->format,
                &mousemap_pixel_colour.r,
                &mousemap_pixel_colour.g,
                &mousemap_pixel_colour.b,
                &mousemap_pixel_colour.a
            );

            if (
                source_pixel_colour.a != SDL_ALPHA_TRANSPARENT && 
                mousemap_pixel_colour.b == 0 && (
                    mousemap_pixel_colour.r == 255 ||
                    mousemap_pixel_colour.g == 255
                )
            ) {
                *target_pixel = *source_pixel;
            }
        }
    }
    
    SDL_UnlockSurface(input_surface);
    SDL_UnlockSurface(vertical_components_surface);
    SDL_UnlockSurface(mousemap_surface);
}

SpriteSheet::SpriteSheet(const std::string& image_path, const std::string& atlas_path, SDL_Renderer* renderer)
    : image_path{ image_path }, atlas_path{ atlas_path }
{
    spdlog::info("Creating spritesheet for " + image_path);

    // Load the texture
    // spritesheet = IMG_LoadTexture(image_path.c_str());
    
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

    // Load the xml Atlas file
    rapidxml::file<> xml_file{ atlas_path.c_str() };
    rapidxml::xml_document<> xml_doc;
    xml_doc.parse<0>(xml_file.data());
    rapidxml::xml_node<>* node = xml_doc.first_node(); // TextureAtlas

    [[maybe_unused]] SDL_Surface* vertical_components_surface { nullptr };
    [[maybe_unused]] SDL_Surface* mousemap_surface { nullptr };

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

        std::string sprite_name {_iternode->first_attribute("name")->value()};

        [[maybe_unused]] const Sprite* emplaced_sprite {
                &sprites.emplace(
                sprite_name,
                Sprite{ this, _iternode }
            ).first->second
        };

        const int height_delta {emplaced_sprite->source_rect.h - constants::STANDARD_BASE_TILE_HEIGHT};

        // It's all going wrong here
        // This portion of code is only called in case there appears to be a vertical component to the sprite
        // however, the are some tile sprites which do not have a portion outside of the normal footprint, but
        // which DO have a vertical component
        if (
            emplaced_sprite->sprite_type == constants::SpriteType::TILE_SPRITE &&
            emplaced_sprite->has_vertical
        ) {
            spdlog::info(sprite_name);
            copy_sprite_vertical_component(surface, vertical_components_surface, mousemap_surface, emplaced_sprite, height_delta);
        }
    }

    if (vertical_components_surface && !spritesheet_vertical_components) {
        spritesheet_vertical_components = SDL_CreateTextureFromSurface(renderer, vertical_components_surface);
    }
    
    SDL_FreeSurface(vertical_components_surface);
    SDL_FreeSurface(mousemap_surface);
    SDL_FreeSurface(surface);
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

void SpriteSheet::get_sprites_of_type(const uint8_t sprite_type, std::vector<std::pair<std::string, const Sprite*>>& out_sprites) const {
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
