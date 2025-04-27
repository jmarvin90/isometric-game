from __future__ import annotations
from functools import cached_property
from graph.geometry import Point, Directions, directions_dict, reverse_directions_dict
from graph.utils import count_set_bits, count_trailing_zeros


class Direction:
    def __init__(self, direction: int):
        self.direction = direction

    def __int__(self) -> int:
        return self.direction

    def __neg__(self) -> Direction:
        return Direction((self.direction >> 2 | self.direction << 2) & 15)
    
    def __and__(self, comparator: int) -> int:
        return self.direction & comparator
    
    def __or__(self, comparator: int) -> int:
        return self.direction | comparator

    def as_vector(self) -> Point:
        return directions_dict[self.direction]
    
    def __str__(self) -> str:
        return Directions(self.direction).name
    
    def __hash__(self) -> int:
        return hash(self.direction)
    
    def __eq__(self, comparator: Direction) -> bool:
        return self.direction == comparator.direction
    
    @cached_property
    def direction_index(self) -> int:
        return self.direction.bit_length() -1


class Tile:
    """Represents a tile on the TileMap."""
    def __init__(self, position: Point, tilemap: TileMap):
        self.position = position                # the position on the grid
        self.__tilemap = tilemap
        self.tile_connection_bitmask = 0        # the connections bitmask - NESW
        
    def __str__(self) -> str:
        return str(self.position)
    
    def __scan(self, direction: Direction) -> Tile:
        """Get the furthest connected tile in a specified direction."""
        if not direction & self.tile_connection_bitmask:
            return self

        current_tile = self
        valid = True

        while valid:
            next_point = current_tile.position + direction.as_vector()
            next_tile = self.__tilemap[next_point]
            
            if (
                not self.__tilemap.in_bounds(next_point) or
                not -direction & next_tile.tile_connection_bitmask
            ):
                return current_tile
        
            current_tile = next_tile

            if count_set_bits(current_tile.tile_connection_bitmask) > 2:
                valid = False

        return current_tile
    
    def connect(self, tile_connection_bitmask: int) -> None:
        """Connect the tile in the directions specified by bitmask."""
        disconnected_directions = (
            ~tile_connection_bitmask & self.tile_connection_bitmask
        )
    
        self.tile_connection_bitmask = tile_connection_bitmask

        for direction_num in [8, 4, 2, 1]:
            if direction_num & disconnected_directions:
                direction = Direction(direction_num)

                disconnected_node = self.__tilemap[
                    self.position + direction.as_vector()
                ].__scan(direction)

                new_target = disconnected_node.__scan(-direction)
                self.__tilemap.disconnect(disconnected_node, -direction)
                if new_target != disconnected_node:
                    self.__tilemap.connect(disconnected_node, new_target)

        connections = {}

        for direction_num in [8, 4, 2, 1]:
            if direction_num & tile_connection_bitmask:
                direction = Direction(direction_num)
                connections[direction] = self.__scan(direction)

        if self.tile_connection_bitmask in (
            Directions.NORTH.value | Directions.SOUTH.value,
            Directions.EAST.value | Directions.WEST.value
        ):
            start = Direction(
                Directions.NORTH.value 
                if Directions.NORTH.value & self.tile_connection_bitmask
                else Directions.EAST.value
            )
            
            end = -start

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
    
    # @cached_property
    # def connection_bitmask(self) -> int:
    #     return (
    #         self.origin.tile_connection_bitmask & 
    #         self.termination.tile_connection_bitmask
    #     )
    
    @cached_property
    def is_vertical(self) -> bool:
        return self.origin.position.x == self.termination.position.x
    
    @cached_property
    def is_horizontal(self) -> bool:
        return self.origin.position.y == self.termination.position.y
    
    # @cached_property
    # def length(self) -> int:
    #     diff = self.origin - self.termination
    #     return abs(diff.x) if self.is_horizontal else abs(diff.y)
    
    @cached_property
    def direction(self) -> Direction:
        return Direction(
            reverse_directions_dict[
                (
                    self.termination.position - self.origin.position
                ).vector_direction
            ]
        )
    
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

    def disconnect(self, node: Tile, direction: Direction) -> None:
        # Exit early if the node is not already connected
        if (
            node not in self.edges or 
            self.edges[node][direction.direction_index] is None
        ):
            return

        # Set the node's connection at that index to None
        self.edges[node][direction.direction_index] = None

    def __connect(self, origin: Tile, termination: Tile) -> None:
        edge = Edge(origin, termination)

        # 'Default initialise' the tile's connection entry if we've not seen it
        if (origin not in self.edges):
            self.edges[origin] = [None, None, None, None]

        for tile, connections in self.edges.items():
            if connections[edge.direction.direction_index] == termination:
                self.disconnect(tile, edge.direction)

        # Create the connection
        self.edges[origin][edge.direction.direction_index] = termination

    def connect(self, origin: Tile, termination: Tile) -> None:
        # if origin == termination:
        #     return
        
        self.__connect(origin, termination)
        self.__connect(termination, origin)
