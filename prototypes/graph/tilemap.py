from __future__ import annotations
from graph.geometry import Point, Directions, directions_dict
# from graph.constants import Directions, directions_dict
from graph.utils import count_set_bits


class Tile:
    """Represents a tile on the TileMap."""
    def __init__(self, position: Point, tilemap: TileMap):
        self.position = position                # the position on the grid
        self.__tilemap = tilemap

        self.tile_connection_bitmask = 0        # the connections bitmask - NESW
        self.connected_tiles = set()            # Node connections (edges)

    def __scan(self, direction: int) -> Tile:
        """Return the the most distant tile connected in the given direction."""
        opposing_direction = ((direction >> 2 | direction << 2) & 15)

        valid = True
        current_tile = self

        while valid:

            next_tile = self.__tilemap[
                current_tile.position + directions_dict[direction]
            ]

            # Check if the next tile has a connection further on in the same 
            # direction; remove the connection and return the termination point
            # if so
            for connected_tile in next_tile.connected_tiles:
                if (
                    connected_tile.position - next_tile.position
                ).vector_direction_bitmask == direction:
                    next_tile.__disconnect(connected_tile)
                    connected_tile.__disconnect(next_tile)
                    return connected_tile

            valid = next_tile.tile_connection_bitmask & opposing_direction

            if valid:
                current_tile = next_tile

        return current_tile
    
    def __connect(self, tile: Tile) -> None:
        self.connected_tiles.add(tile)
        self.__tilemap.nodes = self.__tilemap.nodes | {tile, self}

    def __disconnect(self, tile: Tile) -> None:
        self.connected_tiles.remove(tile)
        self.__tilemap.nodes = self.__tilemap.nodes - {tile, self}

    def connect(self, directions: int) -> None:
        """Update the tile's connections to connect in specified directions."""
        disconnected_directions = self.tile_connection_bitmask & ~directions
        self.tile_connection_bitmask = self.tile_connection_bitmask | directions

        output = {}
        for direction in (1, 2, 4, 8):
            if directions & direction:
                output[direction] = self.__scan(direction)
            else:
                output[direction] = self

        # Create a node from the current tile if it connects in more than two 
        # directions, or the connections represent a direction change
        if (
            count_set_bits(directions) != 2 
            or directions not in (
                Directions.EAST.value | Directions.WEST.value, 
                Directions.NORTH.value | Directions.SOUTH.value
            )
        ):
            for connected_tile in output.values():
                if connected_tile.position != self.position:
                    self.__connect(connected_tile)
                    connected_tile.__connect(self)
            return
        
        for lhs, rhs in (
            (output[Directions.EAST.value], output[Directions.WEST.value]),
            (output[Directions.NORTH.value], output[Directions.SOUTH.value])
        ):
            if lhs != rhs:
                lhs.__connect(rhs)
                rhs.__connect(lhs)

    def clear_connections(self) -> None:
        """Clear the tile's connections."""
        self.tile_connection_bitmask = 0
        self.connected_tiles = set()


class TileMap:
    """Represents a (square) tilemap as a list of tiles."""
    def __init__(self, size: int):
        self.size = size

        self.tiles = [
            Tile(Point(number % self.size, number // self.size), self) 
            for number in range(0, self.size**2)
        ]

        self.nodes = set()

    def __getitem__(self, position: Point) -> Tile:
        """Get the tile from a point (x,y) position on the tilemap."""
        return self.tiles[
            (position.y * self.size) + position.x
        ]
    
    def clear_connections(self) -> None:
        """Clear connections for every tile, and reset the graph."""
        for tile in self.tiles:
            tile.clear_connections()