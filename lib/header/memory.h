#ifndef MEMORY_H
#define MEMORY_H

/** INCLUDES **/

#include <math.h>
#include <time.h>
#include "list.h"

/** DEFINES **/

#define CHUNK_SIZE  ((uint16_t)32)
#define CHUNK_ALLOC_REG_SZ(size) (uint16_t) ceil( (double)size / (CHUNK_SIZE*8) )
#define METADATA_SIZE ((uint16_t)2)
#define HIGHEST_BIT ((uint16_t) 0x80)
#define REG_BITS ((uint16_t) 8)
#define DEFAULT_POOL "Default Pool"

/**MACROS**/

#define malloc(SIZE)            Memory_MonitorCustomMalloc(SIZE, DEFAULT_POOL, __FILE__, __LINE__)
#define calloc(SIZE, T_SIZE)    Memory_MonitorCustomMalloc(SIZE * T_SIZE, DEFAULT_POOL, __FILE__, __LINE__)
#define free(PTR)               Memory_MonitorCustomFree(PTR, DEFAULT_POOL, __FILE__, __LINE__)

//#warning malloc, calloc and free have been overrided 

/**ENUMS**/

typedef enum {
    CALLOC_OP = 0,
    MALLOC_OP,
    FREE_OP,
} MEM_OP;

/*

The memory structure consists of a set of chunks with a size given by the preprocessor
constant CHUNK_SIZE and a smaller section acting as register where each bit represents
a chunk where its value (1/0) signifies if the chunk is allocated.

POOL :
________________________________________________________________
|           |           |           |               |  CHUNK    |
|   CHUNK1  |   CHUNK2  |    ...    |   CHUNK N     |  REGISTER |
|___________|___________|___________|_______________|___________|

^                                                   ^
|                                                   |
(poolNode) node. (void*)data                        (poolNode) node. (void*)chunkReg

________________________________________________________________________________________
________________________________________________________________________________________

CHUNK N :

    BYTE 0      BYTE1       ...         BYTE N
________________________________________________
|           |           |           |           |
|   SIZE    |   DATA    |           |   DATA    |
|___________|___________|___________|___________|

________________________________________________________________________________________
________________________________________________________________________________________

CHUNK REGISTER :

    BYTE 0                      BYTE 1                          BYTE N

0 | 1 | 2 | 3 4 5 6 7 8   0 1 2 3 4 5 6 7 8  ...  N+0 N+1 N+2 N+3 N+4 N+5 N+6 N+7 N+8

C | C | C |                                        C
H | H | H |                                        H
U | U | U |                                        U
N | N | N |                                        N
K | K | K |                                        K
1 | 2 | 3 |                                        N

*/

/** STRUCTURES **/

typedef struct Monitor
{
    uint16_t sizeAllocated;
    uint16_t sizeEffective;
    List trace;
}Monitor;

typedef struct traceNode
{
    char* FILE;
    uint16_t LINE;
    void* ptr_id;
    clock_t time;
    size_t size;
    MEM_OP op;
}traceNode;

typedef struct chunkNode
{
    void* ptr;
    size_t size;
}chunkNode;

typedef struct poolNode
{
    char id[50];
    uint16_t size;
    uint16_t reserved;
    
    uint8_t* data;
    uint8_t* chunkReg;

    List rsvdChunk;
    Monitor monHandler;
    FILE* file_trace_hr;
    FILE* file_trace;

}poolNode;

/** GLOBAL FUNCTIONS **/

/**
 * @brief Initializes Main memory pool with size
 * 
 * @param size amount of reserved memory for the main pool
 * @return void* 
 */
void* Memory_DefaultInit(size_t size);

/**
 * @brief Initializes a memory pool with an id and size
 * 
 * @param id pool identifier
 * @param size amount of reserved memory for the pool
 * @return void* 
 */
void* Memory_Init(char* id, size_t size, char* traceFileRoute);

/**
 * @brief Custom malloc call
 * 
 * @param size to be allocated
 * @param id pool identifier
 * @return void* pointer to start of allocated space
 */
void* Memory_CustomMalloc(size_t size, char* id);

/**
 * @brief Custom free call
 * 
 * @param ptr pointer to start address
 * @param id pool identifier
 */
void Memory_CustomFree(void* ptr, char* id);

/**
 * @brief Prints memory pool
 * 
 * @param id pool identifier
 */
void Memory_Print(char* id);

/**
 * @brief Wrapper for Memory_CustomMalloc function including extra-info
 * 
 * @param size to be allocated
 * @param id pool identifier
 * @param FILE where the call is made
 * @param LINE where the call is made
 * @return void* pointer to start of allocated space
 */
void* Memory_MonitorCustomMalloc(size_t size, char* id, char* FILE, uint16_t LINE);

/**
 * @brief Wrapper for Memory_CustomFree function including extra-information
 * 
 * @param ptr pointer to start address
 * @param id pool identifier
 * @param FILE where the call is made
 * @param LINE where the call is made
 */
void Memory_MonitorCustomFree(void* ptr, char* id, char* FILE, uint16_t LINE);

/**
 * @brief Prints memory pool with extra-info
 * 
 * @param id pool identifier
 */
void Memory_MonitorPrint(char *id);

#endif