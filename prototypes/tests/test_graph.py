import pytest

from graph.tilemap import TileMap
from graph.geometry import Point

@pytest.fixture
def tilemap() -> TileMap:
    return TileMap(size=8)

@pytest.mark.parametrize(
    "from_position,from_directions,to_position,to_directions",
    [
        (Point(4, 4), 14, Point(5, 4), 11),     # LHS: N,E,S; RHS: N,W,S (E->W)
        (Point(4, 4), 10, Point(4, 5), 10),     # LHS: N,S; RHS: N,S (N->S)
        (Point(4, 4), 6, Point(5, 4), 9)        # LHS: E,S; RHS: W,N (E->W)     
    ]
)
def test_valid_connections(
    tilemap: TileMap, 
    from_position: Point,
    from_directions: int,
    to_position: Point,
    to_directions: int
):
    tilemap.clear_connections()
    
    tilemap.connect(from_position, from_directions)
    tilemap.connect(to_position, to_directions)

    assert tilemap.valid_connections == 1
