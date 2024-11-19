#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "transform.h"
#include "sprite.h"
#include "constants.h"


void render_sprite(
    SDL_Renderer* renderer,
    const SDL_Rect& camera,
    const SDL_Rect& render_rect,
    const Transform& transform, 
    const Sprite& sprite
) {

    SDL_Rect source_rect {0, 0, sprite.width_px, sprite.height_px};
    SDL_Rect dest_rect {
        (static_cast<int>(transform.position.x) + sprite.horizontal_offset_px) - camera.x, 
        (static_cast<int>(transform.position.y) + sprite.vertical_offset_px) - camera.y, 
        sprite.width_px,
        sprite.height_px
    };

    if (SDL_HasIntersection(&dest_rect, &render_rect)) {
        SDL_RenderCopyEx(
            renderer,
            sprite.texture,
            &source_rect,
            &dest_rect,
            transform.rotation,
            NULL,
            SDL_FLIP_NONE
        );
    }
}

#endif