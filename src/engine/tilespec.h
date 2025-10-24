#ifndef TILESPEC_H
#define TILESPEC_H

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <vector>

struct TileSpec {
    const glm::ivec2 size;
    const glm::mat2 matrix;
    const glm::mat2 matrix_inverted;
    TileSpec(const glm::ivec2 size)
    : size{size}
    , matrix {size.x / 2.0f, size.y / 2.0f, -size.x / 2.0f, size.y / 2.0f}
    , matrix_inverted {glm::inverse(matrix)} 
    {}
    ~TileSpec() = default;
    TileSpec(const TileSpec&) = delete;

    const glm::ivec2 centre() const {
        return size / 2;
    }

    const std::vector<SDL_Point> iso_points() const {
        const glm::ivec2 centre {size / 2};
        return {
            SDL_Point {centre.x, 0},
            SDL_Point {size.x, centre.y},
            SDL_Point {centre.x, size.y},
            SDL_Point {0, centre.y},
            SDL_Point {centre.x, 0},
        };
    }

    const std::vector<SDL_Point> tile_points() const {
        return {
            {0, 0},
            {size.x, 0},
            {size.x, size.y},
            {0, size.y},
            {0, 0}
        };
    }
};

#endif