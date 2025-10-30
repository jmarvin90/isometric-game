#include <directions.h>

namespace Direction {
    TDirection reverse_direction(const TDirection direction) {
        using Bits = std::underlying_type_t<TDirection>;
        Bits bits {static_cast<Bits>(direction)};
        return TDirection(
            (bits >> 2 | bits << 2) & 15
        );
    };
}

