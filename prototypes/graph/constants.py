from enum import Enum

from graph.geometry import Point

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