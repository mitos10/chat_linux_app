#include "memory_test.h"

int main(){

    memDefaultInit(10000);
    uint8_t* ptr = malloc(10);
    memMonPrint(DEFAULT_POOL);
    memPrint(DEFAULT_POOL);
    free(ptr);
    memMonPrint(DEFAULT_POOL);
    memPrint(DEFAULT_POOL);
    free(ptr);
    malloc(2000);

    //for(long int i = 0; i < 1000000000; i++);

    malloc(100);
    memMonPrint(DEFAULT_POOL);
    memPrint(DEFAULT_POOL);

}