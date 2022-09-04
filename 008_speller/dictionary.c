// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <math.h>

#include "dictionary.h"

unsigned int bucket_qty(void);

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of letters to include in the hash table index
const unsigned int LETTER_DEPTH = 3;

// Number of buckets in hash table
const unsigned int N = 18277;

// Hash table
node *table[N];

// Number of words in dictionary
unsigned int word_qty = 0;

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    unsigned int word_index = hash(word);

    node *cursor = table[word_index];

    // Check if the node matches the word
    while (cursor != NULL)
    {
        if (strcasecmp(word, cursor->word) == 0)
        {
            return true;
        }
        else
        {
            cursor = cursor->next;
        }
    }

    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    int starting_index;

    // Check if first character is a letter and skip if not
    if (((64 < word[0]) && (91 > word[0])) || ((96 < word[0]) && (123 > word[0])))
    {
        starting_index = 0;
    }
    else
    {
        starting_index = 1;
    }

    int word_length = strlen(word);
    int letter_qty;

    // Determine the number of letters to include in bucket
    if (word_length < LETTER_DEPTH)
    {
        letter_qty = word_length;
    }
    else
    {
        letter_qty = LETTER_DEPTH;
    }

    // Create array to store the letter numbers
    int letters[letter_qty];

    for (int i = 0; i < letter_qty; i++)
    {
        letters[i] = 0;
    }

    // Determine index of each letter in alphabet
    for (int i = starting_index; i < letter_qty + starting_index; i++)
    {
        char letter = word[i];

        // Get letter index in alphabet
        if ((64 < letter) && (91 > letter))
        {
            letter = letter - 64;
        }
        else
        {
            letter = letter - 96;
        }

        letters[i - starting_index] = letter;
    }

    // Determine the word index based on the quantity of letters in the array
    if (letter_qty == 1)
    {
        return letters[0] - 1;
    }
    else
    {
        int word_index = 0;
        int array_pow = letter_qty - 1;

        for (int i = 0; i < letter_qty; i++)
        {
            if (array_pow == 0)
            {
                word_index += letters[i];
            }
            else
            {
                word_index += pow(26, array_pow) * letters[i];
                array_pow -= 1;
            }
        }

        return word_index - 1;
    }
}

// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // Open user specified ditionary
    FILE *selected_dictionary = fopen(dictionary, "r");
    if (selected_dictionary == NULL)
    {
        printf("Could not open dictonary.");
        fclose(selected_dictionary);
        return false;
    }

    char buffer[LENGTH + 1]; // this might cause problems

    unsigned int word_count = 0;

    // Read words from dictionary and place in hash table
    while (fscanf(selected_dictionary, "%s", buffer) != EOF)
    {
        word_count += 1;

        node *new_word = malloc(sizeof(node));
        if (new_word == NULL)
        {
            printf("Could not create new word.");
            return 1;
        }

        // Set the next node to NULL
        new_word->next = NULL;

        // Copy the word into the node
        strcpy(new_word->word, buffer);

        int word_index = hash(new_word->word);

        // Add new word node to the head of the table index
        if (table[word_index] == NULL)
        {
            table[word_index] = new_word;
        }
        else
        {
            new_word->next = table[word_index];
            table[word_index] = new_word;
        }
    }

    fclose(selected_dictionary);
    unsigned int *pword_qty = &word_qty;
    *pword_qty = word_count;
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    if (word_qty != 0)
    {
        return word_qty;
    }
    else
    {
        return 0;
    }

}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    unsigned int i;
    for (i = 0; i < N; i++)
    {
        node *cursor = table[i];

        while (cursor != NULL)
        {
            node *tmp_cursor = cursor;
            cursor = cursor->next;
            free(tmp_cursor);
        }
    }

    return true;
}