#include "../header/list.h"
#include "../header/debug.h"

void List_Insert(List* l, void* const data, size_t const size, int16_t const pos){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	__ASSERT__(data != NULL,__ASSERT_NULL__)
	__ASSERT__(pos >= -1,__ASSERT_NULL__)
	
	//Reserving Memory
	node_t* nd = malloc(sizeof(node_t));
	__ASSERT__(nd!=NULL,__ASSERT_NULL__)
	nd->data = memcpy(malloc(size), data, size);
	__ASSERT__(nd->data, __ASSERT_NULL__);
	nd->next_ptr = NULL;
	
	//Inits node internal data
	if(l->initNode != NULL)
		l->initNode(nd->data);

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_NODE);

	//Iterating through list
	root_t* aux_root = &l->root;
	int16_t i=0;
	for( ; (*aux_root)!=NULL && i < (uint16_t)pos; aux_root = & ((*aux_root)->next_ptr), i++);
	
	//Saving data into node
	if( i == pos || is_last_node){
		nd->next_ptr = (*aux_root);
		(*aux_root) = nd;
		l->size++;
	}
	
}

void List_InsertNode(List* l, node_t* nd, int16_t const pos){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	__ASSERT__(nd != NULL,__ASSERT_NULL__)
	__ASSERT__(pos >= -1,__ASSERT_NULL__)

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_NODE);

	//Iterating through list
	root_t* aux_root = &l->root;
	int16_t i=0;
	for( ; (*aux_root)!=NULL && i < (uint16_t)pos; aux_root = & ((*aux_root)->next_ptr), i++);
	
	//Saving data into node
	if( i == pos || is_last_node){
		nd->next_ptr = (*aux_root);
		(*aux_root) = nd;
		l->size++;
	}

}

void List_Move(List* l, uint16_t const pos_dest, uint16_t const pos_src){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)

	if(pos_dest != pos_src && pos_dest < l->size && pos_src < l->size){
		
		node_t** prev_src_node_next = NULL;
		node_t** src_node_next = NULL;
		node_t** prev_dest_node_next = NULL;
		
		uint16_t i = 0;
		
		for(root_t* it = &l->root; *it != NULL; it = &(*it)->next_ptr, i++){

			//Takes position source node and previous one
			if(i == pos_src){
				prev_src_node_next = it;
				src_node_next = &(*it)->next_ptr;
			}
			
			//Takes position dest node
			//E.g. if moving 1 to 3 -- 2-3-1 the node is put after
			//node 3, if moving 5 to 3 -- 1-2-5-3-4 the node is
			//put after node 2 
			if(i == pos_dest){
				if(pos_dest > pos_src)
					prev_dest_node_next = &(*it)->next_ptr;
				else prev_dest_node_next = it;
			}
		}
		
		//E.g. Moving 3 to 5

		//aux = 2->next_ptr
		node_t* aux = *prev_src_node_next;
		
		//2->next_ptr = 3->next_ptr (node 4)
		//now 1-2-4-5-6-7... 3 is out
		*prev_src_node_next = *src_node_next;
		
		//3->next_ptr = 5->next_ptr (node 6)
		*src_node_next = *prev_dest_node_next;
		
		//5->next_ptr = node 3
		//now 1-2-4-5-3-6-7...
		*prev_dest_node_next = aux;
	}
}

void List_Swap(List* l, uint16_t const fst_pos, uint16_t const snd_pos){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)

	uint16_t i= 0, is_fst_found = 0, is_snd_found = 0;
	root_t it_fst_pos = NULL, it_snd_pos = NULL;
	
	if(fst_pos != snd_pos)
		for(root_t it = l->root; it != NULL && !(is_fst_found && is_snd_found); it = it->next_ptr, i++){
			if(i == fst_pos){
				is_fst_found = 1;
				it_fst_pos = it;
			}
			if(i == snd_pos){
				is_snd_found = 1;
				it_snd_pos = it;
			}
		}

	//Just swaps data from both nodes
	void* aux = it_fst_pos->data;
	it_fst_pos->data = it_snd_pos->data;
	it_snd_pos->data = aux;
}

