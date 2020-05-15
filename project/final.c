#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Default file with elements data
#define DATAFILE "elements"

// Represent a node in a linked list
typedef struct node
{
    char element[3];
    char type[3];
    struct node *next;
}
node;

// Linked list
node *elements = NULL;

// Prototypes
bool unload(void);
bool load(const char *data_file);
bool add_node(char *line);
void print_list(void);

int main(void)
{
    // Load elements data to memory
    bool loaded = load(DATAFILE);

    // Exit if data not loaded
    if (!loaded)
    {
        printf("Could not load %s\n", DATAFILE);
        return 1;
    }

    // Print linked list
    print_list();

    // Unload elements data from memory
    bool unloaded = unload();

    // Report error if data not unloaded
    if (!unloaded )
    {
        printf("Could not unload %s\n", DATAFILE);
        return 1;
    }
}

// Load data into memory, returning true if successful else false
bool load(const char *data_file)
{
    // Open data file
    FILE *file = fopen(data_file, "r");

    // Exit if file not opened correctly
    if (file == NULL)
    {
        return false;
    }

    // Allocate memory for line
    char *line = malloc(200 * sizeof(char));
    if (line == NULL)
    {
        return false;
    }

    // Get the first character
    char ch = fgetc(file);

    // Initialize char counter
    int counter;

    // Read data from the file
    while (ch != EOF)
    {
        // Get next char if current one is a newline character
        if (ch == '\n')
        {
            ch = fgetc(file);
            continue;
        }

        // Reset counter for new line
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

        // Create a new node and copy data
        bool node_added = add_node(line);
        if (!node_added)
        {
            return false;
        }
    }

    // Close data file and free memory
    free(line);
    fclose(file);

    return true;
}

// Unload data from memory, return true if successful
bool unload(void)
{
    while (elements != NULL)
    {
        node *t = elements->next;
        free(elements);
        elements = t;
    }
    return true;
}

bool add_node(char *line)
{
    // Allocate memorey for new node
    node *n = malloc(sizeof(node));
    if (n == NULL)
    {
        return false;
    }

    // Copy data into node
    char *data = strtok(line, ",");
    strcpy(n->element, data);
    data = strtok(NULL, ",");
    strcpy(n->type, data);

    // Last node in linked list
    n->next = NULL;

    // Insert node to the linked list
    if (elements != NULL)
    {
        node *tmp = elements;
        while (tmp->next != NULL)
        {
            tmp = tmp->next;
        }
        tmp->next = n;
    }
    else
    {
        elements = n;
    }

    return true;
}

// Print all data from linked list
void print_list(void)
{
    for (node *cursor = elements; cursor != NULL; cursor = cursor->next)
    {
        printf("Element: %s; type: %s;\n", cursor->element, cursor->type);
    }
}