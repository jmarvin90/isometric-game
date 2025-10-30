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

    template <typename T>
    constexpr uint8_t to_underlying(T t) {
        return static_cast<std::underlying_type_t<T>>(t);
    } 

    constexpr TDirection operator|(const TDirection lhs, const TDirection rhs) {
        return static_cast<Direction::TDirection>(
            to_underlying(lhs) | to_underlying(rhs)
        );
    }

    constexpr TDirection operator&(const TDirection lhs, const TDirection rhs) {
        return static_cast<TDirection>(
            to_underlying(lhs) & to_underlying(rhs)
        );
    }

    constexpr bool any(const TDirection d) {
        return to_underlying(d) & 15;
    }
    
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

    TDirection reverse_direction(const TDirection direction);
};

#endif