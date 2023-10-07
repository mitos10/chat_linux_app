#include "../header/socket.h"
#include <pthread.h>

/** LOCAL FUNCTIONS **/

static void* connectionThread(void * param);

/**
 * @brief Compare user node on dictionary user - file descriptor
 * 
 * @param s_data string with username
 * @param data dictionary node
 * @return unsigned TRUE/FALSE
 */

static int16_t cmp_username(void* s_data, void* data);

/**
 * @brief Compare pack id on queue
 * 
 * @param s_data ptr to id number
 * @param data queue node
 * @return unsigned TRUE/FALSE
 */

static int16_t cmp_id(void* s_data, void* data);

/**
 * @brief Compare user node on dictionary file descriptor - user
 * 
 * @param s_data pointer with file descriptor
 * @param data dictionary node
 * @return unsigned TRUE/FALSE
 */

static int16_t cmp_fd(void* s_data, void* data);

/**
 * @brief Internal function to send package from queue
 * 
 * @return int 
 */

static int32_t sendPackage();

/**
 * @brief Internal function to receive package to queue
 * 
 * @param fd file descriptor
 * @return int size received
 */

static int32_t recvPackage(int32_t fd);

/**
 * @brief Print queue to file
 * 
 * @param f File pointer
 * @param nd data
 */

static void print_queue(FILE*f, void* nd);

static void delete_queue(void* data);

static void print_dict(FILE*f, void* nd);

/** END LOCAl FUNCTIONS **/

enum { CMP_USERNAME = 0, CMP_FD, CMP_ID = 0};

UserSockHandler_t _UsrSck_h = {
	.sockFD = {0},
	.sockFDind = 0,
	.sndQ = {
		.root = NULL,
		.size = 0,
		.cmpData = {
			[CMP_ID] = cmp_id,
		},
		.deleteNode = delete_queue,
		.printNode = print_queue,
	},
	.rcvQ = {
		.root = NULL,
		.size = 0,
		.cmpData = {
			[CMP_ID] = cmp_id,
		},
		.deleteNode = delete_queue,
		.printNode = print_queue,
	},
	.userFDdict = {
		.root = NULL,
		.size = 0,
		.cmpData = {
			[CMP_USERNAME] = cmp_username,
			[CMP_FD] = cmp_fd,
		},
		.printNode = print_dict,
	},
};

void Socket_Init(){
	pthread_t thread_id;
	_UsrSck_h.sockFDind = 0;
	pthread_create(&thread_id, NULL, connectionThread, NULL);
}

SOCKET_ADD_RESP Socket_Add(struct sockaddr_in *sockaddr, char *user, SOCKET_TYPE type){
	int32_t sockfd = socket(AF_INET, type == TCP? SOCK_STREAM : SOCK_DGRAM, 0);
    
	if (sockfd == -1) 
        return SOCKET_CREATION_FAIL;

    if (connect(sockfd, (struct sockaddr*)sockaddr, sizeof(*sockaddr)) != 0)
        return SOCKET_CONNECTION_FAIL;

	_UsrSck_h.sockFD[_UsrSck_h.sockFDind].fd = sockfd;
	_UsrSck_h.sockFD[_UsrSck_h.sockFDind].events = POLLIN | POLLOUT;
	_UsrSck_h.sockFDind++;

    dict_node_t dict_sock = {
		.fd = sockfd,
		.type = type,
		.user = strcpy((uint8_t*)malloc(strlen(user)), user),
	};

	List_Insert(&(_UsrSck_h.userFDdict),&dict_sock,sizeof(dict_node_t), LAST_POS);

	List_Print(&_UsrSck_h.userFDdict, stdout);

    return SUCCESS;	
}

int32_t Socket_WriteQueue(pack_node_t *data){
	dict_node_t* user = List_Find(&_UsrSck_h.userFDdict, NULL, data->user, CMP_USERNAME);
	if( NULL != user )
		List_Insert(&_UsrSck_h.sndQ, data, sizeof(pack_node_t), LAST_NODE);
	__DEBUG_PRINT__(__DEBUG_SOCK_WRITE_Q__)
	return 1;
}	

