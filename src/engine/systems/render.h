#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "transform.h"
#include "sprite.h"
#include "constants.h"

class Renderer{

    void render_sprite(
        const glm::ivec2& camera_position,
        const Transform& transform, 
        const Sprite& sprite,
        bool render_bounding_box
    );

    void render_sprites(
        const entt::registry& registry, 
        const glm::ivec2& camera_position,
        bool render_bounding_box
    );

    public:
        // No compelling reason to make this private yet?
        SDL_Renderer* renderer;
        SDL_Rect render_clip_rect;
        
        Renderer(SDL_Window* window, const SDL_DisplayMode& display_mode, uint32_t render_flags, int index);
        ~Renderer();

        void render(const entt::registry& registry, const glm::ivec2& camera_position, bool render_bounding_box);
};

#endif