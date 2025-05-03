from __future__ import annotations
from dataclasses import dataclass, field
import math
import queue
from typing import Any, Iterable

from graph.geometry import Point, Directions, directions_dict
from graph.utils import count_set_bits

@dataclass(order=True)
class PrioritizedItem:
    priority: int
    item: Any=field(compare=False)

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

def distance_between(point_a: Point, point_b: Point) -> int:
    x_diff = point_a.x - point_b.x
    y_diff = point_a.y - point_b.y
    return int(math.sqrt(x_diff**2 + y_diff**2))

def point_is_on_line(
    point: Point, line_origin: Point, line_termination: Point
) -> bool:
    same_x = point.x == line_origin.x == line_termination.x
    same_y = point.y == line_origin.y == line_termination.y
    
    if same_x:
        smaller, larger = sorted([line_origin.x, line_termination.x])
        return smaller <= point.x <= larger
    
    elif same_y:
        smaller, larger = sorted([line_origin.y, line_termination.y])
        return smaller <= point.y <= larger
    
    else:
        return False


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
                self.__tilemap.connect(new_target, node, direction)
                self.__tilemap.connect(node, new_target, reverse(direction))

        # Deal with connections in a straight line
        if self.tile_connection_bitmask in (
            Directions.NORTH.value | Directions.SOUTH.value,
            Directions.EAST.value | Directions.WEST.value
        ):
            direction = (
                Directions.NORTH.value 
                if Directions.NORTH.value & self.tile_connection_bitmask
                else Directions.EAST.value
            )
            
            opposite_direction = reverse(direction)

            if connections[direction] != connections[opposite_direction]:
                self.__tilemap.connect(
                    connections[direction],
                    connections[opposite_direction],
                    opposite_direction
                )

                self.__tilemap.connect(
                    connections[opposite_direction],
                    connections[direction],
                    direction
                )
        
        # Deal with connections which change direction
        else:
            for direction in connections:
                if connections[direction] != self:
                    self.__tilemap.connect(
                        self, connections[direction], direction
                    )

                    self.__tilemap.connect(
                        connections[direction], self, reverse(direction)
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
    
    def __graph_iterator(self) -> Iterable:
        for node, connections in self.edges.items():
            for direction in [8, 4, 2, 1]:
                connected_node = connections[direction_index(direction)]
                if connected_node:
                    yield (node, connected_node)
    
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

    def connect(self, origin: Tile, termination: Tile, direction: int) -> None:
        # 'Default initialise' the tile's connection entry if we've not seen it
        if (origin not in self.edges):
            self.edges[origin] = [None, None, None, None]

        # Check whether anything connects to termination from the same direction
        for tile, connections in self.edges.items():
            if connections[direction_index(direction)] == termination:
                # and disconnect if so
                self.disconnect(tile, direction)

        # Create the connection
        self.edges[origin][direction_index(direction)] = termination

    def points_are_navigable(self, point_a: Point, point_b: Point) -> bool:
        point_a_is_navigable = False
        point_b_is_navigable = False

        for node, connected_node in self.__graph_iterator():

            if not point_a_is_navigable:
                point_a_is_navigable |= (
                    node.position == point_a or
                    connected_node.position == point_a or
                    point_is_on_line(
                        point_a, node.position, connected_node.position
                    )
                )
                
            if not point_b_is_navigable:
                point_b_is_navigable |= (
                    node.position == point_b or
                    connected_node.position == point_b or
                    point_is_on_line(
                        point_b, node.position, connected_node.position
                    )
                )

            if point_a_is_navigable & point_b_is_navigable:
                return True

        return False

    def get_path_between(self, point_a: Point, point_b: Point) -> list | None:
        if not self.points_are_navigable(point_a, point_b):
            return
        
        frontier = queue.PriorityQueue()
        frontier.put(PrioritizedItem(0, point_a))

        came_from = {point_a: None}
        cost_so_far = {point_a: 0}

        while not frontier.empty():
            current = frontier.get().item

            if current == point_b:
                break

            for connection in [
                node for node in self.edges[self[current]]
                if node is not None
            ]:
                new_cost = (
                    cost_so_far[current] + 
                    distance_between(current, connection.position)
                )

                if (
                    connection.position not in cost_so_far or
                    new_cost < cost_so_far[connection.position]
                ):
                    cost_so_far[connection.position] = new_cost
                    priority = new_cost
                    frontier.put(PrioritizedItem(priority, connection.position))
                    came_from[connection.position] = current

        path = []
        current = point_b
        while current != point_a:
            path.append(current)
            current = came_from[current]
        path.append(point_a)
        path.reverse()
        return path