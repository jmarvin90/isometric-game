#include <SDL2/SDL.h>

#include "render.h"

Renderer::Renderer(SDL_Window* window, const SDL_DisplayMode& display_mode, uint32_t render_flags, int index): 
    renderer {SDL_CreateRenderer(window, index, render_flags)},
    render_clip_rect{20, 20, display_mode.w - 40, display_mode.h - 40} {
        
        if (!renderer) {
            spdlog::error("Could not initialise the SDL Renderer.");
        }

        SDL_RenderSetClipRect(renderer, &render_clip_rect);    
    }

Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer);
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

void Renderer::render_sprite(
    const glm::ivec2& camera_position,
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

void Renderer::render_sprites(
    const entt::registry& registry, 
    const glm::ivec2& camera_position,
    bool render_bounding_box
) {
    auto sprites = registry.view<Transform, Sprite>();
    sprites.use<Transform>();
    for (auto entity: sprites) {
        const auto& transform {sprites.get<Transform>(entity)};
        const auto& sprite {sprites.get<Sprite>(entity)};
        render_sprite(camera_position, transform, sprite, render_bounding_box);
    }
}

void Renderer::render(const entt::registry& registry, const glm::ivec2& camera_position, bool render_bounding_box) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    render_sprites(registry, camera_position, render_bounding_box);

}
