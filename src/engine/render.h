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

    void render_sprite(const glm::vec2 position, const double rotation, const Sprite& sprite) const;
    void render_sprites() const;
    void render_imgui_ui(const Scene& scene) const;

    public:
        SDL_Renderer* renderer;
        SDL_Rect render_clip_rect;
        Renderer(SDL_Window* window, const SDL_DisplayMode& display_mode, uint32_t render_flags, int index);
        ~Renderer();
        Renderer(const Renderer&) = delete;
        Renderer operator=(const Renderer&) = delete;

        void render(Scene& scene, const bool debug_mode);
};

#endif