#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "../header/debug.h"
#include "../header/memory.h"

//Macro are defined in memory.h as wrappers for the real functions
//masking the primitives in this source file
#undef malloc
#undef calloc
#undef free

/** LOCAL FUNCTIONS **/

static int16_t cmp_id(void* s_data, void* data);

static int16_t cmp_trace_id_ptr(void* s_data, void* data);
static void print_trace_node(FILE* f, void* nd);
static void delete_trace_node(void* data);

static int16_t cmp_rsvd_chunk_id_ptr(void* s_data, void* data);
// static void _print_rsvd_chunk_node(FILE* f, void* nd);
// static void _delete_rsvd_chunk_node(void* data);


enum { CMP_ID = 0, CMP_ID_PTR = 0};

List _pool_list = {
    .root = NULL,
    .size = 0,
    .cmpData = {[CMP_ID] = cmp_id, NULL},
    .printNode = NULL,
    .deleteNode = NULL,
    .sortNode = {NULL},
};

void* Memory_DefaultInit(size_t size){
    return Memory_Init(DEFAULT_POOL, size, "trace_default_mem");
}

void* Memory_Init(char* id, size_t size, char* traceFileRoute){

	//Argument sanity check
	__ASSERT__(id != NULL,__ASSERT_NULL__)
	__ASSERT__(traceFileRoute != NULL,__ASSERT_NULL__)

    //Initialization of memory pool
    uint16_t total_size = CHUNK_ALLOC_REG_SZ(size) * CHUNK_SIZE * 8;
    printf("SIZE %d\n",CHUNK_ALLOC_REG_SZ(size));
    
    char* buffer = NULL;
    poolNode aux = {
        .data = calloc(total_size + CHUNK_ALLOC_REG_SZ(size), sizeof(uint8_t)),
        .chunkReg = aux.data + total_size,
        .size = total_size,
        .id = "",
        .file_trace_hr = traceFileRoute? fopen((buffer = strcat(
                                                            strcpy(
                                                                (char*) calloc(strlen(traceFileRoute) + strlen("_hr.txt") + 1, sizeof(char)), 
                                                                traceFileRoute),
                                                        "_hr.txt")),
                                                "w")
                                        : stdout,
        .file_trace = traceFileRoute? fopen( strcat(
                                                strcpy(buffer, traceFileRoute),
                                                ".txt"),
                                            "w")
                                        : stdout,
        .rsvdChunk = {
            .root = NULL,
            .size = 0,
            .printNode = NULL, //_print_rsvd_chunk_node,
            .cmpData = {[CMP_ID_PTR] = cmp_rsvd_chunk_id_ptr, NULL},
            .deleteNode = NULL,//_delete_rsvd_chunk_node,
        },
        .monHandler = {
            .sizeAllocated = 0,
            .sizeEffective = 0,
            .trace = {
                .root = NULL,
                .size = 0,
                .printNode = print_trace_node,
                .cmpData = {[CMP_ID_PTR] = cmp_trace_id_ptr, NULL},
                .deleteNode = delete_trace_node,
            },
        }
    };
    strcpy(aux.id, id);

    //Add pool to list
    node_t* nd = malloc(sizeof(node_t));
	nd->data = memcpy(malloc(sizeof(poolNode)), &aux, sizeof(poolNode));
	nd->next_ptr = NULL;
    List_InsertNode(&_pool_list, nd, LAST_POS);
    
    //Return ptr to initial position
    return aux.data;
}

void* Memory_CustomMalloc(size_t size, char* id){

    poolNode* aux = List_Find(&_pool_list, NULL, id, CMP_ID);

    //If block of memory is not found return NULL
    if(NULL == aux)
        return NULL;

    //Total size in chunks of the memory block reserved by Memory_Init(char* id , size_t size)
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
        memset(init_ptr + METADATA_SIZE, 0x00, size);

        return init_ptr + METADATA_SIZE;
    }

    return NULL;
}

