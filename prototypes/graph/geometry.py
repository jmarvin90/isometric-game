from __future__ import annotations
from enum import Enum
# from graph.constants import directions_dict
import math
import functools

class Point:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def __str__(self) -> str:
        return f"{self.x},{self.y}"

    def __add__(self, point: Point) -> Point:
        return Point(
            self.x + point.x,
            self.y + point.y
        )
    
    def __mul__(self, multiple: int) -> Point:
        return Point(
            self.x * multiple,
            self.y * multiple
        )
    
    def __eq__(self, comparator: Point) -> bool:
        return (
            self.x == comparator.x and 
            self.y == comparator.y
        )
    
    def __abs__(self) -> Point:
        return Point(
            abs(self.x), abs(self.y)
        )
    
    def __sub__(self, comparator: Point) -> Point:
        return Point(
            self.x - comparator.x, self.y - comparator.y
        )  
    
    @functools.cached_property
    def vector_direction(self) -> Point:
        return Point(
            int(math.copysign(1, self.x)) if self.x != 0 else 0,
            int(math.copysign(1, self.y)) if self.y != 0 else 0
        )
    
    @functools.cached_property
    def vector_direction_bitmask(self) -> int:
        for direction, vector in directions_dict.items():
            if vector == self.vector_direction:
                return direction


    def is_adjacent_to(self, comparator: Point):
        diff = abs(self - comparator)
        return diff.x <= 1 and diff.y <= 1 and diff != Point(0, 0)

    
class Directions(Enum):
    NORTH = 8
    EAST = 4
    SOUTH = 2
    WEST = 1
    NO_DIRECTION = 0

directions_dict = {
    Directions.NORTH.value: Point(0, -1),
    Directions.EAST.value: Point(1, 0),
    Directions.SOUTH.value: Point(0, 1),
    Directions.WEST.value: Point(-1, 0),
    Directions.NO_DIRECTION.value: Point(0, 0)
}

reverse_directions_dict = {
    Point(0, -1): Directions.NORTH.value,
    Point(1, 0): Directions.EAST.value,
    Point(0, 1): Directions.SOUTH.value,
    Point(-1, 0): Directions.WEST.value,
    Point(0, 0): Directions.NO_DIRECTION.value
}
