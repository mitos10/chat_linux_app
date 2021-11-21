#include "socket_test.h"

void main(){
    
    struct sockaddr_in aux;
    unsigned short port = 8888;
    aux.sin_family = AF_INET;
    aux.sin_addr.s_addr = inet_addr("127.0.0.1");
    char buffer_id[50];
    pack_node a[4];
    a[0].data = (char*)malloc(50);
    a[0].size = 50;
    a[1].data = (char*)malloc(50);
    a[1].size = 50;
    a[2].data = (char*)malloc(50);
    a[2].size = 50;

    init_sockets();
    for(int i = 0; i < 3; i++, port++ ){
        aux.sin_port = htons(port);
        a[i].user = malloc(50);
        sprintf(a[i].user,"Pato_Malo%d",port);
        sprintf(buffer_id,"Pato_Malo%d",port);
        sprintf(a[i].data,"Hola Server%d",port);
        add_socket(&aux,buffer_id,TCP);
        write_queue(&a[i]);
    }

    init_DNS_serv();
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, init_NTP_serv, NULL);
    while(process_test() != TRUE);

    //printf("IP MAIN: %s\n",get_IP(id_DNS));

    printf("TIME: %u\n",get_UNIX_time());

}

