#include <cmath>
#include <directions.h>

namespace Direction {
TDirection reverse(const TDirection direction) { return direction >> 2 | direction << 2; }

bool is_junction(Direction::TDirection direction) { return __builtin_popcount(to_underlying(direction)) > 2; }

uint8_t index_position(Direction::TDirection direction) { return __builtin_ctz(to_underlying(direction)); }

glm::ivec2 to_direction_vector(const glm::ivec2& vector)
{
    return {
        vector.x != 0 ? std::copysign(1, vector.x) : 0,
        vector.y != 0 ? std::copysign(1, vector.y) : 1
    };
}

glm::vec2 to_direction_vector(const glm::vec2& vector)
{
    return {
        vector.x != 0 ? std::copysign(1, vector.x) : 0,
        vector.y != 0 ? std::copysign(1, vector.y) : 1
    };
}

TDirection from_vector(const glm::ivec2& vector)
{

    return Direction::vector_directions[to_direction_vector(vector)];
}

}; // namespace Direction