#ifndef __CEPOLL_H__
#define __CEPOLL_H__

#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "cclient.h"

typedef struct{
    struct epoll_event * events;
    int epfd;
    int max_event;
}Epoll;

Epoll * epoll_new(int nMaxEvents);

void epoll_free(void * epoll);

//向epoll对象注册需要管理、监听的Socket文件描述符
int epoll_ctl_fd(Epoll * epoll, int op, int sockfd, uint32_t events);

//向epoll对象注册需要管理、监听的Socket文件描述符
int epoll_ctl_ptr(Epoll * epoll, int op, Client * pClient, uint32_t events);

int epoll_register_fd_in(Epoll * epoll, int sockfd);

int epoll_register_fd_out(Epoll * epoll, int sockfd);

int epoll_register_fd_in_out(Epoll * epoll, int sockfd);

int epoll_modify_fd_in(Epoll * epoll, int sockfd);

int epoll_modify_fd_out(Epoll * epoll, int sockfd);

int epoll_modify_fd_in_out(Epoll * epoll, int sockfd);

void epoll_unregister_fd(Epoll * epoll, int sockfd);

int epoll_register_ptr_in(Epoll * epoll, Client * client);

int epoll_register_ptr_out(Epoll * epoll, Client * client);

int epoll_register_ptr_in_out(Epoll * epoll, Client * client);

int epoll_modify_ptr_in(Epoll * epoll, Client * client);

int epoll_modify_ptr_out(Epoll * epoll, Client * client);

int epoll_modify_ptr_in_out(Epoll * epoll, Client * client);

void epoll_unregister_ptr(Epoll * epoll, Client * client);

int epoll_wait_event(Epoll * epoll, int timeout);

struct epoll_event* epoll_get_events(Epoll * epoll);


#endif // !__CEPOLL_H__

