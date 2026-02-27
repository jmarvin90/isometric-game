#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <SDL2/SDL.h>
#include <components/navigation_component.h>
#include <components/sprite_component.h>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility.h>

struct SpriteSheetEntry {
    SpriteComponent sprite_definition;
    std::optional<NavigationComponent> navigation_definition;
    SpriteSheetEntry(SpriteComponent sprite_def, std::optional<NavigationComponent> nav_def)
        : sprite_definition { sprite_def }
        , navigation_definition { nav_def }
    {
    }
};

class SpriteSheet {
    std::unique_ptr<SDL_Texture, Utility::SDLDestroyer> spritesheet;
    std::unordered_map<std::string, SpriteSheetEntry> sprites;

public:
    SpriteSheet(
        const std::string spritesheet_path,
        const std::string atlas_path,
        std::unique_ptr<SDL_Renderer, Utility::SDLDestroyer>& renderer
    );

    const SpriteSheetEntry& get(const std::string name) const;

    SpriteSheet(const SpriteSheet&) = delete;
    SpriteSheet& operator=(const SpriteSheet&) = delete;
    SpriteSheet(SpriteSheet&&) = default;
    SpriteSheet& operator=(SpriteSheet&&) = default;
};

#endif