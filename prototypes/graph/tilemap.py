from __future__ import annotations
from functools import cached_property
from graph.geometry import Point, Directions, directions_dict, reverse_directions_dict
from graph.utils import count_set_bits, count_trailing_zeros
import queue

def reverse(direction: int) -> int:
    return (direction >> 2 | direction << 2) & 15

def as_vector(direction: int) -> Point:
    return directions_dict[direction]

def direction_index(direction: int) -> int:
        return direction.bit_length() -1

def point_in_bounds(tilemap: TileMap, position: Point) -> bool:
    return (
        (0 <= position.x < tilemap.size) and
        (0 <= position.y < tilemap.size)
    )


class Tile:
    """Represents a tile on the TileMap."""
    def __init__(self, position: Point, tilemap: TileMap):
        self.position = position                # the position on the grid
        self.__tilemap = tilemap
        self.tile_connection_bitmask = 0        # the connections bitmask - NESW
        
    def __str__(self) -> str:
        return str(self.position)
    
    def set_connection_bitmask(self, tile_connection_bitmask: int) -> None:
        """Connect the tile in the directions specified by bitmask."""
        tile_disconnection_bitmask = (
            ~tile_connection_bitmask & self.tile_connection_bitmask
        )
        
        connections = {}
        disconnections = {}

        # Look in all directions
        for direction in [8, 4, 2, 1]:

            # Find the nodes we need to disconnect
            if direction & tile_disconnection_bitmask:
                disconnections[direction] = self.__tilemap.scan(self, direction)

            # Update bit value for the given direction
            self.tile_connection_bitmask &= ~direction
            self.tile_connection_bitmask |= direction & tile_connection_bitmask

            # Find the nodes we need to connect
            if direction & self.tile_connection_bitmask:
                connections[direction] = self.__tilemap.scan(self, direction)

        # Deal with disconnections
        for direction, node in disconnections.items():
            new_target = self.__tilemap.scan(node, reverse(direction))
            if new_target != node:
                self.__tilemap.connect(node, new_target)
                self.__tilemap.connect(new_target, node)

        # Deal with connections in a straight line
        if self.tile_connection_bitmask in (
            Directions.NORTH.value | Directions.SOUTH.value,
            Directions.EAST.value | Directions.WEST.value
        ):
            start = (
                Directions.NORTH.value 
                if Directions.NORTH.value & self.tile_connection_bitmask
                else Directions.EAST.value
            )
            
            end = reverse(start)

            if connections[start] != connections[end]:
                self.__tilemap.connect(connections[start], connections[end])
                self.__tilemap.connect(connections[end], connections[start])
        
        # Deal with connections which change direction
        else:
            for direction in connections:
                if connections[direction] != self:
                    self.__tilemap.connect(self, connections[direction]) 
                    self.__tilemap.connect(connections[direction], self) 


class Edge:
    def __init__(self, origin: Tile, termination: Tile):
        self.origin = origin
        self.termination = termination

    def __str__(self) -> str:
        return f"{self.origin} -> {self.termination}"
    
    @cached_property
    def is_vertical(self) -> bool:
        return self.origin.position.x == self.termination.position.x
    
    @cached_property
    def is_horizontal(self) -> bool:
        return self.origin.position.y == self.termination.position.y
    
    @cached_property
    def direction(self) -> int:
        return reverse_directions_dict[
            (
                self.termination.position - self.origin.position
            ).vector_direction
        ]


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
        for tile in self.tiles:
            tile.tile_connection_bitmask = 0
        self.edges = {}

    def scan(self, tile: Tile, direction: int) -> Tile:
        """Get the furthest connected tile in a specified direction."""
        if not direction & tile.tile_connection_bitmask:
            return tile

        current_tile = tile
        valid = True

        while valid:
            next_point = current_tile.position + as_vector(direction)
            next_tile = self[next_point]
            
            if (
                not point_in_bounds(self, next_point) or
                not reverse(direction) & next_tile.tile_connection_bitmask
            ):
                return current_tile
        
            current_tile = next_tile

            if count_set_bits(current_tile.tile_connection_bitmask) > 2:
                valid = False

        return current_tile

    def disconnect(self, node: Tile, direction: int) -> None:
        # Exit early if the node is not already connected
        if (
            node not in self.edges or 
            self.edges[node][direction_index(direction)] is None
        ):
            return

        # Set the node's connection at that index to None
        self.edges[node][direction_index(direction)] = None

    def connect(self, origin: Tile, termination: Tile) -> None:
        edge = Edge(origin, termination)

        # 'Default initialise' the tile's connection entry if we've not seen it
        if (origin not in self.edges):
            self.edges[origin] = [None, None, None, None]

        # Check whether anything connects to termination from the same direction
        for tile, connections in self.edges.items():
            if connections[direction_index(edge.direction)] == termination:
                # and disconnect if so
                self.disconnect(tile, edge.direction)

        # Create the connection
        self.edges[origin][direction_index(edge.direction)] = termination

    def navigate_between(self, origin: Tile, termination: Tile) -> None:      
        
        for tile, connections in self.edges:
            pass