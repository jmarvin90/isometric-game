import pytest

from graph.tilemap import TileMap, Edge
from graph.geometry import Point

@pytest.fixture
def tilemap() -> TileMap:
    return TileMap(size=12)

@pytest.fixture
def edge(tilemap: TileMap) -> Edge:
    return Edge(
        tilemap[Point(2, 2)], tilemap[Point(10, 2)]
    )

def test_connect(tilemap: TileMap) -> None:
    for number in range(2, 11):
        tilemap[Point(number, 2)].set_connection_bitmask(5)

    tilemap[Point(4, 2)].set_connection_bitmask(7)
    tilemap[Point(8, 2)].set_connection_bitmask(7)

    for number in range(3, 7):
        tilemap[Point(4, number)].set_connection_bitmask(10)

    tilemap[Point(4, 7)].set_connection_bitmask(12)

    for number in range(5, 8):
        tilemap[Point(number, 7)].set_connection_bitmask(5)

    tilemap[Point(8, 7)].set_connection_bitmask(9)

    for number in range(3, 7):
        tilemap[Point(8, number)].set_connection_bitmask(10)

    tilemap[Point(6, 2)].set_connection_bitmask(0)

    for tile, edges in tilemap.edges.items():
        for edge in edges:
            if edge is not None:
                print(tile, edge)

    assert False