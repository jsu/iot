#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

struct Node *_head, *_curr_node;

struct Node *nalloc()
{
    return (struct Node *) malloc(sizeof(struct Node));
}

char *mstrdup(char *s)
{
    char *p;
    p = (char *)malloc(strlen(s) + 1);
    if(p != NULL)
        strcpy(p, s);
    return p;
}

struct Node *insert_next(struct Node *p, char *w, uint16_t id, struct Node *parent)
{
    struct Node *node, *ptr;
    node = nalloc();
    node->name = mstrdup(w);
    node->id = id;
    node->parent = parent;
    node->child = NULL;
    node->next = node->prev = NULL;

	if(p == NULL) 
        return node;
	else
    {
        ptr = p;
        while(ptr->next)
            ptr = ptr->next;
        ptr->next = node;
        node->prev = ptr;
	    return p;
    }
}

int init_menu()
{
    struct Node *parent, *ptr;

    ptr = parent = NULL;
    ptr = insert_next(ptr, "1.Manual", 1, parent);
    ptr = insert_next(ptr, "2.Air Purifier", 2, parent);
    ptr = insert_next(ptr, "3.Comfort", 3, parent);
    ptr = insert_next(ptr, "4.Remote", 4, parent);
    _curr_node = _head = ptr;

    parent = _head;
    ptr = NULL;
    ptr = insert_next(ptr, "1-1", 11, parent);
    ptr = insert_next(ptr, "1-2", 12, parent);
    ptr = insert_next(ptr, "1-3", 13, parent);
    ptr = insert_next(ptr, "1-4", 14, parent);
    ptr = insert_next(ptr, "1-5", 15, parent);
    ptr = insert_next(ptr, "1-6", 16, parent);
    ptr = insert_next(ptr, "1-7", 17, parent);
    ptr = insert_next(ptr, "1-8", 18, parent);
    _head->child = ptr;

    parent = _head->child;
    ptr = NULL;
    ptr = insert_next(ptr, "1-1-1", 111, parent);
    ptr = insert_next(ptr, "1-1-2", 112, parent);
    ptr = insert_next(ptr, "1-1-3", 113, parent);
    ptr = insert_next(ptr, "1-1-4", 114, parent);
    ptr = insert_next(ptr, "1-1-5", 115, parent);
    ptr = insert_next(ptr, "1-1-6", 116, parent);
    ptr = insert_next(ptr, "1-1-7", 117, parent);
    _head->child->child = ptr;

    parent = _head->child->next;
    ptr = NULL;
    ptr = insert_next(ptr, "1-2-1", 121, parent);
    ptr = insert_next(ptr, "1-2-2", 122, parent);
    ptr = insert_next(ptr, "1-2-3", 123, parent);
    ptr = insert_next(ptr, "1-2-4", 124, parent);
    _head->child->next->child = ptr;

    parent = _head->child->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "1-3-1", 131, parent);
    ptr = insert_next(ptr, "1-3-2", 132, parent);
    ptr = insert_next(ptr, "1-3-3", 133, parent);
    ptr = insert_next(ptr, "1-3-4", 134, parent);
    _head->child->next->next->child = ptr;

    parent = _head->child->next->next->next->next->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "1-7-1", 171, parent);
    ptr = insert_next(ptr, "1-7-2", 172, parent);
    ptr = insert_next(ptr, "1-7-3", 173, parent);
    ptr = insert_next(ptr, "1-7-4", 174, parent);
    ptr = insert_next(ptr, "1-7-5", 175, parent);
    ptr = insert_next(ptr, "1-7-6", 176, parent);
    ptr = insert_next(ptr, "1-7-7", 177, parent);
    ptr = insert_next(ptr, "1-7-8", 178, parent);
    _head->child->next->next->next->next->next->next->child = ptr;

    parent = _head->child->next->next->next->next->next->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "1-8-1", 181, parent);
    ptr = insert_next(ptr, "1-8-2", 182, parent);
    ptr = insert_next(ptr, "1-8-3", 183, parent);
    ptr = insert_next(ptr, "1-8-4", 184, parent);
    _head->child->next->next->next->next->next->next->next->child = ptr;

    parent = _head->next;
    ptr = NULL;
    ptr = insert_next(ptr, "2-1", 21, parent);
    ptr = insert_next(ptr, "2-2", 22, parent);
    ptr = insert_next(ptr, "2-3", 23, parent);
    ptr = insert_next(ptr, "2-4", 24, parent);
    _head->next->child = ptr;

    parent = _head->next->child->next;
    ptr = NULL;
    ptr = insert_next(ptr, "2-2-1", 221, parent);
    ptr = insert_next(ptr, "2-2-2", 222, parent);
    ptr = insert_next(ptr, "2-2-3", 223, parent);
    ptr = insert_next(ptr, "2-2-4", 224, parent);
    _head->next->child->next->child = ptr;

    parent = _head->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "3-1", 31, parent);
    ptr = insert_next(ptr, "3-2", 32, parent);
    ptr = insert_next(ptr, "3-3", 33, parent);
    ptr = insert_next(ptr, "3-4", 34, parent);
    _head->next->next->child = ptr;

    return 0;
}

void list_menu()
{
    struct Node *ptr, *cptr, *ccptr;

    ptr = _head;
    while(ptr)
    {
        printf("%s: %d\n", ptr->name, ptr->id); 
        if(ptr->child)
        {
            cptr = ptr->child;
            while(cptr)
            {
                printf("\t%s: %d -> %d\n", cptr->name, cptr->parent->id, cptr->id);
                if(cptr->child)
                {
                    ccptr = cptr->child;
                    while(ccptr)
                    {
                        printf("\t\t%s: %d -> %d\n", ccptr->name, ccptr->parent->id, ccptr->id);
                        ccptr = ccptr->next;
                    }
                }
                cptr= cptr->next;
            }                    
        }
        ptr = ptr->next;
    }
}

struct Node *menu_u()
{
    if(_curr_node->prev)
        _curr_node = _curr_node->prev; 
    return _curr_node;
}

struct Node *menu_d()
{
    if(_curr_node->next)
        _curr_node = _curr_node->next;
    return _curr_node;
}

struct Node *menu_e()
{
    if(_curr_node->child)
        _curr_node = _curr_node->child;
    return _curr_node;
}

struct Node *menu_b()
{
    if(_curr_node->parent)
    {
        _curr_node = _curr_node->parent;
    }
    return _curr_node;
}

struct Node *current_menu_node()
{
    return _curr_node;
}

void reset_current_menu_node()
{
    _curr_node = _head;    
}

/*
int main(int argc, char **argv)
{
    init_menu();
    printf("Start from %d\n", _head->id);
    printf("U %d\n", menu_u()->id);
    printf("D %d\n", menu_d());
    printf("U %d\n", menu_u());
    printf("E %d\n", menu_e());
    printf("D %d\n", menu_d());
    printf("D %d\n", menu_d());
    printf("D %d\n", menu_d());
    printf("D %d\n", menu_d());
    printf("B %d\n", menu_b());

	return 0;
}
*/
