from __future__ import annotations
from registry import Registry, Entity
from geometry import Vector
from components import PositionComponent

class TileMap:
    def __init__(self, size: int, registry: Registry):
        self.size = size
        self.tiles = []
        
        for number in range(0, self.size**2):
            entity = registry.create()
            self.tiles.append(entity)
            
            registry.add_component(
                entity, PositionComponent, number % self.size, number // self.size
            )


    def __getitem__(self, point: Vector) -> Entity:
        return self.tiles[(point.y * self.size + point.x)]