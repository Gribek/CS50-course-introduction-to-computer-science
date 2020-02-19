#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

int main(int argc, string argv[])
{
    string key = argv[1];

    // check if command argument provided correctly
    if (argc != 2)
    {
        printf("Usage: ./substitution key\n");
        return 1;
    }

    // key validation
    // key length
    if (strlen(key) != 26)
    {
        printf("Key must contain 26 characters.\n");
        return 1;
    }
    // key contains only alphabetic characters, each of them occurring once
    for (int i = 0; i < 26; i++)
    {
        char letter = tolower(key[i]);
        if (!isalpha(letter))
        {
           printf("Key must contain only alphabetic characters.\n");
           return 1;
        }
        for (int j = i + 1; j < 26; j++)
        {
            if (letter == tolower(key[j]))
            {
                printf("Each letter can occur only once.\n");
                return 1;
            }
        }
    }
    // promt user for text to encrypt
    string plaintext = get_string("plaintext:  ");
    int text_length = strlen(plaintext);

    // encrypt the text
    char ciphertext[text_length];
    for (int i = 0; i < text_length; i++)
    {
        char character = plaintext[i];
        // lower case characters
        if (islower(character))
        {
            character -= 97;
            ciphertext[i] = tolower(key[(int) character]);
        }

        // upper case characters
        else if (isupper(character))
        {
            character -= 65;
            ciphertext[i] = toupper(key[(int) character]);
        }
        // non letter characters
        else
        {
            ciphertext[i] = character;
        }
    }
    printf("ciphertext: ")
    for (int i = 0; i < text_length; i++)
    {
        printf("%c", ciphertext[i]);
    }
    printf("\n");
}