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

void* memDefaultInit(size_t size){
    memInit(DEFAULT_POOL, size, "trace_default_mem.txt");
}

void* memInit(char* id, size_t size, char* traceFileRoute){


    //Initialization of memory pool
    uint16_t total_size = CHUNK_ALLOC_REG_SZ(size) * CHUNK_SIZE * 8;
    printf("SIZE %d\n",CHUNK_ALLOC_REG_SZ(size));
    poolNode aux = {
        .data = calloc(total_size + CHUNK_ALLOC_REG_SZ(size), sizeof(uint8_t)),
        .chunkReg = aux.data + total_size,
        .size = total_size,
        .id = "",
        .file = traceFileRoute? fopen(traceFileRoute, "w"): stdout,
        .monHandler = {
            .sizeAllocated = 0,
            .sizeEffective = 0,
            .trace = {
            .root = NULL,
            .size = 0,
            .printNode = _print_node,
            .cmpData = {[CMP_ID_PTR] = _cmp_id_ptr, NULL},
            .deleteNode = _delete_node,
            },
        }
    };
    strcpy(aux.id, id);

    //Add pool to list
    NODE* nd = malloc(sizeof(NODE));
	nd->data = memcpy(malloc(sizeof(poolNode)), &aux, sizeof(poolNode));
	nd->next_ptr = NULL;
    listInsertRsvd(&_pool_list, nd, LAST_POS);
    //listInsert(&_pool_list, &aux, sizeof(poolNode), LAST_POS);
    
    //Return ptr to initial position
    return aux.data;
}

void* cmalloc(size_t size, char* id){

    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);

    //If block of memory is not found return NULL
    if(NULL == aux)
        return NULL;

    //Total size in chunks of the memory block reserved by memInit(char* id , size_t size)
    uint16_t block_chunk_sz = CHUNK_ALLOC_REG_SZ(aux->size);
    printf("SIZE %d %d\n",block_chunk_sz, aux->size);
    
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
        //memset(init_ptr + METADATA_SIZE, 0x00, size);

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

    //Find memory pool by id
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);

    //If pool is not found return 
    if(NULL == aux)
        return;

    //Print Layout
    fprintf(aux->file, "Memory Layout\n");        
    for(int i = 0; i < CHUNK_ALLOC_REG_SZ(aux->size); i++)
        fprintf(aux->file, "0x%.2hhX ",aux->chunkReg[i]);
    fprintf(aux->file, "\n\n");
}

void* memMonCmalloc(size_t size, char* id, char* FILE, uint16_t LINE){
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);

    if(NULL == aux)
        return NULL;

    traceNode monNd = {
        .FILE = strcpy( calloc(strlen(FILE), sizeof(uint8_t) ) , FILE),
        .LINE = LINE,
        .ptr_id = cmalloc(size, id),
        .time = clock() / (CLOCKS_PER_SEC / 1000),
        .size = size, 
    };

    NODE* nd = malloc(sizeof(NODE));
	nd->data = memcpy(malloc(sizeof(traceNode)), &monNd, sizeof(traceNode));
	nd->next_ptr = NULL;

    if(NULL == monNd.ptr_id){
        fprintf(aux->file, "Memory pool: \"%s\" out of memory.\tSize: %-10lu Time %-10ld File: %-20s Line: %-5d\n\n",
                                    id, monNd.size, monNd.time, monNd.FILE, monNd.LINE);
        #ifdef RECORD_ALL
            memMonPrint(id);
        #endif
        free(monNd.FILE);
        return NULL;
    }
    aux->monHandler.sizeAllocated += ceil( (float)(size+METADATA_SIZE) / CHUNK_SIZE) * CHUNK_SIZE;
    aux->monHandler.sizeEffective += size;
    listInsertRsvd(& aux->monHandler.trace, nd, LAST_POS);
    
    #ifdef RECORD_ALL
        memMonPrint(id);
    #endif

    return monNd.ptr_id;
}

void memMonCfree(void* ptr, char* id, char* FILE, uint16_t LINE){
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);
    if(NULL == aux)
        return;
    uint16_t pos = 0;
    traceNode* aux_node = listFind(&aux->monHandler.trace, &pos, ptr, CMP_ID_PTR);
    if(NULL == aux_node){
        fprintf(aux->file, "Ptr %p already freed or never allocated. FILE %s, LINE %d\n\n", ptr, FILE, LINE);
        return; 
    }
    aux->monHandler.sizeAllocated -= ceil( (float)(aux_node->size+METADATA_SIZE) / CHUNK_SIZE) * CHUNK_SIZE;
    aux->monHandler.sizeEffective -= aux_node->size;
    free(listDeleteRsvd(&aux->monHandler.trace, pos));
    cfree(ptr, id);
    #ifdef RECORD_ALL
        memMonPrint(id);
    #endif
}

void memMonPrint(char *id){
    poolNode* aux = listFind(&_pool_list, NULL, id, CMP_ID);
    if(NULL == aux)
        return;
    if(listGetSize(&aux->monHandler.trace)){
        fprintf(aux->file, "Allocated Memory on \"%s\" (%.dB/%.dB) Space effectively used: %.2f%% :\n",
        id, aux->monHandler.sizeAllocated, aux->size, (float)aux->monHandler.sizeEffective / aux->monHandler.sizeAllocated * 100);
        listPrint(&aux->monHandler.trace, aux->file);
        fprintf(aux->file, "\n");
    }
}

static void _print_node(FILE* f, void* nd){
    traceNode* aux =nd;
    fprintf(f, "Memory calloc at position : "
            "%-20p Size: %-10lu Time: %-10ld File: %-20s Line: %-5d\n",
            aux->ptr_id, aux->size, aux->time, aux->FILE, aux->LINE);
    fflush(f);
}

static void _delete_node(void* data){
    traceNode* aux = data;
    free(aux->FILE);
}

static int16_t _cmp_id(void* s_data, void* data){
    return strcmp(s_data, ((poolNode*)data)->id);
}

static int16_t _cmp_id_ptr(void* s_data, void* data){
    return !(s_data == ((traceNode*)data)->ptr_id);
}