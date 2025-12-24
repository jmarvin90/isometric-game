#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL2/SDL.h>

#include <entt/entt.hpp>

class RenderSystem {
public:
    static void render(
        entt::registry& registry,
        SDL_Renderer* renderer,
        const SDL_DisplayMode& display_mode,
        const bool debug_mode
    );
};

#endif