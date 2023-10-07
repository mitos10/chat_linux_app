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
uint8_t History_AddMessage(history_node* hist_nd);

/**
 * @brief Write history list to file
 * 
 * @param file_path of the history file
 * @param isEncripted uses aes encryption or plain text
 * @return uint8_t error code
 */
uint8_t History_WriteAllToFile(char* file_path, uint8_t isEncripted);

/**
 * @brief Read history from file to history list
 * 
 * @param file_path of the history file
 * @param isEncripted uses aes encryption or plain text
 */
void History_ReadAllFromFile(char* file_path, uint8_t isEncripted);

/**
 * @brief clears history list
 * 
 */
void History_ClearAll();

#endif