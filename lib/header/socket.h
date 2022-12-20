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

typedef struct pack_node {
	uint32_t id : 30;
	uint32_t status : 2;
	char *user;
	void *data;
	uint32_t size;
} pack_node;

typedef struct dict_node{ 
	char *user;
	int32_t fd;
	SOCKET_TYPE type;
}dict_node;

typedef struct {
	struct pollfd sockFD[MAX_SOCKETS_OPEN];
	int32_t sockFDind;
	List sndQ;
	List rcvQ;
	List userFDdict;
}UserSockHandler;

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
void sockInit();

/**
 * @brief Adds socket to dictionary entry
 * 
 * @param sockaddr contains ip and port
 * @param user string with user name
 * @param type 
 * @return int enum SOCKET_ADD_RESP
 */
SOCKET_ADD_RESP sockAdd(struct sockaddr_in *sockaddr, char *user, SOCKET_TYPE type);

/**
 * @brief Write to send queue
 * 
 * @param data pointer to send data (pack_node type)
 * @return int id value of package, -1 on error
 */

int32_t sockWriteQ(pack_node* data);

/**
 * @brief Read to send queue
 * 
 * @return pack_node*
 */

pack_node* sockReadQ(pack_node *data);

/**
 * @brief Search if package with id is sent
 * 
 * @param id package id searched
 * @return int TRUE/FALSE
 */

uint8_t sockIsPckgSent(int32_t id);

/** END GLOBAL FUNCTIONS **/

///////////////////////////////////////////////////////////////////////////

/** LOCAL FUNCTIONS **/

static void* _connectionThread(void * param);

/**
 * @brief Compare user node on dictionary user - file descriptor
 * 
 * @param s_data string with username
 * @param data dictionary node
 * @return unsigned TRUE/FALSE
 */

static int16_t _cmp_username(void* s_data, void* data);

/**
 * @brief Compare pack id on queue
 * 
 * @param s_data ptr to id number
 * @param data queue node
 * @return unsigned TRUE/FALSE
 */

static int16_t _cmp_id(void* s_data, void* data);

/**
 * @brief Compare user node on dictionary file descriptor - user
 * 
 * @param s_data pointer with file descriptor
 * @param data dictionary node
 * @return unsigned TRUE/FALSE
 */

static int16_t _cmp_fd(void* s_data, void* data);

/**
 * @brief Internal function to send package from queue
 * 
 * @return int 
 */

static int32_t _sendPackage();

/**
 * @brief Internal function to receive package to queue
 * 
 * @param fd file descriptor
 * @return int size received
 */

static int32_t _recvPackage(int32_t fd);

/**
 * @brief Print queue to file
 * 
 * @param f File pointer
 * @param nd data
 */

static void _print_queue(FILE*f, void* nd);

static void _delete_queue(void* data);

static void _print_dict(FILE*f, void* nd);

/** END LOCAl FUNCTIONS **/

#endif