#include "../../lib/header/history.h"
#include "../../lib/header/password_manager.h"

void main(){

    memDefaultInit(10000);
    
        password_nd test = {
        .id = 1,
        .type = 0,
        .password = "abcdefgh",
        .pass_sz = strlen("abcdefgh"),
    };

    savePassword(&test);
    showPasswords();

    history_node nd = {
        .file_path = "test path",
        .file_type = 1,
        .group = "gr",
        .msg = "test msg",
        .user = "test user",
    };


    addMessage(&nd);
    history_node nd2 = {
        .file_path = "test path 2",
        .file_type = 1,
        .group = "gr2",
        .msg = "test msg2",
        .user = "test user2",
    };
    addMessage(&nd2);

    writeHistoryToFile("test.txt", 1);
    clearHistory();
    readHistoryFromFile("test.txt", 1);
}