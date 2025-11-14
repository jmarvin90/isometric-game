from dataclasses import dataclass
from geometry import Vector

@dataclass
class SegmentComponent:
    segment_id: int

@dataclass
class NavigationComponent:
    directions: int

@dataclass
class PositionComponent:
    position: Vector

    def __init__(self, x: int, y: int):
        self.position = Vector(x, y)
    
    def __str__(self) -> str:
        return f"{self.position.x},{self.position.y}"