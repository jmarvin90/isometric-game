#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL2/SDL.h>
#include <components/camera_component.h>
#include <components/mouse_component.h>
#include <components/transform_component.h>
#include <entt/entt.hpp>
#include <spritesheet.h>

struct Renderable {
    const TransformComponent* transform;
    const SpriteDefinition* sprite;
    bool mouseover;
    bool selected;
    glm::ivec2 screen_position;

    Renderable(
        const TransformComponent* transform,
        const SpriteDefinition* sprite,
        bool mouseover,
        bool selected,
        glm::ivec2 screen_position
    )
        : transform { transform }
        , sprite { sprite }
        , mouseover { mouseover }
        , selected { selected }
        , screen_position { screen_position }
    {
    }
};

namespace RenderSystem {
void update(entt::registry& registry, const bool debug_mode);
void render(const entt::registry& registry, SDL_Renderer* renderer, const bool debug_mode);
void render_imgui_ui(entt::registry& registry, SDL_Renderer* renderer);
void render_junction_gates(const entt::registry& registry, SDL_Renderer* renderer);
void render_segments(const entt::registry& registry, SDL_Renderer* renderer);
};

#endif