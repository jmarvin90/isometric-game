#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include <components/sprite.h>
#include <components/transform.h>
#include <systems/render.h>
#include <scene.h>

Renderer::Renderer(
    SDL_Window* window, 
    const SDL_DisplayMode& display_mode, 
    uint32_t render_flags, 
    int index
)
    : renderer {SDL_CreateRenderer(window, index, render_flags)}
    , render_clip_rect{20, 20, display_mode.w - 40, display_mode.h - 40} 
    {

        spdlog::info("Renderer constructor called.");
        
        if (!renderer) {
            spdlog::error("Could not initialise the SDL Renderer.");
            // Realistically should exit here
        }

        SDL_RenderSetClipRect(renderer, &render_clip_rect);    
    }

Renderer::~Renderer() {
    spdlog::info("Renderer destructor called.");
    SDL_DestroyRenderer(renderer);
}

void Renderer::render_sprite(
    const glm::ivec2 camera_position,
    const Transform& transform,
    const Sprite& sprite
) const {
    // TODO: vec2 type mismatch
    const glm::vec2 position {
        transform.position.x - camera_position.x,
        transform.position.y - camera_position.y
    }; // + sprite.offset;

    SDL_FRect target_rect {
        position.x,
        position.y,
        static_cast<float>(sprite.source_rect.w),
        static_cast<float>(sprite.source_rect.h)
    };

    SDL_RenderCopyExF(
        renderer,
        sprite.texture,
        &sprite.source_rect,
        &target_rect,
        transform.rotation,
        NULL,
        SDL_FLIP_NONE
    );
}

// TODO: check how to (if it's possible) make the scene ref const
void Renderer::render(Scene& scene) const {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    auto sprites = scene.get_registry().view<Transform, Sprite>();
    const glm::ivec2 offset {scene.camera_position + scene.get_border_px()};
    const glm::ivec2 highlight_offset {0, -30};

    for (auto entity: sprites) {
        const auto& transform {sprites.get<Transform>(entity)};
        const auto& sprite {sprites.get<Sprite>(entity)};
        render_sprite(scene.camera_position, transform, sprite);
    }

    for (Tile& tile: scene.get_tiles()) {
        std::array<SDL_Point, 5> points {};

        Tile* highlighted_tile {scene.highlighted_tile()};

        tile.iso_sdl_points(
            points, 
            highlighted_tile && &tile == highlighted_tile
            ? offset + highlight_offset 
            : offset
        );

        SDL_SetRenderDrawColor(
            renderer,
            0, 0, 255, 255
        );

        SDL_RenderDrawLines(
            renderer,
            points.data(),
            5
        );
    }

    SDL_RenderPresent(renderer);
}
