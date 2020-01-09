#ifndef LLQHTTP_ACTION_H_
#define LLQHTTP_ACTION_H_
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <iconv.h>
#include <errno.h>
#include "llqvarymem.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {//HTTP缓存数据，
	unsigned int size;//数据的字节长度
	unsigned char* data;//可变数据存储区，实
} httpcache_t, *httpcache_p;//属性对


typedef struct {
    char method[16];//POST|GET|PUT|HEAD
    char charset[16];//字符串编码
    char host[128];//域名
    char content_type[256];
    char *url;//相对URL地址
    void *content;
    uint32_t content_length;
    uint32_t off_boundary;//multipart/form-data; boundary=的偏移
    httpcache_t params;//请求参数
    httpcache_t cookie;//
    httpcache_t headers;//请求头信息
} httprequest_t, * httprequest_p;


#define HTTPREQUEST_INITIALIZE  { {0}, {0}, {0}, {0}, NULL, NULL, 0, 0, {0, NULL}, {0, NULL}, {0, NULL}}

/**从流的前N字节判断是否为HTTP协议*/
extern int httprequest_discern(const char*p);
extern int httprequest_context(httprequest_p o, const char* ptr, size_t pln);

extern int httprequest_nextheader(httprequest_p o, int* offset, const char** key, const char** value);
extern const char* httprequest_getheader(httprequest_p o, const char* pkey);

/** 用于参数遍历，如果有数据返回1,否则返回0 */
extern int httprequest_nextformdata(httprequest_p o, int* offset, char** key, char** filename, char** contentType, void** data, int *dln);
/*** 获取formdata数据，存在返回1，不存在返回0 */
extern int httprequest_getformdata(httprequest_p o, const char* pkey, char** filename, char** contentType, void** data, int *dln);

extern int httprequest_nextparam(httprequest_p o, int* offset, const char** key, const char** value);
extern const char* httprequest_getparam(httprequest_p o, const char* pkey);

extern int httprequest_nextcookie(httprequest_p o, int* offset, const char** key, const char** value);
extern const char* httprequest_getcookie(httprequest_p o, const char* pkey);

extern void httprequest_reclaim(httprequest_p o);




extern int urlhandle_matchencode(const char* vpath, char** str);
extern int urlhandle_urlencode(const char* vpath, char** str);
/** URL 编码key value数据 */
extern int urlhandle_encodekeyvalues(sizeptr_p keyvalues, char **pdata);
 /** url字符串解码
  * src:源数据; sln:源数据长度; dst:接收目标数据的缓存区; dln:接收目标数据的缓存区长度; unicode:返回参数,编码是否为UNICODE */
extern int urlhandle_urldecode(const char *src, char **dst, int* unicode);

/**将键值对列表生成formdata请求参数 @return 成功返回1，失败返回0
 *  boundary:边界; key:参数名称; fname:文件名; strtype:流类型; data:数据; dln：数据长度 * */
extern int httpformdata_write_element(offsetptr_p o, const char *boundary, const char* key, const char* fname, const char* strtype, const void* data, size_t dln);
/**将键值对列表生成formdata请求参数 @return 成功返回1，失败返回0
 *  boundary:边界; key:参数名称; fname:文件名; strtype:流类型; data:数据; dln：数据长度 * */
extern int httpformdata_write_last(offsetptr_p o, const char *boundary, const char* key, const char* fname, const char* strtype, const void* data, size_t dln);

/** 向响应中写入文件,并写入结束标志
 * o:数据流对象; boundary:数据分隔符; key:参数名称; fpath:文件地址 */
extern int httpformdata_writefile(offsetptr_p o, const char *boundary, const char* key, char* fpath);
/** 向响应中写入文件,并写入结束标志
 * o:数据流对象; boundary:数据分隔符; key:参数名称; fpath:文件地址 */
extern int httpformdata_writelastfile(offsetptr_p o, const char *boundary, const char* key, char* fpath);


/**站点地址映射,如果是有效地址则返回映射后的地址,伪地址则返回NULL @return 真实URL地址(http,https,ftp)返回malloc出来的地址空间,否则返回NULL
 * burl:基网址; vpath:虚网址 * */
extern char* urlhandle_sitemap(char* burl, char* vpath);

#ifdef __cplusplus
};
#endif

#endif