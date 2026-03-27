#include <algorithm>
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
#include <components/tilespec_component.h>
#include <components/transform_component.h>
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

namespace {

bool transform_comparison(
    const Renderable& lhs, const Renderable& rhs, const bool debug_mode
)
{
    if (debug_mode && (lhs.mouseover != rhs.mouseover))
        return !lhs.mouseover && rhs.mouseover;

    if (lhs.transform->z_index != rhs.transform->z_index)
        return lhs.transform->z_index < rhs.transform->z_index;

    int lhs_bottom { int(lhs.transform->position.y) + lhs.sprite->source_rect.h };
    int rhs_bottom { int(rhs.transform->position.y) + rhs.sprite->source_rect.h };
    return lhs_bottom < rhs_bottom;
}
} // namespace

namespace RenderSystem {

void update(entt::registry& registry, const bool debug_mode)
{
    /*
        TODO: possibility to early out if nothing has changed - incl.
        - mouse
        - camera
        - movement
    */

    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    const Grid<entt::entity, SpatialMapProjection>& spatial_map { registry.ctx().get<const Grid<entt::entity, SpatialMapProjection>>() };

    auto spatialmap_cells { registry.view<SpatialMapCellComponent, TransformComponent>() };
    std::vector<Renderable>& renderables { registry.ctx().get<std::vector<Renderable>>() };
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
            for (entt::entity renderable_entity : cell.entities) {
                if (!registry.all_of<TransformComponent, SpriteComponent>(renderable_entity))
                    continue;

                const TransformComponent& transform { registry.get<const TransformComponent>(renderable_entity) };

                renderables.emplace_back(
                    &transform,
                    registry.get<const SpriteComponent>(renderable_entity).sprite_definition,
                    registry.try_get<const HighlightComponent>(renderable_entity),
                    registry.all_of<MouseOverComponent>(renderable_entity),
                    Position::world_to_screen(transform.position, camera.position)
                );
            }

            // TODO - can I add segments to the renderables pile?
        }
    }

    // TODO - it would be preferable to have some more explicit logic in the
    // render function
    std::sort(
        renderables.begin(),
        renderables.end(),
        [debug_mode](const Renderable& lhs, const Renderable& rhs) {
            return transform_comparison(lhs, rhs, debug_mode);
        }
    );
}

void render(const entt::registry& registry, SDL_Renderer* renderer, [[maybe_unused]] const bool debug_mode)
{
    const SpriteSheet& spritesheet { registry.ctx().get<SpriteSheet>() };
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
            spritesheet.texture.get(),
            &renderable.sprite->source_rect,
            &target_rect,
            renderable.transform->rotation,
            NULL,
            SDL_FLIP_NONE
        );

        if (debug_mode && renderable.mouseover) {
            SDL_RenderCopyEx(
                renderer,
                spritesheet.outline_texture.get(),
                &renderable.sprite->source_rect,
                &target_rect,
                renderable.transform->rotation,
                NULL,
                SDL_FLIP_NONE
            );
        }
    }
}

void render_imgui_ui(
    const entt::registry& registry,
    SDL_Renderer* renderer
)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    const MouseComponent& mouse { registry.ctx().get<const MouseComponent>() };
    const Grid<entt::entity, TileMapProjection>& tilemap {
        registry.ctx().get<const Grid<entt::entity, TileMapProjection>>()
    };

    // The mouse and world positions

    const glm::ivec2 grid_position {
        TileMapProjection::world_to_grid(mouse.world_position, tilemap)
    };

    ImGui::SeparatorText("Mouse Position");

    ImGui::Text(
        "Mouse Screen position: (%d) (%d)",
        mouse.screen_position.x,
        mouse.screen_position.y
    );

    ImGui::Text(
        "Mouse World position: (%d) (%d)",
        mouse.world_position.x,
        mouse.world_position.y
    );

    ImGui::Text(
        "Mouse Grid position: (%d) (%d)",
        grid_position.x,
        grid_position.y
    );

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}
}