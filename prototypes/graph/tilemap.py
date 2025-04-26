from __future__ import annotations
from functools import cached_property
from graph.geometry import Point, Directions, directions_dict, reverse_directions_dict
from graph.utils import count_set_bits


class Tile:
    """Represents a tile on the TileMap."""
    def __init__(self, position: Point, tilemap: TileMap):
        self.position = position                # the position on the grid
        self.__tilemap = tilemap
        self.tile_connection_bitmask = 0        # the connections bitmask - NESW
        
    def __str__(self) -> str:
        return str(self.position)
    
    def __scan(self, direction: int) -> Tile:
        if not direction & self.tile_connection_bitmask:
            return self
        
        opposite_direction = (direction >> 2 | direction << 2) & 15
        valid = True
        current_tile = self

        while valid:
            next_point = current_tile.position + directions_dict[direction]
            
            if not self.__tilemap.in_bounds(next_point):
                return current_tile
        
            next_tile = self.__tilemap[next_point]

            # TODO: a missed opportunity to short-circuit the function
            # at the bottom of this conditional with an early return of the 
            # 'connection' tile. This was causing problems when 'reconnecting'
            if next_tile.tile_connection_bitmask & opposite_direction:
                if next_tile in self.__tilemap.edges:
                    for connection in self.__tilemap.edges[next_tile]:
                        if (
                            Edge(next_tile, connection).connection_bitmask &
                            (direction | opposite_direction)
                        ):
                            self.__tilemap.disconnect(next_tile, connection)

                current_tile = next_tile
            
            else:
                valid = False

        return current_tile
    
    def connect(self, tile_connection_bitmask: int) -> None:
        self.tile_connection_bitmask = tile_connection_bitmask

        connections = {
            direction: self.__scan(direction)
            for direction in [8, 4, 2, 1]
            if direction & tile_connection_bitmask
        }

        if self.tile_connection_bitmask in (
            Directions.NORTH.value | Directions.SOUTH.value,
            Directions.EAST.value | Directions.WEST.value
        ):
            start = (
                Directions.NORTH.value if (
                    Directions.NORTH.value & tile_connection_bitmask
                ) else Directions.EAST.value
            )

            end = start >> 2

            if connections[start] != connections[end]:
                self.__tilemap.connect(connections[start], connections[end])
        
        else:
            for direction in connections:
                if connections[direction] != self:
                    self.__tilemap.connect(self, connections[direction]) 


class Edge:
    def __init__(self, origin: Tile, termination: Tile):
        self.origin = origin
        self.termination = termination

    def __str__(self) -> str:
        return f"{self.origin} -> {self.termination}"
    
    @cached_property
    def connection_bitmask(self) -> int:
        return (
            self.origin.tile_connection_bitmask & 
            self.termination.tile_connection_bitmask
        )
    
    @cached_property
    def is_vertical(self) -> bool:
        return self.origin.position.x == self.termination.position.x
    
    @cached_property
    def is_horizontal(self) -> bool:
        return self.origin.position.y == self.termination.position.y
    
    @cached_property
    def length(self) -> int:
        diff = self.origin - self.termination
        return abs(diff.x) if self.is_horizontal else abs(diff.y)
    
    def runs_adjacent_to(self, tile: Tile) -> bool:
        if self.is_horizontal:
            smallest, largest = sorted(
                [self.origin.position.x, self.termination.position.x]
            )

            return (
                abs(self.origin.position.y - tile.position.y) == 1 and
                smallest <= tile.position.x <= largest
            )
        
        if self.is_vertical:
            smallest, largest = sorted(
                [self.origin.position.y, self.termination.position.y]
            )

            return (
                abs(self.origin.position.x - tile.position.x) == 1 and
                smallest <= tile.position.y <= largest
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

    def __connect(self, origin: Tile, termination: Tile) -> None:
        if origin not in self.edges:
            self.edges[origin] = [termination]
        
        else:
            self.edges[origin].append(termination)

        return Edge(origin, termination)
    
    def __disconnect(self, origin: Tile, termination: Tile) -> None:
        if origin in self.edges:
            self.edges[origin].remove(termination)

            if not self.edges[origin]:
                del self.edges[origin]
    
    def connect(self, origin: Tile, termination: Tile) -> Edge:
        self.__connect(origin, termination)
        self.__connect(termination, origin)
        return Edge(origin, termination)
    
    def disconnect(self, origin:Tile, termination: Tile) -> None:
        self.__disconnect(origin, termination)
        self.__disconnect(termination, origin)

    def __getitem__(self, position: Point) -> Tile:
        """Get the tile from a point (x,y) position on the tilemap."""
        return self.tiles[
            (position.y * self.size) + position.x
        ]
    
    def in_bounds(self, position: Point) -> bool:
        return (
            (0 <= position.x < self.size) and
            (0 <= position.y < self.size)
        )
    
    def clear_connections(self) -> None:
        """Clear connections for every tile, and reset the graph."""
        for tile in self.tiles:
            tile.tile_connection_bitmask = 0
        self.edges = {}