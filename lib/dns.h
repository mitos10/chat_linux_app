
#ifndef DNS_H
#define DNS_H

#include <string.h>
#include "general_definitions.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>


typedef struct __attribute__((__packed__)){
    uint16_t ID;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
}dns_header;

void init_DNS_serv();
void dns_request(char *host_name);
char* _dns_parse_hn(char *host_name);
void _process_dns_packet(pack_node *DNS_pack);

#endif