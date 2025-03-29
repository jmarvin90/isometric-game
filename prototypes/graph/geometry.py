from __future__ import annotations

class Point:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def __add__(self, point: Point) -> Point:
        return Point(
            self.x + point.x,
            self.y + point.y
        )
    
    def __mul__(self, multiple: int) -> Point:
        return Point(
            self.x * multiple,
            self.y * multiple
        )
    
    