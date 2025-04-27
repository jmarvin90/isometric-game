from typing import Callable, Any
from graph.geometry import Directions

# TODO: study this a bit to understand how it's working
def count_set_bits(number: int) -> int:
    count = 0

    while number:
        number &= number -1
        count += 1

    return count

def count_trailing_zeros(value: int) -> int:
    return (value & -value).bit_length() -1

# TODO: An interesting idea?
def apply_in_directions(
    directions: int, 
    function: Callable,
    *args: Any,
    **kwargs: Any
) -> None:
    direction = Directions.NORTH.value
    while direction:
        if directions & direction:
            function(*args, *kwargs)

        direction >>= 1

