#include <directions.h>

namespace Direction {
    TDirection reverse(const TDirection direction)
    {
        return direction >> 2 | direction << 2;
    }

    bool is_junction(Direction::TDirection direction)
    {
        return __builtin_popcount(to_underlying(direction)) > 2;
    }

    uint8_t index_position(Direction::TDirection direction)
    {
        return __builtin_ctz(to_underlying(direction));
    }

    // constexpr TDirection operator<<(const Direction::TDirection lhs, const int places)
    // {
    //     uint8_t bits { Direction::to_underlying(lhs) };
    //     return Direction::TDirection((bits >> places) & 15);
    // }
}; // namespace Direction