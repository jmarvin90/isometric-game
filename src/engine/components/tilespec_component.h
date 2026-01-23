#ifndef TILESPECCOMPONENT_H
#define TILESPECCOMPONENT_H

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <vector>

struct Gate {
    glm::ivec2 entry;
    glm::ivec2 exit;
};

struct TileSpecComponent {
    const int width;
    const int depth;
    const int road_depth_offset;
    const int road_width;

    const glm::ivec2 iso_area;
    const glm::ivec2 total_area;
    const glm::ivec2 centre;
    const glm::ivec2 road_mark_offset;
    const glm::mat2 matrix;
    const glm::mat2 matrix_inverted;

    TileSpecComponent(const int width, const int depth, const int road_depth_offset, const int road_width)
        : width { width }
        , depth { depth }
        , road_depth_offset { road_depth_offset }
        , road_width { road_width }
        , iso_area { width, width / 2 }
        , total_area { iso_area.x, iso_area.y + depth }
        , centre { iso_area / 2 }
        , road_mark_offset { road_width / 4, road_width / 8 }
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

    // TODO: spurious copies
    const std::vector<Gate> road_gates() const
    {
        return {
            Gate { centre + (road_mark_offset * glm::ivec2 { 3, -1 }), centre + (road_mark_offset * glm::ivec2 { 1, -3 }) },
            Gate { centre + (road_mark_offset * glm::ivec2 { -1, -3 }), centre + (road_mark_offset * glm::ivec2 { -3, -1 }) },
            Gate { centre + (road_mark_offset * glm::ivec2 { -3, 1 }), centre + (road_mark_offset * glm::ivec2 { -1, 3 }) },
            Gate { centre + (road_mark_offset * glm::ivec2 { 1, 3 }), centre + (road_mark_offset * glm::ivec2 { 3, 1 }) }
        };
    };
};

#endif