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


    char ch = fgetc(file);
    int counter;
    while (ch != EOF)
    {
        if (ch == '\n')
        {
            ch = fgetc(file);
            continue;
        }

        char *line = malloc(200 * sizeof(char));

        counter = 0;

        do
        {
            line[counter] = ch;
            counter++;
            ch = fgetc(file);
        }
        while (ch != '\n' && ch != EOF);

        line[counter] = '\0';
        printf("%s\n", line);

        free(line);
    }

    // Close data file
    fclose(file);
}
