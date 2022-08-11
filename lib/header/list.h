#ifndef LIST_H
#define LIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LAST_NODE -1
#define LAST_POS -1

struct node;

typedef struct node {
	void* data;
	struct node* next_ptr;
}NODE;

typedef NODE*  LIST;
typedef NODE** ROOT;

typedef struct list List;

struct list{
	//Data
	LIST root;
	size_t size;

	//Functions
	void (*deleteNode)(void*);
	void (*printNode)(FILE*, void*);
	void (*sortNode[32])(List* , uint16_t);
	int16_t (*cmpData[32])(void*, void*);

};


#define LIST(x) LIST x = NULL;

/**
 * @brief Insert element list
 * 
 * @param l list structure
 * @param data data pointer
 * @param size size of data
 * @param pos position to save
 */
void listInsert(List* l, void* const data, size_t const size, int16_t const pos);

/**
 * @brief Moves node in list from src position to dest position
 * 
 * @param l list structure
 * @param pos_dest 
 * @param pos_src 
 */
void listMove(List* l, uint16_t const pos_dest, uint16_t const pos_src);

/**
 * @brief Swaps two nodes position in list
 * 
 * @param l list structure
 * @param fst_pos 
 * @param snd_pos 
 */
void listSwap(List* l, uint16_t const fst_pos, uint16_t const snd_pos);

/**
 * @brief Get element from list
 * 
 * @param l list structure
 * @param pos position to get
 * @return void* data
 */
void* listGetData(List* l, int16_t const pos);

/**
 * @brief Delete element from list
 * 
 * @param l list structure
 * @param pos position to delete
 */
void listDelete(List* l, int16_t const pos);

/**
 * @brief Delete all elements fron list
 * 
 * @param l list structure
 */
void listDeleteAll(List* l);

/**
 * @brief Get size of list
 * 
 * @param l list structure
 * @return size_t size of list
 */
size_t listGetSize(List* l);

/**
 * @brief Print all elements from list
 * 
 * @param l list structure
 * @param f output file
 */
void listPrint(List* l, FILE* f);

/**
 * @brief Find element from list
 * 
 * @param l list structure
 * @param pos pointer to position where data found
 * @param s_data data to be searched
 * @return void* data
 */
void* listFind(List* l, uint16_t* pos, void* const s_data, uint16_t func);

/**
 * @brief Copies list from src to dest
 * 
 * @param dest pointer to dest list
 * @param src pointer to src list
 * @param size size of data structure
 */
void listCopy(List* dest, List* src, size_t const size);

/**
 * @brief Sort list
 * 
 * @param l pointer to list to sort
 * @param func_sort index of sorting function
 * @param func_cmp index of compare function
 */
void listSort(List* l, uint16_t func_sort, uint16_t func_cmp);

#endif