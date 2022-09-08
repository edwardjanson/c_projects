from cs50 import get_int

# Get the user's card number.
number = get_int("Please input your card number here: ")

digits_total = 0
digit_count = len(str(number))

# Add up every second digit starting from the end of the number.
for i in range(digit_count, 0, -2):
    digits_total += int(str(number)[i - 1])

# Double every second digit starting from the second number at the end of the number.
for i in range(digit_count - 1, 0, -2):
    single_digit = int(str(number)[i - 1]) * 2

    # In case of double digits after multiplication, seperate the digits and add to sum.
    if single_digit > 9:
        digits_total += int(str(single_digit)[0]) + int(str(single_digit)[1])
    else:
        digits_total += single_digit

# Check if the checksum has a remainder of 0.
if (digits_total % 10 == 0):
    # Get the first two digits of the card number.
    first_digits = int(str(number)[:2])

    # Determine the card type or if invalid based on the first two card digits.
    if digit_count == 13 and first_digits >= 40 and first_digits <= 49:
        print("VISA")
    elif digit_count == 15 and (first_digits == 34 or first_digits == 37):
        print("AMEX")
    elif digit_count == 16 and first_digits >= 40 and first_digits <= 49:
        print("VISA")
    elif digit_count == 16 and first_digits >= 51 and first_digits <= 55:
        print("MASTERCARD")
    else:
        print("INVALID")
else:
    print("INVALID")
