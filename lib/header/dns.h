
#ifndef DNS_H
#define DNS_H

#include <string.h>
#include "debug.h"
#include "socket.h"
#include "list.h"
#include <stdlib.h>
#include <time.h>

#ifdef MEMORY
    #include "memory.h"
#endif

//https://www.binarytides.com/dns-query-code-in-c-with-winsock/
//https://electronicspost.com/dns-messages/
//http://www.tcpipguide.com/free/t_DNSMessageHeaderandQuestionSectionFormat.htm
//https://courses.cs.duke.edu//fall16/compsci356/DNS/DNS-primer.pdf


/**
 * @brief Init DNS service
 * 
 */
void DNS_initServ();

/**
 * @brief DNS_ request
 * 
 * @param host_name 
 * @return uint16_t 
 */
uint16_t DNS_request(uint8_t *host_name);

/**
 * @brief 
 * 
 * @param ID 
 * @return uint8_t* 
 */
uint8_t* DNS_getIP(uint16_t ID);

/**
 * @brief 
 * 
 * @param DNS_pack 
 */
void DNS_processPacket(pack_node_t *DNS_pack);

#endif