void Memory_CustomFree(void* ptr, char* id){

    // Gets Pool with id "id"
    poolNode* aux = List_Find(&_pool_list, NULL, id, CMP_ID);
    
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

void Memory_Print(char* id){

    //Find memory pool by id
    poolNode* aux = List_Find(&_pool_list, NULL, id, CMP_ID);

    //If pool is not found return 
    if(NULL == aux)
        return;

    //Print Layout
    fprintf(aux->file_trace_hr, "Memory Layout\n");        
    for(int i = 0; i < CHUNK_ALLOC_REG_SZ(aux->size); i++)
        fprintf(aux->file_trace_hr, "0x%.2hhX ",aux->chunkReg[i]);
    fprintf(aux->file_trace_hr, "\n\n");
}

void* Memory_MonitorCustomMalloc(size_t size, char* id, char* FILE, uint16_t LINE){
    poolNode* aux = List_Find(&_pool_list, NULL, id, CMP_ID);

    if(NULL == aux)
        return NULL;

    chunkNode chNd = {
        .ptr = Memory_CustomMalloc(size, id),
        .size = size,
    };

    traceNode monNd = {
        .FILE = strcpy( calloc(strlen(FILE), sizeof(uint8_t) ) , FILE),
        .LINE = LINE,
        .op = MALLOC_OP,
        .ptr_id = chNd.ptr,
        .time = clock() / (CLOCKS_PER_SEC / 1000),
        .size = size, 
    };


    if(NULL == monNd.ptr_id){
        fprintf(aux->file_trace_hr, "Memory pool: \"%s\" out of memory.\tSize: %-10lu Time %-10ld File: %-20s Line: %-5d\n\n",
                                    id, monNd.size, monNd.time, monNd.FILE, monNd.LINE);
        #ifdef __RECORD_ALL__
            Memory_MonitorPrint(id);
        #endif
        free(monNd.FILE);
        return NULL;
    }

    node_t* nd = malloc(sizeof(node_t));
	nd->data = memcpy(malloc(sizeof(chunkNode)), &chNd, sizeof(chunkNode));
	nd->next_ptr = NULL;

    List_InsertNode(& aux->rsvdChunk, nd, LAST_POS);

    nd = malloc(sizeof(node_t));
	nd->data = memcpy(malloc(sizeof(traceNode)), &monNd, sizeof(traceNode));
	nd->next_ptr = NULL;

    aux->monHandler.sizeAllocated += ceil( (float)(size+METADATA_SIZE) / CHUNK_SIZE) * CHUNK_SIZE;
    aux->monHandler.sizeEffective += size;
    List_InsertNode(& aux->monHandler.trace, nd, LAST_POS);
    
    #ifdef __RECORD_ALL__
        Memory_MonitorPrint(id);
    #endif

    return monNd.ptr_id;
}

void Memory_MonitorCustomFree(void* ptr, char* id, char* FILE, uint16_t LINE){
    poolNode* aux = List_Find(&_pool_list, NULL, id, CMP_ID);
    if(NULL == aux)
        return;
    uint16_t pos = 0;
    chunkNode* aux_node = List_Find(&aux->rsvdChunk, &pos, ptr, CMP_ID_PTR);
    if(NULL == aux_node){
        fprintf(aux->file_trace_hr, "Ptr %p already freed or never allocated. FILE %s, LINE %d\n\n", ptr, FILE, LINE);
        return; 
    }

    traceNode monNd = {
        .FILE = strcpy( calloc(strlen(FILE), sizeof(uint8_t) ) , FILE),
        .LINE = LINE,
        .op = FREE_OP,
        .ptr_id = aux_node->ptr,
        .time = clock() / (CLOCKS_PER_SEC / 1000),
        .size = aux_node->size, 
    };

    node_t* nd = malloc(sizeof(node_t));
	nd->data = memcpy(malloc(sizeof(traceNode)), &monNd, sizeof(traceNode));
	nd->next_ptr = NULL;

    aux->monHandler.sizeAllocated -= ceil( (float)(aux_node->size+METADATA_SIZE) / CHUNK_SIZE) * CHUNK_SIZE;
    aux->monHandler.sizeEffective -= aux_node->size;
    List_InsertNode(& aux->monHandler.trace, nd, LAST_POS);

    free(List_DeleteNode(&aux->rsvdChunk, pos));
    Memory_CustomFree(ptr, id);
    #ifdef __RECORD_ALL__
        Memory_MonitorPrint(id);
    #endif
}

void Memory_MonitorPrint(char *id){
    poolNode* aux = List_Find(&_pool_list, NULL, id, CMP_ID);
    if(NULL == aux)
        return;
    if(List_GetSize(&aux->monHandler.trace)){
        fprintf(aux->file_trace_hr, "Allocated Memory on \"%s\" (%dB/%dB) Space effectively used: %.2f%% :\n",
        id, aux->monHandler.sizeAllocated, aux->size, 
        (float)aux->monHandler.sizeEffective / (aux->monHandler.sizeAllocated?aux->monHandler.sizeAllocated:1) * 100);
        List_Print(&aux->monHandler.trace, aux->file_trace_hr);
        fprintf(aux->file_trace_hr, "\n");
    }
}

static void print_trace_node(FILE* f, void* nd){
    traceNode* aux =nd;
    fprintf(f, "Memory %s at position : "
            "%-20p Size: %-10lu Time: %-10ld File: %-20s Line: %-5d\n",
            aux->op == MALLOC_OP ? "calloc" : aux->op == FREE_OP? "free" : "calloc", aux->ptr_id, aux->size, aux->time, aux->FILE, aux->LINE);
    fflush(f);
}

static void delete_trace_node(void* data){
    traceNode* aux = data;
    free(aux->FILE);
}

static int16_t cmp_id(void* s_data, void* data){
    return strcmp(s_data, ((poolNode*)data)->id);
}

static int16_t cmp_trace_id_ptr(void* s_data, void* data){
    return !(s_data == ((traceNode*)data)->ptr_id);
}

static int16_t cmp_rsvd_chunk_id_ptr(void* s_data, void* data){
    return !(s_data == ((chunkNode*)data)->ptr);
}