from __future__ import annotations

class Point:
    def __init__(self, x: int, y: int):
        self.x = x
        self.y = y

    def __str__(self) -> str:
        return f"{self.x},{self.y}"

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
    
    def __eq__(self, comparator: Point) -> bool:
        return (
            self.x == comparator.x and 
            self.y == comparator.y
        )
    
    def __abs__(self) -> Point:
        return Point(
            abs(self.x), abs(self.y)
        )
    
    def __sub__(self, comparator: Point) -> Point:
        return Point(
            self.x - comparator.x, self.y - comparator.y
        )  
    
    def is_adjacent_to(self, comparator: Point):
        diff = abs(self - comparator)
        return diff.x <= 1 and diff.y <= 1 and diff != Point(0, 0)
