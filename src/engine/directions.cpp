#include <cmath>
#include <directions.h>

namespace Direction {

bool opposed(Direction::TDirection direction) { 
    return (
        direction == (Direction::TDirection::NORTH | Direction::TDirection::SOUTH)
        || direction == (Direction::TDirection::EAST | Direction::TDirection::WEST)
    );
}
TDirection reverse(const TDirection direction) { return direction >> 2 | direction << 2; }

bool is_junction(Direction::TDirection direction) { 
    auto popcount { __builtin_popcount(to_underlying(direction)) };
    return (popcount != 2 || (popcount == 2 && !opposed(direction)));
}

uint8_t index_position(Direction::TDirection direction) { return __builtin_ctz(to_underlying(direction)); }

}; // namespace Direction