import pytest

from graph.tilemap import TileMap, Edge
from graph.geometry import Point

@pytest.fixture
def tilemap() -> TileMap:
    return TileMap(size=8)

@pytest.fixture
def example_edge(tilemap:TileMap) -> Edge:
    return Edge(tilemap[Point(4, 4)], tilemap[Point(7, 4)], tilemap)

def test_add_node(tilemap:TileMap) -> None:
    for number in range(1, 4):
        tilemap.connect(Point(number, 3), 5)

    for number in range(1, 3):
        tilemap.connect(Point(4, number), 10)
        tilemap.connect(Point(4, number+3), 10)

    for edge in tilemap.graph.edges:
        print(edge)

    assert False
