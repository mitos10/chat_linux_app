#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <arpa/inet.h>

enum {
    AES_128 = 4,
    AES_192 = 6,
    AES_256 = 8
};

enum { 
    EBC = 1,
    CBC = 2
};

size_t AES_Encrypt(char* pass, char* string, size_t size, char** str_out, int mode, char* IV);
size_t AES_Decrypt(char* pass, char* string, size_t size, char** str_out, int mode, char* IV);