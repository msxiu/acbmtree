#ifndef LLQ_WEB_SOCKET_H_
#define LLQ_WEB_SOCKET_H_

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

#include "libprotocolhttp.h"
#include "libsockacceptepoll.h"


#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
#define PROMPT_MSG(format, args...)	do {\
	fprintf(stdout, "%s:%d=>", __FILE__, __LINE__); \
	fprintf(stdout, format, ##args);\
	fprintf(stdout, "%s", "\r\n"); \
	fflush(stdout);\
} while(0)
#else
#define PROMPT_MSG(format, args...)	do {}while(0)
#endif

#define WEBSOCKET_FRAME_CONTINUATION    0x00
#define WEBSOCKET_FRAME_TEXT            0x01
#define WEBSOCKET_FRAME_BINARY          0x02
#define WEBSOCKET_FRAME_CLOSE           0x08
#define WEBSOCKET_FRAME_PING            0x09
#define WEBSOCKET_FRAME_PONG            0x0A


#define CHECK_IO_STATUS(o)  (o == EINTR || o == EWOULDBLOCK || o == EAGAIN)
 
#define MAX_EVENTS           	10240	//最大连接数 
#define PER_LINE_MAX            256		//client key值最大长度
#define REQUEST_LEN_MAX         2048	//request包的最大字节数
#define WEB_SOCKET_KEY_LEN_MAX  256		//websocket key值最大长度
#define RESPONSE_HEADER_LEN_MAX 2048	//response包的最大字节数

struct wsaction;
typedef struct wsaction wsaction_t, *wsaction_p;
typedef int(*websocket_passive_cbk)(sizeptr_p req, sizeptr_p res);

extern int wsmake_sha1(const char *source, void *buf, int buf_len);
extern char* wsbase64_encode(const unsigned char* data, int data_len);
extern int wsbase64_decode(const char *data, int data_len, unsigned char* dst, int dstln);


typedef struct httpkeyvalue {
    char key[32];
    char value[96];
}httpkeyvalue_t, * httpkeyvalue_p;

//int (*passive)(sizeptr_p req, sizeptr_p res);//被动处理程序
struct wsaction {
    char uri[256];//比对URI
    websocket_passive_cbk passive;//被动回调处理
    unsigned int intval;//计动调用间隔
    int (*initiative)(int sockid);//主动通知
};
//extern int wsaction_wsbind(wsaction_p actions, epolldataptr_p ptr);
static inline wsaction_p wsaction_find(wsaction_p actions, const char* uri) {
    wsaction_p action = actions;
    while((action->passive || action->initiative)) {
        if(0 == strcasecmp(action->uri, uri)) {
            return action;
        }
        action++;
    }
    return NULL;
}

typedef int (*response_handle_cbk)(httprequest_p req, sizeptr_p res);
typedef struct webaction {
    char uri[256];//比对URI
    char contentype[128];//流类型
    response_handle_cbk handle;//被动处理程序
} webaction_t, *webaction_p;

static inline webaction_p webaction_find(webaction_p actions, const char* uri) {
    webaction_p action = actions;
    while(action->handle) {
        if(0 == strcasecmp(action->uri, uri)) {
            return action;
        }
        action++;
    }
    return NULL;
}


extern int websocket_acceptkey(const char * clientkey, char* serverkey, int sln);
extern int websocket_clientkey(const char * data, char* key, int kln);
extern int websocket_request_read(int sockid, unsigned char *hdr, sizeptr_p req);//读取websocket请求
extern int websocket_response(sizeptr_p res, const unsigned char *message, unsigned long n);
extern int websocket_initiative_response(sock_t sock, void* data, size_t dln);
extern int websocket_shakehand(const char *clientkey, sizeptr_p req, sizeptr_p res);
extern void websocket_handle(int sockid, wsaction_p action);

extern int webhttp_request_read(int sock, sizeptr_p data);
extern int webhttp_response(sizeptr_p data, const char* contentype, const unsigned char *message, unsigned long n);
extern int webhttp_response_execute(httprequest_p req, webaction_p action, sizeptr_p res);
extern const char* webhttp_contenttype(httpkeyvalue_p kvs, const char* faddr);
extern int webhttp_response_localfile(const char* faddr, sizeptr_p data);



#ifdef __cplusplus
};
#endif

#endif