pack_node_t* Socket_ReadQueue(pack_node_t *data){
	pack_node_t* temp = List_GetData(&_UsrSck_h.rcvQ, LAST_NODE);
	if(temp){
		*data = *temp;
		List_Delete(&_UsrSck_h.rcvQ, LAST_NODE);
		return temp;
	}
	return NULL;
}

uint8_t Socket_IsPackageSent(int32_t id){
	return NULL == List_Find(&_UsrSck_h.sndQ, NULL, &id, CMP_ID);
}

static void* connectionThread(void * param){
	//epoll
	while(1){
		poll(_UsrSck_h.sockFD,_UsrSck_h.sockFDind,1000);
		while(sendPackage() != 0);
		for(int i = 0; i < _UsrSck_h.sockFDind; i++){
			if(_UsrSck_h.sockFD[i].revents & POLLIN)
				recvPackage(_UsrSck_h.sockFD[i].fd);
		}
	}
}

static int32_t sendPackage(){

	int pos = 0;
	if(List_GetSize(&_UsrSck_h.sndQ)){
		
		//List_Print(&_UsrSck_h.sndQ, stdout);

		pack_node_t fst_nd_q = *(pack_node_t*)List_GetData(&_UsrSck_h.sndQ, 0);
		dict_node_t*  user_dict = List_Find(&_UsrSck_h.userFDdict, NULL, fst_nd_q.user, CMP_USERNAME);

		if(user_dict->type == TCP){
			uint8_t pack_buffer[HEADER_SIZE + fst_nd_q.size + LAST_SYMBOL_SIZE];
			uint8_t end_symbol[LAST_SYMBOL_SIZE] = {0};
			
			memcpy(pack_buffer, &fst_nd_q.size, sizeof(uint32_t)); 								//Header
			memcpy(pack_buffer + HEADER_SIZE, fst_nd_q.data, fst_nd_q.size); 					//Payload
			memcpy(pack_buffer + HEADER_SIZE + fst_nd_q.size, end_symbol, sizeof(end_symbol)); 	//End symbol
			List_Delete(&_UsrSck_h.sndQ, 0);

			return send(user_dict->fd, pack_buffer, sizeof(pack_buffer), 0);
		}else{ 
			List_Delete(&_UsrSck_h.sndQ, 0);
			return send(user_dict->fd, fst_nd_q.data, fst_nd_q.size, 0);
		}
	}
	return 0;
}

static int32_t recvPackage(int32_t fd){
	char buffer_recv[MAX_PACKAGE_SIZE];
	printf("Rcv Queue\n");
	List_Print(&_UsrSck_h.rcvQ, stdout);
	uint16_t size = recv(fd,buffer_recv,MAX_PACKAGE_SIZE,0);
	if(size > 0){
		char* user_name = ((dict_node_t*)List_Find(&_UsrSck_h.userFDdict,NULL, &fd, CMP_FD))->user ;
		pack_node_t pack_aux = {
			.data = memcpy(malloc(size), buffer_recv,size),
			.size = size,
			.user = strcpy(malloc(strlen(user_name)), user_name),
		};
		List_Insert(&_UsrSck_h.rcvQ, &pack_aux, sizeof(pack_node_t),LAST_POS);
	}
	return size;
}

static void delete_queue(void* data){
	free(data);
}

static int16_t cmp_username(void* s_data, void* data){
	dict_node_t *aux = (dict_node_t*) data;
	return strcmp(aux->user,s_data);
}

static int16_t cmp_fd(void* s_data, void* data){
	dict_node_t *aux = (dict_node_t*) data;
	return ! (aux->fd == *(uint32_t*)s_data);
}

static int16_t cmp_id(void* s_data, void* data){
	pack_node_t *aux = (pack_node_t*) data;
	return ! (aux->id == *(uint32_t*)s_data);
}

static void print_queue(FILE*f, void* nd){
	pack_node_t *aux = (pack_node_t*) nd;
	fprintf(f, "User: %s Size: %d Message: ",aux->user,aux->size);
	for(int i = 0; i < aux->size; i++)
		fprintf(f,"%.2hhX",((char*)aux->data)[i]);
	fprintf(f,"\n");
}

static void print_dict(FILE*f, void* nd){
	dict_node_t *aux = nd;
	fprintf(f, "FD : %d, Type : %s, User : %s\n", aux->fd, aux->type?"TCP":"UDP", aux->user);
}
