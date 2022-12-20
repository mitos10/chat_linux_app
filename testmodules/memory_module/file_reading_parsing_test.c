#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
//#include "../../lib/header/memory.h"

int main(int argc, char* argv[]){

    FILE* fp = fopen("trace_default_mem.txt", "w+");
    for(uint64_t i = 0; i < 20000; i++)
        fputc(1, fp);
    fseek(fp,0L,SEEK_SET);
    clock_t end = 0;
    clock_t init = clock();
    fseek(fp,0L,SEEK_END);
    end = clock();
    uint32_t size = ftell(fp);
    fseek(fp,0L,SEEK_SET);
    uint32_t diff = end - init;
    printf("Size: %u\n", size);
    printf("Time for opening file and calculating size: %u(uS)%ld %ld\n", diff, init, end);

    uint32_t acc = 0;
    init = clock();
    for(int i = 0; i < size; i++)
        acc += fgetc(fp);
    end = clock();
    diff = end - init;
    printf("Time using fgetc for each character %u(uS) %ld %ld %u\n",diff, init, end, acc);

    rewind(fp);
    uint8_t *ptr= malloc(size);
    acc = 0;
    init = clock();
    fread(ptr, size, 1, fp);
    for(int i = 0; i < size; i++)
        acc+= ptr[i];
    end = clock();
    diff = end - init;
    printf("Time using fread for all characters %u(uS)%ld %ld %u\n",diff, init, end, acc);

    

}