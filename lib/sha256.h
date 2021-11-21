#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <netinet/in.h>

#include "general_definitions.h"

#define SHA_CHUNK_SIZE 64

typedef struct __attribute__((__packed__)){
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;
    uint32_t h;
}h_state;

char* sha256sum( char* pass);

#endif