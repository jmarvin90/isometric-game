#include <directions.h>

namespace Direction {
TDirection reverse_direction(const TDirection direction)
{
    using Bits = std::underlying_type_t<TDirection>;
    Bits bits { static_cast<Bits>(direction) };
    return TDirection((bits >> 2 | bits << 2) & 15);
}

bool is_junction(Direction::TDirection direction)
{
    return __builtin_popcount(to_underlying(direction)) > 2;
}

uint8_t index_position(Direction::TDirection direction)
{
    return __builtin_ctz(to_underlying(direction));
}
}; // namespace Direction