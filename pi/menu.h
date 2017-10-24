#include <stdint.h>
struct Node
{
	char *name;
	uint16_t id;
    struct Node *parent;
    struct Node *child;
	struct Node *prev;
	struct Node *next;
};

int init_menu();
void list_menu();
struct Node *menu_u(); /* Move Up */
struct Node *menu_d(); /* Move Down */
struct Node *menu_e(); /* Enter */
struct Node *menu_b(); /* Exit (Back) */
struct Node *current_menu_node();
void reset_current_menu_node();
