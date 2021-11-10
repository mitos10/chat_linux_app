#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "general_definitions.h"
#include "list.h"


#define QUEUE LIST
#define DICTIONARY LIST

#define QUEUE_MAX_SIZE 100
#define HEADER_SIZE 4
#define LAST_SYMBOL_SIZE 4
#define MAX_PACKAGE_SIZE 4 + 4096 + 4

enum { SOCKET_CREATION_FAIL = 0, SOCKET_CONNECTION_FAIL = 1, SUCCESS = 2} SOCKET_ADD_RESP;

typedef struct pack_node {
	unsigned int id : 30;
	unsigned int status : 2;
	char *user;
	void *data;
	unsigned int size;
} pack_node;

/*

PACKAGE FORMAT:

┌─────────────────────┬────────────────────────┬─────────────────────────┐
│				      │                        │						 │
│  HEADER (4 BYTES)   │  PAYLOAD (4096 BYTES)  │  END SYMBOL (4 BYTES)	 │
│				      │						   │  						 │
└─────────────────────┴────────────────────────┴─────────────────────────┘

*/

/*
-Func definition:
 Inits main socket and listening thread
-Params:
 None
-Return:
 None
*/
void init_sockets();

/*
-Func definition:
 Adds socket to dictionary entry
-Params:
 sockaddr -> contains ip and port
 user -> string with user name
-Return: 
 enum SOCKET_ADD_RESP
 
*/
int add_socket(struct sockaddr_in *sockaddr, char *user);

/*
-Func definition:
 Write to send queue
-Params:
 data -> pointer to send data (pack_node type)
-Return:
 id value of package
 -1 on error
*/
int write_queue(pack_node* data);

/*
-Func definition:
 Read to send queue
-Params:
 data -> pointer to send data (pack_node type)
-Return:
 Size read
 < = 0 on error
*/
int read_queue(pack_node* data);

/*
-Func definition:
 Search if package with id is sent
-Params:
 id -> package id searched
-Return:
 TRUE or FALSE
*/
int package_is_sent(int id);
