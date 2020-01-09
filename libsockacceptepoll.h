#ifndef LLQ_EPOLL_SERVER_H_
#define LLQ_EPOLL_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <pthread.h>

#include "llqvarymem.h"
#include "llqsocket.h"
#include "llqepollevent.h"

struct sockacceptepoll;
typedef struct sockacceptepoll sockacceptepoll_t, * sockacceptepoll_p;



typedef int (*sockacceptepoll_accept)(sockacceptepoll_p o, int sockid);

struct sockacceptepoll {
	unsigned int flag;//bit mask{0:onloop}
    int epoll;//多路复用标识
	int sock;//服务端监听套接字
    pthread_mutex_t	lock;//互斥锁,用于进程与线程接收 socket的accept使用
    sockacceptepoll_accept handle;//线程监听处理程
    void* handids;//处理进程或线程的ID列表
};
#define EPOLLSERVER_INITIALIZE(handle)    {0, 0, 0, PTHREAD_MUTEX_INITIALIZER, handle }


extern void sockacceptepoll_stop(sockacceptepoll_p o);
/** 阻塞当前进程监听与处理多路复用套接字请求 */
extern int sockacceptepoll_listen(sockacceptepoll_p o, const char* ip, int port, int maxlisten, int maxepoll);

/** 不阻塞当前进程监听端口的accpet的多路复用实现，为接收到请求后为每一个请求创建一个线程来完成任务 */
extern int sockacceptepoll_noblockthreads_listen(sockacceptepoll_p o, const char* ip, int port, int maxlisten, int maxepoll);
/** 阻塞当前进程监听端口的accpet的多路复用实现，为接收到请求后为每一个请求创建一个线程来完成任务 */
extern int sockacceptepoll_multithreads_listen(sockacceptepoll_p o, const char* ip, int port, int maxlisten, int maxepoll);
/** 阻塞当前进程监听端口的accpet的多路复用实现，为接收到请求后为每一个请求创建一个单独进程来完成任务 */
extern int sockacceptepoll_multiprocess_listen(sockacceptepoll_p o, const char* ip, int port, int maxlisten, int maxepoll);

/** 不阻塞当前进程；创建指定N个子进程依次监听端口的accept，使用互斥解决惊群完成任务执行 */
extern int sockacceptepoll_fixprocess_listen(sockacceptepoll_p o, const char* ip, int port, int maxlisten, int maxepoll, int processcount);
/** 不阻塞当前进程；创建指定N个线程依次监听端口的accept，使用互斥解决惊群完成任务执行 */
extern int sockacceptepoll_fixthreads_listen(sockacceptepoll_p o, const char* ip, int port, int maxlisten, int maxepoll, int threadcount);
extern void sockacceptepoll_fixthreads_close(sockacceptepoll_p o);



#ifdef __cplusplus
};
#endif

#endif