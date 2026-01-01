#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <camera_component.h>
#include <components/grid_position_component.h>
#include <components/highlight_component.h>
#include <components/junction_component.h>
#include <components/mouse_component.h>
#include <components/navigation_component.h>
#include <components/screen_position_component.h>
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
#include <optional>
#include <utility>
#include <vector>

int total_count { 0 };

namespace {

struct Renderable {
    const TransformComponent* transform;
    const ScreenPositionComponent* screen_position;
    const SpriteComponent* sprite;
    std::optional<const HighlightComponent*> highlight;
    Renderable(
        const TransformComponent* transform,
        const ScreenPositionComponent* screen_position,
        const SpriteComponent* sprite,
        std::optional<const HighlightComponent*> highlight
    )
        : transform { transform }
        , screen_position { screen_position }
        , sprite { sprite }
        , highlight { highlight }
    {
    }
};

[[maybe_unused]] bool transform_comparison(
    const Renderable& lhs, const Renderable& rhs
)
{
    return (
        lhs.transform->z_index < rhs.transform->z_index || (lhs.transform->z_index == rhs.transform->z_index && lhs.transform->position.y < rhs.transform->position.y)
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
    registry.clear<ScreenPositionComponent>();

    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    auto spatialmap_cells { registry.view<SpatialMapCellComponent>() };

    for (auto [entity, cell] : spatialmap_cells.each()) {
        if (SDL_HasIntersection(&cell.cell, &camera.camera_rect)) {
            for (entt::entity renderable : cell.entities) {
                registry.emplace<VisibilityComponent>(renderable);
                const TransformComponent& transform { registry.get<const TransformComponent>(renderable) };
                registry.emplace<ScreenPositionComponent>(
                    renderable,
                    WorldPosition(transform.position).to_screen_position(camera)
                );
            }
        }
    }
}

void RenderSystem::render(const entt::registry& registry, SDL_Renderer* renderer)
{
    auto renderables_view {
        registry.view<
            const VisibilityComponent,
            const TransformComponent,
            const ScreenPositionComponent,
            const SpriteComponent>()
    };

    std::vector<Renderable> renderables_pile;
    renderables_pile.reserve(100);

    for (auto [entity, transform, screen_position, sprite] : renderables_view.each()) {
        renderables_pile.emplace_back(&transform, &screen_position, &sprite, std::nullopt);
    }

    std::sort(
        renderables_pile.begin(),
        renderables_pile.end(),
        transform_comparison
    );

    for (Renderable renderable : renderables_pile) {

        SDL_Rect target_rect {
            renderable.screen_position->position.x,
            renderable.screen_position->position.y,
            renderable.sprite->source_rect.w,
            renderable.sprite->source_rect.h
        };

        SDL_RenderCopyEx(
            renderer,
            renderable.sprite->texture,
            &renderable.sprite->source_rect,
            &target_rect,
            renderable.transform->rotation,
            NULL,
            SDL_FLIP_NONE
        );
    }
}

void RenderSystem::render_highlights(const entt::registry& registry, SDL_Renderer* renderer)
{
    auto renderables { registry.view<VisibilityComponent, TransformComponent, ScreenPositionComponent, HighlightComponent>() };
    for (auto [entity, transform, screen_position, highlight] : renderables.each()) {

        std::vector<SDL_Point> absolute_points;
        absolute_points.reserve(highlight.points.size());

        for (auto point : highlight.points) {
            absolute_points.emplace_back(
                SDL_Point {
                    screen_position.position.x + point.x,
                    screen_position.position.y + point.y }
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
    const entt::registry& registry,
    SDL_Renderer* renderer
)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    const MouseComponent& mouse { registry.ctx().get<const MouseComponent>() };
    const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };

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