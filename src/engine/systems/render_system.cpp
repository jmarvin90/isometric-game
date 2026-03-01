#include <algorithm>
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
#include <components/tilespec_component.h>
#include <components/transform_component.h>
#include <components/visibility_component.h>
#include <constants.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <imgui.h>
#include <pathfinding.h>
#include <position.h>
#include <projection.h>
#include <spritesheet.h>
#include <systems/render_system.h>
#include <tilespec_component.h>
#include <utility>
#include <vector>

int total_count { 0 };

namespace {

// TODO: be aware of possible pointer invalidation

bool transform_comparison(
    const Renderable& lhs, const Renderable& rhs
)
{
    return (
        lhs.transform->z_index < rhs.transform->z_index || //
        (
            lhs.transform->z_index == rhs.transform->z_index && //
            lhs.transform->position.y < rhs.transform->position.y //
        )
    );
}

void draw_segment_path(
    SDL_Renderer* renderer,
    const entt::registry& registry,
    const TileSpecComponent& tilespec,
    entt::entity segment_start,
    entt::entity segment_end,
    Direction::TDirection direction
)
{
    // TODO: this function is nasty
    const glm::ivec2 camera_position { registry.ctx().get<const CameraComponent>().position };
    glm::ivec2 segment_start_world { registry.get<TransformComponent>(segment_start).position };
    glm::ivec2 segment_end_world { registry.get<TransformComponent>(segment_end).position };
    ScreenPositionComponent segment_start_screen { Position::world_to_screen(segment_start_world, camera_position) };
    ScreenPositionComponent segment_end_screen { Position::world_to_screen(segment_end_world, camera_position) };
    glm::ivec2 entry { segment_start_screen.position + tilespec.road_gates.at(Direction::index_position(direction)).exit };
    glm::ivec2 exit { segment_end_screen.position + tilespec.road_gates.at(Direction::index_position(Direction::reverse(direction))).entry };
    SDL_RenderDrawLine(
        renderer,
        entry.x,
        entry.y,
        exit.x,
        exit.y
    );
}

}; // namespace

void RenderSystem::render_segment_lines(
    const entt::registry& registry, SDL_Renderer* renderer
)
{
    auto segments = registry.view<SegmentComponent, VisibilityComponent>();
    const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };

    for (auto [entity, segment] : segments.each()) {
        draw_segment_path(renderer, registry, tilespec, segment.origin, segment.termination, segment.direction);
        draw_segment_path(renderer, registry, tilespec, segment.termination, segment.origin, Direction::reverse(segment.direction));
    }
}

void RenderSystem::update(entt::registry& registry)
{
    /*
        TODO: possibility to early out if nothing has changed - incl.
        - mouse
        - camera
        - movement
    */

    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    const Grid<SpatialMapProjection>& spatial_map { registry.ctx().get<const Grid<SpatialMapProjection>>() };
    std::vector<Renderable>& renderables { registry.ctx().get<std::vector<Renderable>>() };
    auto spatialmap_cells { registry.view<SpatialMapCellComponent, TransformComponent>() };

    renderables.clear();

    for (auto [entity, cell, transform] : spatialmap_cells.each()) {

        SDL_Rect comparator {
            static_cast<int>(transform.position.x),
            static_cast<int>(transform.position.y),
            spatial_map.cell_size.x,
            spatial_map.cell_size.y
        };

        SDL_Rect camera_rect {
            camera.position.x,
            camera.position.y,
            camera.size.x,
            camera.size.y
        };

        if (SDL_HasIntersection(&comparator, &camera_rect)) {
            for (entt::entity renderable : cell.entities) {
                if (registry.all_of<TransformComponent, SpriteComponent>(renderable)) {
                    const TransformComponent& transform { registry.get<const TransformComponent>(renderable) };
                    renderables.emplace_back(
                        &transform,
                        &registry.get<const SpriteComponent>(renderable),
                        registry.try_get<const HighlightComponent>(renderable),
                        Position::world_to_screen(transform.position, camera.position)
                    );
                }
            }

            for (entt::entity renderable : cell.segments) {
                registry.emplace_or_replace<VisibilityComponent>(renderable);
            }
        }
    }

    std::sort(renderables.begin(), renderables.end(), transform_comparison);
}

