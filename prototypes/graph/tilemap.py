from graph.geometry import Point
from graph.constants import Directions, directions_dict

class Tile:
    def __init__(self):
        self.connections = 0

    def connect(self, directions: int) -> None:
        self.connections = self.connections | directions

    def clear_connections(self) -> None:
        self.connections = 0

class TileMap:
    def __init__(self, size: int):
        self.size = size
        self.tiles = [
            Tile() for number in range(0, self.size**2)
        ]

        self.valid_connections = 0

    def __getitem__(self, position: Point) -> Tile:
        return self.tiles[
            (position.y * self.size) + position.x
        ]
    
    def clear_connections(self) -> None:
        for tile in self.tiles:
            tile.clear_connections
    
    # TODO: think of a better name
    def connect(self, position: Point, directions: int) -> None:
        direction = 8

        tile = self[position]
        tile.connect(directions)

        while direction:
            if bool(direction & directions):
                corresponding_position = position + directions_dict[direction]
                corresponding_tile = self[corresponding_position]
                opposite_direction = (direction >> 2 | direction << 2) & 15
                connection = corresponding_tile.connections & opposite_direction
                if bool(connection):
                    # Adjacent tile in question has a receiving connection
                    self.valid_connections += 1
            direction = direction >> 1