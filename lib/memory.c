#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "memory.h"

LIST (pool_list);
unsigned int id = 0;

unsigned int _cmp_id(void* s_data, void* data);

void* init_memory(char* id, size_t size){
    pool_node* aux = (pool_node*) malloc(sizeof (pool_node));
    aux->data = calloc(size + CHUNK_SECTION_SIZE(size), sizeof(char));
    strcpy(aux->id, id);
    aux->pool_chunks = aux->data + size;
    aux->size =size;
    insert_node(&pool_list,create_node(aux),LAST_NODE);
    return aux->data;
}

void* cmalloc(size_t size, char* id){
    pool_node* aux = (pool_node*)find_node(&pool_list,_cmp_id,id)->data;
    unsigned int pool_chunks_sz = CHUNK_SECTION_SIZE(aux->size), gap_size = 0, i, ii;
    unsigned int reserve_flag = FALSE;
    char* init_ptr = aux->data;
    for(i = 0; i < pool_chunks_sz && reserve_flag == FALSE; i++){  
        for(ii = 0; ii < 8 && reserve_flag == FALSE; ii++){
            if( ( ( ( (char*)aux->pool_chunks)[i] << ii) & 0x80 ) == 0x80 ){
                gap_size = 0;
                init_ptr = aux->data + i * 8 * CHUNK_SIZE + (ii + 1) * CHUNK_SIZE;
            }
            else gap_size++;
            if(gap_size >= ceil( (float)(size + METADATA_SIZE) / CHUNK_SIZE))
                reserve_flag = TRUE;
        }
    }

    if(reserve_flag == TRUE){
        unsigned int init_char = (init_ptr - (char*)aux->data) / (CHUNK_SIZE * 8 );
        unsigned int offset = ( (init_ptr - (char*)aux->data) % (CHUNK_SIZE * 8 ) ) / CHUNK_SIZE;

        init_ptr[0] = ((char*)&gap_size)[0];
        init_ptr[1] = ((char*)&gap_size)[1];
        
        for(int i = init_char; gap_size != 0; i++ ){
            do{
                ((char*)aux->pool_chunks)[i] |= (0x80 >> (offset % 8) );
                offset++;
                gap_size--;
            }while(offset % 8 != 0 && gap_size != 0);
        }
        return init_ptr + METADATA_SIZE;
    }
    return NULL;
}

void cfree(void* ptr, char* id){
    pool_node* aux = (pool_node*)find_node(&pool_list,_cmp_id,id)->data;
    unsigned int init_char = ((char*)ptr - (char*)aux->data) / (CHUNK_SIZE * 8 );
    unsigned int offset = ( ((char*)ptr - (char*)aux->data) % (CHUNK_SIZE * 8 ) ) / CHUNK_SIZE;
    unsigned short gap_size = *((unsigned short*) (ptr - 2) );
    for(int i = init_char; gap_size != 0; i++ ){
        do{
            ((char*)aux->pool_chunks)[i] &= ~(0x80 >> (offset % 8) );
            offset++;
            gap_size--;
        }while(offset % 8 != 0 && gap_size != 0);
    }
    return;
}

void print_mem(char* id){
    pool_node* aux = (pool_node*)find_node(&pool_list,_cmp_id,id)->data;
    for(int i = 0; i < CHUNK_SECTION_SIZE(aux->size); i++)
        printf("0x%.2hhX ",((char*)aux->pool_chunks)[i]);
    printf("\n");
}

unsigned int _cmp_id(void* s_data, void* data){
    pool_node *aux = (pool_node*) data;
    return !strcmp(s_data, aux->id);
}