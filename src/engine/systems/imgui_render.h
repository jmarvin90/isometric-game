#ifndef IMGUIRENDER_H
#define IMGUIRENDER_H

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

void render_imgui_gui(
    SDL_Renderer* renderer,
    entt::registry& registry,
    const std::unordered_map<std::string, SDL_Texture*>& textures,
    const Mouse& mouse
) {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow(&show_demo_window);

    const glm::ivec2 world_position {mouse.get_world_position()};
    const glm::ivec2 grid_position {mouse.get_grid_position()};

    ImGui::Text(
        "World position: (%s) (%s)", 
        std::to_string(world_position.x).c_str(), 
        std::to_string(world_position.y).c_str()
    );

    ImGui::Text(
        "Grid position: (%s) (%s)",
        std::to_string(grid_position.x).c_str(),
        std::to_string(grid_position.y).c_str()
    );

    // bool show_demo_window {true};

    static glm::ivec2 position;
    static glm::ivec2 velocity;    

    // Input for X
    ImGui::InputInt("X postion", &position.x);

    // Input for Y
    ImGui::InputInt("Y position", &position.y);

    // Velocity X
    ImGui::InputInt("X velocity", &velocity.x);

    // Velocity Y
    ImGui::InputInt("Y velocity", &velocity.y);

    static std::string selected_sprite_texture{"moveable_sprite_tall_test.png"};

    if(ImGui::BeginCombo("Sprite image", selected_sprite_texture.c_str())) {
        for (std::pair<std::string, SDL_Texture*> item: textures)
        {
            const bool is_selected = (selected_sprite_texture == item.first);

            if (ImGui::Selectable(item.first.c_str(), is_selected)) {
                selected_sprite_texture = item.first;
            };

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Create sprite")) {
        spdlog::info("Creating a new entity!");
        entt::entity new_entity {registry.create()};
        registry.emplace<Transform>(new_entity, position, 1, 0.0f);
        registry.emplace<Sprite>(new_entity, textures.at(selected_sprite_texture));
        registry.emplace<RigidBody>(new_entity, velocity);
    }
    
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}


#endif