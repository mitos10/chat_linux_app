#include "user_manager.h"

int open_socket();

typedef struct {
	char* user,
	int socket_id,
	char* ip,
	int port
} user_data;

user_data* user_table[50] = {NULL};
static int socket_id = open_socket();

char* get_server_publ_key(int socket);
void get_client_keys(HCkRsa* rsa_client);

//Creates server socket and listening socket
int initial_sockets(){

	int socket_desc;
	struct sockaddr_in server;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	server.sin_family = AF_INET;
	server.sin_port = htons(SERVER_PORT);

	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server) ) < 0)
	{
		puts("connect error");
		return -1;
	}

	int socket_listen_client;
	struct sockaddr_in listen_client;
	
	//Create socket
	socket_listen_client = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_listen_client == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	listen_client.sin_addr.s_addr = INADDR_ANY;
	listen_client.sin_family = AF_INET;
	listen_client.sin_port = htons( LISTENING_PORT );
	
	//Bind
	if( bind(socket_listen_client,(struct sockaddr *)&listen_client , sizeof(listen_client)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_listen_client , 3);

	puts("Connected\n");
}

void get_client_keys(HCkRsa* rsa_client){
	
	rsa_client = CkRsa_Create();
	success = CkRsa_GenerateKey(rsa_client,1024);
    if (success != TRUE) {
        printf("%s\n",CkRsa_lastErrorText(rsa_client));
        CkRsa_Dispose(rsa_client);
        return NULL;
    }

    /*
    client_keys* cl_keys;
    
    cl_keys = malloc(sizeof(client_keys));
    cl_keys.public = CkRsa_exportPublicKey(*rsa);
    cl_keys.private = CkRsa_exportPrivateKey(*rsa);
    return cl_keys;
    */
}

int publ_key_exchng(int socket, HCkRsa* rsa_client, HCkRsa* rsa_server){
	
	str_cpy( buffer, CkRsa_exportPublicKey(*rsa_client));
	send(socket, buffer, 2048, 0);

	recv(socket, buffer, 2048, 0);
	str_cpy( buffer, CkRsa_importPublicKey(*rsa_server, buffer));
}

int rsa_send(int socket, char* buffer, int len, HCkRsa* rsa_client, HCkRsa* rsa_server){
	str_cpy(buffer, CkRsa_encryptBytesENC(*rsa_client,buffer,TRUE));
	str_cpy(buffer, CkRsa_encryptBytesENC(*rsa_server,buffer,FALSE));
	if(send(socket_id, buffer, 2048, 0) < 0){
		return -1;
	}
	return 0;
}

int rsa_recv(int socket, char* buffer, int len, HCkRsa* rsa_client, HCkRsa* rsa_server){
	
	str_cpy(buffer, CkRsa_decryptBytesENC(rsa_server,buffer,FALSE));
	str_cpy(buffer, CkRsa_decryptBytesENC(rsa_client,buffer,TRUE));
	if( recv(socket_id, buffer, 2048, 0) < 0){
		return -1;
	}
	return 0;
}

int user_registration(string user, string password){
	
	char buffer[2048] = {'\0'};
	char aux[2048] = {'\0'};
	HCkRsa rsa_client, rsa_server;

	open_socket();

	get_client_keys(&rsa_client);
	publ_key_exchng(socket_id,&rsa_client,&rsa_server);

	sprintf(aux, "%c/%s/%s", REGIST_COMMAND, user, password);
	str_cpy(buffer,aux);

	rsa_send(socket_id, buffer, 2048, &rsa_client, &rsa_server);
	rsa_recv(socket_id, buffer, 2048, &rsa_client, &rsa_server); //Server sends confirm or denial
	
	CkRsa_Dispose(rsa_client);
	if( strcmp( aux, buffer ) == 0)
		return;
}

int user_login(string username, string password){


	/*
		Send login petition to server, send username using RSA protocol
		Receive salt and hashed password from server
		Hash the password with salt and check with the received hashed password
		Send plain text password
		Server checks if hashed salted password == to stored salted hashed password
	*/

	HCkRsa rsa_client, rsa_server;
	char buffer[2048] = {'\0'};

	get_client_keys(&rsa_client);
	publ_key_exchng(socket_id, &rsa_client, &rsa_server);

	sprintf(buffer,"%c/%s",LOGIN_COMMAND,user);
	rsa_send(socket_id, buffer, 2048, &rsa_client, &rsa_server);
	rsa_recv(socket_id, buffer, 2048, &rsa_client, &rsa_server);
	//strcmp(sha256( password+salt), buffer.hashed_value);
	str_cpy(buffer,password);
	rsa_send(socket_id, buffer, 2048, &rsa_client, &rsa_server);
	rsa_recv(socket_id, buffer, 2048, &rsa_client, &rsa_server);

	if( strcmp("login accepted", buffer) == 0){


		return;
	}

}

int connection_refresh(){

	get_client_keys(&rsa_client);
	publ_key_exchng(socket_id, &rsa_client, &rsa_server);

	sprintf(buffer, "%c/%s/%s/%s", REFRESH, user, ip, port);
	rsa_send(socket_id, buffer, 2048, &rsa_client, &rsa_server);
	rsa_recv(socket_id, buffer, 2048, &rsa_client, &rsa_server);
	//strcmp(sha256( password+salt), buffer.hashed_value);
}

user_data* user_user_connection(string other_user){

}

int chat_connection_received(){

}

int send_message(string other_username, char* message){

	rsa_send(socket_id, buffer, 2048, &rsa_client, &rsa_other_user);

}

int receive_message(){
	//Check sockets
	rsa_recv(socket_id, buffer, 2048, &rsa_client, &rsa_other_user);
}