#include "../header/ntp.h"

Time _time = {
    .userTime = 0,
    .UNIXtime = 0,
    .isTimeUpdated = FALSE,
};

char* const _NTPdomains[] = {
                        "es.pool.ntp.org",
                        "hora.rediris.es ",
                        "hora.roa.es",
                        "ntp.i2t.ehu.es",
};

void* NTPinitServ()
{
    uint16_t ID_DNS = DNSrequest(_NTPdomains[0]);
    uint8_t *IP_DNS = NULL;
    while( (IP_DNS = DNSgetIP(ID_DNS)) == NULL);
    
    struct sockaddr_in DNS_sock = { 
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = inet_addr(IP_DNS)},
        .sin_port = htons(123), 
    };
    sockAdd(&DNS_sock, "NTP", UDP);

    NTPrequest();
}

void NTPrequest()
{
    pack_node ntp_pack = {
        .data = memcpy(calloc(50, sizeof(uint8_t)), "\x3\x0", sizeof("\x3\x0")),
        .user = strcpy(calloc(48, sizeof(uint8_t)), "NTP"),
        .size = 48,
    };
    sockWriteQ(&ntp_pack);
}

uint8_t NTPisTimeUpdated(){
    return _time.isTimeUpdated;
}

uint32_t NTPgetUNIXtime(){
    if(_time.isTimeUpdated != TRUE)
        return 0;
    else return _time.UNIXtime;
}

void _NTPprocessPack(pack_node* NTP_pack){
    _time.userTime = clock()/1000000;
    _time.UNIXtime = ntohl( ( (uint32_t *)NTP_pack->data) [10] ) - 2208988800;
    _time.isTimeUpdated = TRUE;
}