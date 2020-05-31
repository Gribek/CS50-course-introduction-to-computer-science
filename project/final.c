#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

// Function prototypes & struct definitions
#include "final.h"

// Default file with elements data
#define DATAFILE "elements"

// Linked lists
// Elements data
node *elements = NULL;
// List of elements to balance
balance_node *to_balance = NULL;

// Equation data
equation *eqtn = NULL;


int main(int argc, char *argv[])
{
    // Equation validation
    bool correct = check_equation(argc, argv);
    if (!correct)
    {
        printf("Invalid equation format\n");
        return 1;
    }

    // Count number of substrates and products
    int subs_number = 0, prod_number = 0;
    count_compounds(argc, argv, &subs_number, &prod_number);

    // Count total number of compounds
    int comp_number = subs_number + prod_number;

    // Load elements data to memory
    bool loaded = load(DATAFILE);

    // Exit if data not loaded
    if (!loaded)
    {
        printf("Could not load %s\n", DATAFILE);
        return 1;
    }

    // Allocate memory for the equation data
    eqtn = malloc(sizeof(equation));
    if (eqtn == NULL)
    {
        printf("Could not allocate memmory\n");
        return 1;
    }
    eqtn->substrates = malloc(sizeof(compound) * subs_number);
    eqtn->products = malloc(sizeof(compound) * prod_number);
    eqtn->subs_coefficients = malloc(sizeof(int) * subs_number);
    eqtn->prod_coefficients = malloc(sizeof(int) * prod_number);
    if (eqtn->substrates == NULL || eqtn->products == NULL ||
        eqtn->subs_coefficients == NULL || eqtn->prod_coefficients == NULL)
    {
        printf("Could not allocate memmory\n");
        return 1;
    }

    // Assign coefficients to 1
    for (int i = 0; i < subs_number; i++)
    {
        eqtn->subs_coefficients[i] = 1;
    }
    for (int i = 0; i < prod_number; i++)
    {
        eqtn->prod_coefficients[i] = 1;
    }

    // Load substrates
    analyze_compounds(argv, subs_number, eqtn->substrates, 1);

    // Load products
    analyze_compounds(argv, prod_number, eqtn->products, 2 * subs_number + 1);

    // Prepare elements to balance in the correct order
    for (int i = 0; i < comp_number; i++)
    {
        // Get current compound
        compound *c = get_compound(i, subs_number);

        // Loop through the atoms
        for (int j = 0; j < c->atom_count; j++)
        {
            // Get current atom
            node *elem = c->atoms[j].element;

            // Check if element is in balance list
            bool in_list = check_list(elem);

            // Add element to balance list if not present
            if (!in_list)
            {
                // Count number of occurrences in equation
                int occ = count_occurrences(elem, subs_number, comp_number, i);

                // Declare pointer to the new node
                balance_node *p = NULL;

                // Add node to the list in the correct order
                bool added = add_balance_node(elem, occ, &p);
                if (!added)
                {
                    printf("Could not allocate memmory\n");
                    return 1;
                }

                // Set compound numbers in the node
                set_balance_data(p, occ, elem, subs_number, comp_number, i);
            }
        }
    }

    // Poiter to the next element that needs balancing
    balance_node *next = NULL;

    // Keep checking if the equation is balanced
    while (!check_balance(subs_number, &next))
    {
        // Balance the selected element
        balance_element(next, subs_number);
    }

    // Print a balanced equation
    print_equation(argc, argv, subs_number);

    // Unload elements data from memory
    bool unloaded = unload();

    // Report error if data not unloaded
    if (!unloaded)
    {
        printf("Could not unload %s\n", DATAFILE);
        return 1;
    }

    // Free memory for balance data
    bool unloaded_balance = free_balance();
    if (!unloaded_balance)
    {
        printf("Could not unload balance data");
        return 1;
    }

    // Free memory for equation data
    bool unloaded_eqtn = unload_eqtn();
    if (!unloaded_eqtn)
    {
        printf("Could not unload equation data");
        return 1;
    }
}

// ELEMENT DATA - LINKED LIST
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
        free(elements);
        elements = t;
    }
    return true;
}

// Add new node to the linked list
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
    n->balance_priority = atoi(data);

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

