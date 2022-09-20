from cs50 import get_string

# Get the text.
text = get_string("Text: ")

# Set up the counters.
letter_count = 0
space_count = 1
punct_count = 0

for i in range(len(text)):
    char = text[i]

    # Check if the character is a letter.
    if char.isalpha():
        letter_count += 1
    # Check if the character is a space.
    elif char.isspace():
        space_count += 1
    # Check if the character is a "!", "?", or "."
    elif char == "!" or char == "?" or char == ".":
        punct_count += 1

# Calculate the grade.
l = letter_count / space_count * 100
s = punct_count / space_count * 100
grade = round(0.0588 * l - 0.296 * s - 15.8)

# Print the grade.
if grade > 16:
    print("Grade 16+")
elif grade < 1:
    print("Before Grade 1")
else:
    print(f"Grade {grade}")