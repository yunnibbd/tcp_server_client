#include "cclient.h"
#include "cbuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

Client * client_new(int sockfd, int sendsize, int recvsize){
    Client * client = (Client *)malloc(sizeof(Client));
    client->sockfd = sockfd;
    client->sendbuffer = buffer_new(sendsize);
    client->recvbuffer = buffer_new(recvsize);
    return client;
}

int client_recvdata(Client * client){
    buffer_read4socket(client->recvbuffer, client->sockfd);
}

int client_sendbuffer(Client * client, Buffer * buffer){
    buffer_push(client->sendbuffer, buffer->data, buffer->data_size);
}

int client_senddata(Client * client, const char * data, int len){
    buffer_push(client->sendbuffer, data, len);
}

int client_senddata_real(Client * client){
    buffer_write2socket(client->sendbuffer, client->sockfd);
}

int client_need_write(Client * client){
    return client->sendbuffer->data_size;
}

int client_need_read(Client * client){
    return client->recvbuffer->data_size;
}

void client_clear_recvbuffer(Client * client){
    buffer_pop(client->recvbuffer, client->recvbuffer->data_size);
}

void client_clear_sendbuffer(Client * client){
    buffer_pop(client->sendbuffer, client->sendbuffer->data_size);
}

int client_connect_to(Client * client, const char * ip, int port){
    if(!ip || port <= 0 || port > 65535)
        return -1;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    return connect(client->sockfd, (struct sockaddr *)&addr, sizeof(addr));
}

void client_free(void * ptr){
    Client * client = (Client *)ptr;
    if(client->sockfd > 0){
        close(client->sockfd);
        client->sockfd = -1;
    }
    buffer_free(client->sendbuffer);
    buffer_free(client->recvbuffer);
    free(ptr);
}


