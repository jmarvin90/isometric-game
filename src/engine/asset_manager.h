#ifndef ASSET_MANAGER
#define ASSET_MANAGER

#include <unordered_map>
#include <memory>
#include <utility>

#include "spritesheet.h"

class AssetManager{

    private:
        std::unordered_map<std::string, std::unique_ptr<SpriteSheet>> sprite_sheets;
    
    public:
        AssetManager() = default;
        void add_spritesheet(std::string spritesheet_name, const std::string& image_path, const std::string& atlas_path, SDL_Renderer* renderer);
        void remove_spritesheet(const std::string& spritesheet_name);
        const Sprite* get_sprite(std::string sprite_name) const;
        void get_sprites_of_type(const uint8_t sprite_type, std::vector<std::pair<std::string, const Sprite*>>& sprites) const;
};

#endif