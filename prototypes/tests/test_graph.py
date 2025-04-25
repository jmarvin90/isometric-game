import pytest

from graph.tilemap import TileMap, Edge
from graph.geometry import Point

@pytest.fixture
def tilemap() -> TileMap:
    return TileMap(size=12)

def test_connect(tilemap: TileMap) -> None:
    for number in range(2, 8):
        point = Point(4, number)
        tilemap[point].connect(10)

    for number in range(4, 8):
        point = Point(number, 2)
        tilemap[point].connect(5)

    for tile, connections in tilemap.edges.items():
        for connection in connections:
            print(tile, connection)
    assert False