#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "transform.h"
#include "sprite.h"
#include "constants.h"

// Allow us to perform substraction with a vec2 and an SDL_Rect
glm::vec2& operator-(glm::vec2& lhs, const SDL_Rect& rhs) {
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

SDL_FRect get_render_target(const Transform& transform, const Sprite& sprite, const glm::ivec2& camera) {
    glm::vec2 position {transform.position + sprite.offset};
    position -= camera;
    return SDL_FRect {
        position.x,
        position.y,
        static_cast<float>(sprite.source_rect.w),
        static_cast<float>(sprite.source_rect.h)
    };
}

void render_sprite(
    SDL_Renderer* renderer,
    const glm::ivec2& camera_position,
    const SDL_Rect& render_rect,
    const Transform& transform, 
    const Sprite& sprite,
    bool render_bounding_box
) {
    SDL_FRect dest_rect {get_render_target(transform, sprite, camera_position)};

    SDL_RenderCopyExF(
        renderer,
        sprite.texture,
        &sprite.source_rect,
        &dest_rect,
        transform.rotation,
        NULL,
        SDL_FLIP_NONE
    );

    if (render_bounding_box && transform.z_index != 0) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderDrawRectF(renderer, &dest_rect);
    }
}

void render_sprites(
    entt::registry& registry, 
    const glm::ivec2& camera_position, 
    SDL_Renderer* renderer, 
    const SDL_Rect& render_clip_rect, 
    bool render_bounding_box
) {
    auto sprites = registry.view<Transform, Sprite>();
    sprites.use<Transform>();
    for (auto entity: sprites) {
        const auto& transform {sprites.get<Transform>(entity)};
        const auto& sprite {sprites.get<Sprite>(entity)};
        render_sprite(renderer, camera_position, render_clip_rect, transform, sprite, render_bounding_box);
    }
}

#endif