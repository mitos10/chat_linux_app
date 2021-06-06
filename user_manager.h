#include <string.h>
#include <C_CkRsa.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>

#define SERVER_ADDR
#define SERVER_PORT

int user_registration(string user, string password);

int user_login(string user, string password);