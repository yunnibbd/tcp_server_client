#include "cbuffer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Buffer * buffer_new(int size){
    if(size <= 0)
        return NULL;
    Buffer * buffer = (Buffer *)malloc(sizeof(Buffer));
    memset(buffer, 0, sizeof(Buffer));
    char * data = (char *)malloc(size);
    memset(data, 0 , size);
    buffer->data = data;
    buffer->size = size;
    return buffer;
}

int buffer_push(Buffer * buffer, char * data, int len){
    if(buffer->data_size + len <= buffer->size){
        memcpy(buffer->data + buffer->data_size, data, len);
        buffer->data_size += len;
        return 0;
    }
    return -1;
}

int buffer_pop(Buffer * buffer, int len){
    int n = buffer->data_size - len;
    if(n > 0)
        memcpy(buffer->data, buffer->data + len, n);
    buffer->data_size = n;
}

int buffer_read4socket(Buffer * buffer, int sockfd){
    if(buffer->size - buffer->data_size > 0){
        //接收客户端数据
        char * szRecv = buffer->data + buffer->data_size;
#ifdef _WIN32
        int nLen = (int)recv(sockfd, szRecv, buffer->size - buffer->data_size, 0);
#else
        int nLen = (int)read(sockfd, szRecv, buffer->size - buffer->data_size);
#endif
        if(nLen <= 0)
            return -1;
        //消息缓冲区的数据尾部位置后移
        buffer->data_size += nLen;
        return nLen;
    }
    return 0;
}

int buffer_write2socket(Buffer * buffer, int sockfd){
    int ret = 0;
    //缓冲区有数据
    if(buffer->data_size > 0 && sockfd > 0){
        //发送数据
#ifdef _WIN32
        ret = send(sockfd, buffer->data, buffer->data_size, 0);
#else
        ret = write(sockfd, buffer->data, buffer->data_size);
#endif
        if(ret <= 0)
            return -1;
        if (ret == buffer->data_size){
            buffer->data_size = 0;
        }else{
            buffer->data_size == ret;
            memcpy(buffer->data, buffer->data + ret, buffer->data_size);
        }
    }
    return ret;
}

void buffer_free(void * buffer){
    Buffer * b = (Buffer *)buffer;
    free(b->data);
    free(b);
}


