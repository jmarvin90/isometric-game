from dataclasses import dataclass, field
from geometry import Vector

@dataclass
class ConnectionsComponent:
    connections: list = field(default_factory = lambda: [None] * 8)

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