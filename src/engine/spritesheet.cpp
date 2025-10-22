#include <spritesheet.h>
// #include <rapidjson/document.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <spdlog/spdlog.h>


SpriteSheet::SpriteSheet(
    const std::string spritesheet_path,
    const std::string atlas_path, 
    SDL_Renderer* renderer
) {
    SDL_Surface* surface {IMG_Load(spritesheet_path.c_str())};
    if (!surface) {
        spdlog::info(
            "Could not load texture from path: " +
            spritesheet_path
        );
    };

    spritesheet = SDL_CreateTextureFromSurface(renderer, surface);
    if (!spritesheet) {
        spdlog::info(
            "Could not load texture from surface using image: " +
            spritesheet_path
        );
    }

    SDL_FreeSurface(surface);
}

SpriteSheet::~SpriteSheet() {
    SDL_DestroyTexture(spritesheet);
}
