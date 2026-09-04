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
def reverse(direction: int) -> int:
    return (direction >> 4 | direction << 4) & 255

