#ifndef MEMORY_H
#define MEMORY_H

#include <math.h>
#include <time.h>
#include "general_definitions.h"
#include "list.h"

#define CHUNK_SIZE 32
#define CHUNK_SECTION_SIZE(size) (unsigned int) ceil( (double)size / (CHUNK_SIZE*8) )
#define METADATA_SIZE 2

typedef struct pool_node
{
    char id[50];
    void* data;
    void* pool_chunks;
    LIST monitor;
    unsigned int size;
    unsigned int reserved;
}pool_node;

typedef struct monitor_node
{
    char* FILE;
    int LINE;
    void* ptr_id;
    clock_t time;
}monitor_node;


void* init_memory(char* id, size_t size);

void* cmalloc(size_t size, char* id);

void cfree(void* ptr, char* id);

void print_mem(char* id);

void* monitor_cmalloc(size_t size, char* id, char* FILE, int LINE);

void monitor_cfree(void* ptr, char* id);

unsigned int total_mem(void);

void monitor_print(char *id);

#endif