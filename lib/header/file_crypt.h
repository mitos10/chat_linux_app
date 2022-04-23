#ifndef FILE_CRYPT_H
#define FILE_CRYPT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "aes.h"

struct file_crypt
{
    FILE* file;
    char* pass;
    char* buff;
    int size;
    int iv_sz;
};

char* generate_iv(int size);
void enc_file(struct file_crypt *data);
void dec_file(struct file_crypt *data);

#endif