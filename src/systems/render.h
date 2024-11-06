#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "../components/transform.h"
#include "../components/sprite.h"
#include "../game/constants.h"


void render_sprite(
    SDL_Renderer* renderer,
    const SDL_Rect& camera,
    const SDL_Rect& render_rect,
    const std::unordered_map<int, SDL_Texture*>& textures,
    const Transform& transform, 
    const Sprite& sprite
) {

    SDL_Rect source_rect {0, 0, sprite.width_px, sprite.height_px};
    SDL_Rect dest_rect {
        (static_cast<int>(transform.position.x) + sprite.horitonzal_offset_px) - camera.x, 
        (static_cast<int>(transform.position.y) + sprite.vertical_offset_px) - camera.y, 
        sprite.width_px,
        sprite.height_px
    };

    SDL_Texture* texture {textures.find(sprite.texture_id)->second};

    if (SDL_HasIntersection(&dest_rect, &render_rect)) {
        SDL_RenderCopyEx(
            renderer,
            texture,
            &source_rect,
            &dest_rect,
            transform.rotation,
            NULL,
            SDL_FLIP_NONE
        );
    }
}

#endif