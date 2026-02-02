#ifndef TILESPECCOMPONENT_H
#define TILESPECCOMPONENT_H

#include <SDL2/SDL.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>

#include <directions.h>

struct Gate {
    glm::ivec2 entry;
    glm::ivec2 exit;
};

constexpr std::array<Gate, 4> translations {
    {
        { { 3, -1 }, { 1, -3 } }, // North
        { { -1, -3 }, { -3, -1 } }, // East
        { { -3, 1 }, { -1, 3 } }, // South
        { { 1, 3 }, { 3, 1 } } // West
    }
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

    std::array<Gate, 4> road_gates;
    std::vector<SDL_Point> iso_points;

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
        , iso_points {
            SDL_Point { centre.x, 0 },
            SDL_Point { iso_area.x, centre.y },
            SDL_Point { centre.x, iso_area.y },
            SDL_Point { 0, centre.y },
            SDL_Point { centre.x, 0 },
        }
    {
        for (int i = 0; i < 4; i++) {
            road_gates[i] = {
                centre + (road_mark_offset * translations[i].entry), // entry
                centre + (road_mark_offset * translations[i].exit) // exit
            };
        }
    }
};

#endif