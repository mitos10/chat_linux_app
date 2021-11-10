#include "socket_test.h"

void main(){
    struct sockaddr_in aux;
    unsigned short port = 8888;
    aux.sin_family = AF_INET;
    aux.sin_addr.s_addr = inet_addr("127.0.0.1");
    char buffer_id[50];
    init_sockets();
    for(int i = 0; i < 3; i++, port++ ){
        aux.sin_port = htons(port);
        sprintf(buffer_id,"Pato_Malo%d",port);
        add_socket(&aux,buffer_id);
    }
    while(1);
}