#ifndef NTP_H
#define NTP_H

#include "socket.h"
#include "general_definitions.h"
#include "dns.h"

void* init_NTP_serv();

void request_NTP_time();

unsigned int get_UNIX_time();

void _process_NTP_pack(pack_node* NTP_pack);

#endif