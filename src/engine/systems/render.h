#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include "transform.h"
#include "sprite.h"
#include "constants.h"

SDL_Rect get_render_target(const Transform& transform, const Sprite& sprite, const SDL_Rect& camera) {
    return SDL_Rect {
        (static_cast<int>(transform.position.x) + sprite.horizontal_offset_px) - camera.x, 
        (static_cast<int>(transform.position.y) + sprite.vertical_offset_px) - camera.y, 
        sprite.width_px,
        sprite.height_px
    };
}

void render_bounding_box(const entt::registry& registry, const SDL_Rect& camera, SDL_Renderer* renderer) {
    auto vertical_tiles = registry.view<Transform, VerticalSprite>();
    vertical_tiles.use<Transform>();
    for (auto entity: vertical_tiles) {

        const Transform& transform {vertical_tiles.get<Transform>(entity)};
        const VerticalSprite& sprite {vertical_tiles.get<VerticalSprite>(entity)};

        SDL_Rect bounding_box {get_render_target(transform, sprite, camera)};
                
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }
}

void render_sprite(
    SDL_Renderer* renderer,
    const SDL_Rect& camera,
    const SDL_Rect& render_rect,
    const Transform& transform, 
    const Sprite& sprite
) {

    SDL_Rect source_rect {0, 0, sprite.width_px, sprite.height_px};
    SDL_Rect dest_rect {get_render_target(transform, sprite, camera)};

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