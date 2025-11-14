from components import PositionComponent, NavigationComponent, SegmentComponent
from registry import Registry, Entity
from tilemap import TileMap
from geometry import Direction, directions_dict, Segments
import utils

class PositionSystem:
    @classmethod
    def on_create(cls, registry: Registry, entity: Entity) -> None:
        position = registry.get_component(entity, PositionComponent)

class NavigationSystem:
    @classmethod
    def connect(cls, registry: Registry, entity: Entity) -> None:
        tilemap = registry.context_get(TileMap)
        segments = registry.context_get(Segments)

        position = registry.get_component(entity, PositionComponent)
        navigation = registry.get_component(entity, NavigationComponent)

        # All directions we can connect in
        directions = [
            Direction(1 << number) 
            for number in range(0, 8) 
            if (1 << number) & navigation.directions
        ]

        # Whether we have extended a segment: used to create an orphan
        has_joined = False

        for direction in directions:
            vector = directions_dict[direction]
            next_position = position.position + vector
            print(f"Scan from {position.position} in direction {direction} to {next_position}")
            next_tile_entity = tilemap[next_position]
            next_tile_navigation = registry.get_component(next_tile_entity, NavigationComponent)
            next_tile_segment = registry.get_component(next_tile_entity, SegmentComponent)
            
            if not next_tile_navigation:
                continue

            if next_tile_segment:
                segment = segments.get_segment(next_tile_segment.segment_id)
                seggy_is_orphan_can_connect = segment.is_orphan and next_tile_navigation.directions & utils.reverse(direction.value)
                seggy_not_orphan_can_connect = segment.directions & direction.value
                if (
                    seggy_is_orphan_can_connect or seggy_not_orphan_can_connect
                ):
                    print("Squeenchy!")

        if not directions or not has_joined:
            print("Squawk")
            new_segment = segments.add_segment(position.position, position.position)
            registry.add_component(entity, SegmentComponent, new_segment.id)