// BALANCE DATA - LINKED LIST
// Add new node to the linked list
bool add_balance_node(node *elem, int occur, balance_node **p)
{
    // Allocate memory for new node
    balance_node *n = malloc(sizeof(balance_node));
    if (n == NULL)
    {
        return false;
    }

    // Save address of the node
    *p = n;

    // Save data into the node
    n->element_node = elem;
    n->occurrence = occur;

    // Allocate memory for node data
    n->compound_numbers = malloc(sizeof(int) * occur);
    n->atom_quantity = malloc(sizeof(int) * occur);
    n->coefficients_ratio = malloc(sizeof(int) * occur);
    if (n->compound_numbers == NULL || n->coefficients_ratio == NULL)
    {
        return false;
    }


    // Add first node of the list
    if (to_balance ==  NULL)
    {
        n->next = NULL;
        n->prev = NULL;
        to_balance = n;
        return true;
    }

    // Create a cursor to traverse across the list
    balance_node *cursor = to_balance;

    // Insert node in the correct order
    while (true)
    {
        // Check if thi is right place for new node
        if (check_priority(n, cursor))
        {
            // Inser new balance node
            if (cursor->prev != NULL)
            {
                n->prev = cursor->prev;
                cursor->prev->next = n;
            }
            else
            {
                n->prev = NULL;
                to_balance = n;
            }
            n->next = cursor;
            cursor->prev = n;
            return true;
        }

        // Check if this is the last node
        if (cursor->next == NULL)
        {
            break;
        }
        cursor = cursor->next;
    }

    // Add node at the end
    n->next = NULL;
    n->prev = cursor;
    cursor->next = n;
    return true;
}

// Compare two balance nodes, return true if new node has priority else false
bool check_priority(balance_node *n, balance_node *current)
{
    // Calculate differences in priority and occurrence
    int prior_difference = n->element_node->balance_priority - current->element_node->balance_priority;
    int occur_difference = n->occurrence - current->occurrence;

    // Give priority in these situations
    if (prior_difference < -2)
    {
        return true;
    }

    if (prior_difference == -2 && occur_difference < 2)
    {
        return true;
    }

    if (prior_difference == -1 && occur_difference < 1)
    {
        return true;
    }

    if (prior_difference == 0 && occur_difference < 0)
    {
        return true;
    }
    if (prior_difference == 1 && occur_difference < 0)
    {
        return true;
    }
    if (prior_difference == 2 && occur_difference < -1)
    {
        return true;
    }

    // Return false in other cases
    return false;
}

