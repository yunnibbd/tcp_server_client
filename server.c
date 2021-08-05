#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "cepoll.h"
#include "cclient.h"
#include "cserver.h"

Epoll * epoll = NULL;

void watch_client_cb(void * key, void * data){
    Client * client = (Client *)data;
    if(client_need_write(client) > 0){
        epoll_modify_ptr_in_out(epoll, client);
    }else{
        epoll_modify_ptr_in(epoll, client);
    }
}

int main(int argc, const char * const * argv){
    if(argc < 2){
        printf("unkown server port!\n");
        exit(1);
    }

    signal(SIGPIPE, SIG_IGN);

    Server * server = server_new("127.0.0.1", 8000, 128, 4096, 4096);
    int server_fd = server->sockfd;
    if(!server){
        exit(1);
    }

    epoll = epoll_new(1024);
    if(!epoll){
        printf("epoll_new failed\n");
        exit(1);
    }

    epoll_register_fd_in(epoll, server_fd);

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    Client * client = NULL;
    while(1){
        server_foreach_clients(server, watch_client_cb);
        int ret = epoll_wait_event(epoll, -1);
        if(ret <= 0)
            continue;
        struct epoll_event * events = epoll_get_events(epoll);
        for(int i = 0; i < ret; ++i){
            if(events[i].data.fd == server_fd){
                if(events[i].events & EPOLLIN){
                    client = server_accept_client(server, &client_addr);
                    if(!client)
                        continue;
                    printf("%s:%d join!\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    client_senddata(client, "hello client", 12);
                    epoll_register_ptr_in(epoll, client);
                }
            }else{
                client = (Client *)events[i].data.ptr;
                if(events[i].events & EPOLLIN){
                    int recv_len = client_recvdata(client);
                    if(recv_len <= 0){
                        server_remove_client(server, client);
                        printf("client leave\n");
                        continue;
                    }
                    client->recvbuffer->data[recv_len] = 0;
                    printf("recv from client %d:%s\n", recv_len, client->recvbuffer->data);
                    client_clear_recvbuffer(client);
                    client_senddata(client, "OK\0", 3);
                }
                if(events[i].events & EPOLLOUT){
                    client_senddata_real(client);
                }
            }
        }
    }

    server_free(server);
    epoll_free(epoll);
    return 0;
}

