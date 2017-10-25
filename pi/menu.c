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
    ptr = insert_next(ptr, "Manual", 1, parent);
    ptr = insert_next(ptr, "Comfort", 2, parent);
    ptr = insert_next(ptr, "Purify", 3, parent);
    ptr = insert_next(ptr, "Remote/Local", 4, parent);
    _curr_node = _head = ptr;

    parent = _head;
    ptr = NULL;
    ptr = insert_next(ptr, "Fan", 11, parent);
    ptr = insert_next(ptr, "ESP", 12, parent);
    ptr = insert_next(ptr, "Ions", 13, parent);
    ptr = insert_next(ptr, "UV", 14, parent);
    ptr = insert_next(ptr, "Ozone", 15, parent);
    _head->child = ptr;

    parent = _head->child;
    ptr = NULL;
    ptr = insert_next(ptr, "Fan Stop", 111, parent);
    ptr = insert_next(ptr, "Fan Duty 250", 112, parent);
    ptr = insert_next(ptr, "Fan Duty 400", 113, parent);
    ptr = insert_next(ptr, "Fan Duty 550", 114, parent);
    ptr = insert_next(ptr, "Fan Duty 700", 115, parent);
    ptr = insert_next(ptr, "Fan Duty 850", 116, parent);
    _head->child->child = ptr;

    parent = _head->child->next;
    ptr = NULL;
    ptr = insert_next(ptr, "ESP Start", 121, parent);
    ptr = insert_next(ptr, "ESP Stop", 122, parent);
    _head->child->next->child = ptr;

    parent = _head->child->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "Ions Start", 131, parent);
    ptr = insert_next(ptr, "Ions Stop", 132, parent);
    _head->child->next->next->child = ptr;

    parent = _head->child->next->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "UV Light Start", 141, parent);
    ptr = insert_next(ptr, "UV Light Stop", 142, parent);
    ptr = insert_next(ptr, "UV Set Time", 143, parent);
    _head->child->next->next->next->child = ptr;

    parent = _head->child->next->next->next->child;
    ptr = NULL;
    ptr = insert_next(ptr, "UV 10 Minutes", 1431, parent);
    ptr = insert_next(ptr, "UV 15 Minutes", 1432, parent);
    ptr = insert_next(ptr, "UV 30 Minutes", 1433, parent);
    ptr = insert_next(ptr, "UV 60 Minutes", 1433, parent);
    _head->child->next->next->next->child->child = ptr;

    parent = _head->child->next->next->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "O3 Start", 151, parent);
    ptr = insert_next(ptr, "O3 Stop", 152, parent);
    ptr = insert_next(ptr, "O3 Set Time", 153, parent);
    _head->child->next->next->next->next->child = ptr;

    parent = _head->child->next->next->next->next->child;
    ptr = NULL;
    ptr = insert_next(ptr, "O3  5 Minutes", 1531, parent);
    ptr = insert_next(ptr, "O3 10 Minutes", 1532, parent);
    ptr = insert_next(ptr, "O3 15 Minutes", 1533, parent);
    ptr = insert_next(ptr, "O3 30 Minutes", 1534, parent);
    _head->child->next->next->next->next->child->child = ptr;

    parent = _head->next;
    ptr = NULL;
    ptr = insert_next(ptr, "Comfort Start", 21, parent);
    ptr = insert_next(ptr, "Comfort Stop", 22, parent);
    _head->next->child = ptr;

    parent = _head->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "Purify Stop", 31, parent);
    ptr = insert_next(ptr, "UV Mode", 32, parent);
    ptr = insert_next(ptr, "O3 Mode", 33, parent);
    ptr = insert_next(ptr, "UV/O3 Hybrid", 34, parent);
    _head->next->next->child = ptr;

    parent = _head->next->next->next;
    ptr = NULL;
    ptr = insert_next(ptr, "Remote", 41, parent);
    ptr = insert_next(ptr, "Local", 42, parent);
    _head->next->next->next->child = ptr;

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
