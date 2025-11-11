#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <array>
#include <glm/glm.hpp>

/*
        1
    128     2
64              4
    32      8
        16
*/

namespace Direction {
    enum class TDirection : uint8_t {
        NORTH = 1 << 0, // 64
        NORTH_WEST = 1 << 1, // 128
        WEST = 1 << 2, // 1
        SOUTH_WEST = 1 << 3, // 2
        SOUTH = 1 << 4, // 4
        SOUTH_EAST = 1 << 5, // 8
        EAST = 1 << 6, // 16
        NORTH_EAST = 1 << 7, // 32
    };

    template <typename T>
    constexpr uint8_t to_underlying(T t)
    {
        return static_cast<std::underlying_type_t<T>>(t);
    }

    constexpr TDirection operator|(const TDirection lhs, const TDirection rhs)
    {
        return static_cast<Direction::TDirection>(to_underlying(lhs) | to_underlying(rhs));
    }

    constexpr TDirection operator&(const TDirection lhs, const TDirection rhs)
    {
        return static_cast<TDirection>(to_underlying(lhs) & to_underlying(rhs));
    }

    constexpr TDirection operator!(const TDirection op)
    {
        return static_cast<TDirection>(!to_underlying(op));
    }

    constexpr bool any(const TDirection d)
    {
        return to_underlying(d);
    }

    struct DirectionInfo {
        TDirection direction;
        glm::ivec2 vec;
    };

    constexpr std::array<DirectionInfo, 4> directions {
        { { TDirection::WEST, { -1, 0 } },
            { TDirection::SOUTH, { 0, 1 } },
            { TDirection::EAST, { 1, 0 } },
            { TDirection::NORTH, { 0, -1 } } }
    };

    TDirection reverse_direction(const TDirection direction);

    bool is_junction(Direction::TDirection direction);

    uint8_t index_position(Direction::TDirection direction);
}; // namespace Direction

#endif