#include "sha256_test.h"

int main(int argc, char* argv[] ){
    
    //char* sha256_hash = SHA256_CheckSum("hello world");
    
    if (argc == 2){
        
        char* sha256_hash = SHA256_CheckSum( argv[1] );

        for(int i = 0; i < 32; i++){
            printf("%.2hhX",sha256_hash[i]);
        }
        printf("\n");
    }
    return 1;
}