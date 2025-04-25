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

    @cached_property
    def partition_index(self) -> int:
        x_div = self.position.x // self.__tilemap.zone_size
        y_div = self.position.y // self.__tilemap.zone_size
        return int((self.__tilemap.zone_size * y_div) + x_div)

    def is_adjacent_to(self, tile: Tile) -> bool:
        diff = self.position - tile.position
        abs_diff = Point(abs(diff.x), abs(diff.y))
        result = abs_diff == Point(1, 0) or abs_diff == Point(0, 1)
        return result
    
    def connects_with(self, tile: Tile) -> bool:
        # TODO: performs a redundant adjacency check if we're in
        # an algorithm that moves one square at a time; it could be assumed
        # in that context that we're adjacent and could then skip the check
        if self.is_adjacent_to(tile):
            direction = reverse_directions_dict[tile.position - self.position]
            opposite_direction = (direction >> 2 | direction << 2) & 15
            return (
                bool(self.tile_connection_bitmask & direction) and
                bool(tile.tile_connection_bitmask & opposite_direction)
            )
    
        return False

    def extends(self, edge: Edge) -> bool:
        output = (
            self not in [edge.origin, edge.termination] and 
            (
                self.connects_with(edge.origin) or
                self.connects_with(edge.termination)
            )
        )

        return output
    
    def __scan(self, direction: int) -> Tile:
        if not self.tile_connection_bitmask & direction:
            return self
        
        valid = True
        current_tile = self

        while valid:
            next_tile = self.__tilemap[
                current_tile.position + directions_dict[direction]
            ]

            # Here's the redundant check mentioned in earlier comment!
            # This looks janky but I'm not sure why
            if current_tile.connects_with(next_tile):
                print(f"\t{current_tile.position} connects with {next_tile.position}")
                current_tile = next_tile
                # if count_set_bits(current_tile.tile_connection_bitmask) > 2:
                #     valid = False
            else:
                print(f"\t{current_tile.position} does not connect with {next_tile.position}")
                valid = False

        return current_tile
    
    def connect(self, tile_connection_bitmask: int) -> None:
        print(f"Connecting tile {self.position}")
        # Doesn't consider a case where we're 'disconnecting'
        self.tile_connection_bitmask = tile_connection_bitmask

        # Remove edges that we'd be extending (replacing) anyway
        if (self.partition_index in self.__tilemap.edges):
            for edge in self.__tilemap.edges[self.partition_index]:
                print(f"\tChecking existing edge {edge}...", end="")
                if self.extends(edge):
                    print(f"extends! removing")
                    self.__tilemap.remove_edge(edge)
                else:
                    print(f"does not extend - leaving")

        connections = {
            direction: self.__scan(direction)
            for direction in [8, 4, 2, 1]
            if direction & self.tile_connection_bitmask
        }

        if tile_connection_bitmask in (
            Directions.NORTH.value | Directions.SOUTH.value,
            Directions.EAST.value | Directions.WEST.value
        ):
            start = (
                Directions.NORTH.value 
                if tile_connection_bitmask & Directions.NORTH.value 
                else Directions.EAST.value
            )

            end = start >> 2

            if connections[start] != connections[end]:
                print(f"\tSpotted a connection between {connections[start]} and {connections[end]} - creating new edge")
                self.__tilemap.create_edge(connections[start], connections[end])

        for direction in connections.keys():
            if connections[direction] != self:
                self.__tilemap.create_edge(self, connections[direction])     
       

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
    
    def get_spatial_partitions(self, divisor: int) -> list:
        factor = 1 if self.is_horizontal else divisor

        if self.origin.partition_index > self.termination.partition_index:
            factor *= -1

        return list(
            range(
                self.origin.partition_index,
                self.termination.partition_index + 1,
                factor
            )
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
        for tile in self.tiles:
            tile.tile_connection_bitmask = 0

        self.edges = {}

    def create_edge(self, lhs: Tile, rhs:Tile) -> Edge:
        edge = Edge(lhs, rhs)
        for partition in edge.get_spatial_partitions(
            int(self.size / self.zone_size)
        ):
            if partition not in self.edges:
                self.edges[partition] = [edge]

            else:
                self.edges[partition].append(edge)

        return edge
    
    def remove_edge(self, edge: Edge) -> None:
        print(f"\t\tRemoving {edge} from edges")
        for partition in edge.get_spatial_partitions(
            int(self.size / self.zone_size)
        ):
            partitions = self.edges[partition].remove(edge)

            if not partitions:
                self.edges.pop(partition)

            if partition in self.edges:
                self.edges[partition] = self.edges[partition].remove(edge)