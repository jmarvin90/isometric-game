#ifndef TILESPECCOMPONENT_H
#define TILESPECCOMPONENT_H

#include <SDL2/SDL.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>

#include <directions.h>

// TODO - probably move gates / translations
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
    glm::ivec2 tile_size;
    const int road_width;

    const glm::ivec2 centre;
    const glm::ivec2 road_mark_offset;

    std::array<Gate, 4> road_gates;
    std::vector<SDL_Point> iso_points;

    TileSpecComponent(const glm::ivec2 tile_size, const int road_width)
        : tile_size { tile_size }
        , road_width { road_width }
        , centre { tile_size / 2 }
        , road_mark_offset { road_width / 4, road_width / 8 }
        , iso_points {
            SDL_Point { centre.x, 0 },
            SDL_Point { tile_size.x, centre.y },
            SDL_Point { centre.x, tile_size.y },
            SDL_Point { 0, centre.y },
            SDL_Point { centre.x, 0 },
        }
    {
        for (
            Direction::TDirection direction = Direction::TDirection::NORTH;
            direction != Direction::TDirection::NO_DIRECTION;
            direction = direction >> 1
        ) {
            uint8_t index { Direction::index_position(direction) };
            road_gates[index] = {
                centre + (road_mark_offset * translations[index].entry), // entry
                centre + (road_mark_offset * translations[index].exit) // exit
            };
        }
    }
};

#endif