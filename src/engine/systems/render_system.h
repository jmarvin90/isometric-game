#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <entt/entt.hpp>
#include <tilemap_component.h>

class RenderSystem {
public:
    static void update(
        entt::registry& registry
    );

    static void render(const entt::registry& registry, SDL_Renderer* renderer);
    static void render_highlights(const entt::registry& registry, SDL_Renderer* renderer);
    static void render_imgui_ui(const entt::registry& registry, SDL_Renderer* renderer);
    static void render_segment_lines(const entt::registry& registry, SDL_Renderer* renderer);

    static void render_junction_gates(const entt::registry& registry, SDL_Renderer* renderer);
};

#endif