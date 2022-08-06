#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, string argv[])
{
  
    // Check that only two arguments are passed else terminate
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }
  
    // Check that the key contains 26 characters else terminate
    else if (strlen(argv[1]) != 26)
    {
        printf("Key must contain 26 characters.\n");
        return 1;
    }

    // Check if the key contains unique instances of letters
    int i;
    for (i = 0; i < strlen(argv[1]); i++)
    {
        int character = argv[1][i];

        // Change lower ascii character to upper
        if (islower(character))
        {
            character -= 32;
        }

        // Check if character is a letter
        if (islower(character) || isupper(character))
        {
            int letter_reps = 0;

            // Count instances of characters
            int j;
            for (j = 0; j < strlen(argv[1]); j++)
            {
                int character_checker = argv[1][j];

                // Change lower ascii character to upper
                if (islower(character_checker))
                {
                    character_checker -= 32;
                }

                // Increase letter count if character is repeated
                if (character == character_checker)
                {
                    letter_reps += 1;
                }
            }

            // Terminate if a letter is duplicated
            if (letter_reps > 1)
            {
                printf("Key must contain 26 characters.\n");
                return 1;
            }
        }

        // Terminate if the key is not exclusively letters
        else
        {
            printf("Key must contain 26 characters.\n");
            return 1;
        }
    }

    // Get text input to be ciphered
    string plain_text = get_string("plaintext: ");
    string cipher_text = plain_text;

    // Cipher any letter contained in text input based on key
    int k;
    for (k = 0; k < strlen(plain_text); k++)
    {

        // Loop through ascii lower characters
        int l;
        for (l = 65; l < 91; l++)
        {

            // Check if n letter of user input is lower
            if (islower(plain_text[k]))
            {

                // Check if n letter matches looped lower ascii
                if (l == (plain_text[k] - 32))
                {

                    // Replace n letter in cipher with character in key at the same array index and convert to lower
                    cipher_text[k] = tolower(argv[1][l - 65]);
                    break;
                }
            }

            // Check if n letter of user input is upper
            else if (isupper(plain_text[k]))
            {

                // Check if n letter matches looped lower ascii
                if (l == plain_text[k])
                {

                    // Replace n letter in cipher with character in key at the same array index and convert to upper
                    cipher_text[k] = toupper(argv[1][l - 65]);
                    break;
                }
            }
            else
            {
              
                // Continue to next character if not a letter
                break;
            }
        }
    }

    // Print cipher text
    printf("ciphertext: %s\n", cipher_text);
}
