#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Default file with elements data
#define DATAFILE "elements"

int main(void)
{
    // Open data file
    FILE *file = fopen(DATAFILE, "r");

    // Exit if file not opened correctly
    if (file == NULL)
    {
        printf("Could not open %s\n", DATAFILE);
        return 1;
    }

    // Initialize counter
    int counter;

    // Get the first character
    char ch = fgetc(file);

    // Read data from file
    while (ch != EOF)
    {
        // Get next char if current one is a newline character
        if (ch == '\n')
        {
            ch = fgetc(file);
            continue;
        }

        // Allocate memory for next line
        char *line = malloc(200 * sizeof(char));

        // Reset counter
        counter = 0;

        // Get all characters in this line
        do
        {
            line[counter] = ch;
            counter++;
            ch = fgetc(file);
        }
        while (ch != '\n' && ch != EOF);

        // Add null character
        line[counter] = '\0';

        // Print line
        printf("%s\n", line);

        // Free memory for line
        free(line);
    }

    // Close data file
    fclose(file);
}
