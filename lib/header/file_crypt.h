#ifndef FILE_CRYPT_H
#define FILE_CRYPT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "aes.h"

struct file_crypt
{
    FILE* file; //file descriptor of the file to encrypt/ decrypt from
    char* pass; //password used to encrypt/decrypt the file
    char* buff; //plain text
    int size;   //size of the buff
    int iv_sz;  //size of the initial vector
};

/**
 * @brief Encrypts file
 * 
 * @param data data to be encrypted
 */
void File_Encrypt(struct file_crypt *data);

/**
 * @brief Decrypts file
 * 
 * @param data data to be decrypted
 */
void File_Decrypt(struct file_crypt *data);

#endif