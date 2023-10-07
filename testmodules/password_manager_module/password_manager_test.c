#include "../../lib/header/password_manager.h"

void main(){
    
    Memory_DefaultInit(10000);
    password_nd test = {
        .id = 0,
        .type = 0,
        .password = "abcdefgh",
        .pass_sz = strlen("abcdefgh") - 1,
    };

    Password_save(&test);
    
    test.password = "abcdefgh\x0" "\x20" "abcdefgh";
    test.pass_sz = 18;
    test.id = 1;
    Password_save(&test);

    Password_show();

    password_nd* test2_ptr = Password_get(0,-1,NULL,0);
    if(test2_ptr != NULL){
        for(int i = 0; i < test2_ptr->pass_sz; i++)
            printf("%c", test2_ptr->password[i]);
        printf("\n");
    }else printf("Password not found\n");

    test2_ptr = Password_get(1,-1,NULL,0);
    if(test2_ptr != NULL){
        for(int i = 0; i < test2_ptr->pass_sz; i++)
            printf("%c", test2_ptr->password[i]);
        printf("\n");
    }else printf("Password not found\n");

}