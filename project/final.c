#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

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

// Represent an individual atoms in a compound
typedef struct
{
    node *element;
    unsigned short int quantity;
}
atom;

// Represent a single compound
typedef struct
{
    atom atoms[10];
    unsigned short int coefficient;
    short int charge;
}
compound;

// Represent equation
typedef struct
{
    compound substrates[6];
    compound products[6];
}
equation;

// Linked list
node *elements = NULL;

// Prototypes
bool unload(void);
bool load(const char *data_file);
bool add_node(char *line);
void print_list(void);

int main(int argc, char *argv[])
{
    // Check for correct number of args
    if (argc < 4)
    {
        printf("Enter equation correctly\n");
        return 1;
    }

    // Allocate memory for the equation
    equation *eqtn = malloc(sizeof(equation));

    // Declare variables & counters
    char elem[3], quantity[4];
    int e, q, charge;

    // Collect data about each compound
    for (int i = 1; i < argc; i += 2)
    {
        //Reset compound charge
        charge = 0;

        // Reset counters
        e = 0;
        q = 0;

        // Identify elements in the compound, their quantity and compound charge
        for (int j = 0, len = strlen(argv[i]); j < len; j++)
        {
            char ch = argv[i][j];

            if (isupper(ch))
            {
                if (e == 0)
                {
                    elem[e] = ch;
                    e++;
                }
                else
                {
                    // TODO
                    //do sth with element
                    elem[e] = '\0';
                    printf("%s\n", elem);
                    if (q != 0)
                    {
                        quantity[q] = '\0';
                        printf("%d\n", atoi(quantity));
                        q = 0;
                    }

                    elem[0] = ch;
                    e = 1;
                }
            }
            else if (islower(ch))
            {
                if (e == 1)
                {
                    elem[e] = ch;
                    e++;
                }
                else
                {
                    free(eqtn);
                    printf("Invalid equation format\n");
                    return 1;
                }
            }
            else if (isdigit(ch))
            {
                quantity[q] = ch;
                q++;
            }
            else if (ch == '+')
            {

                charge = atoi(&argv[i][j+1]);
                break;
            }
            else if (ch == '-')
            {
                charge = -(atoi(&argv[i][j+1]));
                break;
            }
        }

        // TODO
        // do sth with element
        elem[e] = '\0';
        printf("%s\n", elem);
        if (q != 0)
            {
                quantity[q] = '\0';
                printf("%d\n", atoi(quantity));
                q = 0;
            }
        printf("%d\n", charge);
    }

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

    // Free memory for equation
    free(eqtn);
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

    // Declare char counter
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