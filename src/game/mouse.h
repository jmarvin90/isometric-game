#ifndef MOUSE_H
#define MOUSE_H

#include <string>
#include "glm/glm.hpp"
#include "SDL2/SDL.h"

#include "point.h"

constexpr Point horizontal_vector{1, -1};
constexpr Point vertical_vector{1, 1};

class Mouse {
    Point position;
    SDL_Surface* mousemap;
    SDL_Color mousemap_pixel_colour(const Point& pixel_offset) const;
    Point pixel_colour_vector(const SDL_Color& colour) const;
    Point tile_walk(const Point& tile_offset) const;

    public:
        Mouse(const std::string mousemap_file_path);
        ~Mouse();
        Point pixel_to_grid() const;
        void set_pos(const Point& mouse_position);
};

#endif