// Represent a node in a linked list of elements
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
    short int atom_count;
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

// Represent a node in the list of elements to balance
typedef struct balance_node
{
    node *element_node;
    int occurence;
    int *compound_numbers;
    int *coefficients_ratio;
    struct balance_node *next;
}
balance_node;