#include "../../lib/header/history.h"

void main(){
    
    history_node nd = {
        .file_path = "test path",
        .file_type = 1,
        .group = "gr",
        .msg = "test msg",
        .user = "test user",
    };

    memDefaultInit(10000);

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