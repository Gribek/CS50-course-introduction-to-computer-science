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