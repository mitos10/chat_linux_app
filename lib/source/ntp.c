#include "../header/ntp.h"

Time _time = {
    .UNIXtime = 0,
    .timeUpdated = FALSE,
};

void* NTPinitServ()
{
    uint16_t ID_DNS = DNSrequest("es.pool.ntp.org");
    uint8_t *IP_DNS = NULL;
    while( (IP_DNS = DNSgetIP(ID_DNS)) == NULL);
    
    struct sockaddr_in DNS_sock = { 
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = inet_addr(IP_DNS)},
        .sin_port = htons(123), 
    };
    //aux.sin_addr.s_addr = inet_addr("162.159.200.123");
    sockAdd(&DNS_sock, "NTP", UDP);

    NTPrequest();
}

void NTPrequest()
{
    pack_node ntp_pack = {
        .data = NULL,
        .user = memcpy(calloc(50, sizeof(uint8_t)), "\x3", sizeof("\x3")),
        .size = 48,
    };
    sprintf(ntp_pack.data = calloc(48,sizeof(uint8_t)), "NTP");

    sockWriteQ(&ntp_pack);
}

unsigned int NTPgetUNIXtime(){
    if(_time.timeUpdated != TRUE)
        return 0;
    else return _time.UNIXtime;
}

void _NTPprocessPack(pack_node* NTP_pack){
    _time.UNIXtime = ntohl( ( (uint32_t *)NTP_pack->data) [10] ) - 2208988800;
    _time.timeUpdated = TRUE;
}