void RenderSystem::render(const entt::registry& registry, SDL_Renderer* renderer)
{
    SDL_Texture* texture { registry.ctx().get<SpriteSheet>().texture.get() };
    const std::vector<Renderable>& renderables { registry.ctx().get<const std::vector<Renderable>>() };

    for (auto renderable : renderables) {

        SDL_Rect target_rect {
            renderable.screen_position.x,
            renderable.screen_position.y,
            renderable.sprite->source_rect.w,
            renderable.sprite->source_rect.h
        };

        SDL_RenderCopyEx(
            renderer,
            texture,
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
    [[maybe_unused]] const Grid<TileMapProjection>& tilemap { registry.ctx().get<const Grid<TileMapProjection>>() };
    [[maybe_unused]] const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };

    // The mouse and world positions

    const glm::ivec2 world_position {
        Position::screen_to_world(mouse.window_current_position, camera.position)
    };

    const glm::ivec2 grid_position {
        TileMapProjection::world_to_grid(world_position, tilemap)
    };

    ImGui::SeparatorText("Mouse Position");

    ImGui::Text(
        "Mouse Screen position: (%d) (%d)",
        mouse.window_current_position.x,
        mouse.window_current_position.y
    );

    ImGui::Text(
        "Mouse World position: (%d) (%d)",
        world_position.x,
        world_position.y
    );

    ImGui::Text(
        "Mouse Grid position: (%d) (%d)",
        grid_position.x,
        grid_position.y
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
    const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };
    auto junctions { registry.view<ScreenPositionComponent, JunctionComponent>() };
    for (auto [entity, screen_position, junction] : junctions.each()) {
        for (auto gate : tilespec.road_gates) {
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

void RenderSystem::render_path(
    const entt::registry& registry,
    [[maybe_unused]] SDL_Renderer* renderer,
    entt::entity from_tile,
    entt::entity to_tile
)
{
    const TileSpecComponent& tilespec { registry.ctx().get<const TileSpecComponent>() };
    const CameraComponent& camera_component { registry.ctx().get<const CameraComponent>() };

    std::vector<entt::entity> path;
    Pathfinding::path_between(registry, from_tile, to_tile, path);

    const GridPositionComponent* current_grid_position { nullptr };
    const TransformComponent* current_world_position { nullptr };
    [[maybe_unused]] Direction::TDirection last_direction { Direction::TDirection::NO_DIRECTION };

    for (size_t i = 0; i < path.size() - 1; i++) {
        entt::entity current = path[i];
        entt::entity next = path[i + 1];

        if (!current_grid_position) {
            current_grid_position = &registry.get<const GridPositionComponent>(current);
        }

        if (!current_world_position) {
            current_world_position = &registry.get<const TransformComponent>(current);
        }

        const GridPositionComponent* next_grid_position {
            &registry.get<const GridPositionComponent>(next)
        };

        [[maybe_unused]] const TransformComponent* next_world_position {
            &registry.get<const TransformComponent>(next)
        };

        glm::ivec2 delta { next_grid_position->position - current_grid_position->position };
        glm::ivec2 delta_vec { Direction::to_direction_vector(delta) };
        Direction::TDirection direction { Direction::vector_directions[delta_vec] };

        glm::ivec2 tile_entry_offset {
            tilespec.road_gates[Direction::index_position(Direction::reverse(direction))].entry
        };

        glm::ivec2 tile_exit_offset {
            tilespec.road_gates[Direction::index_position(direction)].exit
        };

        [[maybe_unused]] glm::ivec2 start_screen_position {
            Position::world_to_screen(
                glm::ivec2 { glm::ivec2 { current_world_position->position } },
                camera_component.position
            )
            + tile_exit_offset
        };

        [[maybe_unused]] glm::ivec2 end_screen_position {
            Position::world_to_screen(
                glm::ivec2 { glm::ivec2 { next_world_position->position } },
                camera_component.position
            )
            + tile_entry_offset
        };

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        SDL_RenderDrawLine(
            renderer,
            start_screen_position.x,
            start_screen_position.y,
            end_screen_position.x,
            end_screen_position.y
        );

        current_grid_position = next_grid_position;
        current_world_position = next_world_position;
    }
}