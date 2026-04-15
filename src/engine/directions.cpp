#include <cmath>
#include <directions.h>

namespace Direction {

bool opposed(Direction::TDirection direction) {
    Direction::TDirection NS {(Direction::TDirection::NORTH | Direction::TDirection::SOUTH)};
    Direction::TDirection EW {(Direction::TDirection::EAST | Direction::TDirection::WEST)};
    bool is_opposed { direction == NS || direction == EW };
    return is_opposed;
}

TDirection reverse(const TDirection direction) { 
    return direction >> 2 | direction << 2; 
}

bool is_junction(Direction::TDirection direction) {
    if (direction == Direction::TDirection::NO_DIRECTION)
        return false;

    auto popcount { __builtin_popcount(to_underlying(direction)) };
    bool is_junction {
        (popcount != 2 || (popcount == 2 && !opposed(direction)))
    };
    return is_junction; 
}

uint8_t index_position(Direction::TDirection direction) { 
    return __builtin_ctz(to_underlying(direction)); 
}

}; // namespace Direction