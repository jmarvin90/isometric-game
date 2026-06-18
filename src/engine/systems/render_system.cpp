#include <algorithm>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <camera_component.h>
#include <components/building_pair_component.h>
#include <components/connectivity_component.h>
#include <components/flags.h>
#include <components/grid_position_component.h>
#include <components/junction_component.h>
#include <components/mouse_component.h>
#include <components/render_offset_component.h>
#include <components/segment_component.h>
#include <components/segment_member_component.h>
#include <components/spatialmapcell_component.h>
#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <constants.h>
#include <directions.h>
#include <entt/entt.hpp>
#include <grid.h>
#include <imgui.h>
#include <iso_utility.h>
#include <pathfinding.h>
#include <position.h>
#include <projection.h>
#include <sprite.h>
#include <spritesheet.h>
#include <string>
#include <systems/render_system.h>
#include <tuple>
#include <vector>

namespace {

glm::vec2 resolve_position(
    const TransformComponent& transform,
    const RenderOffsetComponent* offset
)
{
    return glm::vec2 {
        offset ? transform.position + offset->render_offset : transform.position
    };
}

void extract_renderables(
    const entt::registry& registry,
    std::vector<Renderable>& renderables,
    const SpatialMapCellComponent& spatial_map_cell,
    const CameraComponent& camera
)
{
    for (entt::entity renderable_entity : spatial_map_cell.entities) {
        if (!registry.all_of<TransformComponent, SpriteComponent>(renderable_entity))
            continue;

        const TransformComponent& transform { registry.get<const TransformComponent>(renderable_entity) };
        const RenderOffsetComponent* offset { registry.try_get<const RenderOffsetComponent>(renderable_entity) };

        renderables.emplace_back(
            &transform,
            registry.get<const SpriteComponent>(renderable_entity).sprite_definition,
            offset,
            registry.all_of<HighlightedFlag>(renderable_entity),
            registry.all_of<SelectedFlag>(renderable_entity),
            Position::world_to_screen(resolve_position(transform, offset), camera.position)
        );
    }
}

// TODO - get rid of debug mode
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
    const Grid<entt::entity, SpatialMapProjection>& spatial_map {
        registry.ctx().get<const Grid<entt::entity, SpatialMapProjection>>()
    };

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
            extract_renderables(
                registry,
                renderables,
                cell,
                camera
            );
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

