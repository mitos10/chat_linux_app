#include "../header/socket.h"
#include <pthread.h>

enum { CMP_USERNAME = 0, CMP_FD, CMP_ID = 0};

UserSockHandler _UsrSck_h = {
	.sockFD = {0},
	.sockFDind = 0,
	.sndQ = {
		.root = NULL,
		.size = 0,
		.cmpData = {
			[CMP_ID] = _cmp_id,
		},
		.deleteNode = _delete_queue,
		.printNode = _print_queue,
	},
	.rcvQ = {
		.root = NULL,
		.size = 0,
		.cmpData = {
			[CMP_ID] = _cmp_id,
		},
		.deleteNode = _delete_queue,
		.printNode = _print_queue,
	},
	.userFDdict = {
		.root = NULL,
		.size = 0,
		.cmpData = {
			[CMP_USERNAME] = _cmp_username,
			[CMP_FD] = _cmp_fd,
		},
		.printNode = NULL,
	},
};

void sockInit(){
	pthread_t thread_id;
	_UsrSck_h.sockFDind = 0;
	pthread_create(&thread_id, NULL, _connectionThread, NULL);
}

SOCKET_ADD_RESP sockAdd(struct sockaddr_in *sockaddr, char *user, SOCKET_TYPE type){
	int32_t sockfd;
	sockfd = socket(AF_INET, type == TCP? SOCK_STREAM : SOCK_DGRAM, 0);

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
	
	_UsrSck_h.sockFD[_UsrSck_h.sockFDind].fd = sockfd;
	_UsrSck_h.sockFD[_UsrSck_h.sockFDind].events = POLLIN | POLLOUT;
	_UsrSck_h.sockFDind++;

    dict_sock->fd = sockfd;
	#ifdef DEBUG
		printf("SOCK ID %d\n",sockfd);
	#endif
    dict_sock->user = (char*) malloc(strlen(user));
	dict_sock->type = type;
    strcpy(dict_sock->user, user);

    insert_node(&(_UsrSck_h.userFDdict), create_node(dict_sock), LAST_NODE);

    return SUCCESS;	
}

int32_t sockWriteQ(pack_node *data){
	dict_node* user = listFind(&_UsrSck_h.userFDdict, NULL, data->user, CMP_USERNAME);
	if( NULL != user ){
		#ifdef DEBUG
			printf("USER: %s\n",data->user);
		#endif
		listInsert(&_UsrSck_h.sndQ, data, sizeof(data), LAST_NODE);
	}else printf("USER NOT FOUND: %s\n",data->user);

	#ifdef DEBUG
	printf("QUEUE PRINT OUT:\n");
	listPrint(&_UsrSck_h.sndQ, stdout);
	#endif	
	
	return 1;
}	

void sockReadQ(pack_node *data){
	*data = *(pack_node*)listGetData(&_UsrSck_h.rcvQ, LAST_NODE);
	listDelete(&_UsrSck_h.rcvQ, LAST_NODE);
}

uint8_t sockIsPckgSent(int32_t id){
	return NULL == listFind(&_UsrSck_h.sndQ, NULL, &id, CMP_ID);
}

static void* _connectionThread(void * param){
	//epoll
	while(1){
		poll(_UsrSck_h.sockFD,_UsrSck_h.sockFDind,1000);
		while(_sendPackage() != 0);
		for(int i = 0; i < _UsrSck_h.sockFDind; i++){
			if(_UsrSck_h.sockFD[i].revents & POLLIN)
				_recvPackage(_UsrSck_h.sockFD[i].fd);
		}
	}
}

static int32_t _sendPackage(){

	int pos = 0;
	if(listGetSize(&_UsrSck_h.sndQ)){
		
		pack_node fst_nd_q = *(pack_node*)listGetData(&_UsrSck_h.sndQ, 0);
		listDelete(&_UsrSck_h.sndQ, 0);

		dict_node*  user_dict = listFind(&_UsrSck_h.userFDdict, NULL, fst_nd_q.user, CMP_USERNAME);

		if(user_dict->type == TCP){
			uint8_t pack_buffer[HEADER_SIZE + fst_nd_q.size + LAST_SYMBOL_SIZE];
			uint8_t end_symbol[LAST_SYMBOL_SIZE] = {0};
			
			memcpy(pack_buffer, &fst_nd_q.size, sizeof(uint32_t)); 								//Header
			memcpy(pack_buffer + HEADER_SIZE, fst_nd_q.data, fst_nd_q.size); 					//Payload
			memcpy(pack_buffer + HEADER_SIZE + fst_nd_q.size, end_symbol, sizeof(end_symbol)); 	//End symbol
		
			#ifdef DEBUG
				printf("Packet: ");
				for(int i = 0; i < fst_nd_q.size + 4 + 4; i++)
					printf("%c",pack_buffer[i]);
				printf("\n");
			#endif

			return send(user_dict->fd, pack_buffer, sizeof(pack_buffer), 0);
		} 	else return send(user_dict->fd, fst_nd_q.data, fst_nd_q.size, 0);
	}
	return 0;
}

static int32_t _recvPackage(int32_t fd){
	char buffer_recv[MAX_PACKAGE_SIZE];
	unsigned int size = recv(fd,buffer_recv,MAX_PACKAGE_SIZE,0);
	if(size > 0){
		char* user_name = ((dict_node*)listFind(&_UsrSck_h.userFDdict,NULL, &fd, CMP_FD))->user ;
		pack_node pack_aux = {
			.data = malloc(size),
			.size = size,
			.user = malloc(strlen(user_name)),
		};
		memcpy(pack_aux.data,buffer_recv,size);
		strcpy(pack_aux.user, user_name);

		listInsert(&_UsrSck_h.rcvQ, &pack_aux, sizeof(pack_node),LAST_POS);

		#ifdef DEBUG
			printf("QUEUE PRINT IN:\n");
			listPrint(&_UsrSck_h.rcvQ, stdout);
		#endif
	}
	return size;
}

static void _delete_queue(void* data){
	free(data);
}

static uint8_t _cmp_username(void* s_data, void* data){
	dict_node *aux = (dict_node*) data;
	return !strcmp(aux->user,s_data);
}

static uint8_t _cmp_fd(void* s_data, void* data){
	dict_node *aux = (dict_node*) data;
	return aux->fd == *(uint32_t*)s_data;
}

static uint8_t _cmp_id(void* s_data, void* data){
	pack_node *aux = (pack_node*) data;
	return aux->id == *(uint32_t*)s_data;
}

static void _print_queue(FILE*f, void* nd){
	pack_node *aux = (pack_node*) nd;
	fprintf(f, "User: %s Size: %d Message: ",aux->user,aux->size);
	for(int i = 0; i < aux->size; i++)
		fprintf(f,"%.2hhX",((char*)aux->data)[i]);
	fprintf(f,"\n");
}
