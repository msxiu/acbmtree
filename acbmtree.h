#ifndef ACBM_SERVICE_H_
#define ACBM_SERVICE_H_
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

#include "libacbmtree.h"

#define MAXLEN_FOLDER       512

#ifndef INLINE
#ifdef WIN32	//windows平台下的宏定义
#define INLINE	__inline
#else
#define INLINE	inline
#endif	//---#if defined(WIN32)
#endif

#define IS_NEWLINE(c)			('\r' == (c) || '\n' == (c) || '\v' == (c))



#define PROMPT_DIAGNOSE(level, format, args...)    do{\
    if(penv->debug >= level){ \
        fprintf(stdout, "%s:%d=>", __FILE__, __LINE__); \
        fprintf(stdout, format, ##args);\
		fprintf(stdout, "%s", "\r\n");\
		fflush(stdout);\
    }\
}while(0)
#define PROMPT_DEBUG(format, args...)       PROMPT_DIAGNOSE(5, format, ##args)
#define PROMPT_INFO(format, args...)        PROMPT_DIAGNOSE(4, format, ##args)
#define PROMPT_WARN(format, args...)        PROMPT_DIAGNOSE(3, format, ##args)
#define PROMPT_ERROR(format, args...)       PROMPT_DIAGNOSE(2, format, ##args)
#define PROMPT_FATAL(format, args...)       PROMPT_DIAGNOSE(1, format, ##args)
#define PROMPT_NOTE(format, args...)        PROMPT_DIAGNOSE(0, format, ##args)


/** actree树关键词编译,@return 成功返回0,失败返回-1 
 * ptree:关键词初始化的树节点,即将关键词初始化到该树上 */
extern int acbmtree_compile (acbmtree_t *ptree);

/**计算ACBM多模匹配节点树的（好后缀/坏字符）跳转逻辑 
 * ptree:预处理好的匹配树 */
extern int acbmtree_shifts_attach (acbmtree_t *ptree);


#endif