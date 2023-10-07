#ifndef NTP_H
#define NTP_H

#include <time.h>
#include "socket.h"
#include "debug.h"
#include "dns.h"

typedef struct{
    uint32_t initUserTime;
    time_t UNIXtime;
    uint8_t isTimeUpdated;
    char dateStr[100];
}Time;

void* NTP_initServ();

void NTP_Request();

uint32_t NTP_getUNIXtime();

uint32_t NTP_getCurrentEpoch();

char* NTP_getCurrentDate();

uint8_t NTP_isTimeUpdated();

void NTP_proccessPack(pack_node_t* NTP_pack);

#endif