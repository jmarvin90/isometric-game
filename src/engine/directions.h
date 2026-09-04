#ifndef DIRECTIONS_H
#define DIRECTIONS_H
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <numeric>
#include <type_traits>
#include <unordered_map>

/*
 -- Cardinally

    3-------1-------9
    |               |
    2               8
    |               |
    6-------4-------12


 -- Isometrically

            3
        2       1
    6               9
        4      8
            12
*/

// TODO - investigate entt enum bitmask
namespace Direction {
enum class TDirection : uint8_t {
    NO_DIRECTION = 0,

    NORTH = 1 << 0,
    WEST = 1 << 1,
    SOUTH = 1 << 2,
    EAST = 1 << 3,

    NORTH_WEST = NORTH | WEST,
    SOUTH_WEST = SOUTH | WEST,
    NORTH_EAST = NORTH | EAST,
    SOUTH_EAST = SOUTH | EAST,

    ALL_CARDINAL_DIRECTIONS = NORTH | SOUTH | EAST | WEST,
    ALL_DIAGONAL_DIRECTIONS = NORTH_WEST | SOUTH_WEST | SOUTH_EAST | NORTH_EAST,
    ALL_DIRECTIONS = ALL_CARDINAL_DIRECTIONS | ALL_DIAGONAL_DIRECTIONS
};

inline const std::unordered_map<TDirection, glm::ivec2> isometric_direction_vectors { {
    { TDirection::NORTH, { 2, -1 } }, //
    { TDirection::NORTH_WEST, { 0, -1 } }, //
    { TDirection::WEST, { -2, -1 } }, //
    { TDirection::SOUTH_WEST, { -1, 0 } },
    { TDirection::SOUTH, { -2, 1 } }, //
    { TDirection::SOUTH_EAST, { 0, 1 } },
    { TDirection::EAST, { 2, 1 } }, //
    { TDirection::NORTH_EAST, { 1, 0 } } //
    // { TDirection::NORTH, { 2, -1 } }, //
    // { TDirection::NORTH_WEST, { 1, 0 } }, //
    // { TDirection::WEST, { 2, 1 } }, //
    // { TDirection::SOUTH_WEST, { 0, 1 } },
    // { TDirection::SOUTH, { -1, 2 } }, //
    // { TDirection::SOUTH_EAST, { -1, 0 } },
    // { TDirection::EAST, { -2, -1 } }, //
    // { TDirection::NORTH_EAST, { 0, -0 } } //
} };

inline const std::unordered_map<TDirection, glm::ivec2> direction_vectors { {
    { TDirection::NORTH, { 0, -1 } }, //
    { TDirection::NORTH_WEST, { -1, -1 } }, //
    { TDirection::WEST, { -1, 0 } }, //
    { TDirection::SOUTH_WEST, { -1, 1 } }, //
    { TDirection::SOUTH, { 0, 1 } }, //
    { TDirection::SOUTH_EAST, { 1, 1 } }, //
    { TDirection::EAST, { 1, 0 } }, //
    { TDirection::NORTH_EAST, { 1, -1 } } //
} };

inline const std::unordered_map<glm::ivec2, TDirection> vector_directions { {
    { { 0, -1 }, TDirection::NORTH }, //
    { { -1, -1 }, TDirection::NORTH_WEST }, //
    { { -1, 0 }, TDirection::WEST }, //
    { { -1, 1 }, TDirection::SOUTH_WEST }, //
    { { 0, 1 }, TDirection::SOUTH }, //
    { { 1, 1 }, TDirection::SOUTH_EAST }, //
    { { 1, 0 }, TDirection::EAST }, //
    { { 1, -1 }, TDirection::NORTH_EAST } //
} };

struct DirectionIterator {
    Direction::TDirection remaining_directions;
    Direction::TDirection current_direction;
    DirectionIterator(Direction::TDirection directions);
    Direction::TDirection operator*() const;
    DirectionIterator& operator++();
    DirectionIterator operator++(int);
    bool operator==(const DirectionIterator& comparator) const;
    bool operator!=(const DirectionIterator& comparator) const;
};

struct EachDirectionIn {
    Direction::TDirection directions;
    DirectionIterator begin() const;
    DirectionIterator end() const;
};

template <typename T>
constexpr std::underlying_type_t<T> to_underlying(T t)
{
    return static_cast<std::underlying_type_t<T>>(t);
}

constexpr TDirection mask(uint8_t bits)
{
    return TDirection(bits & to_underlying(TDirection::ALL_DIRECTIONS));
}

template <typename T>
glm::vec2 to_direction_vector(const T& vector)
{
    T abs_vector { glm::abs(vector) };
    if (abs_vector.x == abs_vector.y)
        return { std::copysign(1, vector.x), std::copysign(1, vector.y) };
    if (abs_vector.x > abs_vector.y)
        return { std::copysign(1, vector.x), 0 };
    return { 0, std::copysign(1, vector.y) };
}

template <typename T>
TDirection from_vector(const T& vector)
{
    return Direction::vector_directions.at(to_direction_vector<T>(vector));
}

constexpr TDirection operator-(const TDirection op)
{
    return mask(uint8_t((-to_underlying(op))));
}

constexpr TDirection operator|(
    const TDirection lhs, const TDirection rhs
)
{
    return static_cast<Direction::TDirection>(
        to_underlying(lhs) | to_underlying(rhs)
    );
}

constexpr TDirection operator&(
    const TDirection lhs, const TDirection rhs
)
{
    return Direction::TDirection {
        uint8_t(to_underlying(lhs) & to_underlying(rhs))
    };
}

constexpr TDirection operator!(const TDirection op)
{
    return mask(uint8_t((!to_underlying(op))));
}

constexpr TDirection operator>>(
    const TDirection lhs, const int places
)
{
    uint8_t bits { Direction::to_underlying(lhs) };
    return mask(bits << places);
}

constexpr TDirection operator<<(
    const TDirection lhs, const int places
)
{
    uint8_t bits { Direction::to_underlying(lhs) };
    return mask(bits >> places);
}

constexpr bool any(const TDirection d)
{
    return bool(mask(to_underlying(d)));
}

TDirection reverse(const TDirection direction);
bool is_junction(Direction::TDirection direction);
uint8_t index_position(Direction::TDirection direction);
bool opposed(Direction::TDirection direction);

}; // namespace Direction

#endif