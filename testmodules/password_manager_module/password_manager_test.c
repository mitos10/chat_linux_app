#include "../../lib/header/password_manager.h"

void main(){
    
    memDefaultInit(10000);
    password_nd test = {
        .id = 1,
        .type = 0,
        .password = "abcdefgh",
        .pass_sz = strlen("abcdefgh") - 1,
    };

    savePassword(&test);
    
    test.password = "abcdefgh\x0" "\x20" "abcdefgh";
    test.pass_sz = 18;
    test.id = 1;
    savePassword(&test);

    showPasswords();

    password_nd* test2_ptr = getPassword(0,-1,NULL,0);
    if(test2_ptr != NULL){
        for(int i = 0; i < test2_ptr->pass_sz; i++)
            printf("%c", test2_ptr->password[i]);
        printf("\n");
    }else printf("Password not found\n");

    test2_ptr = getPassword(1,-1,NULL,0);
    if(test2_ptr != NULL){
        for(int i = 0; i < test2_ptr->pass_sz; i++)
            printf("%c", test2_ptr->password[i]);
        printf("\n");
    }else printf("Password not found\n");

}