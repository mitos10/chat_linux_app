#ifndef SOCKET_H
#define SOCKET_H

/** INCLUDES **/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include <poll.h>
#include "debug.h"
#include "list.h"

/** DEFINES **/

#define QUEUE LIST
#define DICTIONARY LIST

#define QUEUE_MAX_SIZE 100
#define HEADER_SIZE 4
#define LAST_SYMBOL_SIZE 4
#define PAYLOAD_SIZE 4096
#define MAX_PACKAGE_SIZE (HEADER_SIZE + PAYLOAD_SIZE + LAST_SYMBOL_SIZE)

#define MAX_SOCKETS_OPEN 50

/** MACROS **/

/** ENUMERATIONS **/

typedef enum { SOCKET_CREATION_FAIL = 0, SOCKET_CONNECTION_FAIL = 1, SUCCESS = 2} SOCKET_ADD_RESP;
typedef enum { TCP = 0, UDP = 1} SOCKET_TYPE;

/** STRUCTS **/

typedef struct pack_node_t {
	uint32_t id : 30;
	uint32_t status : 2;
	char *user;
	void *data;
	uint32_t size;
}pack_node_t;

typedef struct dict_node_t{ 
	char *user;
	int32_t fd;
	SOCKET_TYPE type;
}dict_node_t;

typedef struct {
	struct pollfd sockFD[MAX_SOCKETS_OPEN];
	int32_t sockFDind;
	List sndQ;
	List rcvQ;
	List userFDdict;
}UserSockHandler_t;

/*

PACKAGE FORMAT:

┌─────────────────────┬────────────────────────┬─────────────────────────┐
│				      │                        │						 │
│  HEADER (4 BYTES)   │  PAYLOAD (4096 BYTES)  │  END SYMBOL (4 BYTES)	 │
│				      │						   │  						 │
└─────────────────────┴────────────────────────┴─────────────────────────┘

*/

/** GLOBAL FUNCTIONS **/

/**
 * @brief Inits main socket and listening thread
 * 
 */
void Socket_Init();

/**
 * @brief Adds socket to dictionary entry
 * 
 * @param sockaddr contains ip and port
 * @param user string with user name
 * @param type 
 * @return int enum SOCKET_ADD_RESP
 */
SOCKET_ADD_RESP Socket_Add(struct sockaddr_in *sockaddr, char *user, SOCKET_TYPE type);

/**
 * @brief Write to send queue
 * 
 * @param data pointer to send data (pack_node_t type)
 * @return int id value of package, -1 on error
 */

int32_t Socket_WriteQueue(pack_node_t* data);

/**
 * @brief Read to send queue
 * 
 * @return pack_node_t*
 */

pack_node_t* Socket_ReadQueue(pack_node_t *data);

/**
 * @brief Search if package with id is sent
 * 
 * @param id package id searched
 * @return int TRUE/FALSE
 */

uint8_t Socket_IsPackageSent(int32_t id);

/** END GLOBAL FUNCTIONS **/

#endif