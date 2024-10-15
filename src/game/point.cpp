#include "point.h"

Point Point::operator+(const Point& second) const {
    return Point{x + second.x, y + second.y};
}

Point Point::operator*(const int multiplier) const {
    return Point{x * multiplier, y * multiplier};
}

bool Point::operator==(const Point& comparator) const {
    return (x == comparator.x && y == comparator.y);
}

bool Point::operator!=(const Point& comparator) const {
    return (x != comparator.x || y != comparator.y);
}