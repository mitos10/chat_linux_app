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
password_nd* Password_get(uint8_t id, uint8_t id_type, char* password, uint8_t pass_sz);

/**
 * @brief Save password to list
 * 
 * @param nd data containing password's parameters
 */
void Password_save(password_nd* nd);

/**
 * @brief Print all passwords
 * 
 */
void Password_show();

#endif