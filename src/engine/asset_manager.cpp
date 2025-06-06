#include "asset_manager.h"

void AssetManager::add_spritesheet(
    std::string spritesheet_name, 
    const std::string& image_path, 
    const std::string& atlas_path, 
    SDL_Renderer* renderer
) {
    sprite_sheets.emplace(
        spritesheet_name, std::make_unique<SpriteSheet>(image_path, atlas_path, renderer)
    );
}

const Sprite* AssetManager::get_sprite(std::string sprite_name) const {
    for (const auto& [name, sprite_sheet]: sprite_sheets) {
        const Sprite* result { sprite_sheet->get_sprite_definition(sprite_name)};
        if (result) {
            return result;
        }
    }
    return nullptr;
}

void AssetManager::get_sprites_of_type(const uint8_t sprite_type, std::vector<std::pair<std::string, const Sprite*>>& out_sprites) const {
    for (const auto& [name, sprite_sheet]: sprite_sheets) {
        sprite_sheet->get_sprites_of_type(sprite_type, out_sprites);
    }
}