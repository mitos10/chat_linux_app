#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H

#include "list.h"
#include "debug.h"

typedef struct password_nd{
    uint8_t id;         //Password id
    uint8_t type;       //Password type
    char* password;     //Password string
    uint8_t pass_sz;    //Password string's size
}password_nd;

/**
 * @brief Get the password node
 * 
 * @param id password id
 * @param id_type password type
 * @param password output string
 * @param pass_sz output string's size
 * @return password_nd* output data node
 */
password_nd* getPassword(uint8_t id, uint8_t id_type, char* password, uint8_t pass_sz);

/**
 * @brief Save password to list
 * 
 * @param nd data containing password's parameters
 */
void savePassword(password_nd* nd);

/**
 * @brief Print all passwords
 * 
 */
void showPasswords();

/**
 * @brief Local function to init password node
 * 
 * @param nd data containing password's parameters
 */
void _init_password(void* nd);

/**
 * @brief Local function to delete password node
 * 
 * @param nd data containing password's parameters
 */
void _delete_password(void* nd);

/**
 * @brief Local function to print password
 * 
 * @param f output file
 * @param nd data containing password's parameters
 */
void _print_password(FILE* f, void* nd);

/**
 * @brief Local function to search id inside node
 * 
 * @param s_data searched data
 * @param data node data
 * @return int16_t 
 */
int16_t _cmp_id(void* s_data, void* data);

/**
 * @brief Local function to search type inside node
 * 
 * @param s_data searched data
 * @param data node data
 * @return int16_t 
 */
int16_t _cmp_id_type(void* s_data, void* data);

#endif