/*
 * llqactree.h
 *  Created on: 2017-11-22
 *      Author: Administrator
 *      ACBM多模匹配算法
acbmtree_match_t* item = NULL;
acbmtree_t acbm = ACTREE_INITIALIZE;
acbmtree_match_p pmatcheds = NULL;
//acbmtree_keywords_records, acbmtree_keywords_list
if(acbmtree_keywords_records(&acbm, rs, rcnt, rsize, keyoffset)) {//从文件中加载关键词
	acbmtree_search(&acbm, buffer, flen, pmatcheds, 0);//
	varyarray_foreach(pmatcheds, p, item, acbmtree_match_t*){//遍历输出结果
		fprintf(o->stream, "%d=>{position:%d,key:'%s'}\n", p, item->offset, item->pattern->data);
	}
	free(pmatcheds);//回收资源
	acbmtree_reclaim(&acbm);//回收资源
}
 */

#ifndef LLQACTREE_H_
#define LLQACTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "llqreflect.h"

#ifdef __cplusplus
extern "C" {
#endif


//单个关键词的最大长度
#define ACTREE_PATTERN_LEN		512
//每个节点最大子节点数
#define ACTREE_MAX_CHILDS		256

struct acbmtree_node;
typedef struct acbmtree_node  acbmtree_node_t, *  acbmtree_node_p;

struct acbmtree_node {//关键词节点树
	unsigned char ch;//节点对应的字符
	unsigned char gonech;//gshift算法中保存的跳转字节个数
	int label;//表示到该节点有关键词出口,记录出口关键词下标{-1:没有出口,-2:根节点}
	int depth;//关建词首字母到当前字母的深度
	int gshift;//根据设置关键词,计算出来的当前节点的跳过字节数
	int nchild;//子节点个数
	acbmtree_node_p childs[ACTREE_MAX_CHILDS];//子节点
	acbmtree_node_p parent;//父节点
};

typedef struct acbmtree_key {//一个匹配关键词
	unsigned char data[ACTREE_PATTERN_LEN] ;//匹配关键词
	int len ;//关键词长度
	void* ptr;//附加参数指针
} acbmtree_key_t, * acbmtree_key_p;


typedef struct acbmtree_match {//匹配的一个结果单元
	acbmtree_key_t *pattern;//pattern_data关键词数组下标
	size_t offset ;//在文本中的位置
} acbmtree_match_t, *acbmtree_match_p;


typedef struct acbmtree {//编译生成好的匹配关键词对象树
	acbmtree_node_t *root;// 树根节点
	int max_depth;//关键词最大深度
	int min_pattern_size;//关键词的最短长度
	int bcshift[256];//坏字符跳转表

	acbmtree_key_p pattern_list;//指向节点数组第一个字串的指针
	int pattern_count;// 包含关键词字串符个数
} acbmtree_t, * acbmtree_p;
#define ACBMTREE_INITIALIZE	{NULL, 0, 0, {0}, NULL, 0}


/** acbm树关键词重置 
 * ptree:匹配树;argc:关键词对象个数;argv:关键词对象列表 */
extern int acbmtree4keywords(acbmtree_p ptree, int argc, const char* argv[]);
/** 从文本文件中加载关键词，一行为一个关键词 */
extern int acbmtree4filelines(acbmtree_p o, const char* faddr);
/** acbm树关键词重置 
 * ptree:匹配树;fld:关键词对象的反射类型;records:固定大小对象关键词列表指针;recordcount:关键词对象个数; */
extern int acbmtree4records (acbmtree_p ptree, reflect_p fld, const void* records, size_t recordcount);
/** 显示加载的关键词 */
extern void acbmtree_keywords_show(acbmtree_p ptree, FILE* out);


/** 匹配数据内容是否以多模匹配树中搜索关键词开头，返回找到的关键词的位置,未找到返回-1
 * original：查找关键词的内容数据；tlen：查找内容长度;matchedkey:匹配到的关键词对象，无需释放
 * maxoffset:从头开始的最大偏移字节数 */
extern int acbmtree_startwith(acbmtree_t *ptree, const void *original, int tlen, acbmtree_key_p *matchedkey, int maxoffset);
/** 匹配数据内容是否以多模匹配树中搜索关键词结尾，返回找到的关键词的位置,未找到返回-1
 * original：查找关键词的内容数据；tlen：查找内容长度;matchedkey:匹配到的关键词对象，无需释放
 * maxoffset:从头开始的最大偏移字节数 */
extern int acbmtree_endwith(acbmtree_t *ptree, const void *original, int tlen, acbmtree_key_p *matchedkey, int maxoffset);
/** 从指定的多模匹配树中搜索关键词;返回匹配到的关键词个数
 * original：查找关键词的内容数据；tlen：查找内容长度;results:匹配到的关键词结果,由搜索函数申请内存，外部负责释放
 * nreply:匹配结果中是否去掉重复告警,0:产生重复,1:不产生重复 */
extern int acbmtree_search(acbmtree_t *ptree, const void *original, int tlen, acbmtree_match_p* results, int nreply);


/**回收资源,不回收对象指针*/
extern void acbmtree_reclaim(acbmtree_t *ptree);
/**回收资源,并回收对象指针 */
extern void acbmtree_destory (acbmtree_t *ptree);


#ifdef __cplusplus
}
#endif


#endif /* LLQACTREE_H_ */
