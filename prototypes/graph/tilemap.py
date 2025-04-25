from __future__ import annotations
from functools import cached_property
from graph.geometry import Point, Directions, directions_dict
from graph.utils import count_set_bits


class Tile:
    """Represents a tile on the TileMap."""
    def __init__(self, position: Point, tilemap: TileMap):
        self.position = position                # the position on the grid
        self.__tilemap = tilemap
        self.tile_connection_bitmask = 0        # the connections bitmask - NESW
        
    def __str__(self) -> str:
        return str(self.position)

    @cached_property
    def partition_index(self) -> int:
        x_div = self.position.x // self.__tilemap.zone_size
        y_div = self.position.y // self.__tilemap.zone_size
        return int((self.__tilemap.zone_size * y_div) + x_div)
    
    def set_tile_connection_bitmask(self, tile_connection_bitmask: int) -> None:
        self.tile_connection_bitmask = tile_connection_bitmask


class Edge:
    def __init__(self, origin: Tile, termination: Tile):
        self.origin = origin
        self.termination = termination

    def __str__(self) -> str:
        return f"{self.origin} -> {self.termination}"

    @cached_property
    def is_vertical(self) -> bool:
        return bool(
            self.origin.tile_connection_bitmask &
            self.termination.tile_connection_bitmask &
            (Directions.NORTH.value | Directions.SOUTH.value)
        )
    
    @cached_property
    def is_horizontal(self) -> bool:
        return bool(
            self.origin.tile_connection_bitmask &
            self.termination.tile_connection_bitmask &
            (Directions.EAST.value | Directions.WEST.value)
        )

class TileMap:
    """Represents a (square) tilemap as a list of tiles."""
    def __init__(self, size: int):
        self.size = size
        self.zone_size = int(size / 3)

        self.tiles = [
            Tile(Point(number % self.size, number // self.size), self) 
            for number in range(0, self.size**2)
        ]

        self.edges = {}

    def __getitem__(self, position: Point) -> Tile:
        """Get the tile from a point (x,y) position on the tilemap."""
        return self.tiles[
            (position.y * self.size) + position.x
        ]
    
    def clear_connections(self) -> None:
        """Clear connections for every tile, and reset the graph."""
        pass

    def create_edge(self, lhs: Tile, rhs:Tile) -> Edge:
        edge = Edge(lhs, rhs)
        factor = 1 if edge.is_horizontal else int(self.size / self.zone_size)

        if edge.origin.partition_index > edge.termination.partition_index:
            factor *= -1

        for number in range(
            edge.origin.partition_index, 
            edge.termination.partition_index + 1,
            factor
        ):
            if number not in self.edges:
                self.edges[number] = [edge]

            else:
                self.edges[number].append(edge)

        return edge
