#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include <components/sprite_component.h>
#include <components/transform_component.h>
#include <constants.h>

class Scene;

class Renderer{

    template <typename T>
    void draw_lines(const T& line_component, const glm::ivec2 offset) const {
        std::vector<SDL_Point> points;
        SDL_Point my_point {0, 0};

        for (auto& point: line_component->points) {
            points.push_back(SDL_Point{point.x + offset.x, point.y + offset.y});
        }

        SDL_SetRenderDrawColor(
            renderer,
            line_component->colour.r, 
            line_component->colour.g,
            line_component->colour.b, 
            line_component->colour.a 
        );

        SDL_RenderDrawLines(
            renderer,
            points.data(),
            points.size()
        );
    }

    void render_sprite(
        entt::registry& registry,
        const TransformComponent& transform,
        const SpriteComponent& sprite
    ) const;

    void render_sprites() const;
    void render_imgui_ui(entt::registry& registry) const;

    public:
        SDL_Renderer* renderer;
        SDL_Rect render_clip_rect;
        Renderer(SDL_Window* window, const SDL_DisplayMode& display_mode, uint32_t render_flags, int index);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer operator=(const Renderer&) = delete;

        void render(entt::registry& registry, const bool debug_mode);
};

#endif