#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <components/grid_position_component.h>
#include <components/highlight_component.h>
#include <components/junction_component.h>
#include <components/mouse_component.h>
#include <components/navigation_component.h>
#include <components/segment_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <imgui.h>
#include <position.h>
#include <systems/render_system.h>
#include <tilespec_component.h>

#include <entt/entt.hpp>

namespace {
    bool transform_comparison([[maybe_unused]] const TransformComponent& lhs,
        [[maybe_unused]] const TransformComponent& rhs)
    {
        return (lhs.z_index < rhs.z_index || (lhs.z_index == rhs.z_index && lhs.position.y < rhs.position.y));
    }

    template <typename T>
    void draw_lines(SDL_Renderer* renderer,
        const T& line_component,
        const glm::ivec2 offset)
    {
        std::vector<SDL_Point> points;

        for (auto& point : line_component->points) {
            points.push_back(SDL_Point { point.x + offset.x, point.y + offset.y });
        }

        SDL_SetRenderDrawColor(renderer, line_component->colour.r,
            line_component->colour.g, line_component->colour.b,
            line_component->colour.a);

        SDL_RenderDrawLines(renderer, points.data(), points.size());
    }

    void render_sprite(entt::registry& registry,
        SDL_Renderer* renderer,
        const TransformComponent& transform,
        const SpriteComponent& sprite)
    {
        const glm::ivec2 screen_position {
            WorldPosition { transform.position }.to_screen_position(registry)
        };

        SDL_Rect target_rect { screen_position.x, screen_position.y,
            sprite.source_rect.w, sprite.source_rect.h };

        SDL_RenderCopyEx(renderer, sprite.texture, &sprite.source_rect, &target_rect,
            transform.rotation, NULL, SDL_FLIP_NONE);
    }

    void render_imgui_ui(entt::registry& registry, SDL_Renderer* renderer)
    {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        const MouseComponent& mouse { registry.ctx().get<const MouseComponent>() };
        const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };

        // The mouse and world positions
        const glm::ivec2 screen_position { mouse.window_current_position };

        const WorldPosition world_position {
            ScreenPosition(mouse.window_current_position)
                .to_world_position(registry)
        };

        const GridPosition grid_position { world_position.to_grid_position(registry) };

        ImGui::SeparatorText("Mouse Position");
        ImGui::Text("Mouse Screen position: (%s) (%s)",
            std::to_string(screen_position.x).c_str(),
            std::to_string(screen_position.y).c_str());

        ImGui::Text("Mouse World position: (%s) (%s)",
            std::to_string(glm::ivec2 { world_position }.x).c_str(),
            std::to_string(glm::ivec2 { world_position }.y).c_str());

        ImGui::Text("Mouse Grid position: (%s) (%s)",
            std::to_string(glm::ivec2 { grid_position }.x).c_str(),
            std::to_string(glm::ivec2 { grid_position }.y).c_str());

        if (tilemap.highlighted_tile != entt::null) {
            const NavigationComponent* nav {
                registry.try_get<const NavigationComponent>(tilemap.highlighted_tile)
            };

            if (nav) {
                ImGui::Text("Tile Connection Direction(s): (%d)",
                    Direction::to_underlying(nav->directions));
            }
        }

        ImGui::Render();
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    }

    [[maybe_unused]] void render_segment_lines(const entt::registry& registry, SDL_Renderer* renderer)
    {

        auto segments = registry.view<SegmentComponent>();

        for (auto [entity, segment] : segments.each()) {
            glm::ivec2 start { WorldPosition { registry, segment.start }.to_screen_position(registry) };
            glm::ivec2 end { WorldPosition { registry, segment.end }.to_screen_position(registry) };
            SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
        }
    }
}; // namespace

void RenderSystem::render(entt::registry& registry,
    SDL_Renderer* renderer,
    [[maybe_unused]] const bool debug_mode)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    registry.sort<TransformComponent>(transform_comparison);
    auto sprites = registry.view<TransformComponent, SpriteComponent>();
    [[maybe_unused]] const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };

    for (auto [entity, transform, sprite] : sprites.each()) {
        [[maybe_unused]] const auto* highlight {
            registry.try_get<HighlightComponent>(entity)
        };

        [[maybe_unused]] const auto* tile_highlight {
            registry.try_get<TileHighlightComponentComponent>(entity)
        };

        glm::ivec2 screen_position {
            WorldPosition(transform.position).to_screen_position(registry)
        };

        render_sprite(registry, renderer, transform, sprite);

        if (highlight && debug_mode) {
            draw_lines(renderer, highlight, screen_position);
        }

        // if (tile_highlight && debug_mode) {
        //     draw_lines(tile_highlight, screen_position);
        // }
    }

    if (debug_mode) {
        render_imgui_ui(registry, renderer);
        render_segment_lines(registry, renderer);
    }

    SDL_RenderPresent(renderer);
}