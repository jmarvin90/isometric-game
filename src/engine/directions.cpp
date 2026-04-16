#include <cmath>
#include <directions.h>

namespace Direction {

DirectionIterator::DirectionIterator(Direction::TDirection directions)
    : remaining_directions { directions }
    , current_direction { directions & -directions }
{
}

Direction::TDirection DirectionIterator::operator*() const
{
    return current_direction;
}

DirectionIterator& DirectionIterator::operator++()
{
    remaining_directions = Direction::TDirection(
        Direction::to_underlying(remaining_directions)
        & (Direction::to_underlying(remaining_directions) - 1)
    );
    current_direction = remaining_directions & -remaining_directions;
    return *this;
}

bool DirectionIterator::operator==(const DirectionIterator& comparator) const
{
    return (this->remaining_directions == comparator.remaining_directions);
}

bool DirectionIterator::operator!=(const DirectionIterator& comparator) const
{
    return !(*this == comparator);
}

DirectionIterator EachDirectionIn::begin() const
{
    return DirectionIterator(directions);
}

DirectionIterator EachDirectionIn::end() const
{
    return DirectionIterator(Direction::TDirection::NO_DIRECTION);
}

TDirection reverse(const TDirection direction)
{
    return direction >> 2 | direction << 2;
}

bool is_junction(Direction::TDirection direction)
{
    if (direction == Direction::TDirection::NO_DIRECTION)
        return false;

    auto popcount { __builtin_popcount(to_underlying(direction)) };
    bool is_junction {
        (popcount != 2 || (popcount == 2 && !opposed(direction)))
    };
    return is_junction;
}

uint8_t index_position(Direction::TDirection direction)
{
    return __builtin_ctz(to_underlying(direction));
}

bool opposed(Direction::TDirection direction)
{
    Direction::TDirection NS { (Direction::TDirection::NORTH | Direction::TDirection::SOUTH) };
    Direction::TDirection EW { (Direction::TDirection::EAST | Direction::TDirection::WEST) };
    bool is_opposed { direction == NS || direction == EW };
    return is_opposed;
}

}; // namespace Direction