void* List_GetData(List* l, int16_t const pos){
	
	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	__ASSERT__(pos >= -1,__ASSERT_NULL__)
	
	uint16_t i = 0;

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_POS);
	
	if(l->root != NULL){

		//Iterating through list
		root_t it = NULL;
		for(it = l->root; it->next_ptr != NULL && i < (uint16_t)pos; it = it->next_ptr, i++);

		//Return data
		if(i == pos || is_last_node)
			return it->data;
	}

	return NULL;
}

void List_Delete(List* l, int16_t const pos){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	__ASSERT__(pos >= 0 || pos == LAST_POS,__ASSERT_NULL__)
	
	node_t* nd = NULL;

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_POS);

	if(l->root != NULL){

		root_t* aux_root = & l->root;
		root_t* prev_node = aux_root;
		aux_root =  & ((*aux_root)->next_ptr);

		//Iterating through lists
		uint16_t i;
		for(i=0; (*aux_root)!=NULL && i < (uint16_t)pos ; aux_root = & ((*aux_root)->next_ptr), i++)
			prev_node = aux_root;

		//Selecting node
		node_t* aux =  *prev_node;
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

node_t* List_DeleteNode(List* l, int16_t const pos){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	__ASSERT__(pos >= 0,__ASSERT_NULL__)

	node_t* nd = NULL;

	//Checking if last node
	uint16_t is_last_node = (pos == LAST_POS);

	if(l->root != NULL){

		root_t* aux_root = & l->root;
		root_t* prev_node = aux_root;
		aux_root =  & ((*aux_root)->next_ptr);

		//Iterating through lists
		uint16_t i;
		for(i=0; (*aux_root)!=NULL && i < (uint16_t)pos ; aux_root = & ((*aux_root)->next_ptr), i++)
			prev_node = aux_root;

		//Selecting node
		node_t* aux =  *prev_node;
		if(is_last_node ||  i == pos){
			*prev_node = (*aux_root);
			nd = aux;
		}else nd = NULL;

	}else nd = l->root;

	//Delete node
	return nd;
}

void List_DeleteAll(List* l){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	
	for(uint16_t i = 0; l->size; i++)
		List_Delete(l, 0);
	l->root = NULL;
}

size_t List_GetSize(List* l){
	
	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)

	return l->size;
}

void List_Print(List* l, FILE* f){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	__ASSERT__(f != NULL,__ASSERT_NULL__)

	int i = 0;
	for(root_t it = l->root; it != NULL; it = it->next_ptr, i++){
		fprintf(f, "%d. ", i+1);
		(*l->printNode)(f,it->data);
	}
}

void* List_Find(List* l, uint16_t* pos, void* const s_data, uint16_t func){

	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	//__ASSERT__(pos != NULL,__ASSERT_NULL__)
	__ASSERT__(s_data != NULL,__ASSERT_NULL__)
	__ASSERT__(l->cmpData[func] != NULL,__ASSERT_BACKTRACE__)

	if(l->root != NULL){
		//Iterating through list
		uint16_t i = 0;
		for(root_t it = l->root; it != NULL; it = it->next_ptr, i++)
			if(!(*l->cmpData[func])(s_data, it->data)){
				if(pos!=NULL) *pos = i;
				return it->data;
			}
	}
	return NULL;
}

void List_Copy(List* dest, List* src, size_t const size){

	//Argument sanity check
	__ASSERT__(dest != NULL,__ASSERT_NULL__)
	__ASSERT__(src != NULL,__ASSERT_NULL__)

	for(root_t it = src->root; it != NULL; it = it->next_ptr)
		List_Insert(dest,it->data, size, LAST_POS);
	dest = src;
}

void List_Sort(List* l, uint16_t func_sort, uint16_t func_cmp){
	
	//Argument sanity check
	__ASSERT__(l != NULL,__ASSERT_NULL__)
	__ASSERT__(l->sortNode[func_sort] != NULL,__ASSERT_NULL__)
	
	l->sortNode[func_sort](l, func_cmp);
}