// Check if the element is in the list
bool check_list(node *elem)
{
    // Create cursor to traverse through the list
    balance_node *cursor = to_balance;

    // Loop througt the list
    while (cursor != NULL)
    {
        // Check if elements are the same
        if (cursor->element_node == elem)
        {
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

// Prepare and save data to the new balnce node
void set_balance_data(balance_node *p, int occ, node *elem, int subs_number, int comp_number, int k)
{
    // Loop through all compounds
    for (int i = k, m = 0; i < comp_number; i++)
    {
        // Get next compound
        compound *c = get_compound(i, subs_number);

        // Look for the element in question
        for (int j = 0; j < c->atom_count; j++)
        {
            // Save data if element is in current compound
            if (elem == c->atoms[j].element)
            {
                p->compound_numbers[m] = i;
                p->atom_quantity[m] = c->atoms[j].quantity;
                m++;
                break;
            }
        }

        // Quit afer identifying all compounds where element is present
        if (m == occ)
        {
            break;
        }
    }
}

// Free memory for balance list, return true if successful
bool free_balance(void)
{
    while (to_balance != NULL)
    {
        balance_node *t = to_balance->next;
        free(to_balance->compound_numbers);
        free(to_balance->coefficients_ratio);
        free(to_balance->atom_quantity);
        free(to_balance);
        to_balance = t;
    }
    return true;
}



// BALANCING FUNCTIONS
// Check if the equation is balanced
bool check_balance(int subs_number, balance_node **next_elem)
{
    // Create a cursor to traverse the list
    balance_node *cursor = to_balance;
    // Initialize variables
    int subs_side, prod_side, coeff;

    // Loop through all elements
    while (cursor != NULL)
    {
        // Reset element counters
        subs_side = 0, prod_side = 0;

        // Count amount of element on both side of the equation
        count_amount(cursor, subs_number, &subs_side, &prod_side);

        // If amounts not equal return false
        if (subs_side != prod_side)
        {
            *next_elem = cursor;
            return false;
        }

        // Go to the next element
        cursor = cursor->next;
    }

    // When all elements are in balance return true
    return true;
}

// Balance the element depending on the number of occurrences in the equation
void balance_element(balance_node *elem, int subs_number)
{
    // Element occurring in two compounds
    if(elem->occurrence == 2)
    {
        balance_two(elem, subs_number);
    }
    // Element occurring in more than two compounds
    else
    {
        balance_multiple(elem, subs_number);
    }
}

// Balance element occurring in more than two compounds
void balance_multiple(balance_node *elem, int subs_number)
{
    // Prepare arrays to store atoms data
    int occ = elem->occurrence;
    int n, coeffs[occ], quantities[occ], sums[occ];

    // Copy data to arrays
    for (int i = 0; i < occ; i++)
    {
        int comp_number = elem->compound_numbers[i];

        if (comp_number < subs_number)
        {
            coeffs[i] = eqtn->subs_coefficients[comp_number];
            n = i;
        }
        else
        {
            coeffs[i] = eqtn->prod_coefficients[comp_number - subs_number];
        }

        quantities[i] = elem->atom_quantity[i];
        sums[i] = coeffs[i] * quantities[i];
    }

    // Variables to track balance process
    bool subs_low;
    int start, end, lowest;

    // Balance quantity of elements on both sides
    while (!sides_equal(occ, sums, n, &subs_low))
    {
        if (subs_low)
        {
            lowest = 0;
            start = 1, end = n + 1;
        }
        else
        {
            lowest = n + 1;
            start = n + 2, end = occ;
        }

        for (int i = start; i < end; i++)
        {
            if (sums[lowest] > sums[i])
            {
                lowest = i;
            }
            else if (sums[lowest] == sums[i])
            {
                lowest = (quantities[lowest] < quantities[i]) ? lowest : i;
            }
        }
        coeffs[lowest] += 1;
        sums[lowest] = coeffs[lowest] * quantities[lowest];
    }

    // Save coefficients in the equation data
    for (int i = 0; i < occ; i++)
    {
        int comp_number = elem->compound_numbers[i];

        if (comp_number < subs_number)
        {
            eqtn->subs_coefficients[comp_number] = coeffs[i];
        }
        else
        {
            eqtn->prod_coefficients[comp_number - subs_number] = coeffs[i];
        }
    }
}

// Check if number of atoms are the same on both side of the equation
bool sides_equal(int n, int sums[], int last_substrate, bool *subs_low)
{
    // Variable to trac amount of atoms
    int subs_side = 0, prod_side = 0;

    // Calculate quantity of atom on both sides
    for (int i = 0; i < n; i++)
    {
        if (i <= last_substrate)
        {
            subs_side += sums[i];
        }
        else
        {
            prod_side += sums[i];
        }
    }

    // Return true if sides balanced
    if (subs_side == prod_side)
    {
        return true;
    }
    // Return false if substrate side low
    else if (subs_side < prod_side)
    {
        *subs_low = true;
        return false;
    }
    // Return false if product side low
    else
    {
        *subs_low = false;
        return false;
    }
}

// Balance element occurring in two compounds
void balance_two(balance_node *elem, int subs_number)
{
    int subs_side = 0, prod_side = 0;

    // Count amount of element on both sides of equation
    count_amount(elem, subs_number, &subs_side, &prod_side);

    // Find LCM of both numbers
    int lcm = find_lcm(subs_side, prod_side);

    // Set new coefficients values
    for (int i = 0; i < elem->occurrence; i++)
    {
        int comp_number = elem->compound_numbers[i];

        if (comp_number < subs_number)
        {
            eqtn->subs_coefficients[comp_number] *= (lcm / subs_side);
        }
        else
        {
            eqtn->prod_coefficients[comp_number - subs_number] *= (lcm / prod_side);
        }
    }
}

// Count the amount of selected atom on both sides of the equation
void count_amount(balance_node *p, int subs_number, int *subs_side, int *prod_side)
{
    for (int i = 0; i < p->occurrence; i++)
    {
        int comp_number = p->compound_numbers[i];

        if (comp_number < subs_number)
        {
            int coeff = eqtn->subs_coefficients[comp_number];
            *subs_side += coeff * p->atom_quantity[i];
        }
        else
        {
            int coeff = eqtn->prod_coefficients[comp_number - subs_number];
            *prod_side += coeff * p->atom_quantity[i];
        }
    }
}

// Calculate lowest common multiple
int find_lcm(int num1, int num2)
{
    // Find which number is higher
    int higher_num = (num1 > num2) ? num1 : num2;
    int value = higher_num;

    // Increase the value until lcm is found
    while(1)
    {
        if (value % num1 == 0 && value % num2 == 0)
        {
            return value;
        }
        value += higher_num;
    }
}

// INPUT VALIDATION
// Check if equation is entered correctly
bool check_equation(int argc, char *argv[])
{
    // Check for correct number of arguments
    if (argc < 4 || argc % 2 != 0)
    {
        return false;
    }

    // Current and previous character in equation
    char c, p;

    // Open/close brackets
    bool square = false, curly = false;

    // Equals sign
    bool equals = false;

    for (int i = 1; i < argc; i++)
    {
        // Compound check
        if (i % 2 == 1)
        {
            // Check first character in compound
            c = argv[i][0];

            if (isupper(c))
            {}
            else if (c == '{')
            {
                curly = true;
            }
            else if (c == '[')
            {
                square = true;
            }
            else
            {
                return false;
            }

            // Check middle characters
            for (int j = 1, len = strlen(argv[i]); j < len; j++)
            {
                p = c;
                c = argv[i][j];

                // Check if char is uppercase letter
                if (isupper(c))
                {}
                // Check if upper letter before lower
                else if (islower(c) && (isupper(p)))
                {}
                // Mark curly braces as open if not already open
                else if (c == '{')
                {
                    if (!curly)
                    {
                        curly = true;
                    }
                    else
                    {
                        return false;
                    }
                }
                // Mark square brackets as open if not already open
                else if (c == '[')
                {
                    if (!square)
                    {
                        square = true;
                    }
                    else
                    {
                        return false;
                    }
                }
                // Mark curly braces as close if open
                else if (c == '}')
                {
                    if (curly)
                    {
                        curly = false;
                    }
                    else
                    {
                        return false;
                    }
                }
                // Mark square brackets as close if open
                else if (c == ']')
                {
                    if (square)
                    {
                        square = false;
                    }
                    else
                    {
                        return false;
                    }
                }
                // Check if letter, close bracket or charge sign before digit
                else if (isdigit(c) && (isalnum(p) || p == '}' || p == '+' || p =='-'))
                {}
                // Check charge sign at the end of compound
                else if (c == '+' || c == '-')
                {
                    if (j == (len - 1))
                    {}
                    else if (j == (len - 2) && isdigit(argv[i][j+1]))
                    {}
                    else
                    {
                        return false;
                    }
                }
                // Return false for other cases
                else
                {
                    return false;
                }
            }
            // Check if brackets are close
            if (curly || square)
            {
                return false;
            }
        }
        // Sign check
        else
        {
            // Check for correct length
            if (strlen(argv[i]) != 1)
            {
                return false;
            }

            c = argv[i][0];

            // Check if the symbol is allowed allowed synbol and
            if (c == '+')
            {}
            // Check if equality sign occurs only once
            else if (c == '=' && !equals)
            {
                equals = true;
            }
            else
            {
                return false;
            }
        }
    }
    // Check if equals sign occurs in equation
    if (!equals)
    {
        return false;
    }

    return true;
}

// COMPOUND ANALYSIS FUNCTIONS
// Conduct compound analysis and save data about them in equation structure
void analyze_compounds(char *argv[], int number, compound *type, int first)
{
    // Declare variables & counters
    char atom[3], quantity[4];
    int a, q, m, charge, multiplier;

    // Collect data about each compound
    for (int i = first, n = 0; n < number; i += 2, n++)
    {
        // Reset charge and counters
        charge = 0;
        multiplier = 1;
        m = 0;
        a = 0;
        q = 0;

        // Identify elements in the compound, their quantity and compound charge
        for (int j = 0, len = strlen(argv[i]); j < len; j++)
        {
            char ch = argv[i][j];

            if (isupper(ch))
            {
                if (a == 0)
                {
                    atom[a] = ch;
                    a++;
                }
                else
                {
                    atom[a] = '\0';
                    quantity[q] = '\0';

                    save_atom(atom, calculate_quantity(quantity, q, multiplier), &(type[n].atoms[m]));

                    q = 0;
                    m++;

                    atom[0] = ch;
                    a = 1;
                }
            }
            else if (islower(ch))
            {
                atom[a] = ch;
                a++;
            }
            else if (ch == '{')
            {
                if (a != 0)
                {
                    atom[a] = '\0';
                    quantity[q] = '\0';

                    save_atom(atom, calculate_quantity(quantity, q, multiplier), &(type[n].atoms[m]));

                    a = 0;
                    q = 0;
                    m++;
                }

                int k = j;
                while (argv[i][k] != '}')
                {
                    k++;
                }
                multiplier = atoi(&argv[i][k+1]);
            }
            else if (ch == '}')
            {
                atom[a] = '\0';
                quantity[q] = '\0';

                save_atom(atom, calculate_quantity(quantity, q, multiplier), &(type[n].atoms[m]));

                a = 0;
                q = 0;
                m++;

                multiplier = 1;
                j++;
            }
            else if (isdigit(ch))
            {
                quantity[q] = ch;
                q++;
            }
            else if (ch == '+')
            {
                if (j == (len - 1))
                {
                    charge = 1;
                }
                else
                {
                    charge = atoi(&argv[i][j+1]);
                    break;
                }
            }
            else if (ch == '-')
            {
                if (j == (len - 1))
                {
                    charge = -1;
                }
                else
                {
                    charge = -(atoi(&argv[i][j+1]));
                    break;
                }
            }
        }

        type[n].charge = charge;

        if (a == 0)
        {
            type[n].atom_count = m;
            continue;
        }

        atom[a] = '\0';
        quantity[q] = '\0';

        save_atom(atom, calculate_quantity(quantity, q, multiplier), &(type[n].atoms[m]));

        type[n].atom_count = m + 1;
    }
}

// Calculate quantity of the atom
int calculate_quantity(char *quantity, int q, int multiplier)
{
    if (q == 0)
    {
        if (multiplier == 1)
        {
            return 1;
        }
        else
        {
            return multiplier;
        }
    }
    else
    {
        return atoi(quantity) * multiplier;
    }
}

// Save atom to the equation data
void save_atom(char *element, int quantity, atom *save_to)
{
    save_to->element = find(element);
    save_to->quantity = quantity;
}

// Find atom in the elements list
node *find(char *atom)
{
    // Create cursor to traverse across linked list
    node *cursor = elements;

    // Loop through nodes in linked list
    while (cursor != NULL)
    {
        // Check if element is the one you are looking for
        if (strcmp(cursor->element, atom) == 0)
        {
        return cursor;
        }
        // Go to the next node
        cursor = cursor->next;
    }
    return NULL;
}

// OTHER SUPPORTING FUNCTIONS
// Free memory for equation data
bool unload_eqtn(void)
{
    free(eqtn->substrates);
    free(eqtn->products);
    free(eqtn->subs_coefficients);
    free(eqtn->prod_coefficients);
    free(eqtn);
    return true;
}

// Count number of substrates and products in the equation
void count_compounds(int argc, char *argv[], int *subs, int *prod)
{
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
        if (isupper(c) || c == '{' || c == '[')
        {
            if (!equal_sign)
            {
                *subs += 1;
            }
            else
            {
                *prod += 1;
            }
        }
    }
}

// Return pointer to compound by its number
compound *get_compound(int comp_number, int subs_number)
{
    compound *c;

    // Substrates
    if (comp_number < subs_number)
    {
        c = &(eqtn->substrates[comp_number]);
    }
    // Porducts
    else
    {
        c = &(eqtn->products[comp_number - subs_number]);
    }
    return c;
}

// Count how many times element occurs in all compounds in the equation
int count_occurrences(node *elem, int subs_number, int comp_number, int k)
{
    // Initialize counter ot zero
    int counter = 0;

    // Loop throug all compounds
    for (int i = k; i < comp_number; i++)
    {
        // Get next compound
        compound *c = get_compound(i, subs_number);

        // Look for the element in compound
        for (int j = 0; j < c->atom_count; j++)
        {
            // If the element is present, increment counter
            if (elem == c->atoms[j].element)
            {
                counter++;
                break;
            }
        }
    }
    return counter;
}

// Print an equation with coefficients
void print_equation(int argc, char *argv[], int subs)
{
    // Compound coefficient
    int coeff;

    // Loop through all parts of the equation and print them
    for (int i = 1, j = 0; i < argc; i++)
    {
        if (i % 2 == 0)
        {
            printf("%s ", argv[i]);
        }
        else
        {
            if (j < subs)
            {
                coeff = eqtn->subs_coefficients[j];
            }
            else
            {
                coeff = eqtn->prod_coefficients[j - subs];
            }
            printf("%d%s ", coeff, argv[i]);
            j++;
        }
    }

    // Add new line
    printf("\n");
}