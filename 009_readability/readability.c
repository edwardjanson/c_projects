#include <cs50.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>


int main(void)
{
    // Get the text
    char *text = get_string("Text: ");

    // Get first character in string
    int i = 0;
    char c = text[i];

    // Set up counters
    float letter_count = 0.0;
    float space_count = 1.0;
    float punct_count = 0.0;

    while (c != 0)
    {
        // Check if character is a letter
        if (isalpha(c))
        {
            letter_count += 1;
        }
        // Check if character is a space
        else if (isspace(c))
        {
            space_count += 1;
        }
        // Check if character is a "!", "?", or "."
        else if (c == 33 || c == 63 || c == 46)
        {
            punct_count += 1;
        }

        i += 1;
        c = text[i];
    }

    // Calculate the grade
    double l = letter_count / space_count * 100;
    double s = punct_count / space_count * 100;
    int grade = round(0.0588 * l - 0.296 * s - 15.8);

    // Return grade
    if (grade > 16)
    {
        printf("Grade 16+\n", grade);
    }
    else if (grade < 1)
    {
        printf("Before Grade 1\n", grade);
    }
    else
    {
        printf("Grade %d\n", grade);
    }
}