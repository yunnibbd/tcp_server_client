#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>
#include "cepoll.h"

#define EPOLL_ERROR -1

Epoll * epoll_new(int nMaxEvents){
    Epoll * epoll = (Epoll *)malloc(sizeof(Epoll));
    //linux 2.6.8 后size就没有作用了
    //由epoll动态管理，理论最大值为filemax
    //通过cat /proc/sys/fs/file-max来查询
    //ulimit -n
    epoll->epfd = epoll_create(nMaxEvents);
    if(EPOLL_ERROR == epoll->epfd)
        return NULL;
    epoll->events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * nMaxEvents);
    epoll->max_event = nMaxEvents;
    return epoll;
}

void epoll_free(void * epoll){
    Epoll * e = (Epoll *)epoll;
    free(e->events);
    free(e);
}

//向epoll对象注册需要管理、监听的Socket文件描述符
int epoll_ctl_fd(Epoll * epoll, int op, int sockfd, uint32_t events){
    struct epoll_event ev;
    //事件类型
    ev.events = events;
    //事件关联的socket描述符对象
    ev.data.fd = sockfd;
    //向epoll对象注册需要管理、监听的Socket文件描述符
    //并且说明关心的事件
    //返回0代表操作成功，返回负值代表失败 -1
    int ret = epoll_ctl(epoll->epfd, op, sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_ctl add fd failed");
    return ret;
}

//向epoll对象注册需要管理、监听的Socket文件描述符
int epoll_ctl_ptr(Epoll * epoll, int op, Client * pClient, uint32_t events){
    struct epoll_event ev;
    //事件类型
    ev.events = events;
    //事件关联的Client对象
    ev.data.ptr = pClient;
    //向epoll对象注册需要管理、监听的Socket文件描述符
    //并且说明关心的事件
    //返回0代表操作成功，返回负值代表失败 -1
    int ret = epoll_ctl(epoll->epfd, op, pClient->sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_ctl add ptr failed");
    return ret;
}

int epoll_register_fd_in(Epoll * epoll, int sockfd){
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_register_fd_in failed");
    return ret;
}

int epoll_register_fd_out(Epoll * epoll, int sockfd){
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.fd = sockfd;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_register_fd_out failed");
    return ret;
}

int epoll_register_fd_in_out(Epoll * epoll, int sockfd){
    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLOUT;
    ev.data.fd = sockfd;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_register_fd_in_out failed");
    return ret;
}

void epoll_unregister_fd(Epoll * epoll, int sockfd){
    epoll_ctl(epoll->epfd, EPOLL_CTL_DEL, sockfd, NULL);
}

int epoll_register_ptr_in(Epoll * epoll, Client * client){
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = client;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, client->sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_register_ptr_in failed");
    return ret;
}

int epoll_register_ptr_out(Epoll * epoll, Client * client){
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.ptr = client;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, client->sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_register_ptr_out failed");
    return ret;
}

int epoll_register_ptr_in_out(Epoll * epoll, Client * client){
    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLOUT;
    ev.data.ptr = client;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_ADD, client->sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_register_ptr_in_out failed");
    return ret;
}

void epoll_unregister_ptr(Epoll * epoll, Client * client){
    epoll_ctl(epoll->epfd, EPOLL_CTL_DEL, client->sockfd, NULL);
}

int epoll_modify_fd_in(Epoll * epoll, int sockfd){
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_MOD, sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_modify_fd_in failed");
    return ret;
}

int epoll_modify_fd_out(Epoll * epoll, int sockfd){
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.fd = sockfd;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_MOD, sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_modify_fd_out failed");
    return ret;
}

int epoll_modify_fd_in_out(Epoll * epoll, int sockfd){
    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLOUT;
    ev.data.fd = sockfd;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_MOD, sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_modify_fd_in_out failed");
    return ret;
}

int epoll_modify_ptr_in(Epoll * epoll, Client * client){
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = client;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_MOD, client->sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_modify_ptr_in failed");
    return ret;
}

int epoll_modify_ptr_out(Epoll * epoll, Client * client){
    struct epoll_event ev;
    ev.events = EPOLLOUT;
    ev.data.ptr = client;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_MOD, client->sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_modify_ptr_out failed");
    return ret;
}

int epoll_modify_ptr_in_out(Epoll * epoll, Client * client){
    struct epoll_event ev;
    ev.events = EPOLLIN|EPOLLOUT;
    ev.data.ptr = client;
    int ret = epoll_ctl(epoll->epfd, EPOLL_CTL_MOD, client->sockfd, &ev);
    if(EPOLL_ERROR == ret)
        perror("epoll_modify_ptr_in_out failed");
    return ret;
}

int epoll_wait_event(Epoll * epoll, int timeout){
    //epfd epoll对象的描述符
    //events 用于接收检测到的网络事件的数组
    //maxevents 接收数组的大小，能够接收的事件数量
    //timeout
    //		t=-1 直到有事件发生才返回
    //		t= 0 立即返回 std::map
    //		t> 0 如果没有事件那么等待t毫秒后返回。
    int ret = epoll_wait(epoll->epfd, epoll->events, epoll->max_event, timeout);
    if(EPOLL_ERROR == ret){
        if(errno == EINTR)
            return 0;
    }
    return ret;
}

struct epoll_event * epoll_get_events(Epoll * epoll){
    return epoll->events;
}

