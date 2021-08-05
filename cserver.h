#ifndef __CSERVER_H__
#define __CSERVER_H__
#include "rbtree.h"
#include "cclient.h"
#include "clist.h"
#include <arpa/inet.h>

typedef struct{
    rbtree * clients;
    int client_size;
    int sockfd;
    int client_sendsize;
    int client_recvsize;
}Server;

Server * server_new(const char * ip, int port, int backlog, int sendsize, int recvsize);

Client * server_accept_client(Server * server, struct sockaddr_in * client_addr);

void server_remove_client(Server * server, Client * client);

void server_foreach_clients(Server * server, foreach_func_t fn);

void server_free(void * server);

#endif
