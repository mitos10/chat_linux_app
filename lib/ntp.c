#include "ntp.h"

//ADD thread to do everything
//ADD DNS server requests
//https://www.binarytides.com/dns-query-code-in-c-with-winsock/

static unsigned int _UNIX_time = 0;
static unsigned int _updated = FALSE;

int init_NTP_serv()
{
    _UNIX_time = 0;
    _updated = FALSE;
    struct sockaddr_in aux;
    aux.sin_family = AF_INET;
    aux.sin_addr.s_addr = inet_addr("162.159.200.123");
    aux.sin_port = htons(123);
    add_socket(&aux, "NTP", UDP);
}

void request_NTP_time()
{
    _updated = FALSE;
    pack_node* ntp_pack = (pack_node*)malloc(sizeof(pack_node));
    ntp_pack->user = malloc(50);
    sprintf(ntp_pack->user,"NTP");
    ntp_pack->data = (char*) calloc(48,sizeof(char));
    ((char*)ntp_pack->data)[0] = 0x03;
    ntp_pack->size = 48;
    write_queue(ntp_pack);
}

unsigned int get_UNIX_time(){
    if(_updated != TRUE)
        return 0;
    else return _UNIX_time;
}

void _process_NTP_pack(pack_node* NTP_pack){
    _UNIX_time = ntohl( ( (unsigned int *)NTP_pack->data) [10] ) - 2208988800;
    _updated = TRUE;
}