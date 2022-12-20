#ifndef NTP_H
#define NTP_H

#include "socket.h"
#include "debug.h"
#include "dns.h"

typedef struct{
    uint32_t userTime;
    uint32_t UNIXtime;
    uint8_t isTimeUpdated;
}Time;

void* NTPinitServ();

void NTPrequest();

uint32_t NTPgetUNIXtime();

uint8_t NTPisTimeUpdated();

void _NTPprocessPack(pack_node* NTP_pack);

#endif