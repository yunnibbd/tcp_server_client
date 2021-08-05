#ifndef __CLIST_H__
#define __CLIST_H__

struct link_node{
    struct link_node * prev;
    struct link_node * next;
    void * data;
};

typedef struct{
    struct link_node * root;
    struct link_node * tail;
    int size;
}List;

List * list_new();

void list_clear(List * list);

void list_pushback(List * list, void * data);

void list_pushfront(List * list, void * data);

void list_popfront(List * list);

void list_popback(List * list);

void list_free(void * list);


#endif

