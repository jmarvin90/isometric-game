#ifndef TILESPEC_H
#define TILESPEC_H

#include <glm/glm.hpp>
#include <SDL2/SDL.h>
#include <vector>

struct TileSpec {
    const glm::ivec2 iso_area;
    const int depth;
    const glm::ivec2 total_area;
    const glm::mat2 matrix;
    const glm::mat2 matrix_inverted;

    TileSpec(const int width, const int depth)
    : iso_area{width, width / 2}
    , depth {depth}
    , total_area {iso_area.x, iso_area.y + depth}
    , matrix {
        iso_area.x / 2.0f,
        iso_area.y / 2.0f,
        -iso_area.x / 2.0f,
        iso_area.y / 2.0f
    }
    , matrix_inverted {glm::inverse(matrix)} 
    {}
    ~TileSpec() = default;
    TileSpec(const TileSpec&) = delete;

    const glm::ivec2 centre() const {
        return iso_area / 2;
    }

    const std::vector<SDL_Point> iso_points() const {
        const glm::ivec2 tile_centre {centre()};
        return {
            SDL_Point {tile_centre.x, 0},
            SDL_Point {iso_area.x, tile_centre.y},
            SDL_Point {tile_centre.x, iso_area.y},
            SDL_Point {0, tile_centre.y},
            SDL_Point {tile_centre.x, 0},
        };
    }

    const std::vector<SDL_Point> tile_points() const {
        return {
            {0, 0},
            {iso_area.x, 0},
            {iso_area.x, iso_area.y},
            {0, iso_area.y},
            {0, 0}
        };
    }
};

#endif