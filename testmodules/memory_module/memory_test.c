#include "memory_test.h"

int main(){

    Memory_DefaultInit(10000);
    uint8_t* ptr = malloc(10);
    Memory_MonitorPrint(DEFAULT_POOL);
    Memory_Print(DEFAULT_POOL);
    free(ptr);
    Memory_MonitorPrint(DEFAULT_POOL);
    Memory_Print(DEFAULT_POOL);
    free(ptr);
    malloc(2000);

    //for(long int i = 0; i < 1000000000; i++);

    malloc(100);
    Memory_MonitorPrint(DEFAULT_POOL);
    Memory_Print(DEFAULT_POOL);

}