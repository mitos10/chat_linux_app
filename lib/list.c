#include "list.h"

NODE* create_node(void* data){

	NODE* nd = malloc(sizeof(NODE));
	nd->data = data;
	nd->next_ptr = NULL;

	return nd;

}

NODE* get_node(LIST* const l, unsigned int index){
	LIST it;
	int i = 0;
	unsigned int end_flag = ! (index + 1);
	if(*l != NULL){
		for(it = *l; it->next_ptr != NULL && i < index; it = it->next_ptr, i++);
		if(i == index || end_flag)
			return it;
	}
	return NULL;
}

unsigned int list_size(LIST* const l){
	LIST it;
	int i = 0;
	for(it = *l; it != NULL; it = it->next_ptr, i++);
	return i;
}

void insert_node(ROOT list_root, NODE* nd, unsigned int pos){

	unsigned int end_flag = ! (pos + 1);

	LIST* aux_root = list_root;
	int i;

	for(i=0; (*aux_root)!=NULL && i < pos; aux_root = & ((*aux_root)->next_ptr), i++);
	if( i == pos || end_flag){
		nd->next_ptr = (*aux_root);
		(*aux_root) = nd;
	}
}

void delete_list(ROOT list_root, void (*delete_nd)(void*) ){
	int i = 0;
	int l_size = list_size(list_root);
	for(i = 0; i < l_size; i++)
		delete_node(list_root, 0, delete_nd);
	*list_root = NULL;
}

void delete_node(ROOT list_root, unsigned int pos, void (*delete_nd)(void*) ){
	
	NODE* nd = remove_node(list_root, pos);
	if(nd != NULL){
		(*delete_nd)( nd->data );
		free(nd);
	}
}

NODE* remove_node(ROOT list_root, unsigned int pos){

	unsigned int end_flag = ! (pos + 1);

	if(list_root != NULL){

		LIST* aux_root = list_root;
		LIST* prev_node = aux_root;
		aux_root =  & ((*aux_root)->next_ptr);
		int i;
		for(i=0; (*aux_root)!=NULL && i < pos ; aux_root = & ((*aux_root)->next_ptr), i++)
			prev_node = aux_root;

		NODE* aux =  *prev_node;
		if(end_flag ||  i == pos){
			*prev_node = (*aux_root);
			return aux;
		}
		return NULL;
	}

	NODE* aux = *list_root;
	return aux;
}

NODE* find_node(LIST* l, unsigned int (*cmp_data)(void* s_data, void* data), void* s_data){
	int i;
	int l_size = list_size(l);
	for(i = 0; i < l_size; i++){
		NODE* node = get_node(l, i);
		if((*cmp_data)(s_data, node->data)==1)
			return node;
	}
	return NULL;
}

void print_list(FILE* f, LIST list, void (*print_nd)(FILE* f, void* nd)){
	for( ;list != NULL; list = list->next_ptr)
		(*print_nd)(f,list->data);
}