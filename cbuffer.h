#ifndef __CBUFFER_H__
#define __CBUFFER_H__

typedef struct{
    //数据指针
    char * data;
    //缓冲区总大小
    int size;
    //缓冲区数据大小
    int data_size;
}Buffer;

Buffer * buffer_new(int size);

int buffer_push(Buffer * buffer, char * data, int len);

int buffer_pop(Buffer * buffer, int len);

int buffer_read4socket(Buffer * buffer, int sockfd);

int buffer_write2socket(Buffer * buffer, int sockfd);

void buffer_free(void * buffer);

#endif

