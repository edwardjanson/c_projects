#include <cs50.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
    long number = get_long("Please input your card number here: ");
    long number_backup = number;
    int digit_counter = 0;
    int second_digit = 0;
    int remaining_digit = 0;
    int i;

    // determine the length of the number
    for (i = 0; number_backup > 0; i++)
    {
        if (i % 2 == 0)
        {
            // get and add up every second digit starting from the end of the number
            int single_digit = number_backup % 10;
            number_backup /= 10;
            remaining_digit += single_digit;
        }
        else
        {
            // get every second digit starting from the second number at the end of the number
            int single_digit = number_backup % 10 * 2;
            number_backup /= 10;

            // in case of double digits after multiplication, seperate the digits and add to sum
            if (single_digit > 9)
            {
                second_digit += single_digit % 10;
                single_digit /= 10;
                second_digit += single_digit;
            }
            else
            {
                // add the digit to the sum
                second_digit += single_digit;
            }
        }
        digit_counter++;
    }

    // calculate the checksum
    int checksum = second_digit + remaining_digit;

    // check the card number length and if the checksum has a remainder of 0
    if ((checksum % 10 == 0) && (digit_counter == 13 || digit_counter == 15 || digit_counter == 16))
    {
        // get the first two digits of the card number
        float first_digits = floor(number / pow(10, (digit_counter - 2)));

        // determine the card type and validity based on the number length and first two card digits
        switch (digit_counter)
        {
            case 13:
                if (first_digits >= 40 && first_digits <= 49)
                {
                    printf("VISA\n");
                    break;
                }
            case 15:
                if (first_digits == 34 || first_digits == 37)
                {
                    printf("AMEX\n");
                    break;
                }
            case 16:
                if (first_digits >= 40 && first_digits <= 49)
                {
                    printf("VISA\n");
                    break;
                }
                else if (first_digits >= 51 && first_digits <= 55)
                {
                    printf("MASTERCARD\n");
                    break;
                }
            default:
                printf("INVALID\n");
        }
    }
    else
    {
        printf("INVALID\n");
    }
}
