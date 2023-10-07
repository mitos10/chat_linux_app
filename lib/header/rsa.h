#ifndef RSA_H
#define RSA_H

#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include "debug.h"

typedef struct {
    uint8_t* num;
    uint16_t size;
} long_num;

long_num _generateLongNum(uint16_t size);
void _sumLongNum(long_num op1, long_num op2, long_num* result);
long_num _multLongNum(long_num op1, long_num op2);
void _printLongNum(long_num num);
//unsigned long long gcd(unsigned long long a, unsigned long long b);
//unsigned long long pow_mod(unsigned long long base, unsigned long long exp, unsigned long long mod);

#endif