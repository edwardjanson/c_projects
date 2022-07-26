#include <cs50.h>
#include <stdio.h>

void char_printer(int n, char c);

int main(void)
{
    int rows;

    do
    {
        // prompt user to provide number of rows between 1 and 8 inclusive
        rows = get_int("How many rows should the pyramid be? ");
    }
    while (rows < 1 || rows > 8);

    if (rows > 0)
    {
        // counter for spaces
        int space_counter = rows - 1;
        // counter for hashes
        int hash_counter = 1;

        // prints spaces and hashes on a set number of rows
        for (int i = 0; i < rows; i++)
        {
            char_printer(space_counter, ' ');

            char_printer(hash_counter, '#');

            printf("  ");

            char_printer(hash_counter, '#');

            printf("\n");

            space_counter -= 1;
            hash_counter += 1;
        }
    }
}

// prints characters based on a single character and number of repetitions
void char_printer(int n, char c)
{
    for (int i = 0; i < n; i++)
    {
        printf("%c", c);
    }
}
