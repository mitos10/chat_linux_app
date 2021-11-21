#include "sha256_test.h"

int main(){
    char* sha256_hash = sha256sum("hello world");
    printf("HASH : ");
    for(int i = 0; i < 32; i++){
        printf("%.2hhX",sha256_hash[i]);
    }
    printf("\n");
}