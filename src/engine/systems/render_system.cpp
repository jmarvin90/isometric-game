#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>

#include <systems/render_system.h>
#include <position.h>

#include <components/transform_component.h>
#include <components/sprite_component.h>
#include <components/highlight_component.h>
#include <components/mouse_component.h>

bool transform_comparison(
    [[maybe_unused]] const TransformComponent& lhs,
    [[maybe_unused]] const TransformComponent& rhs
) {
    return (
        lhs.z_index < rhs.z_index ||
        (
            lhs.z_index == rhs.z_index && 
            lhs.position.y < rhs.position.y
        )
    );
}

void render_sprite(
    entt::registry& registry,
    SDL_Renderer* renderer,
    const TransformComponent& transform,
    const SpriteComponent& sprite
) {

    const glm::ivec2 screen_position {
        WorldPosition{transform.position}.to_screen_position(registry)
    };

    SDL_Rect target_rect {
        screen_position.x,
        screen_position.y,
        sprite.source_rect.w,
        sprite.source_rect.h
    };

    SDL_RenderCopyEx(
        renderer,
        sprite.texture,
        &sprite.source_rect,
        &target_rect,
        transform.rotation,
        NULL,
        SDL_FLIP_NONE
    );
}

void render_imgui_ui(entt::registry& registry, SDL_Renderer* renderer) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    const MouseComponent& mouse {registry.ctx().get<const MouseComponent>()};

    // The mouse and world positions
    const glm::ivec2 screen_position {mouse.window_current_position};

    const WorldPosition world_position {
        ScreenPosition(mouse.window_current_position).to_world_position(registry)
    };

    const GridPosition grid_position {world_position.to_grid_position(registry)};

    ImGui::SeparatorText("Mouse Position");
    ImGui::Text(
        "Mouse Screen position: (%s) (%s)", 
        std::to_string(screen_position.x).c_str(), 
        std::to_string(screen_position.y).c_str()
    );

    ImGui::Text(
        "Mouse World position: (%s) (%s)", 
        std::to_string(glm::ivec2{world_position}.x).c_str(), 
        std::to_string(glm::ivec2{world_position}.y).c_str()
    );

    ImGui::Text(
        "Mouse Grid position: (%s) (%s)",
        std::to_string(glm::ivec2{grid_position}.x).c_str(),
        std::to_string(glm::ivec2{grid_position}.y).c_str()
    );

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void RenderSystem::render(
    entt::registry& registry,
    SDL_Renderer* renderer,
    [[maybe_unused]] const bool debug_mode
) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    registry.sort<TransformComponent>(transform_comparison);
    auto sprites = registry.view<TransformComponent, SpriteComponent>();

    for (auto [entity, transform, sprite]: sprites.each()) {

        [[maybe_unused]] const auto* highlight {registry.try_get<HighlightComponent>(entity)};
        [[maybe_unused]] const auto* tile_highlight {registry.try_get<TileHighlightComponentComponent>(entity)};

        glm::ivec2 screen_position {
            WorldPosition(transform.position).to_screen_position(registry)
        };

        render_sprite(registry, renderer, transform, sprite);

        // if (highlight && debug_mode) {
        //     draw_lines(highlight, screen_position);
        // }

        // if (tile_highlight && debug_mode) {
        //     draw_lines(tile_highlight, screen_position);
        // }
    }

    if (debug_mode) {
        render_imgui_ui(registry, renderer);
    }

    SDL_RenderPresent(renderer);
}