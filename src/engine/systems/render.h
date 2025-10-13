#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include <sprite.h>
#include <transform.h>
#include <constants.h>
#include <scene.h>

class Scene;

class Renderer{

    SDL_Renderer* renderer;
    SDL_Rect render_clip_rect;

    void render_sprite(const glm::ivec2 camera_position, const Transform& transform, const Sprite& sprite) const;

    public:
        
        Renderer(SDL_Window* window, const SDL_DisplayMode& display_mode, uint32_t render_flags, int index);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer operator=(const Renderer&) = delete;

        void render(const Scene& scene) const;
        // void render(const glm::ivec2& camera_position, bool render_bounding_box);
};

#endif