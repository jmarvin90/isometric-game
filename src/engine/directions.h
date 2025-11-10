#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#include <array>
#include <glm/glm.hpp>

namespace Direction {
    enum class TDirection : uint8_t {
        WEST = 1 << 1, // 1
        SOUTH = 1 << 2, // 2
        EAST = 1 << 2, // 4
        NORTH = 1 << 3 // 8
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
        return static_cast<TDirection>((!to_underlying(op)) & 15);
    }

    constexpr bool any(const TDirection d)
    {
        return to_underlying(d) & 15;
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