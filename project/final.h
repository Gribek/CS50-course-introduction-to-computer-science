#include "struct.h"

// Function prototypes
bool unload(void);
bool load(const char *data_file);
bool add_node(char *line);
bool add_balance_node(node *elem, int occur, balance_node **p);
bool check_priority(balance_node *n, balance_node *current);
bool check_list(node *elem);
compound *get_compound(int comp_number, int subs_number);
int count_occurrences(node *elem, int subs_number, int comp_number, int k);
void set_balance_data(balance_node *p, int occ, node *elem, int subs_number, int comp_number, int k);
bool free_balance(void);
bool check_balance(int subs_number, balance_node **next_elem);
void balance_element(balance_node *elem, int subs_number);
void balance_multiple(balance_node *elem, int subs_number);
bool sides_equal(int n, int sums[], int last_substrate, bool *subs_low);
void balance_two(balance_node *elem, int subs_number);
void count_amount(balance_node *p, int subs_number, int *subs_side, int *prod_side);
int find_lcm(int num1, int num2);
node *find(char *elem);
bool check_equation(int argc, char *argv[]);
void count_compounds(int argc, char *argv[], int *subs, int *prod);
void analyze_compounds(char *argv[], int number, compound *type, int first);
int calculate_quantity(char *quantity, int q, int multiplier);
void save_atom(char *element, int quantity, atom *save_to);
void print_equation(int argc, char *argv[], int subs);
bool unload_eqtn(void);