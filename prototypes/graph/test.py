from geometry import Segment, Segments, Vector, Direction
from tilemap import TileMap
from registry import Registry
from components import NavigationComponent, PositionComponent
from systems import NavigationSystem, PositionSystem

my_registry = Registry()

my_registry.add_callback(NavigationComponent, NavigationSystem.connect)
my_registry.add_callback(PositionComponent, PositionSystem.on_create)

my_tilemap = my_registry.context_add(TileMap, 8, my_registry)
my_segments = my_registry.context_add(Segments)

my_registry.add_component(my_tilemap[Vector(4, 4)], NavigationComponent, Direction.EAST.value | Direction.WEST.value)
my_registry.add_component(my_tilemap[Vector(5, 4)], NavigationComponent, Direction.EAST.value | Direction.WEST.value)