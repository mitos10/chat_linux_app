#ifndef NTP_H
#define NTP_H

#include "socket.h"
#include "general_definitions.h"
#include "dns.h"

typedef struct{
    uint32_t UNIXtime;
    uint8_t timeUpdated;
}Time;

void* NTPinitServ();

void NTPrequest();

unsigned int NTPgetUNIXtime();

void _NTPprocessPack(pack_node* NTP_pack);

#endif