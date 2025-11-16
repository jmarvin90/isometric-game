from components import PositionComponent, NavigationComponent, ConnectionsComponent
from registry import Registry, Entity
from tilemap import TileMap
from geometry import Direction, directions_dict, Vector
import utils

class PositionSystem:
    @classmethod
    def on_create(cls, registry: Registry, entity: Entity) -> None:
        position = registry.get_component(entity, PositionComponent)


def scan(registry: Registry, from_pos: Vector, direction: Direction) -> Vector:
    tilemap = registry.context_get(TileMap)

    current_pos = from_pos
    current_entity = tilemap[current_pos]
    current_nav = registry.get_component(current_entity, NavigationComponent)

    while True:
        next_pos = current_pos + directions_dict[direction]
        next_entity = tilemap[next_pos]
        next_nav = registry.get_component(next_entity, NavigationComponent)

        if not next_nav: 
            return current_pos

        can_connect_forward = current_nav.directions & direction.value
        can_connect_back = next_nav.directions & utils.reverse(direction.value)

        if not can_connect_forward or not can_connect_back:
            return current_pos
        
        current_pos = next_pos
        current_entity = next_entity
        current_nav = next_nav

        if utils.count_set_bits(current_nav.directions) > 2:
            return current_pos


def _disconnect(registry: Registry, to_pos: Vector, direction: Direction) -> None:
    print("Disconnecting")
    
    for entity in registry.components.keys():

        if not ConnectionsComponent in registry.components[entity].keys():
            continue

        connections = registry.components[entity][ConnectionsComponent]
        conn = connections.connections[utils.count_trailing_zeros(direction.value)]

        if conn == to_pos:
            connections.connections[utils.count_trailing_zeros(direction.value)] = None

        if not any(connections.connections):
            registry.remove_component(entity, ConnectionsComponent)


def _connect(registry: Registry, from_pos: Vector, to_pos: Vector, direction: Direction) -> None:
    print(f"Connecting {from_pos} and {to_pos} ({direction})")
    _disconnect(registry, to_pos, direction)

    tilemap = registry.context_get(TileMap)
    from_ent = tilemap[from_pos]
    from_conn = registry.get_component(from_ent, ConnectionsComponent)

    if not from_conn:
        conns = [None] * 8
        conns[utils.count_trailing_zeros(direction.value)] = to_pos
        registry.add_component(from_ent, ConnectionsComponent, conns)
    else:
        conns = registry.get_component(from_ent, ConnectionsComponent)
        conns[utils.count_trailing_zeros] = to_pos


class NavigationSystem:
    @classmethod
    def connect(cls, registry: Registry, entity: Entity) -> None:
        nav = registry.get_component(entity, NavigationComponent)
        pos = registry.get_component(entity, PositionComponent)
        is_junction = utils.count_set_bits(nav.directions)

        directions = [None] * 8

        for dirval in range(0, 8):
            direction = 1 << dirval

            if not direction & nav.directions:
                continue

            print(f"Scanning from {pos.position} in direction {Direction(direction)}")
            directions[utils.count_trailing_zeros(direction)] =\
                scan(registry, pos.position, Direction(direction))

        for dirval in range(0, 8):

            if not directions[dirval] or directions[dirval] == pos.position:
                continue

            direction = Direction(1 << dirval)

            if is_junction:
                _connect(registry, pos.position, directions[dirval], direction)
            else:
                _connect(registry, directions[utils.reverse(dirval), directions[dirval], direction])

            

