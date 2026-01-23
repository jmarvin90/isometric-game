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

bool transform_comparison(
    const Renderable& lhs, const Renderable& rhs
)
{
    return (
        lhs.transform->z_index < rhs.transform->z_index || (lhs.transform->z_index == rhs.transform->z_index && lhs.transform->position.y < rhs.transform->position.y)
    );
}

}; // namespace

void RenderSystem::render_segment_lines(
    const entt::registry& registry, SDL_Renderer* renderer
)
{
    auto segments = registry.view<SegmentComponent>();
    const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };
    std::vector<Gate> gates { tilespec.road_gates() };

    for (auto [entity, segment] : segments.each()) {

        WorldPosition segment_start_world { registry.get<TransformComponent>(segment.start).position };
        WorldPosition segment_end_world { registry.get<TransformComponent>(segment.end).position };
        ScreenPosition segment_start_screen { Position::to_screen_position(segment_start_world, registry) };
        ScreenPosition segment_end_screen { Position::to_screen_position(segment_end_world, registry) };

        glm::ivec2 lhs_entry { segment_start_screen.position + gates.at(Direction::index_position(segment.direction)).exit };
        glm::ivec2 lhs_exit { segment_end_screen.position + gates.at(Direction::index_position(Direction::reverse(segment.direction))).entry };

        glm::ivec2 rhs_entry { segment_end_screen.position + gates.at(Direction::index_position(Direction::reverse(segment.direction))).exit };
        glm::ivec2 rhs_exit { segment_start_screen.position + gates.at(Direction::index_position(segment.direction)).entry };

        SDL_RenderDrawLine(
            renderer,
            lhs_entry.x,
            lhs_entry.y,
            lhs_exit.x,
            lhs_exit.y
        );

        SDL_RenderDrawLine(
            renderer,
            rhs_entry.x,
            rhs_entry.y,
            rhs_exit.x,
            rhs_exit.y
        );
    }
}

// TODO - consider segment lines
void RenderSystem::update(entt::registry& registry)
{
    registry.clear<VisibilityComponent>();
    registry.clear<ScreenPositionComponent>();

    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    auto spatialmap_cells { registry.view<SpatialMapCellComponent>() };

    // TODO - tidy up, potentially into the camera component
    SDL_Rect contingency_rect { camera.camera_rect };
    contingency_rect.h *= 2;
    contingency_rect.w *= 2;
    contingency_rect.x -= ((contingency_rect.w - camera.camera_rect.w) / 2);
    contingency_rect.y -= ((contingency_rect.h - camera.camera_rect.h) / 2);

    for (auto [entity, cell] : spatialmap_cells.each()) {
        if (SDL_HasIntersection(&cell.cell, &contingency_rect)) {
            for (entt::entity renderable : cell.entities) {
                registry.emplace<VisibilityComponent>(renderable);
                const TransformComponent& transform { registry.get<const TransformComponent>(renderable) };
                registry.emplace<ScreenPositionComponent>(
                    renderable,
                    Position::to_screen_position(WorldPosition { transform.position }, camera).position
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
    [[maybe_unused]] const TileMapComponent& tilemap { registry.ctx().get<const TileMapComponent>() };

    // The mouse and world positions
    const ScreenPosition screen_position { mouse.window_current_position };
    const WorldPosition world_position { Position::to_world_position(screen_position, registry) };
    const TileMapGridPosition grid_position { Position::to_grid_position(world_position, registry) };

    ImGui::SeparatorText("Mouse Position");

    ImGui::Text(
        "Mouse Screen position: (%d) (%d)",
        screen_position.position.x,
        screen_position.position.y
    );

    ImGui::Text(
        "Mouse World position: (%d) (%d)",
        world_position.position.x,
        world_position.position.y
    );

    ImGui::Text(
        "Mouse Grid position: (%d) (%d)",
        grid_position.position.x,
        grid_position.position.y
    );

    // if (tilemap.highlighted_tile != entt::null) {
    //     const NavigationComponent* nav {
    //         registry.try_get<const NavigationComponent>(tilemap.highlighted_tile)
    //     };

    //     if (nav) {
    //         ImGui::Text(
    //             "Tile Connection Direction(s): (%d)",
    //             Direction::to_underlying(nav->directions)
    //         );
    //     }
    // }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void RenderSystem::render_junction_gates(const entt::registry& registry, SDL_Renderer* renderer)
{
    const TileSpecComponent& tile_spec { registry.ctx().get<const TileSpecComponent>() };
    auto junctions { registry.view<ScreenPositionComponent, JunctionComponent>() };
    for (auto [entity, screen_position, junction] : junctions.each()) {
        for (auto gate : tile_spec.road_gates()) {
            glm::ivec2 entry { glm::ivec2 { screen_position.position } + gate.entry };
            glm::ivec2 exit { glm::ivec2 { screen_position.position } + gate.exit };
            SDL_Rect entry_rect { entry.x - 2, entry.y - 2, 4, 4 };
            SDL_Rect exit_rect { exit.x - 2, exit.y - 2, 4, 4 };
            SDL_SetRenderDrawColor(renderer, 255, 155, 155, 255);
            SDL_RenderFillRect(renderer, &entry_rect);
            SDL_RenderFillRect(renderer, &exit_rect);
        }
    }
}