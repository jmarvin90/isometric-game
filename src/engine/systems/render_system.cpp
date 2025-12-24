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

template <typename T>
void draw_lines(SDL_Renderer* renderer, const T& line_component, const glm::ivec2 offset)
{
    std::vector<SDL_Point> points;
    for (auto& point : line_component.points) {
        points.push_back(SDL_Point { point.x + offset.x, point.y + offset.y });
    }
    SDL_SetRenderDrawColor(
        renderer,
        line_component.colour.r,
        line_component.colour.g,
        line_component.colour.b,
        line_component.colour.a
    );
    SDL_RenderDrawLines(renderer, points.data(), points.size());
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

void RenderSystem::render(
    entt::registry& registry,
    SDL_Renderer* renderer,
    [[maybe_unused]] const SDL_DisplayMode& display_mode,
    [[maybe_unused]] const bool debug_mode
)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    [[maybe_unused]] const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    auto spatialmap_cells = registry.view<SpatialMapCellComponent>();
    std::vector<std::pair<const TransformComponent*, const SpriteComponent*>> renderables;

    SDL_Rect comparator { camera.camera_rect };
    comparator.x -= 1;
    comparator.y -= 1;
    comparator.h += 2;
    comparator.w += 2;

    for (auto [entity, cell] : spatialmap_cells.each()) {
        if (SDL_HasIntersection(&cell.cell, &comparator)) {
            for (entt::entity renderable : cell.entities) {
                const TransformComponent* transform { &registry.get<const TransformComponent>(renderable) };
                const SpriteComponent* sprite { &registry.get<const SpriteComponent>(renderable) };
                renderables.emplace_back(transform, sprite);
            }
        }
    }

    std::sort(renderables.begin(), renderables.end(), transform_comparison);

    for (auto [transform, sprite] : renderables) {
        glm::ivec2 output_position { WorldPosition(transform->position).to_screen_position(camera) };

        SDL_Rect target_rect { output_position.x, output_position.y, sprite->source_rect.w, sprite->source_rect.h };

        SDL_RenderCopyEx(
            renderer, sprite->texture, &sprite->source_rect, &target_rect, transform->rotation, NULL, SDL_FLIP_NONE
        );

        // if (debug_mode) {
        //     [[maybe_unused]] const HighlightComponent* highlight {registry.try_get<const
        //     HighlightComponent>(entity)}; if (!highlight) continue; draw_lines(renderer, *highlight, output_pos);
    }

    // TODO: prevent unecessary line draws using https://wiki.libsdl.org/SDL2/SDL_IntersectRectAndLine

    // if (tile_highlight && debug_mode) {
    //     draw_lines(tile_highlight, screen_position);
    // }

    if (debug_mode) {
        render_imgui_ui(registry, renderer);
        render_segment_lines(registry, renderer);
    }

    SDL_RenderPresent(renderer);
}