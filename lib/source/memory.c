#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../header/memory.h"

//Macro are defined in memory.h as wrappers for the real functions
//masking the primitives in this source file
#undef malloc
#undef calloc
#undef free

enum { CMP_ID = 0, CMP_ID_PTR = 0};

List _pool_list = {
    .root = NULL,
    .size = 0,
    .cmpData = {[CMP_ID] = _cmp_id, NULL},
    .printNode = _print_node,
    .deleteNode = NULL,
    .sortNode = {NULL},
};

uint16_t id = 0;

void* memInit(char* id, size_t size){

    poolNode aux = {
        .data = calloc(size + CHUNK_ALLOC_REG_SZ(size), sizeof(uint8_t)),
        .chunkReg = aux.data + size,
        .size = size,
        .id = "",
        .monitor = {
            .root = NULL,
            .size = 0,
            .printNode = _print_node,
            .cmpData = {[CMP_ID_PTR] = _cmp_id_ptr, NULL},
            .deleteNode = _delete_node,
        },
    };
    strcpy(aux.id, id);

    listInsert(&_pool_list, &aux, sizeof(poolNode), LAST_POS);
    
    return aux.data;
}

void* cmalloc(size_t size, char* id){

    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);

    //Total size in chunks of the memory block reserved by memInit(char* id , size_t size)
    uint16_t block_chunk_sz = CHUNK_ALLOC_REG_SZ(aux->size);
    
    uint16_t gap_sz = 0, is_rsvd = FALSE;
    uint8_t* init_ptr = aux->data;

    //Searches for space to allocate in the chunk register
    for(uint16_t i = 0; i < block_chunk_sz && !is_rsvd; i++){  
        for(uint16_t ii = 0; ii < REG_BITS && !is_rsvd; ii++){
            // Cheks if the bit in the register is allocated or free
            if( (aux->chunkReg[i] << ii) & HIGHEST_BIT ){
                // Resets search to the last position + 1 where chunks are allocated
                gap_sz = 0;
                uint16_t offset = (i * REG_BITS * CHUNK_SIZE) + ((ii + 1) * CHUNK_SIZE);
                init_ptr = aux->data + offset;
            }
            else gap_sz++;

            // if enough size has been found, allocate memory
            if(gap_sz >= ceil((float) (size + METADATA_SIZE) / CHUNK_SIZE))
                is_rsvd = TRUE;
        }
    }

    //Memory allocation
    if(is_rsvd){
        uint16_t init_char = (init_ptr - aux->data) / (CHUNK_SIZE * REG_BITS );
        uint16_t offset = ( (init_ptr - aux->data) % (CHUNK_SIZE * REG_BITS ) ) / CHUNK_SIZE;

        //Saves meta-data (allocated size)
        uint16_t* meta_size = (uint16_t*) init_ptr;
        *meta_size = gap_sz;
        
        //Changes chunk register 
        for(int i = init_char; gap_sz; i++ ){
            do{
                aux->chunkReg[i] |= (HIGHEST_BIT >> (offset % REG_BITS) );
                offset++;
                gap_sz--;
            }while(offset % REG_BITS && gap_sz);
        }

        //Sets mem to zero
        memset(init_ptr + METADATA_SIZE, 0x00, init_ptr - aux->data - METADATA_SIZE);

        return init_ptr + METADATA_SIZE;
    }

    return NULL;
}

void cfree(void* ptr, char* id){

    // Gets Pool with id "id"
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);
    
    uint8_t* data_ptr = ptr;
    
    // Loads init address and offset from the chunk register
    uint16_t init_char = (data_ptr - aux->data) / (CHUNK_SIZE * 8 );
    uint16_t offset = ( data_ptr - aux->data) % (CHUNK_SIZE * 8 ) / CHUNK_SIZE;
    
    // Loads metadata (size to free)
    uint16_t gap_size = *((uint16_t*) (data_ptr - 2));

    // Clears bits representing allocated chunks from the chunk register
    for(int i = init_char; gap_size; i++ ){
        do{
            (aux->chunkReg)[i] &= ~(HIGHEST_BIT >> (offset % REG_BITS) );
            offset++;
            gap_size--;
        }while(offset % REG_BITS && gap_size);
    }
    return;
}

void memPrint(char* id){

    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);

    for(int i = 0; i < CHUNK_ALLOC_REG_SZ(aux->size); i++)
        printf("0x%.2hhX ",aux->chunkReg[i]);
    printf("\n");
}

void* memMonCmalloc(size_t size, char* id, char* FILE, int LINE){
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);

    monitorNode monNd = {
        .FILE = strcpy( calloc(strlen(FILE), sizeof(uint8_t) ) , FILE),
        .LINE = LINE,
        .ptr_id = cmalloc(size, id),
        .time = clock() / (CLOCKS_PER_SEC / 1000),
    };
    listInsert(& aux->monitor, &monNd, sizeof(monitorNode), LAST_POS);
    return monNd.ptr_id;
}

void memMonCfree(void* ptr, char* id){
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);
    uint16_t pos = 0;
    listFind(&_pool_list, &pos, ptr, CMP_ID_PTR);
    listDelete(&aux->monitor, pos);
    cfree(ptr, id);
}

void memMonPrint(char *id){
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);
    fprintf(stdout, "ALLOCATED MEMORY:\n");
    listPrint(&aux->monitor, stdout);
    fprintf(stdout, "\n");
}

void _print_node(FILE* f, void* nd){
    monitorNode* aux =nd;
    fprintf(f, "Memory calloc at position : "
            "%p\tTime: %ld\tFile: %s\tLine: %d\n", 
            aux->ptr_id, aux->time, aux->FILE, aux->LINE);
}

void _delete_node(void* data){
    monitorNode* aux = data;
    free(aux->FILE);
}

int16_t _cmp_id(void* s_data, void* data){
    return strcmp(s_data, ((poolNode*)data)->id);
}

int16_t _cmp_id_ptr(void* s_data, void* data){
    return ! (s_data == ((monitorNode*)data)->ptr_id);
}