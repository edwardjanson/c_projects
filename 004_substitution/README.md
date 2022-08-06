Design and implement a program, substitution, that encrypts messages using a substitution cipher.

- The program must accept a single command-line argument, the key to use for the substitution. The key itself should be case-insensitive, so whether any character in the key is uppercase or lowercase should not affect the behavior of the program.
- If the program is executed without any command-line arguments or with more than one command-line argument, the program should print an error message of your choice (with printf) and return from main a value of 1 (which tends to signify an error) immediately.
- If the key is invalid (as by not containing 26 characters, containing any character that is not an alphabetic character, or not containing each letter exactly once), the program should print an error message of your choice (with printf) and return from main a value of 1 immediately.
- The program must output plaintext: (without a newline) and then prompt the user for a string of plaintext (using get_string).
- The program must output ciphertext: (without a newline) followed by the plaintext’s corresponding ciphertext, with each alphabetical character in the plaintext substituted for the corresponding character in the ciphertext; non-alphabetical characters should be outputted unchanged.
- The program must preserve case: capitalized letters must remain capitalized letters; lowercase letters must remain lowercase letters.
- After outputting ciphertext, it should print a newline. The program should then exit by returning 0 from main.
