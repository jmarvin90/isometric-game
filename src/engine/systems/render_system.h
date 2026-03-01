#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/highlight_component.h>
#include <components/mouse_component.h>
#include <entt/entt.hpp>

struct Renderable {
    const TransformComponent* transform;
    const SpriteComponent* sprite;
    const HighlightComponent* highlight;
    glm::ivec2 screen_position;

    Renderable(
        const TransformComponent* transform,
        const SpriteComponent* sprite,
        const HighlightComponent* highlight,
        glm::ivec2 screen_position
    )
        : transform { transform }
        , sprite { sprite }
        , highlight { highlight }
        , screen_position { screen_position }
    {
    }
};

class RenderSystem {

public:
    static void update(
        entt::registry& registry
    );

    static void render(const entt::registry& registry, SDL_Renderer* renderer, const bool debug_mode);
    static void render_imgui_ui(const entt::registry& registry, SDL_Renderer* renderer);
    static void render_junction_gates(const entt::registry& registry, SDL_Renderer* renderer);
    static void render_path(const entt::registry& registry, SDL_Renderer* renderer, entt::entity from_tile, entt::entity to_tile);
};

#endif