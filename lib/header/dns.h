
#ifndef DNS_H
#define DNS_H

#include <string.h>
#include "general_definitions.h"
#include "socket.h"
#include <stdlib.h>
#include <time.h>

//https://www.binarytides.com/dns-query-code-in-c-with-winsock/
//https://electronicspost.com/dns-messages/
//http://www.tcpipguide.com/free/t_DNSMessageHeaderandQuestionSectionFormat.htm
//https://courses.cs.duke.edu//fall16/compsci356/DNS/DNS-primer.pdf


/**
 * @brief Init DNS service
 * 
 */
void DNSinitServ();

/**
 * @brief DNS request
 * 
 * @param host_name 
 * @return uint16_t 
 */
uint16_t DNSrequest(uint8_t *host_name);

/**
 * @brief 
 * 
 * @param ID 
 * @return uint8_t* 
 */
uint8_t* DNSgetIP(uint16_t ID);

/**
 * @brief 
 * 
 * @param host_name 
 * @return uint8_t* 
 */
uint8_t* _DNSparseHn(uint8_t *host_name);

/**
 * @brief 
 * 
 * @param DNS_pack 
 */
void _DNSprocessPacket(pack_node *DNS_pack);

/**
 * @brief 
 * 
 * @param s_data 
 * @param data 
 * @return int16_t 
 */
static int16_t _cmp_id(void* s_data, void* data);

#endif