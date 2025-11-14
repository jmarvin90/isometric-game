from __future__ import annotations
import enum
import math

class Direction(enum.Enum):
    NORTH = 1 << 0
    NORTH_EAST = 1 << 1
    EAST = 1 << 2
    SOUTH_EAST = 1 << 3
    SOUTH = 1 << 4
    SOUTH_WEST = 1 << 5
    WEST = 1 << 6
    NORTH_WEST = 1 << 7
    NO_DIRECTION = 0


class Vector:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def __str__(self) -> str:
        return f"({self.x},{self.y})"

    def __eq__(self, query: Vector) -> bool:
        return self.x == query.x and self.y == query.y
    
    def __sub__(self, query: Vector) -> Vector:
        return Vector(self.x - query.x, self.y - query.y)
    
    def __add__(self, point: Vector) -> Vector:
        return Vector(self.x + point.x, self.y + point.y)
    
    def adjacent_to(self, point: Vector) -> bool:
        comparison = self - point
        return max(abs(comparison.x), abs(comparison.y)) == 1
    
    @property
    def direction(self) -> Direction:
        vec = (
            int(math.copysign(1, self.x)) if self.x != 0 else 0,
            int(math.copysign(1, self.y)) if self.y != 0 else 0
        )
        return reverse_directions_dict[vec]


directions_dict = {
    Direction.NORTH: Vector(0, -1),
    Direction.NORTH_EAST: Vector(1, -1),
    Direction.EAST: Vector(1, 0),
    Direction.SOUTH_EAST: Vector(1, 1),
    Direction.SOUTH: Vector(0, 1),
    Direction.SOUTH_WEST: Vector(-1, 1),
    Direction.WEST: Vector(-1, 0),
    Direction.NORTH_WEST: Vector(-1, -1),
    Direction.NO_DIRECTION: Vector(0, 0)
}

reverse_directions_dict = {
    (0, -1): Direction.NORTH.value,  
    (1, -1): Direction.NORTH_EAST.value,
    (1, 0): Direction.EAST.value, 
    (1, 1): Direction.SOUTH_EAST.value,
    (0, 1): Direction.SOUTH.value,
    (-1, 1): Direction.SOUTH_WEST.value,
    (-1, 0): Direction.WEST.value,
    (-1, -1): Direction.NORTH_WEST.value,
    (0, 0): Direction.NO_DIRECTION.value
}


class Segment:
    _next_id = 0
   
    def __init__(self, origin: Vector, termination: Vector):
        self.origin = origin
        self.termination = termination
        self.id = Segment._next_id
        Segment._next_id += 1

    def __str__(self) -> str:
        return f"{self.origin} -> {self.termination}"

    @property
    def directions(self) -> int:
        return (
            (self.origin - self.termination).direction |
            (self.termination - self.origin).direction
        )
    
    @property
    def is_orphan(self) -> bool:
        return self.origin == self.termination

    def __contains__(self, query: Vector) -> bool:
        # Vertical segment: x is constant
        if self.origin.x == self.termination.x == query.x:
            y1, y2 = self.origin.y, self.termination.y
            return min(y1, y2) <= query.y <= max(y1, y2)

        # Horizontal segment: y is constant
        if self.origin.y == self.termination.y == query.y:
            x1, x2 = self.origin.x, self.termination.x
            return min(x1, x2) <= query.x <= max(x1, x2)

        return False

class Segments:
    def __init__(self):
        self.segments = {}

    def add_segment(self, origin: Vector, termination: Vector) -> Segment:
        new_segment = Segment(origin, termination)
        self.segments[new_segment.id] = new_segment
        return new_segment

    def remove_segment(self, id: int) -> None:
        del self.segments[id]

    def get_segment(self, id: int) -> Segment | None:
        return self.segments[id]
 
