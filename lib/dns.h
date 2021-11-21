
#ifndef DNS_H
#define DNS_H

#include <string.h>
#include "general_definitions.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>

void init_DNS_serv();
uint16_t DNS_request(char *host_name);
char* get_IP(uint16_t ID);
void _process_dns_packet(pack_node *DNS_pack);

#endif