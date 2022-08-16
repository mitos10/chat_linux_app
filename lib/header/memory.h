#ifndef MEMORY_H
#define MEMORY_H

/** INCLUDES **/

#include <math.h>
#include <time.h>
#include "general_definitions.h"
#include "list.h"

/** DEFINES **/

#define CHUNK_SIZE 32
#define CHUNK_ALLOC_REG_SZ(size) (uint16_t) ceil( (double)size / (CHUNK_SIZE*8) )
#define METADATA_SIZE 2
#define HIGHEST_BIT 0x80
#define REG_BITS 8

#define malloc(SIZE) memMonCmalloc(SIZE, "Main Memory", __FILE__, __LINE__)
#define calloc(SIZE, T_SIZE) memMonCmalloc(SIZE * T_SIZE, "Main Memory", __FILE__, __LINE__)
#define free(PTR) memMonCfree(PTR, "Main Memory")


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

typedef struct poolNode
{
    char id[50];
    uint16_t size;
    uint16_t reserved;
    
    uint8_t* data;
    uint8_t* chunkReg;

    List monitor;

}poolNode;

typedef struct monitorNode
{
    char* FILE;
    uint16_t LINE;
    void* ptr_id;
    clock_t time;
}monitorNode;

/** GLOBAL FUNCTIONS **/

/**
 * @brief Initializes a memory pool with an id and size
 * 
 * @param id pool identifier
 * @param size amount of reserved memory for the pool
 * @return void* 
 */
void* memInit(char* id, size_t size);

/**
 * @brief Custom malloc call
 * 
 * @param size to be allocated
 * @param id pool identifier
 * @return void* pointer to start of allocated space
 */
void* cmalloc(size_t size, char* id);

/**
 * @brief Custom free call
 * 
 * @param ptr pointer to start address
 * @param id pool identifier
 */
void cfree(void* ptr, char* id);

/**
 * @brief Prints memory pool
 * 
 * @param id pool identifier
 */
void memPrint(char* id);

/**
 * @brief Wrapper for cmalloc function including extra-info
 * 
 * @param size to be allocated
 * @param id pool identifier
 * @param FILE where the call is made
 * @param LINE where the call is made
 * @return void* pointer to start of allocated space
 */
void* memMonCmalloc(size_t size, char* id, char* FILE, int LINE);

/**
 * @brief Wrapper for cfree function including extra-information
 * 
 * @param ptr pointer to start address
 * @param id pool identifier
 */
void memMonCfree(void* ptr, char* id);

/**
 * @brief Prints memory pool with extra-info
 * 
 * @param id pool identifier
 */
void memMonPrint(char *id);

/** LOCAL FUNCTIONS **/

int16_t _cmp_id(void* s_data, void* data);
int16_t _cmp_id_ptr(void* s_data, void* data);
void _print_node(FILE* f, void* nd);
void _delete_node(void* data);

#endif