#include "socket.h"
#include <pthread.h>
#include <poll.h>

QUEUE (_send_queue);
QUEUE (_recv_queue); 

typedef struct dict_node{ 
	char *user;
	int fd;
}dict_node;

DICTIONARY (user_fd_dict);

void* _connection_thread(void * param);

/*
-Func definition:
 Compare user node on dictionary user - file descriptor
-Params:
 s_data -> string with username
 data -> dictionary node
-Return: 
 !0 on TRUE or 0 on FALSE
 
*/
unsigned int _cmp_username(void* s_data, void* data);

/*
-Func definition:
 Compare pack id on queue
-Params:
 s_data -> ptr to id number
 data -> queue node
-Return: 
 !0 on TRUE or 0 on FALSE
 
*/
unsigned int _cmp_id(void* s_data, void* data);

/*
-Func definition:
 Compare user node on dictionary file descriptor - user
-Params:
 s_data -> pointer with file descriptor
 data -> dictionary node
-Return: 
 !0 on TRUE or 0 on FALSE
 
*/
unsigned int _cmp_fd(void* s_data, void* data);

/*
-Func definition:
 Internal function to send package from queue
-Params:
 None
-Return: 
 Size sent
 
*/
int _send_package();

/*
-Func definition:
 Internal function to receive package to queue
-Params:
 fd -> file descriptor
-Return: 
 Size received
 
*/
int _recv_package(int fd);

void _print_queue(FILE*f, void* nd);

static int _nfds;
static struct pollfd _fds[50];

void init_sockets(){
	pthread_t thread_id;
	_nfds = 0;
	pthread_create(&thread_id, NULL, _connection_thread, NULL);
}

int add_socket(struct sockaddr_in *sockaddr, char *user){
	int sockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
		printf("SOCKET_CREATION_FAIL\n");
        return SOCKET_CREATION_FAIL;
    }

    if (connect(sockfd, (struct sockaddr*)sockaddr, sizeof(*sockaddr)) != 0){
		printf("SOCKET_CONNECTION_FAIL\n");
        return SOCKET_CONNECTION_FAIL;
	}

    dict_node *dict_sock = (dict_node*) malloc(sizeof(dict_node));
    _fds[_nfds].fd = sockfd;
	_fds[_nfds].events = POLLIN;
	_nfds++;
    dict_sock->fd = sockfd;
    dict_sock->user = (char*) malloc(strlen(user));
    strcpy(dict_sock->user, user);

    insert_node(&user_fd_dict, create_node(dict_sock), LAST_NODE);

    return SUCCESS;	
}

int write_queue(pack_node *data){
	insert_node(&_send_queue, create_node(data), LAST_NODE);
	return 1;
}

int read_queue(pack_node *data){
	data = remove_node(&_recv_queue, LAST_NODE)->data;
	return 1;
}

int package_is_sent(int id){
	return find_node(&_send_queue, _cmp_id, &id) == NULL;
}

void* _connection_thread(void * param){
	//epoll
	printf("INSIDE");
	while(1){
		poll(_fds,_nfds,-1);
		for(int i = 0; i < _nfds; i++){
			if(_fds[i].revents & POLLIN){
				_recv_package(_fds[i].fd);
			} else printf("EMPTY PACKET?\n");

		}
	}
}

int _send_package(){
	NODE *pn_aux = remove_node(&_send_queue, 0);
	char header[HEADER_SIZE], *payload, end_symbol[LAST_SYMBOL_SIZE];
	memcpy(header, &((pack_node*)pn_aux->data)->size, sizeof(unsigned int));
	payload  = ((pack_node*)pn_aux->data)->data;
	memset(end_symbol, 0, sizeof(unsigned int));

	char pack_buffer[((pack_node*)pn_aux->data)->size + 4 + 4];
	
	memcpy(pack_buffer, header, sizeof(header));
	memcpy(pack_buffer + sizeof(header), payload, ((pack_node*)pn_aux->data)->size);
	memcpy(pack_buffer + sizeof(header) +((pack_node*)pn_aux->data)->size, end_symbol, sizeof(end_symbol));
	
	int user_fd = ((dict_node*) find_node(&user_fd_dict, _cmp_username, ((pack_node*)pn_aux->data)->user)->data)->fd;
	return send(user_fd, pack_buffer, sizeof(pack_buffer), 0);
	//DELETE NODE AT LIST
}

int _recv_package(int fd){
	fflush(stdout);
	char buffer_recv[MAX_PACKAGE_SIZE];
	unsigned int size = recv(fd,buffer_recv,MAX_PACKAGE_SIZE,0);
	if(size > 0){
	char* user_name = ((dict_node*)find_node(&user_fd_dict, _cmp_fd, &fd)->data)->user;
	pack_node *pack_aux = (pack_node*) malloc(sizeof(pack_node));
	pack_aux->size = size;
	pack_aux->data = (void*) malloc(size);
	memcpy(pack_aux->data,buffer_recv,size);
	pack_aux->user = (char*) malloc(strlen(user_name));
	strcpy(pack_aux->user, user_name);

	insert_node(&_recv_queue, create_node(pack_aux), -1);
	print_list(stdout,_recv_queue,_print_queue);
	}
	return size;
	//DELETE NODE AT LIST
}

unsigned int _cmp_username(void* s_data, void* data){
	dict_node *aux = (dict_node*) data;
	return !strcmp(aux->user,s_data);
}

unsigned int _cmp_fd(void* s_data, void* data){
	dict_node *aux = (dict_node*) data;
	return aux->fd == *(unsigned int*)s_data;
}

unsigned int _cmp_id(void* s_data, void* data){
	pack_node *aux = (pack_node*) data;
	return aux->id == *(unsigned int*)s_data;
}

void _print_queue(FILE*f, void* nd){
	printf("NODE");
	fflush(stdout);
	pack_node *aux = (pack_node*) nd;
	fprintf(f, "User: %s Size: %d Message: %s \n",aux->user,aux->size, (char*)aux->data);
}
