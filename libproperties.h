#ifndef LLQ_PROPERTIES_H_
#define LLQ_PROPERTIES_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "llqreflect.h"

#ifdef __cplusplus
extern "C" {
#endif

struct properties {
	unsigned int size;//数据的字节长度
	char buffer[];//可变数据存储区
};
typedef struct properties properties_t, 		* properties_p;
typedef struct properties propertysection_t,	* propertysection_p;//属性对

/**从缓存中加载ini配置, return 返回解析到的参数个数 
 * @param faddr:	文件地址
 * @param trim:		是否清空头尾空格(0:不清除,1:清除头部,2:清除尾部,3:头尾都清除) */
extern properties_p properties_loadbody(const char* buf, int bln, int trim);
/**从缓存中加载ini配置, return 返回解析到的参数个数 
 * @param faddr:	文件地址
 * @param trim:		是否清空头尾空格(0:不清除,1:清除头部,2:清除尾部,3:头尾都清除) */
extern properties_p properties_loader(const char* faddr, int trim);

/**用于在while循环中查找是否有下一个记录*/
extern int properties_nexof(properties_p o, size_t* offset, const char** section, const char** key, const char** value);
extern const char* properties_valueof(properties_p o, const char* psection, const char* pkey);
extern propertysection_p properties_defaultsection(properties_p o);//获得默认节点


extern propertysection_p propertysection_find(properties_p o, const char *section);//通过段名查找属性段
extern int propertysection_loader(properties_p o, int argc, propertysection_p argv[]);//加载全部段
extern int propertysection_nexof(propertysection_p o, size_t* offset, const char** key, const char** value);
extern const char* propertysection_valueof(propertysection_p o, const char* pkey);


extern int properties2entiry(properties_p o, void* e, reflect_p flds);
/*** 从配置文件中加载到实体对象中 */
extern int propertiesentity4file(void* entity, reflect_p flds, const char* faddr);

#ifdef __cplusplus
}
#endif

#endif