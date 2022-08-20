#include "socket_test.h"

int process_test(){
    pack_node a;
    uint8_t found = FALSE;
    pack_node* aux = NULL;
    uint16_t time = clock() / 1000000;
    do{
        aux = sockReadQ(&a);
        if(aux != NULL){
            printf("USERss: %s %d\n",a.user, a.size);
            for(int i = 0; i < a.size; i++)
                printf("%.2hhX ", *((char*)a.data+i));
            printf("\n");
            if(strcmp (a.user, "NTP") == 0){
                found = TRUE;
                _NTPprocessPack(&a);
            }
            if(strcmp (a.user, "DNS") == 0){
                _DNSprocessPacket(&a);
            }
        }
        if(clock() / 1000000 - time >= 1){
            
            //memMonPrint(DEFAULT_POOL);
            printf("Resend Request");
            fflush(stdout);
            time = clock() / 1000000;
            NTPrequest();
            
        }
    }
    while (aux != NULL);
    return found;
}

void main(){
    
    memDefaultInit(10000);
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

    //memMonPrint(DEFAULT_POOL);

    sockInit();

    for(int i = 0; i < 10; i++, port++ ){
        aux.sin_port = htons(port);
        a[i].user = malloc(50);
        // sprintf(a[i].user,"Pato_Malo%d",port);
        // sprintf(buffer_id,"Pato_Malo%d",port);
        // sprintf(a[i].data,"Hola Server%d",port);
        // sockAdd(&aux,buffer_id,TCP);
        // sockWriteQ(&a[i]);
    }

    DNSinitServ();
    //memMonPrint(DEFAULT_POOL);

    //memMonPrint(DEFAULT_POOL);
	pthread_t thread_id;
    //NTPinitServ();
    //memMonPrint(DEFAULT_POOL);
	pthread_create(&thread_id, NULL, NTPinitServ, NULL);
    //memMonPrint(DEFAULT_POOL);
    while(process_test() != TRUE);

    //printf("IP MAIN: %s\n",get_IP(id_DNS));

    // printf("TIME: %u\n",NTPgetUNIXtime());

}

