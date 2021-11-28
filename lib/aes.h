#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <arpa/inet.h>

#define AES_128 4
#define AES_192 6
#define AES_256 8

char* _round_keys(char* key, int mode);