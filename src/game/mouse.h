#ifndef MOUSE_H
#define MOUSE_H

#include <string>
#include "SDL2/SDL.h"
#include <glm/glm.hpp>

constexpr glm::vec2 horizontal_vector{1, -1};
constexpr glm::vec2 vertical_vector{1, 1};

class Mouse {
    glm::vec2 previous_position;
    glm::vec2 position;
    SDL_Surface* mousemap;
    SDL_Color mousemap_pixel_colour(const glm::vec2& pixel_offset) const;
    glm::vec2 pixel_colour_vector(const SDL_Color& colour) const;
    glm::vec2 tile_walk(const glm::vec2& tile_offset) const;

    public:
        Mouse(const std::string mousemap_file_path);
        ~Mouse();
        glm::vec2 pixel_to_grid() const;
        void update();
};

#endif