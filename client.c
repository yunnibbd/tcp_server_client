#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include "cclient.h"
#include "cepoll.h"
#include "cbuffer.h"

Epoll * epoll = NULL;

int main(int argc, const char * const * argv){

    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    Client * client = client_new(client_fd, 4096, 4096);
    if(!client){
        printf("client new failed!\n");
        exit(1);
    }

    if(-1 == client_connect_to(client, "127.0.0.1", 8000)){
        printf("client_connect_to failed!\n");
        exit(1);
    }

    epoll = epoll_new(2);
    if(!epoll){
        printf("epoll_new failed!\n");
        exit(1);
    }

    epoll_register_ptr_in(epoll, client);
    Client * tmp_client = NULL;
    Buffer * buffer = buffer_new(1024);
    if(!buffer){
        printf("buffer_new error\n");
        exit(1);
    };
    while(1){
        if(client_need_write(client) > 0){
            epoll_modify_ptr_in_out(epoll, client);
        }else{
            epoll_modify_ptr_in(epoll, client);
        }

        int ret = epoll_wait_event(epoll, -1);
        if(ret <= 0)
            continue;
        struct epoll_event * events = epoll_get_events(epoll);
        for(int i = 0; i < ret; ++i){
            tmp_client = (Client *)events[i].data.ptr;
            if(events[i].events & EPOLLIN){
                int recv_len = client_recvdata(tmp_client);
                if(recv_len <= 0)
                    continue;
                tmp_client->recvbuffer->data[recv_len] = 0;
                printf("recv from server %d:%s\n", recv_len, client->recvbuffer->data);
                client_clear_recvbuffer(tmp_client);
                client_senddata(client, "hello server", 12);
            }
            if(events[i].events & EPOLLOUT){
                client_senddata_real(client);
            }
        }
        sleep(1);
    }


    epoll_free(epoll);
    client_free(client);
    bufer_free(buffer);
    return 0;
}

