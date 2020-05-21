#include "struct.h"

// Function prototypes
bool unload(void);
bool load(const char *data_file);
bool add_node(char *line);
node *find(char *elem);
void count_compounds(int argc, char *argv[], int *subs, int *prod);
void analyze_compounds(char *argv[], int number, compound *type, int first);
int calculate_quantity(char *quantity, int q, int multiplier);
void save_atom(char *element, int quantity, atom *save_to);
void print_list(void);
bool unload_eqtn(equation *eqtn);