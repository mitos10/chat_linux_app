#include "socket.h"
#include <pthread.h>
#include <poll.h>

QUEUE (_send_queue);
QUEUE (_recv_queue); 

typedef struct dict_node{ 
	char *user;
	int fd;
	int num_packets;
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
 fd -> file descriptor
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
		#ifdef DEBUG
			printf("SOCKET_CREATION_FAIL\n");
		#endif
        return SOCKET_CREATION_FAIL;
    }

    if (connect(sockfd, (struct sockaddr*)sockaddr, sizeof(*sockaddr)) != 0){
		#ifdef DEBUG
			printf("SOCKET_CONNECTION_FAIL\n");
		#endif
        return SOCKET_CONNECTION_FAIL;
	}

    dict_node *dict_sock = (dict_node*) malloc(sizeof(dict_node));
    _fds[_nfds].fd = sockfd;
	_fds[_nfds].events = POLLIN | POLLOUT;
	_nfds++;
    dict_sock->fd = sockfd;
	#ifdef DEBUG
		printf("SOCK ID %d\n",sockfd);
	#endif
    dict_sock->user = (char*) malloc(strlen(user));
	dict_sock->num_packets = 0;
    strcpy(dict_sock->user, user);

    insert_node(&user_fd_dict, create_node(dict_sock), LAST_NODE);

    return SUCCESS;	
}

int write_queue(pack_node *data){
	dict_node* user;
	if(	( user = (dict_node*)find_node(&user_fd_dict, NULL,_cmp_username, data->user)->data ) != NULL ){
		#ifdef DEBUG
			printf("USER: %s\n",data->user);
		#endif
		user->num_packets++;
		insert_node(&_send_queue, create_node(data), LAST_NODE);
	}else printf("USER NOT FOUND: %s\n",data->user);

	#ifdef DEBUG
	printf("QUEUE PRINT:\n");
	print_list(stdout,_send_queue,_print_queue);
	fflush(stdout);
	#endif	
	
	return 1;
}

int read_queue(pack_node *data){
	data = remove_node(&_recv_queue, LAST_NODE)->data;
	return 1;
}

int package_is_sent(int id){
	return find_node(&_send_queue, NULL,_cmp_id, &id) == NULL;
}

void* _connection_thread(void * param){
	//epoll
	while(1){
		poll(_fds,_nfds,1000);
		while(_send_package() != 0);
		for(int i = 0; i < _nfds; i++){

			if(_fds[i].revents & POLLIN){
				_recv_package(_fds[i].fd);
			}
		}
	}
}

int _send_package(){

	int pos = 0;
	if(list_size(&_send_queue)){
		NODE *pn_aux = remove_node(&_send_queue, 0);
		char header[HEADER_SIZE], *payload, end_symbol[LAST_SYMBOL_SIZE];
		
		memcpy(header, &((pack_node*)pn_aux->data)->size, sizeof(unsigned int));
		payload  = ((pack_node*)pn_aux->data)->data;
		memset(end_symbol, '"', sizeof(unsigned int));

		char pack_buffer[((pack_node*)pn_aux->data)->size + 4 + 4];
		
		memcpy(pack_buffer, &(((pack_node*)pn_aux->data)->size), sizeof(unsigned int));
		memcpy(pack_buffer + sizeof(header), payload, ((pack_node*)pn_aux->data)->size);
		memcpy(pack_buffer + sizeof(header) +((pack_node*)pn_aux->data)->size, end_symbol, sizeof(end_symbol));
	
	#ifdef DEBUG
		printf("Packet: ");
		for(int i = 0; i < ((pack_node*)pn_aux->data)->size + 4 + 4; i++)
			printf("%c",pack_buffer[i]);
		printf("\n");
	#endif

		int user_fd = ((dict_node*) find_node(&user_fd_dict, NULL,_cmp_username, ((pack_node*)pn_aux->data)->user)->data)->fd;
		fflush(stdout);
		return send(user_fd, pack_buffer, sizeof(pack_buffer), 0);
	}
	return 0;
	//DELETE NODE AT LIST
}

int _recv_package(int fd){
	fflush(stdout);
	char buffer_recv[MAX_PACKAGE_SIZE];
	unsigned int size = recv(fd,buffer_recv,MAX_PACKAGE_SIZE,0);
	if(size > 0){
	char* user_name = ((dict_node*)find_node(&user_fd_dict, NULL, _cmp_fd, &fd)->data)->user;
	pack_node *pack_aux = (pack_node*) malloc(sizeof(pack_node));
	pack_aux->size = size;
	pack_aux->data = (void*) malloc(size);
	memcpy(pack_aux->data,buffer_recv,size);
	pack_aux->user = (char*) malloc(strlen(user_name));
	strcpy(pack_aux->user, user_name);

	insert_node(&_recv_queue, create_node(pack_aux), -1);
	#ifdef DEBUG
		printf("QUEUE PRINT:\n");
		print_list(stdout,_recv_queue,_print_queue);
	#endif
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
	pack_node *aux = (pack_node*) nd;
	fprintf(f, "User: %s Size: %d Message: %s \n",aux->user,aux->size, (char*)aux->data);
}
