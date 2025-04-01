from __future__ import annotations
import typing
from graph.geometry import Point
from graph.constants import Directions, directions_dict
from graph.utils import count_set_bits

class Tile:
    def __init__(self, position: Point):
        self.position = position
        self.connections = 0

    def connect(self, directions: int) -> None:
        self.connections = self.connections | directions

    def clear_connections(self) -> None:
        self.connections = 0


class TileMap:
    def __init__(self, size: int):
        self.size = size
        self.graph = Graph()
        self.tiles = [
            Tile(Point(number % self.size, number // self.size)) 
            for number in range(0, self.size**2)
        ]

    def __getitem__(self, position: Point) -> Tile:
        return self.tiles[
            (position.y * self.size) + position.x
        ]
    
    def clear_connections(self) -> None:
        for tile in self.tiles:
            tile.clear_connections
        self.graph.reset()
    
    # TODO: think of a better name
    def connect(self, position: Point, directions: int) -> None:
        tile = self[position]
        tile.connect(directions)
        self.graph.add_node(tile)


class Edge:
    def __init__(self, origin: Tile, termination: Tile):
        self.origin = origin
        self.termination = termination

    def __str__(self) -> str:
        return f"{self.origin.position} -> {self.termination.position}"
    
    @property
    def is_horizontal(self) -> bool:
        return self.origin.position.x == self.termination.position.y
    
    @property
    def is_vertical(self) -> bool:
        return self.origin.position.y == self.termination.position.y
    
    def covers(self, point: Point) -> bool:
        if self.is_horizontal:
            smallest, largest = sorted(
                (self.origin.position.x, self.termination.position.x)
            )
            return smallest <= point.x <= largest

        if self.is_vertical:
            smallest, largest = sorted(
                self.origin.position.y, self.termination.position.y
            )
            return smallest <= point.y <= largest
        
        return False
    
    def __edge_and_tile(self, comparator: Tile) -> tuple[Edge]:
        if not self.covers(comparator.position):
            print("Does not cover")
            return ()
        
        directions_bitmask = 5 if self.is_horizontal else 10
        comparator_n_directions = count_set_bits(comparator.connections)

        # If the comparator only connects in the same direction
        if (
            comparator_n_directions == 2 and 
            bool(directions_bitmask & comparator.connections)
        ):
            return (self)

        return (
            Edge(self.origin, comparator),
            Edge(comparator, self.termination)
        )

    def __and__(self, comparator: typing.Any) -> tuple[Edge]:
        if type(comparator) == Tile:
            return self.__edge_and_tile(comparator)


class Graph:
    def __init__(self):
        self.nodes = set()
        self.edges = set()

    def reset(self) -> None:
        self.nodes = set()
        self.edges = set()

    def add_node(self, tile: Tile) -> None:
        # Skip if it's already been added
        if tile in self.nodes:
            return
        
        self.nodes.add(tile)

        direction = 8
        
        # Iterate through relevant directions to calculate the tilemap positions
        while direction:
            if direction & tile.connections:
                vector = directions_dict[direction]
                corresponding_position = tile.position + vector
                
                # Create a new edge if an adjacent node is already in the graph
                # and reciprocates the connection 
                for node in self.nodes:
                    if node.position == corresponding_position:
                        opposing_direction = (
                            (direction >> 2 | direction << 2) & 15
                        )
                        if node.connections & opposing_direction:
                            self.edges.add(Edge(tile, node))

            direction = direction >> 1

        """
        1. The node is not adjacent to another node, and
            a. is "on" an edge; and
                i. connects along the edge only;
                ii. connects along the edge and at least one more direction;
                iii. does not connect along the edge
            b. is not "on" an edge
        2. The node is adjacent to another node, which: 
            a. is not part of an edge, and
                i. reciprocates the connection; or
                ii. does not reciprocate the connection
            b. is part of an edge, which:       
        """

    def remove_node(self, tile: Tile) -> None:
        # exit out if the tile isn't in the graph
        if not tile in self.nodes:
            return
        
        # remove the tile as a node
        self.nodes.remove(tile)
        
        # if not, it was probably the start or end of an edge
        affected_edges = {
            edge for edge in self.edges 
            if edge.origin == tile
            or edge.termination == tile
        }

        # remove any edges it was part of, and remove it from the list of nodes
        self.edges = self.edges - affected_edges