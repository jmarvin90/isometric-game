#ifndef POINT_H
#define POINT_H

struct Point {
    int x;
    int y;
    // Point(int x, int y): x{x}, y{y};
    bool operator==(const Point& comparator) const;
    Point operator+(const Point& second) const;
    Point operator*(int multiplier) const;
    bool operator!=(const Point& comparator) const;
};

#endif