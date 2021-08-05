#include "cserver.h"
#include "cclient.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

static List list;

static int client_compare_cb(void * a, void * b){
    if(*(int*)a > *(int*)b)
        return 1;
    else if(*(int*)a == *(int*)b)
        return 0;
    else
        return -1;
}

Server * server_new(const char * ip, int port, int backlog, int client_sendsize, int client_recvsize){
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0) ;
    if(server_fd < 0){
        perror("socket");
        return NULL;
    }else{
        int flag = 1;
        if (-1 == setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&flag, sizeof(flag))){
            perror("setsockopt");
            return NULL;
        }
    }
    if(-1 == bind(server_fd, (struct sockaddr *) & server_addr, sizeof(server_addr))){
        perror("bind");
        return NULL;
    }
    if(-1 == listen(server_fd, backlog)){
        perror("listen");
        return NULL;
    }
    Server * server = (Server *)malloc(sizeof(Server));
    memset(server, 0, sizeof(Server));
    server->sockfd = server_fd;
    server->clients = rbtree_init(client_compare_cb);
    server->client_sendsize = client_sendsize;
    server->client_recvsize = client_recvsize;
    if(!server->clients)
        return NULL;
    return server;
}

static int client_addr_len = sizeof(struct sockaddr);

Client * server_accept_client(Server * server, struct sockaddr_in * client_addr){
    int client_fd = accept(server->sockfd, (struct sockaddr *)client_addr, &client_addr_len);
    if(client_fd < 0)
        return NULL;
    Client * client = client_new(client_fd, server->client_sendsize, server->client_recvsize);
    if(!client)
        return NULL;
    client_fd = rbtree_insert(server->clients, &client->sockfd, client);
    if(client_fd != 0)
        return NULL;
    return client;
}

void server_remove_client(Server * server, Client * client){
    rbtree_remove(server->clients, &client->sockfd);
    client_free(client);
}

void server_foreach_clients(Server * server, foreach_func_t fn){
    rbtree_foreach(server->clients, fn);
}

static void server_free_cb(void * key, void * data){
    list_pushback(&list, data);
}

void server_free(void * server){
    Server * s = (Server *)server;
    rbtree_foreach(s->clients, server_free_cb);
    struct link_node * node = list.root;
    while(node){
        client_free(node->data);
        node = node->next;
    }
    free(s);
}

