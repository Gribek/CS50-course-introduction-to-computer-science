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
    char *element;
    char *type;
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
    short int charge;
}
compound;

// Represent equation
typedef struct
{
    compound *substrates;
    compound *products;
    unsigned short int *subs_coefficients;
    unsigned short int *prod_coefficients;
}
equation;

// Linked list
node *elements = NULL;

// Prototypes
bool unload(void);
bool load(const char *data_file);
bool add_node(char *line);
node *find(char *elem);
void print_list(void);
bool unload_eqtn(equation *eqtn);

int main(int argc, char *argv[])
{
    // Check for correct number of args
    if (argc < 4)
    {
        printf("Enter equation correctly\n");
        return 1;
    }

    // TODO
    // Equation validation

    // Count number of substrates and products
    int subs_number = 0, prod_number = 0;
    bool equal_sign = false;
    char c;
    for (int i = 1; i < argc; i++)
    {
        c = argv[i][0];
        if (c == '=')
        {
            equal_sign = true;
            continue;
        }
        if (isupper(c) || c == '(' || c == '[')
        {
            if (!equal_sign)
            {
                subs_number++;
            }
            else
            {
                prod_number++;
            }
        }
    }

    // Load elements data to memory
    bool loaded = load(DATAFILE);

    // Exit if data not loaded
    if (!loaded)
    {
        printf("Could not load %s\n", DATAFILE);
        return 1;
    }

    // Allocate memory for the equation data
    equation *eqtn = malloc(sizeof(equation));
    eqtn->substrates = malloc(sizeof(compound) * subs_number);
    eqtn->products = malloc(sizeof(compound) * prod_number);
    eqtn->subs_coefficients = malloc(sizeof(int) * subs_number);
    eqtn->prod_coefficients = malloc(sizeof(int) * prod_number);

    // Declare variables & counters
    char elem[3], quantity[4];
    int e, q, a, charge;

    // Collect data about each compound
    for (int i = 1, n = 0; i < argc; i += 2, n++)
    {
        // Reset counters
        charge = 0;
        a = 0;
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
                    elem[e] = '\0';
                    quantity[q] = '\0';


                    // TODO
                    // save data to substrates and products !


                    eqtn->substrates[n].atoms[a].element = find(elem);
                    eqtn->substrates[n].atoms[a].quantity = (q == 0) ? 1 : atoi(quantity);
                    q = 0;
                    a++;

                    printf("%s\n", elem);
                    printf("%d\n", atoi(quantity));

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
                    unload_eqtn(eqtn);
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

        elem[e] = '\0';
        quantity[q] = '\0';

        eqtn->substrates[n].atoms[a].element = find(elem);
        eqtn->substrates[n].atoms[a].quantity = (q == 0) ? 1 : atoi(quantity);

        eqtn->substrates[n].charge = charge;

        printf("%s\n", elem);
        printf("%d\n", atoi(quantity));

        printf("%d\n", charge);
    }

    // Print linked list
    print_list();

    node *cursor = eqtn->substrates[0].atoms[0].element;
    printf("TEST\nElement: %s; type: %s;\n", cursor->element, cursor->type);
    printf("Q: %d", eqtn->substrates[0].atoms[0].quantity);

    // Unload elements data from memory
    bool unloaded = unload();

    // Report error if data not unloaded
    if (!unloaded )
    {
        printf("Could not unload %s\n", DATAFILE);
        return 1;
    }

    // Free memory for equation data
    bool unloaded_eqtn = unload_eqtn(eqtn);
    if (!unloaded_eqtn)
    {
        printf("Could not unload equation data");
        return 1;
    }
}

bool unload_eqtn(equation *eqtn)
{
    free(eqtn->substrates);
    free(eqtn->products);
    free(eqtn->subs_coefficients);
    free(eqtn->prod_coefficients);
    free(eqtn);
    return true;
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
        free(elements->element);
        free(elements->type);
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
    n->element = malloc(strlen(data) + 1);
    strcpy(n->element, data);
    data = strtok(NULL, ",");
    n->type = malloc(strlen(data) + 1);
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

node *find(char *elem)
{
    // Create cursor to traverse across linked list
    node *cursor = elements;

    // Loop through nodes in linked list
    while (cursor != NULL)
    {
        // Check if element is the one you are looking for
        if (strcmp(cursor->element, elem) == 0)
        {
        return cursor;
        }
        // Go to the next node
        cursor = cursor->next;
    }
    return NULL;
}

// Print all data from linked list
void print_list(void)
{
    for (node *cursor = elements; cursor != NULL; cursor = cursor->next)
    {
        printf("Element: %s; type: %s;\n", cursor->element, cursor->type);
    }
}