#include "socket.h"
#include "ntp.h"
#include "general_definitions.h"
#include "dns.h"

int process_test(){
    pack_node* a = NULL;
    unsigned int found = FALSE;
    do{
        a=read_queue();
        if(a != NULL){
            printf("USERss: %s %d\n",a->user, a->size);
            for(int i = 0; i < a->size; i++)
                printf("%.2hhX ", *((char*)a->data+i));
            printf("\n");
            if(strcmp (a->user, "NTP") == 0){
                found = TRUE;
                _process_NTP_pack(a);
            }
            if(strcmp (a->user, "DNS") == 0){
                _process_dns_packet(a);
            }
        }
    }
    while (a != NULL);
    return found;
}