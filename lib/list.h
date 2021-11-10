#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>

#define LAST_NODE -1

struct node;

typedef struct node {
	void* data;
	struct node* next_ptr;
}NODE;

typedef NODE *LIST;
typedef NODE **ROOT;

#define LIST(x) LIST x = NULL;

NODE* create_node(void* data);

NODE* get_node(LIST* const l, unsigned int index);

unsigned int list_size(LIST* const l);

void insert_node(ROOT list_root, NODE *nd, unsigned int pos);

NODE* remove_node(ROOT list_root, unsigned int pos);

void delete_list(ROOT list_root, void (*delete_nd)(void*) );

void delete_node(ROOT list_root, unsigned int pos, void (*delete_node)(void*) );

NODE* find_node(LIST* l, unsigned int *pos, unsigned int (*cmp_data)(void* s_data, void *data), void* s_data);

void print_list(FILE* f, LIST list, void (*print_node)(FILE* f, void* nd));

#endif