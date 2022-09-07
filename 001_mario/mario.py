from cs50 import get_int


def print_hashes(rows):
    """ Print double half-pyramid of specified height. """
    row_count = 1
    space_count = rows
    for i in range(rows):
        space_count -= 1
        print(" " * space_count, end="")
        print("#" * row_count, end="")
        print("  ", end="")
        print("#" * row_count)
        row_count += 1


# Print half-pyramid.
if __name__ == "__main__":
    rows = get_int("Height: ")

    # Prompt user to provide number of rows between 1 and 8 inclusive.
    while 0 >= rows or rows > 8:
        rows = get_int("Height: ")

    print_hashes(rows)