        if (debug_mode && (renderable.mouseover || renderable.selected)) {
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
    entt::registry& registry,
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
    const Grid<entt::entity, SpatialMapProjection>& spatial_map {
        registry.ctx().get<const Grid<entt::entity, SpatialMapProjection>>()
    };
    const SpriteSheet& spritesheet { registry.ctx().get<const SpriteSheet>() };

    // The mouse and world positions

    const glm::ivec2 grid_position {
        TileMapProjection::world_to_grid(mouse.world_position, tilemap)
    };

    const glm::ivec2 spatial_map_position {
        SpatialMapProjection::world_to_grid(mouse.world_position, spatial_map)
    };

    ImGui::SeparatorText("Mouse Position");

    ImGui::Text(
        "Mouse Screen position: (%d, %d)",
        mouse.screen_position.x,
        mouse.screen_position.y
    );

    ImGui::Text(
        "Mouse World position: (%d, %d)",
        mouse.world_position.x,
        mouse.world_position.y
    );

    ImGui::Text(
        "Mouse Grid position: (%d, %d)",
        grid_position.x,
        grid_position.y
    );

    ImGui::Text(
        "Spatial Map cell: (%d, %d)",
        spatial_map_position.x,
        spatial_map_position.y
    );

    ImGui::SeparatorText("Graph");
    auto junctions_view { registry.view<JunctionComponent>() };
    auto segments_view { registry.view<SegmentComponent>() };
    ImGui::Text("Junctions: %d", static_cast<int>(junctions_view.size()));
    ImGui::Text("Segments: %d", static_cast<int>(segments_view.size()));

    auto pairs_view { registry.view<BuildingPairComponent>() };
    if (pairs_view.begin() != pairs_view.end()) {
        ImGui::SeparatorText("Building Pairs");
        ImGui::Text(
            "# paired entities: %d",
            static_cast<int>(pairs_view.size())
        );
    }

    if (auto view { registry.view<SelectedFlag>() }; view.begin() != view.end()) {
        entt::entity selected_entity { view.front() };
        SpriteComponent& sprite { registry.get<SpriteComponent>(selected_entity) };
        const TransformComponent& transform { registry.get<const TransformComponent>(selected_entity) };
        const SegmentMemberComponent* segment_membership { registry.try_get<const SegmentMemberComponent>(selected_entity) };

        ImGui::SeparatorText("Selected Entity");

        glm::ivec2 grid_position {
            TileMapProjection::world_to_grid(
                glm::ivec2(transform.position) + sprite.sprite_definition->anchor,
                tilemap
            )
        };

        ImGui::Text(
            "Selected entity position: (%d, %d)",
            grid_position.x,
            grid_position.y
        );

        ImGui::Text(
            "Selected entity sprite name: %s",
            sprite.sprite_definition->name.c_str()
        );

        if (segment_membership) {
            ImGui::Text(
                "Selected entity segment: %d",
                static_cast<int>(segment_membership->segment)
            );
        }

        if (ImGui::Button("Delete entity")) {
            registry.emplace<EntityReleaseFlag>(selected_entity);
            registry.clear<SelectedFlag>();
        }

        if (ImGui::BeginCombo("Sprite", sprite.sprite_definition->name.c_str())) {
            for (const auto& [name, sprite_definition] : spritesheet.sprites) {
                bool is_selected = (name == sprite.sprite_definition->name);

                if (ImGui::Selectable(name.c_str(), is_selected)) {
                    // TODO - move elsewhere
                    registry.replace<SpriteComponent>(selected_entity, &sprite_definition);
                    if (sprite_definition.directions != Direction::TDirection::NO_DIRECTION) {
                        registry.emplace_or_replace<ConnectivityComponent>(selected_entity, sprite_definition.directions);
                    } else if (registry.all_of<ConnectivityComponent>(selected_entity)) {
                        registry.emplace_or_replace<ConnectivityComponent>(selected_entity, Direction::TDirection::NO_DIRECTION);
                    }
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        if (sprite.sprite_definition->sprite_type == SpriteType::TILE) {
            static std::string selected_walker_sprite { "walker_w" };
            static std::string selected_building_sprite { "building_short" };

            ImGui::SeparatorText("Walker Spawn");

            if (ImGui::BeginCombo("Walker Sprites", selected_walker_sprite.c_str())) {
                for (const auto& [name, sprite_definition] : spritesheet.sprites) {
                    if (sprite_definition.sprite_type != SpriteType::WALKER)
                        continue;

                    if (ImGui::Selectable(name.c_str(), selected_walker_sprite == name)) {
                        selected_walker_sprite = name;
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Spawn Walker")) {
                entt::entity new_sprite { registry.create() };
                TransformComponent& transform {
                    registry.emplace<TransformComponent>(
                        new_sprite, glm::ivec2 {}, 1, 0.0
                    )
                };

                const SpriteComponent& sprite {
                    registry.emplace<SpriteComponent>(
                        new_sprite, &spritesheet.sprites.at(selected_walker_sprite)
                    )
                };

                ISOUtility::align_sprite_to(
                    registry,
                    new_sprite,
                    transform,
                    sprite,
                    SpriteAnchor::SPRITE_ANCHOR,
                    registry.get<const TransformComponent>(selected_entity).position
                );
            }

            ImGui::SeparatorText("Building Spawn");

            if (ImGui::BeginCombo("Building Sprites", selected_building_sprite.c_str())) {
                for (const auto& [name, sprite_definition] : spritesheet.sprites) {
                    if (
                        sprite_definition.sprite_type != SpriteType::BUILDING_SENDER
                        && sprite_definition.sprite_type != SpriteType::BUILDING_RECEIVER
                        && sprite_definition.sprite_type != SpriteType::FURNITURE
                    )
                        continue;

                    if (ImGui::Selectable(name.c_str(), selected_building_sprite == name)) {
                        selected_building_sprite = name;
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Spawn Building")) {
                entt::entity new_sprite { registry.create() };
                const SpriteDefinition& sprite_def {
                    spritesheet.sprites.at(selected_building_sprite)
                };

                registry.emplace<TransformComponent>(
                    new_sprite,
                    glm::ivec2 {
                        (
                            glm::ivec2 { registry.get<const TransformComponent>(selected_entity).position }
                            + Constants::TILE_CENTRE
                        )
                        - glm::ivec2 { sprite_def.anchor } },
                    1,
                    0.0
                );

                registry.emplace<SpriteComponent>(
                    new_sprite, &spritesheet.sprites.at(selected_building_sprite)
                );
            }
        }
    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

void render_segments(
    [[maybe_unused]] const entt::registry& registry,
    [[maybe_unused]] SDL_Renderer* renderer
)
{
    const CameraComponent& camera { registry.ctx().get<const CameraComponent>() };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    auto segments { registry.view<SegmentComponent>() };
    [[maybe_unused]] const auto* road_gates { &Constants::ROAD_GATES };

    for (auto [entity, segment] : segments.each()) {
        for (
            auto [origin, termination, direction] //
            : std::array<std::tuple<entt::entity, entt::entity, Direction::TDirection>, 2> {
                std::tuple { segment.origin, segment.termination, segment.direction },
                std::tuple { segment.termination, segment.origin, Direction::reverse(segment.direction) } //
            } //
        ) {
            [[maybe_unused]] uint8_t index_position { Direction::index_position(direction) };

            glm::ivec2 origin_position {
                Position::world_to_screen(
                    glm::ivec2 { registry.get<const TransformComponent>(origin).position }
                        + Constants::ROAD_GATES[Direction::index_position(direction)].exit,
                    camera.position
                )
            };

            glm::ivec2 termination_position {
                Position::world_to_screen(
                    glm::ivec2 { registry.get<const TransformComponent>(termination).position }
                        + Constants::ROAD_GATES[Direction::index_position(Direction::reverse(direction))].entry,
                    camera.position
                )
            };

            SDL_RenderDrawLine(
                renderer,
                origin_position.x,
                origin_position.y,
                termination_position.x,
                termination_position.y
            );
        }
    }
}
}