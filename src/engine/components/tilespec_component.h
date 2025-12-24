#ifndef TILESPECCOMPONENT_H
#define TILESPECCOMPONENT_H

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <vector>

struct TileSpecComponent {
    const int width;
    const int depth;

    const glm::ivec2 iso_area;
    const glm::ivec2 total_area;
    const glm::ivec2 centre;
    const glm::mat2 matrix;
    const glm::mat2 matrix_inverted;

    TileSpecComponent(const int width, const int depth)
        : width { width }
        , depth { depth }
        , iso_area { width, width / 2 }
        , total_area { iso_area.x, iso_area.y + depth }
        , centre { iso_area / 2 }
        , matrix { iso_area.x / 2.0f, iso_area.y / 2.0f, -iso_area.x / 2.0f, iso_area.y / 2.0f }
        , matrix_inverted { glm::inverse(matrix) }
    {
    }

    const std::vector<SDL_Point> iso_points() const
    {
        return {
            SDL_Point { centre.x, 0 },
            SDL_Point { iso_area.x, centre.y },
            SDL_Point { centre.x, iso_area.y },
            SDL_Point { 0, centre.y },
            SDL_Point { centre.x, 0 },
        };
    }

    const std::vector<SDL_Point> tile_points() const
    {
        return { { 0, 0 }, { iso_area.x, 0 }, { iso_area.x, iso_area.y }, { 0, iso_area.y }, { 0, 0 } };
    }
};

#endif