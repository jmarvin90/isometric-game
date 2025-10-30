#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <glm/glm.hpp>
#include <array>

namespace Direction {
    enum class TDirection: uint8_t {
        EAST = 1 << 0,
        SOUTH = 1 << 1,
        WEST = 1 << 2,
        NORTH = 1 << 3
    };

    struct DirectionInfo {
        TDirection direction;
        glm::ivec2 vec;
    };

    constexpr std::array<DirectionInfo, 4> directions {{
        { TDirection::EAST, {1, 0} },
        { TDirection::SOUTH, {0, 1} },
        { TDirection::WEST, {-1, 0} },
        { TDirection::NORTH, {0, -1} }
    }};
};

#endif