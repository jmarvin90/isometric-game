#include <SDL2/SDL.h>
#include <spdlog/spdlog.h>

#include <components/sprite.h>
#include <components/transform.h>
#include <components/highlight.h>
#include <systems/render.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <scene.h>
#include <tilemap.h>

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
    const glm::vec2 position,
    const double rotation,
    const Sprite& sprite
) const {

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
        rotation,
        NULL,
        SDL_FLIP_NONE
    );
}

bool transform_comparison(
    [[maybe_unused]] const Transform& lhs,
    [[maybe_unused]] const Transform& rhs
) {
    return lhs.position.y < rhs.position.y;
    // return true;
}

// TODO: check how to (if it's possible) make the scene ref const
void Renderer::render(Scene& scene, const bool debug_mode) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    const glm::vec2 highlight_offset {0, 30};

    // scene.registry.sort<Transform>(transform_comparison);
    auto sprites = scene.registry.view<Transform>();
    std::optional<entt::entity> highlighted_tile {scene.tilemap.highlighted_tile()};

    for (auto entity: sprites) {
        const auto& transform {sprites.get<Transform>(entity)};
        const auto* sprite {scene.registry.try_get<Sprite>(entity)};
        const auto* highlight {scene.registry.try_get<Highlight>(entity)};
        const auto* tile_highlight {scene.registry.try_get<TileHighlight>(entity)};

        glm::vec2 offset {
            transform.position.x + scene.camera_position.x,
            transform.position.y + scene.camera_position.y
        };
        
        if (sprite) {
            render_sprite(offset, transform.rotation, *sprite);
        }

        if (highlighted_tile && entity == scene.tilemap.highlighted_tile()) {
            offset -= highlight_offset;
        }

        if (highlight) {
            draw_lines(highlight, offset);
        }

        if (tile_highlight) {
            draw_lines(tile_highlight, offset);
        }
    }

    if (debug_mode) {
        render_imgui_ui(scene);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
    const SDL_Rect my_rect{896, 15, 256, 128};
    SDL_RenderDrawRect(renderer, &my_rect);

    SDL_RenderPresent(renderer);
}

void Renderer::render_imgui_ui(const Scene& scene) const {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // The mouse and world positions
    const glm::ivec2 screen_position {scene.mouse_position.on_screen()};
    const glm::ivec2 world_position {scene.mouse_position.in_world()};
    const glm::vec2 grid_position_gross {scene.mouse_position.on_grid_gross()};
    const glm::ivec2 grid_position {scene.mouse_position.on_grid()};

    ImGui::SeparatorText("Mouse Position");
    ImGui::Text(
        "Mouse Screen position: (%s) (%s)", 
        std::to_string(screen_position.x).c_str(), 
        std::to_string(screen_position.y).c_str()
    );

    ImGui::Text(
        "Mouse World position: (%s) (%s)", 
        std::to_string(world_position.x).c_str(), 
        std::to_string(world_position.y).c_str()
    );

    ImGui::Text(
        "Mouse Grid position (gross): (%s) (%s)",
        std::to_string(grid_position_gross.x).c_str(),
        std::to_string(grid_position_gross.y).c_str()
    );

    ImGui::Text(
        "Mouse Grid position: (%s) (%s)",
        std::to_string(grid_position.x).c_str(),
        std::to_string(grid_position.y).c_str()
    );

    ImGui::SeparatorText("Selected Tile Position");
    if (scene.tilemap.highlighted_tile()) {
        
        // ImGui::Text(
        //     "Selected Tile World position: (%s) (%s)", 
        //     std::to_string(tilemap.selected_tile->world_position().x).c_str(), 
        //     std::to_string(tilemap.selected_tile->world_position().y).c_str()
        // );
    
        // ImGui::Text(
        //     "Selected Tile Grid position: (%s) (%s)",
        //     std::to_string(tilemap.selected_tile->get_grid_position().x).c_str(),
        //     std::to_string(tilemap.selected_tile->get_grid_position().y).c_str()
        // );    
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}
