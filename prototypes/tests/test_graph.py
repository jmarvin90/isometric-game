import pytest

from graph.tilemap import TileMap, Edge
from graph.geometry import Point

@pytest.fixture
def tilemap() -> TileMap:
    return TileMap(size=12)
