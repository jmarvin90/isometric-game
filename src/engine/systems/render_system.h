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

    static void render(
        entt::registry& registry,
        SDL_Renderer* renderer,
        const SDL_DisplayMode& display_mode,
        const CameraComponent& camera
    );

    static void render_highlights(
        entt::registry& registry,
        SDL_Renderer* renderer,
        const SDL_DisplayMode& display_mode
    );

    static void render_imgui_ui(
        entt::registry& registry,
        SDL_Renderer* renderer,
        const MouseComponent& mouse,
        const TileMapComponent& tilemap
    );

    static void render_segment_lines(
        const entt::registry& registry,
        SDL_Renderer* renderer
    );
};

#endif