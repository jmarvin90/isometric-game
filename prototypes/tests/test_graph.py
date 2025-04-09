import pytest

from graph.tilemap import TileMap
from graph.geometry import Point, Directions

@pytest.fixture
def tilemap() -> TileMap:
    return TileMap(size=12)


def test_out_in_space(tilemap: TileMap) -> None:
    tilemap.clear_connections()

    top_left = tilemap[Point(4, 2)]
    top_right = tilemap[Point(7, 2)]
    bottom_left = tilemap[Point(4, 7)]
    bottom_right = tilemap[Point(7, 7)]

    out_in_space = tilemap[Point(1, 4)]

    top_left.connect(Directions.EAST.value | Directions.SOUTH.value)
    top_right.connect(Directions.WEST.value | Directions.SOUTH.value)
    bottom_left.connect(Directions.NORTH.value | Directions.EAST.value)
    bottom_right.connect(Directions.NORTH.value | Directions.WEST.value)

    for cell in range(5, 7):
        tilemap[Point(cell, 2)].connect(
            Directions.EAST.value | Directions.WEST.value
        )

        tilemap[Point(cell, 7)].connect(
            Directions.EAST.value | Directions.WEST.value
        )

    for cell in range(3, 7):
        tilemap[Point(4, cell)].connect(
            Directions.NORTH.value | Directions.SOUTH.value
        )

        tilemap[Point(7, cell)].connect(
            Directions.NORTH.value | Directions.SOUTH.value
        )

    out_in_space.connect(
        Directions.EAST.value | Directions.WEST.value
    )

    control = {top_left, top_right, bottom_left, bottom_right}
    assert (
        top_left.connected_tiles == {top_right, bottom_left} and
        top_right.connected_tiles == {top_left, bottom_right} and
        bottom_right.connected_tiles == {top_right, bottom_left} and
        bottom_left.connected_tiles == {top_left, bottom_right}
        and tilemap.nodes == control
    )


def test_next_to_edge_diff_direction(tilemap: TileMap) -> None:
    tilemap.clear_connections()

    top_left = tilemap[Point(4, 2)]
    top_right = tilemap[Point(7, 2)]
    bottom_left = tilemap[Point(4, 7)]
    bottom_right = tilemap[Point(7, 7)]

    top_left.connect(6)
    top_right.connect(3)
    bottom_left.connect(12)
    bottom_right.connect(9)

    for cell in range(5, 8):
        tilemap[Point(cell, 2)].connect(5)
        tilemap[Point(cell, 7)].connect(5)

    for cell in range(3, 8):
        tilemap[Point(4, cell)].connect(10)
        tilemap[Point(7, cell)].connect(10)

    tilemap[Point(3, 4)].connect(5)

    control = {top_left, top_right, bottom_left, bottom_right}
    assert (
        top_left.connected_tiles == {top_right, bottom_left} and
        top_right.connected_tiles == {top_left, bottom_right} and
        bottom_right.connected_tiles == {top_right, bottom_left} and
        bottom_left.connected_tiles == {top_left, bottom_right}
        and tilemap.nodes == control
    )

def test_on_edge_same_direction(tilemap: TileMap) -> None:
    tilemap.clear_connections()

    top_left = tilemap[Point(4, 2)]
    top_right = tilemap[Point(7, 2)]
    bottom_left = tilemap[Point(4, 7)]
    bottom_right = tilemap[Point(7, 7)]

    top_left.connect(6)
    top_right.connect(3)
    bottom_left.connect(12)
    bottom_right.connect(9)

    for cell in range(5, 8):
        tilemap[Point(cell, 2)].connect(5)
        tilemap[Point(cell, 7)].connect(5)

    for cell in range(3, 8):
        if cell != 5:
            tilemap[Point(4, cell)].connect(10)
        tilemap[Point(7, cell)].connect(10)

    tilemap[Point(4, 5)].connect(10)

    control = {top_left, top_right, bottom_left, bottom_right}
    assert (
        top_left.connected_tiles == {top_right, bottom_left} and
        top_right.connected_tiles == {top_left, bottom_right} and
        bottom_right.connected_tiles == {top_right, bottom_left} and
        bottom_left.connected_tiles == {top_left, bottom_right}
        and tilemap.nodes == control
    )

def test_on_edge_additional_direction(tilemap: TileMap) -> None:
    tilemap.clear_connections()

    top_left = tilemap[Point(4, 2)]
    top_right = tilemap[Point(7, 2)]
    bottom_left = tilemap[Point(4, 7)]
    bottom_right = tilemap[Point(7, 7)]

    top_left.connect(6)
    top_right.connect(3)
    bottom_left.connect(12)
    bottom_right.connect(9)

    for cell in range(5, 8):
        tilemap[Point(cell, 2)].connect(5)
        tilemap[Point(cell, 7)].connect(5)

    for cell in range(3, 8):
        if cell != 5:
            tilemap[Point(4, cell)].connect(10)
        tilemap[Point(7, cell)].connect(10)

    complication_point = tilemap[Point(4, 5)]

    complication_point.connect(
        Directions.WEST.value | Directions.NORTH.value | Directions.SOUTH.value
    )

    control = {
        top_left, top_right, bottom_left, bottom_right, complication_point
    }

    assert (
        top_left.connected_tiles == {top_right, complication_point} and
        top_right.connected_tiles == {top_left, bottom_right} and
        bottom_right.connected_tiles == {top_right, bottom_left} and
        bottom_left.connected_tiles == {complication_point, bottom_right}
        and tilemap.nodes == control
    )

def test_on_edge_additional_direction(tilemap: TileMap) -> None:
    tilemap.clear_connections()

    top_left = tilemap[Point(4, 2)]
    top_right = tilemap[Point(7, 2)]
    bottom_left = tilemap[Point(4, 7)]
    bottom_right = tilemap[Point(7, 7)]

    top_left.connect(6)
    top_right.connect(3)
    bottom_left.connect(12)
    bottom_right.connect(9)

    for cell in range(5, 8):
        tilemap[Point(cell, 2)].connect(5)
        tilemap[Point(cell, 7)].connect(5)

    for cell in range(3, 8):
        tilemap[Point(4, cell)].connect(10)
        tilemap[Point(7, cell)].connect(10)

    complication_point = tilemap[Point(4, 5)]

    complication_point.connect(
        Directions.WEST.value | Directions.NORTH.value | Directions.SOUTH.value
    )

    control = {
        top_left, top_right, bottom_left, bottom_right, complication_point
    }

    for tile in control:
        for connected_tile in tile.connected_tiles:
            print(tile.position, connected_tile.position)

    assert (
        top_left.connected_tiles == {top_right, complication_point} and
        top_right.connected_tiles == {top_left, bottom_right} and
        bottom_right.connected_tiles == {top_right, bottom_left} and
        bottom_left.connected_tiles == {complication_point, bottom_right}
        and tilemap.nodes == control
    )
