# TODO: study this a bit to understand how it's working
def count_set_bits(number: int) -> int:
    count = 0

    while number:
        number &= number -1
        count += 1

    return count