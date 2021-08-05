#ifndef __CCLIENT_H__
#define __CCLIENT_H__
#include "cbuffer.h"

typedef struct{
    int sockfd;
    Buffer * sendbuffer;
    Buffer * recvbuffer;
} Client;

Client * client_new(int sockfd, int sendsize, int recvsize);

int client_recvdata(Client * client);

int client_sendbuffer(Client * client, Buffer * buffer);

int client_senddata(Client * client, const char * data, int len);

int client_senddata_real(Client * client);

int client_need_write(Client * client);

int client_need_read(Client * client);

void client_clear_recvbuffer(Client * client);

void client_clear_sendbuffer(Client * client);

int client_connect_to(Client * client, const char * ip, int port);

void client_free(void * ptr);

#endif

