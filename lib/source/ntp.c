#include "../header/ntp.h"

Time ntp_time = {
    .initUserTime = 0,
    .UNIXtime = 0,
    .isTimeUpdated = FALSE,
};

char* const NTP_domains[] = {
                        "es.pool.ntp.org",
                        "hora.rediris.es ",
                        "hora.roa.es",
                        "ntp.i2t.ehu.es",
};

void* NTP_initServ()
{
    uint16_t ID_DNS = DNS_request(NTP_domains[0]);
    uint8_t *IP_DNS = NULL;
    while( (IP_DNS = DNS_getIP(ID_DNS)) == NULL);
    
    struct sockaddr_in DNS_sock = { 
        .sin_family = AF_INET,
        .sin_addr = { .s_addr = inet_addr(IP_DNS)},
        .sin_port = htons(123), 
    };
    Socket_Add(&DNS_sock, "NTP_", UDP);

    NTP_Request();
}

void NTP_Request()
{
    pack_node_t ntp_pack = {
        .data = memcpy(calloc(50, sizeof(uint8_t)), "\x3\x0", sizeof("\x3\x0")),
        .user = strcpy(calloc(48, sizeof(uint8_t)), "NTP_"),
        .size = 48,
    };
    
    __ASSERT__(ntp_pack.data != NULL, __ASSERT_NULL__)
    __ASSERT__(ntp_pack.user != NULL, __ASSERT_NULL__)

    Socket_WriteQueue(&ntp_pack);
}

uint8_t NTP_isTimeUpdated(){
    return ntp_time.isTimeUpdated;
}

uint32_t NTP_getCurrentEpoch(){
    if(ntp_time.isTimeUpdated)
        return ntp_time.UNIXtime + (clock()/1000000) - ntp_time.initUserTime;
    return 0;
}

char* NTP_getCurrentDate(){
    time_t aux = ntp_time.UNIXtime + (clock()/1000000) - ntp_time.initUserTime;
    strftime(ntp_time.dateStr, sizeof(ntp_time.dateStr), "%c", localtime(&aux));
    return ntp_time.dateStr;
}

uint32_t NTP_getUNIXtime(){
    if(ntp_time.isTimeUpdated != TRUE)
        return 0;
    else return ntp_time.UNIXtime;
}

void NTP_proccessPack(pack_node_t* NTP_pack){

    //Argument sanity check
    __ASSERT__(NTP_pack != NULL, __ASSERT_NULL__)

    ntp_time.initUserTime = clock()/1000000;
    ntp_time.UNIXtime = (time_t) ntohl( ( (uint32_t *)NTP_pack->data) [10] ) - 2208988800;
    ntp_time.isTimeUpdated = TRUE;
    strftime(ntp_time.dateStr, sizeof(ntp_time.dateStr), "%c", localtime(&ntp_time.UNIXtime));
    printf("%s\n", ntp_time.dateStr);
}