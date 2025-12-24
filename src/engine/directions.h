#ifndef DIRECTIONS_H
#define DIRECTIONS_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <array>
#include <glm/glm.hpp>
#include <unordered_map>

/*
    1
  8   2
    4
*/

// TODO - investigate entt enum bitmask
namespace Direction {
enum class TDirection : uint8_t { NO_DIRECTION = 0,
                                  NORTH = 1 << 0,
                                  WEST = 1 << 1,
                                  SOUTH = 1 << 2,
                                  EAST = 1 << 3 };

template <typename T>
constexpr uint8_t to_underlying(T t) { return static_cast<std::underlying_type_t<T>>(t); }

constexpr TDirection operator|(const TDirection lhs, const TDirection rhs)
{
    return static_cast<Direction::TDirection>(to_underlying(lhs) | to_underlying(rhs));
}

constexpr TDirection operator&(const TDirection lhs, const TDirection rhs)
{
    return static_cast<TDirection>(to_underlying(lhs) & to_underlying(rhs));
}

constexpr TDirection operator!(const TDirection op) { return static_cast<TDirection>((!to_underlying(op)) & 15); }

constexpr TDirection operator>>(const TDirection lhs, const int places)
{
    uint8_t bits { Direction::to_underlying(lhs) };
    return Direction::TDirection((bits << places) & 15);
}

constexpr TDirection operator<<(const TDirection lhs, const int places)
{
    uint8_t bits { Direction::to_underlying(lhs) };
    return Direction::TDirection((bits >> places) & 15);
}

constexpr bool any(const TDirection d) { return to_underlying(d) & 15; }

inline std::unordered_map<TDirection, glm::ivec2> direction_vectors {
    { TDirection::NORTH, { 0, -1 } },
    { TDirection::EAST, { 1, 0 } },
    { TDirection::SOUTH, { 0, 1 } },
    { TDirection::WEST, { -1, 0 } },
};

inline std::unordered_map<glm::ivec2, TDirection> vector_directions { {
    { { 0, -1 }, TDirection::NORTH },
    { { 1, 0 }, TDirection::EAST },
    { { 0, 1 }, TDirection::SOUTH },
    { { -1, 0 }, TDirection::WEST },
} };

TDirection reverse(const TDirection direction);

bool is_junction(Direction::TDirection direction);

uint8_t index_position(Direction::TDirection direction);
}; // namespace Direction

#endif