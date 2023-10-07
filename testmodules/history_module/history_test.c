#include "../../lib/header/history.h"
#include "../../lib/header/password_manager.h"

void main(){

    Memory_DefaultInit(10000);
    
        password_nd test = {
        .id = 1,
        .type = 0,
        .password = "abcdefgh",
        .pass_sz = strlen("abcdefgh"),
    };

    Password_save(&test);
    Password_show();

    history_node nd = {
        .file_path = "test path",
        .file_type = 1,
        .group = strcpy((char*)malloc(sizeof("gr")),"gr"),
        .msg = strcpy((char*)malloc(sizeof("test msg")),"test msg"),
        .user = strcpy((char*)malloc(sizeof("test user")),"test user"),
    };

    printf("Add 1st message\n");
    History_AddMessage(&nd);
    history_node nd2 = {
        .file_path = "test path 2",
        .file_type = 1,
        .group = strcpy((char*)malloc(sizeof("gr2")),"gr2"),
        .msg = strcpy((char*)malloc(sizeof("test msg2")),"test msg2"),
        .user = strcpy((char*)malloc(sizeof("test user2")),"test user2"),
    };

    printf("Add 2nd message\n");
    History_AddMessage(&nd2);
    printf("Write history to file\n");
    History_WriteAllToFile("test.txt", 1);
    History_ClearAll();
    History_ReadAllFromFile("test.txt", 1);
}