#include "clist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

List * list_new(){
    List * list = (List*)malloc(sizeof(List));
    memset(list, 0, sizeof(list));
    struct link_node * node = (struct link_node *)malloc(sizeof(struct link_node));
    memset(node, 0, sizeof(node));
    list->root = node;
    list->tail = node;
    return list;
}

void list_clear(List * list){
    struct link_node * node = list->root;
    while(node){
        free(node);
        node = node->next;
    }
}

void list_pushback(List * list, void * data){
    struct link_node * node = (struct link_node *)malloc(sizeof(struct link_node));
    memset(node, 0, sizeof(node));
    node->data = data;
    list->tail->next = node;
    node->prev = list->tail;
    list->tail = node;
    list->size += 1;
}

void list_pushfront(List * list, void * data){
    struct link_node * node = (struct link_node *)malloc(sizeof(struct link_node));
    memset(node, 0, sizeof(node));
    node->data = data;
    list->root->prev = node;
    node->next = list->root;
    list->root = node;
    list->size += 1;
}

void list_popfront(List * list){
    struct link_node * tmp = list->root;
    if(!tmp)
        return;
    list->root = tmp->next;
    free(tmp);
    list->size -= 1;
}

void list_popback(List * list){
    struct link_node * tmp = list->tail;
    if(!tmp)
        return;
    list->tail = tmp->prev;
    free(tmp);
    list->size -= 1;
}

void list_free(void * list){
    List * l = (List *)list;
    list_clear(l);
    free(l);
}

