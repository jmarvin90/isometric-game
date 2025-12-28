#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <camera_component.h>
#include <components/grid_position_component.h>
#include <components/highlight_component.h>
#include <components/junction_component.h>
#include <components/mouse_component.h>
#include <components/navigation_component.h>
#include <components/segment_component.h>
#include <components/spatialmapcell_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <components/visibility_component.h>
#include <constants.h>
#include <imgui.h>
#include <position.h>
#include <systems/render_system.h>
#include <tilespec_component.h>

#include <entt/entt.hpp>

#include <algorithm>
#include <utility>
#include <vector>

int total_count { 0 };

namespace {

[[maybe_unused]] bool transform_comparison(const std::pair<const TransformComponent*, const SpriteComponent*> lhs, const std::pair<const TransformComponent*, const SpriteComponent*> rhs)
{
    return (
        lhs.first->z_index < rhs.first->z_index || (lhs.first->z_index == rhs.first->z_index && lhs.first->position.y < rhs.first->position.y)
    );
}

}; // namespace

[[maybe_unused]] void RenderSystem::render_segment_lines(
    const entt::registry& registry, SDL_Renderer* renderer
)
{
    auto segments = registry.view<SegmentComponent>();
    for (auto [entity, segment] : segments.each()) {
        glm::ivec2 start { WorldPosition { registry, segment.start }.to_screen_position(registry) };
        glm::ivec2 end { WorldPosition { registry, segment.end }.to_screen_position(registry) };
        SDL_RenderDrawLine(renderer, start.x, start.y, end.x, end.y);
    }
}

void RenderSystem::update(entt::registry& registry)
{
    registry.clear<VisibilityComponent>();
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    auto spatialmap_cells { registry.view<SpatialMapCellComponent>() };

    for (auto [entity, cell] : spatialmap_cells.each()) {
        if (SDL_HasIntersection(&cell.cell, &camera.camera_rect)) {
            for (entt::entity renderable : cell.entities) {
                registry.emplace<VisibilityComponent>(renderable);
            }
        }
    }
}

void RenderSystem::render(
    entt::registry& registry,
    SDL_Renderer* renderer,
    [[maybe_unused]] const SDL_DisplayMode& display_mode,
    const CameraComponent& camera
)
{
    auto renderables_view { registry.view<VisibilityComponent, TransformComponent, SpriteComponent>() };
    std::vector<std::pair<TransformComponent*, SpriteComponent*>> renderables_pile;
    renderables_pile.reserve(100);

    for (auto [entity, transform, sprite] : renderables_view.each()) {
        renderables_pile.emplace_back(&transform, &sprite);
    }

    std::sort(renderables_pile.begin(), renderables_pile.end(), transform_comparison);

    for (auto [transform, sprite] : renderables_pile) {
        glm::ivec2 output_position { WorldPosition(transform->position).to_screen_position(camera) };

        SDL_Rect target_rect {
            output_position.x,
            output_position.y,
            sprite->source_rect.w,
            sprite->source_rect.h
        };

        SDL_RenderCopyEx(
            renderer,
            sprite->texture,
            &sprite->source_rect,
            &target_rect,
            transform->rotation,
            NULL,
            SDL_FLIP_NONE
        );
    }
}

void RenderSystem::render_highlights(
    entt::registry& registry,
    SDL_Renderer* renderer,
    [[maybe_unused]] const SDL_DisplayMode& display_mode
)
{
    auto renderables { registry.view<VisibilityComponent, TransformComponent, HighlightComponent>() };
    for (auto [entity, transform, highlight] : renderables.each()) {

        std::vector<SDL_Point> absolute_points;
        absolute_points.reserve(highlight.points.size());

        for (auto point : highlight.points) {
            glm::ivec2 screen_position {
                WorldPosition(transform.position).to_screen_position(registry)
            };

            absolute_points.emplace_back(
                SDL_Point { screen_position.x + point.x, screen_position.y + point.y }
            );
        }

        SDL_SetRenderDrawColor(
            renderer,
            highlight.colour.r,
            highlight.colour.g,
            highlight.colour.b,
            highlight.colour.a
        );

        SDL_RenderDrawLines(renderer, absolute_points.data(), absolute_points.size());
    }
}

void RenderSystem::render_imgui_ui(
    entt::registry& registry,
    SDL_Renderer* renderer,
    const MouseComponent& mouse,
    const TileMapComponent& tilemap
)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // The mouse and world positions
    const glm::ivec2 screen_position { mouse.window_current_position };
    const WorldPosition world_position { ScreenPosition(mouse.window_current_position).to_world_position(registry) };
    const GridPosition grid_position { world_position.to_grid_position(registry) };

    ImGui::SeparatorText("Mouse Position");

    ImGui::Text(
        "Mouse Screen position: (%s) (%s)",
        std::to_string(screen_position.x).c_str(),
        std::to_string(screen_position.y).c_str()
    );

    ImGui::Text(
        "Mouse World position: (%s) (%s)",
        std::to_string(glm::ivec2 { world_position }.x).c_str(),
        std::to_string(glm::ivec2 { world_position }.y).c_str()
    );

    ImGui::Text(
        "Mouse Grid position: (%s) (%s)",
        std::to_string(glm::ivec2 { grid_position }.x).c_str(),
        std::to_string(glm::ivec2 { grid_position }.y).c_str()
    );

    if (tilemap.highlighted_tile != entt::null) {
        const NavigationComponent* nav {
            registry.try_get<const NavigationComponent>(tilemap.highlighted_tile)
        };

        if (nav) {
            ImGui::Text(
                "Tile Connection Direction(s): (%d)",
                Direction::to_underlying(nav->directions)
            );
        }
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}