import pytest

from graph.tilemap import TileMap, Edge
from graph.geometry import Point

@pytest.fixture
def tilemap() -> TileMap:
    return TileMap(size=12)

def test_vertical_edge(tilemap: TileMap) -> None:
    for number in range(0, 4):
        tilemap[Point(4, number)].connect(10)

    for partition, edges in tilemap.edges.items():
        for edge in edges:
            print(partition, edge)

    assert False