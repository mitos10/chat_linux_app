#ifndef HISTORY_H
#define HISTORY_H

#include "list.h"
#include "debug.h"
#include "file_crypt.h"

typedef struct history_node {
    char* msg;
    char* user;
    char* group;
    uint8_t file_type;
    char* file_path;
}history_node;

enum {
    MESSAGE_FILE_TYPE = 0,
    IMG_FILE_TYPE,
    VIDEO_FILE_TYPE,
};

/**
 * @brief Add mesage to history list
 * 
 * @param hist_nd 
 * @return uint8_t error code
 */
uint8_t addMessage(history_node* hist_nd);

/**
 * @brief Write history list to file
 * 
 * @param file_path of the history file
 * @param isEncripted uses aes encryption or plain text
 * @return uint8_t error code
 */
uint8_t writeHistoryToFile(char* file_path, uint8_t isEncripted);

/**
 * @brief Read history from file to history list
 * 
 * @param file_path of the history file
 * @param isEncripted uses aes encryption or plain text
 */
void readHistoryFromFile(char* file_path, uint8_t isEncripted);

/**
 * @brief clears history list
 * 
 */
void clearHistory();

/**
 * @brief Get line from strin passed as argument
 * 
 * @param line string extracted
 * @param size_line size of the string extracted
 * @param data_buf input string
 * @param out_buf address after line
 * @param size_buf size of input string after line
 * @return int 
 */
static int _get_line_string(char** line, uint32_t* size_line, char* data_buf, char** out_buf, uint32_t* size_buf);

/**
 * @brief Delete history list
 * 
 * @param nd node from history list to delete
 */
static void _delete_history(void* nd);

/**
 * @brief Prints node from history list
 * 
 * @param f file descriptor to print
 * @param nd node from history list to print
 */
static void _print_history(FILE*f, void* nd);

/**
 * @brief Inits history node
 * 
 * @param nd node to init before including into list
 */
static void _init_history(void* nd);

#endif