#ifndef IMGUIRENDER_H
#define IMGUIRENDER_H

#include <algorithm>
#include <iostream>

#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <spdlog/spdlog.h>

#include <mouse.h>
#include <transform.h>
#include <sprite.h>
#include <rigid_body.h>
#include <constants.h>
#include <spritesheet.h>
#include <utils.h>

void render_imgui_gui(
    SDL_Renderer* renderer,
    entt::registry& registry,
    const Mouse& mouse,
    const std::unique_ptr<TileMap>& tilemap,
    const SpriteSheet<TileSpriteDefinition>& city_tiles,
    const SpriteSheet<TileSpriteDefinition>& building_tiles,
    const SpriteSheet<VehicleSpriteDefinition>& vehicle_tiles
)
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    bool show_demo_window{ true };
    ImGui::ShowDemoWindow(&show_demo_window);

    // The mouse and world positions
    const glm::ivec2 world_position{ mouse.get_world_position() };
    const glm::ivec2 grid_position{ mouse.get_grid_position() };

    ImGui::SeparatorText("Mouse Position");
    ImGui::Text(
        "Mouse World position: (%s) (%s)",
        std::to_string(world_position.x).c_str(),
        std::to_string(world_position.y).c_str());

    ImGui::Text(
        "Mouse Grid position: (%s) (%s)",
        std::to_string(grid_position.x).c_str(),
        std::to_string(grid_position.y).c_str());

    ImGui::SeparatorText("Selected Tile Position");
    if (tilemap->selected_tile)
    {

        ImGui::Text(
            "Selected Tile World position: (%s) (%s)",
            std::to_string(tilemap->selected_tile->world_position().x).c_str(),
            std::to_string(tilemap->selected_tile->world_position().y).c_str());

        ImGui::Text(
            "Selected Tile Grid position: (%s) (%s)",
            std::to_string(tilemap->selected_tile->get_grid_position().x).c_str(),
            std::to_string(tilemap->selected_tile->get_grid_position().y).c_str());
    }

    // The sprite for the selected tile
    static std::string selected_sprite_texture;
    static Sprite* selected_tile_sprite{ nullptr };
    // static const TileSpriteDefinition* sprite_definition{ nullptr };

    std::vector<std::string> city_tile_keys;
    city_tile_keys.reserve(city_tiles.sprites.size());

    std::vector<std::string> building_tile_keys;
    building_tile_keys.reserve(building_tiles.sprites.size());

    std::vector<std::string> vehicle_tile_keys;
    vehicle_tile_keys.reserve(vehicle_tiles.sprites.size());

    for (auto kv : city_tiles.sprites)
    {
        city_tile_keys.push_back(kv.first);
    }

    for (auto kv : building_tiles.sprites)
    {
        building_tile_keys.push_back(kv.first);
    }

    for (auto kv : vehicle_tiles.sprites)
    {
        vehicle_tile_keys.push_back(kv.first);
    }

    if (tilemap->selected_tile)
    {
        entt::entity selected_tile{ tilemap->selected_tile->get_entity() };
        selected_tile_sprite = &registry.get<Sprite>(selected_tile);

        if (selected_tile_sprite->texture == city_tiles.get_spritesheet_texture())
        {
            selected_sprite_texture = city_tiles.reverse_lookup(selected_tile_sprite->source_rect).value();
        }

        if (selected_tile_sprite->texture == building_tiles.get_spritesheet_texture())
        {
            selected_sprite_texture = building_tiles.reverse_lookup(selected_tile_sprite->source_rect).value();
        }

        ImGui::SeparatorText("Tile Options");

        if (
            tilemap->graph.find(tilemap->selected_tile) != tilemap->graph.end())
        {
            for (uint8_t direction = constants::Directions::NORTH; direction; direction >>= 1)
            {
                const Tile* connection{
                    tilemap->graph.at(tilemap->selected_tile).at(direction_index(direction)) };
                if (connection)
                {
                    glm::ivec2 connection_location{ connection->get_grid_position() };
                    ImGui::Text(
                        "Tile connection: (%s, %s)",
                        std::to_string(connection_location.x).c_str(),
                        std::to_string(connection_location.y).c_str());
                }
            }
        }

        if (ImGui::BeginCombo("Sprite image", selected_sprite_texture.c_str()))
        {

            std::sort(city_tile_keys.begin(), city_tile_keys.end());

            for (auto tile_string : city_tile_keys)
            {
                const bool is_selected = (selected_sprite_texture == tile_string);

                if (ImGui::Selectable(tile_string.c_str(), is_selected))
                {
                    selected_sprite_texture = tile_string;
                    tilemap->selected_tile->set_tile_base(selected_sprite_texture, city_tiles);
                    // sprite_definition = &city_tiles.get_sprite_definition(selected_sprite_texture);
                    // selected_tile_sprite->source_rect = sprite_definition->texture_rect;
                    // selected_tile_sprite->offset = glm::ivec2{0, constants::TILE_BASE_HEIGHT - selected_tile_sprite->source_rect.h};
                    // tilemap->selected_tile->set_connection_bitmask(sprite_definition->connection);
                };

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SeparatorText("Vehicle Options");

        static std::string selected_vehicle_sprite_texture {"ambulance_E.png"};

        static Sprite selected_vehicle_sprite{ 
            vehicle_tiles.get_spritesheet_texture(),
            vehicle_tiles.get_sprite_rect(selected_vehicle_sprite_texture)
        };

        static const VehicleSpriteDefinition* vehicle_sprite_definition { nullptr };

        if (ImGui::BeginCombo("Vehicle Sprite image", selected_vehicle_sprite_texture.c_str()))
        {

            std::sort(vehicle_tile_keys.begin(), vehicle_tile_keys.end());

            for (auto tile_string : vehicle_tile_keys)
            {
                const bool is_selected = (selected_vehicle_sprite_texture == tile_string);

                if (ImGui::Selectable(tile_string.c_str(), is_selected))
                {
                    selected_vehicle_sprite_texture = tile_string;
                    vehicle_sprite_definition = &vehicle_tiles.get_sprite_definition(selected_vehicle_sprite_texture);
                    selected_vehicle_sprite.source_rect = vehicle_sprite_definition->texture_rect;
                };

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        /* 
            TODO: here's where we create the vehicle
            
            At some stage the target has to be to align the centre point of the 
            vehicle sprite with the centre point of the tile sprite it's being 
            placed on.

            The difficulty is that there are various offsets for e.g. tile 
            depth that make doing this a little bit tricky.
        */

        if (ImGui::Button("Create Vehicle"))
        {
            entt::entity vehicle_entity {registry.create()};
            [[maybe_unused]] std::remove_const_t<Sprite>* sprite = &registry.emplace<Sprite>(
                vehicle_entity,
                selected_vehicle_sprite
            );

            // sprite->offset = glm::ivec2{0, -constants::MIN_TILE_DEPTH};

            [[maybe_unused]] std::remove_const_t<Transform>* transform = &registry.emplace<Transform>(
                vehicle_entity,
                tilemap->selected_tile->world_position(),
                1,
                0.0
            );

            transform->position +=  constants::TILE_SIZE_HALF;

            transform->position -= glm::ivec2{
                sprite->source_rect.w / 2,
                sprite->source_rect.h / 2
            };

            registry.emplace<RigidBody>(
                vehicle_entity,
                glm::vec2{10, 5}
            );
        }

    }

    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}

#endif