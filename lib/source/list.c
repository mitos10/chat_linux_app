#include "../header/list.h"


void listInsert(List* l, void* const data, size_t const size, int16_t const pos){

	//Reserving Memory
	NODE* nd = malloc(sizeof(NODE));
	nd->data = malloc(size);
	memcpy(nd->data, data, size);
	nd->next_ptr = NULL;

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_NODE);

	//Iterating through list
	LIST* aux_root = &l->root;
	int16_t i=0;
	for( ; (*aux_root)!=NULL && i < (uint16_t)pos; aux_root = & ((*aux_root)->next_ptr), i++);
	
	//Saving data into node
	if( i == pos || is_last_node){
		nd->next_ptr = (*aux_root);
		(*aux_root) = nd;
		l->size++;
	}
	
}

void listMove(List* l, uint16_t const pos_dest, uint16_t const pos_src){
	if(pos_dest != pos_src && pos_dest < l->size && pos_src < l->size){
		NODE** prev_src_node = NULL;
		NODE** src_node = NULL;
		NODE** prev_dest_node = NULL;
		uint16_t i = 0;
		for(LIST* it = &l->root; *it != NULL; it = &(*it)->next_ptr, i++){
			if(i == pos_src){
				prev_src_node = it;
				src_node = &(*it)->next_ptr;
			}
			if(i == pos_dest)
				if(pos_dest > pos_src)
					prev_dest_node = &(*it)->next_ptr;
				else prev_dest_node = it;
		}
		NODE* aux = *prev_src_node;
		*prev_src_node = *src_node;
		*src_node = *prev_dest_node;
		*prev_dest_node = aux;
	}
}

void listSwap(List* l, uint16_t const fst_pos, uint16_t const snd_pos){
	listMove(l, snd_pos, fst_pos);
	if(fst_pos < snd_pos)
		listMove(l, fst_pos, snd_pos - 1);
	else listMove(l, fst_pos, snd_pos + 1);
}

void* listGetData(List* l, int16_t const pos){
	
	uint16_t i = 0;

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_POS);
	
	if(l->root != NULL){

		//Iterating through list
		LIST it = NULL;
		for(it = l->root; it->next_ptr != NULL && i < (uint16_t)pos; it = it->next_ptr, i++);

		//Return data
		if(i == pos || is_last_node)
			return it->data;
	}

	return NULL;
}

void listDelete(List* l, int16_t const pos){

	NODE* nd = NULL;

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_POS);

	if(l->root != NULL){

		LIST* aux_root = & l->root;
		LIST* prev_node = aux_root;
		aux_root =  & ((*aux_root)->next_ptr);

		//Iterating through lists
		uint16_t i;
		for(i=0; (*aux_root)!=NULL && i < (uint16_t)pos ; aux_root = & ((*aux_root)->next_ptr), i++)
			prev_node = aux_root;

		//Selecting node
		NODE* aux =  *prev_node;
		if(is_last_node ||  i == pos){
			*prev_node = (*aux_root);
			nd = aux;
		}else nd = NULL;

	}else nd = l->root;

	//Delete node
	if(nd != NULL){
		(*l->deleteNode)( nd->data );
		free(nd);
		l->size--;
	}
}

void listDeleteAll(List* l){
	for(uint16_t i = 0; l->size; i++)
		listDelete(l, 0);
	l->root = NULL;
}

size_t listGetSize(List* l){
	return l->size;
}

void listPrint(List* l, FILE* f){
	int i = 0;
	for(LIST it = l->root; it != NULL; it = it->next_ptr, i++){
		fprintf(f, "%d. ", i+1);
		(*l->printNode)(f,it->data);
	}
}

void* listFind(List* l, uint16_t* pos, void* const s_data, uint16_t func){

	if(l->root != NULL){

		//Iterating through list
		uint16_t i = 0;
		for(LIST it = l->root; it != NULL; it = it->next_ptr, i++)
			if((*l->cmpData[func])(s_data, it->data)){
				if(pos!=NULL) *pos = i;
				return it->data;
			}
	}
	return NULL;
}

void listCopy(List* dest, List* src, size_t const size){
	for(LIST it = src->root; it != NULL; it = it->next_ptr)
		listInsert(dest,it->data, size, LAST_POS);
	dest = src;
}

void listSort(List* l, uint16_t func_sort, uint16_t func_cmp){
	l->sortNode[func_sort](l, func_cmp);
}