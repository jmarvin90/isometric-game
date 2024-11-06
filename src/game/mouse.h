#ifndef MOUSE_H
#define MOUSE_H

#include <string>
#include "SDL2/SDL.h"
#include <glm/glm.hpp>

constexpr glm::ivec2 horizontal_vector{1, -1};
constexpr glm::ivec2 vertical_vector{1, 1};

class Mouse {
    glm::ivec2 window_previous_position;
    glm::ivec2 window_current_position;
    glm::ivec2 world_position;
    glm::ivec2 grid_position;
    SDL_Surface* mousemap;

    SDL_Color mousemap_pixel_colour(const glm::ivec2& pixel_offset) const;
    glm::ivec2 pixel_colour_vector(const SDL_Color& colour) const;
    glm::ivec2 tile_walk(const glm::ivec2& tile_offset) const;

    glm::ivec2 pixel_to_grid() const;
    void set_position(const SDL_Rect& camera);

    public:
        Mouse(const std::string mousemap_file_path);
        ~Mouse();

        void update(const SDL_Rect& camera);
        const glm::ivec2& get_window_position() const;
        const glm::ivec2& get_world_position() const;
        const glm::ivec2& get_grid_position() const;
        const bool has_moved_this_frame() const;
        const bool is_on_world_grid() const;
};

#endif