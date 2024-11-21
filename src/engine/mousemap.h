#ifndef MOUSEMAP_H
#define MOUSEMAP_H

#include <string>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

constexpr glm::ivec2 horizontal_vector{1, -1};
constexpr glm::ivec2 vertical_vector{1, 1};

class MouseMap {

    SDL_Surface* mousemap;
    SDL_Color mousemap_pixel_colour(const glm::ivec2& pixel_offset) const;
    glm::ivec2 pixel_colour_vector(const SDL_Color& colour) const;
    glm::ivec2 tile_walk(const glm::ivec2& tile_offset) const;

    public:
        MouseMap(const std::string& mousemap_file_path);
        ~MouseMap();
        glm::ivec2 pixel_to_grid(const glm::ivec2& pixel_coordinate) const;
};

#endif