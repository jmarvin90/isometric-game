from __future__ import annotations
import typing
from graph.geometry import Point
from graph.constants import Directions, directions_dict
from graph.utils import count_set_bits

class Tile:
    """Represents a tile on the TileMap."""
    def __init__(self, position: Point):
        self.position = position        # the position on the grid
        self.connections = 0            # the connections bitmask - NESW

    def connect(self, directions: int) -> None:
        """Update the tile's connections to connect in specified directions."""
        self.connections = self.connections | directions

    def clear_connections(self) -> None:
        """Clear the tile's connections."""
        self.connections = 0


class TileMap:
    """Represents a (square) tilemap as a list of tiles."""
    def __init__(self, size: int):
        self.size = size
        self.graph = Graph(self)
        self.tiles = [
            Tile(Point(number % self.size, number // self.size)) 
            for number in range(0, self.size**2)
        ]

    def __getitem__(self, position: Point) -> Tile:
        """Get the tile from a point (x,y) position on the tilemap."""
        return self.tiles[
            (position.y * self.size) + position.x
        ]
    
    def clear_connections(self) -> None:
        """Clear connections for every tile, and reset the graph."""
        for tile in self.tiles:
            tile.clear_connections
        self.graph.reset()      
    
    # TODO: think of a better name
    def connect(self, position: Point, directions: int) -> None:
        """Connect a tile at a given point/in specified directions."""
        # Helpful to do this at the TileMap level to enable the graph logic
        tile = self[position]
        tile.connect(directions)
        self.graph.add_node(tile)


class Edge:
    """Represents an edge on the graph, between two Tiles."""
    def __init__(self, origin: Tile, termination: Tile, tilemap: TileMap):
        self.origin = origin
        self.termination = termination
        self.tilemap = tilemap

    def __str__(self) -> str:
        """Get a string representation of the edge as (start)->(end)."""
        return f"{self.origin.position} -> {self.termination.position}"
    
    @property
    def is_horizontal(self) -> bool:
        """Whether the edge is horizontal."""
        return self.origin.position.y == self.termination.position.y
    
    @property
    def is_vertical(self) -> bool:
        """Whether the edge is vertical."""
        return self.origin.position.x == self.termination.position.x

    @property
    def direction_bitmask(self) -> int:
        if self.is_horizontal:
            return 5

        if self.is_vertical:
            return 10
    
    def covers(self, point: Point) -> bool:
        """Whether the edge 'covers' a given point."""
        if self.is_horizontal:
            smallest, largest = sorted(
                (self.origin.position.x, self.termination.position.x)
            )
            return smallest <= point.x <= largest

        if self.is_vertical:
            smallest, largest = sorted(
                (self.origin.position.y, self.termination.position.y)
            )
            return smallest <= point.y <= largest
        
        return False

    def __crossing_point(self, comparator: Edge) -> Point:
        """Get the theoretical crossing point with the comparator."""
        vert = self if self.is_vertical else comparator
        horiz = self if self.is_horizontal else comparator
        
        # Get the theoretical crossing point
        return Point(
            vert.origin.position.x,
            horiz.origin.position.y
        )
    
    def __intersects(self, comparator: Edge) -> bool:
        """Check if the edge crossed the comparator."""
        # Assume not, if the two lines are colinear 
        if (
            self.is_vertical == comparator.is_vertical and
            self.is_horizontal == comparator.is_horizontal
        ):
            return False

        crossing_point = self.__crossing_point(comparator)

        # Lines cross if they both cover the theoretical crossing point
        return (
            self.covers(crossing_point) and 
            comparator.covers(crossing_point)
        )


class Graph:
    """Represent connections between Tiles on the TileMap."""
    def __init__(self, tilemap: TileMap):
        self.reset()

        self.__tilemap = tilemap

    def reset(self) -> None:
        """Reset the graph by removing all nodes and edges."""
        self.connected_nodes = set()
        self.orphan_nodes = set()
        self.edges = set()

    def add_edge(self, new_edge: Edge) -> None:
        self.edges.add(new_edge)

    def add_node(self, tile: Tile) -> None:
        """Logic to add a node to the graph - adding edges where needed."""
        # All the directions the new node connects in
        directions = (
            direction for direction in (1, 2, 4, 8)
            if direction & tile.connections
        )

        # The new edges we're going to create
        edges = set()

        # Go in every direction
        for direction in directions:

            valid = True

            opposing_direction = (
                (direction >> 2 | direction << 2) & 15
            )

            vector = directions_dict[direction]
            current_tile = tile

            while valid:
                # Get the next tile in the same direction
                next_tile = self.__tilemap[current_tile.position + vector]

                # If that tile reciprocates the connection, keep going
                if next_tile.connections & opposing_direction:
                    current_tile = next_tile
                # Otherwise, stop here
                else:
                    valid = False

            # If we started and ended on a different tile
            if current_tile != tile:
                # Then create a new edge from start -> finish
                edges.add(Edge(tile, current_tile, self.__tilemap))

        self.edges = self.edges | edges

    def remove_node(self, tile: Tile) -> None:
        """Remove a node from the graph, including any related edges."""
        # exit out if the tile isn't in the graph
        if not tile in self.nodes:
            return
        
        # remove the tile as a node
        self.nodes.remove(tile)
        
        # get any edges that the tile was part of
        affected_edges = {
            edge for edge in self.edges 
            if edge.origin == tile
            or edge.termination == tile
        }

        # remove any edges it was part of, and remove it from the list of nodes
        self.edges = self.edges - affected_edges