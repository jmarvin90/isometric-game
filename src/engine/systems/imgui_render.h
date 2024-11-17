#ifndef IMGUIRENDER_H
#define IMGUIRENDER_H

#include <SDL2/SDL.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>

void render_imgui_gui(SDL_Renderer* renderer) {
    bool show_demo_window {true};

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow(&show_demo_window);
    
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
}


#endif