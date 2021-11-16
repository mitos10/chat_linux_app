#include "ntp.h"

//ADD thread to do everything
//ADD DNS server requests
//https://www.binarytides.com/dns-query-code-in-c-with-winsock/

static unsigned int UNIX_time = 0;

int init_NTP_serv()
{
    struct sockaddr_in aux;
    aux.sin_family = AF_INET;
    aux.sin_addr.s_addr = inet_addr("162.159.200.123");
    aux.sin_port = htons(123);
    add_socket(&aux, "NTP", UDP);
}

void request_NTP_time()
{
    pack_node* ntp_pack = (pack_node*)malloc(sizeof(pack_node));
    ntp_pack->user = malloc(50);
    sprintf(ntp_pack->user,"NTP");
    ntp_pack->data = (char*) calloc(48,sizeof(char));
    ((char*)ntp_pack->data)[0] = 0x03;
    ntp_pack->size = 48;
    write_queue(ntp_pack);
}

unsigned int get_UNIX_time(){
    return UNIX_time;
}

void _process_NTP_pack(pack_node* NTP_pack){
    UNIX_time = ( ( *((unsigned long *)NTP_pack->data) >> 32) & 0xFFFFFFFF ) - 2208988800;
}