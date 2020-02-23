// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
const unsigned int N = 10000;

// Number of words in hash table
unsigned int hash_table_words = 0;

// Hash table
node *table[N];

// Returns true if word is in dictionary else false
bool check(const char *word)
{

    // create cursor to traverse across the selected linked list
    node *cursor = table[hash(word)];

    // loop through nodes in linked list
    while (cursor != NULL)
    {
        // check if the word is the one you are looking for
        if (strcasecmp(word, cursor->word) == 0)
        {
            return true;
        }

        // go to the next node
        cursor = cursor->next;
    }

    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    int sum = 0;

    for (int i = 0, len = strlen(word); i < len; i++)
    {
        if (isalpha(word[i]))
        {
            sum += tolower(word[i]) * (len - i);
        }
    }

    return (sum % N);
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Open dictionary file
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }

    // allocate memory for dictionary words
    char *word = malloc(sizeof(char) * (LENGTH + 1));
    if (word == NULL)
    {
        return false;
    }

    // create pointer for nodes
    node *n = NULL;

    unsigned int hash_index;
    int c;
    while ((c =  fscanf(dict, "%s", word)) != EOF)
    {
        n = malloc(sizeof(node));
        if (n == NULL)
        {
            return false;
        }

        // copy dictionary word into node
        strcpy(n->word, word);

        // get index from hash function
        hash_index = hash(word);

        // insert node to the selected linked list
        if (table[hash_index] != NULL)
        {
            n->next = table[hash_index];
            table[hash_index] = n;
        }
        else
        {
            n->next = NULL;
            table[hash_index] = n;
        }

        // increment number of words in hash table
        hash_table_words++;
    }

    // free memory for dictionary words
    free(word);
    fclose(dict);

    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return hash_table_words;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    node *cursor = NULL;

    for (int i = 0; i < N; i++)
    {
        cursor = table[i];
        while (cursor != NULL)
        {
            cursor = cursor->next;
            free(table[i]);
            table[i] = cursor;
        }
    }
    return true;
}
