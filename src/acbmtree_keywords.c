#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llqvarymem.h"
#include "acbmtree.h"

/** 递规调用回收一个已预编译好的关键词节点对象 */
void acbmtree_destory_childs (acbmtree_node_t *root) {
	int i;
	if(root) {
		for (i = 0 ; i < 256 ; i++) {
			if ((i >= 'A') && (i <= 'Z'))continue ;
			if (NULL != root->childs[i]) {
				acbmtree_destory_childs (root->childs[i]);
				free (root->childs[i]) ;
				root->childs[i] = NULL ;
			}
		}
	}
}


/**回收资源,不回收对象指针*/
void acbmtree_reclaim(acbmtree_t *ptree) {
	if (ptree) {
		if (ptree->root) {
			acbmtree_destory_childs (ptree->root) ;
			free (ptree->root) ;
			ptree->root = NULL ;
		}
		if(ptree->pattern_list){
			free(ptree->pattern_list);
			ptree->pattern_list = NULL;
		}
	}
}
/**回收资源,并回收对象指针 */
void acbmtree_destory (acbmtree_t *ptree) {
	if(ptree) {
		acbmtree_reclaim(ptree);
		free (ptree) ;
	}
}

/** 显示加载的关键词 */
void acbmtree_keywords_show(acbmtree_p ptree, FILE* out) {
	int i=0;
	acbmtree_key_p item;
	if(ptree && ptree->pattern_count > 0) {
		for (i=0; i<ptree->pattern_count; i++) {
			item = ((ptree->pattern_list) + i);
			fprintf(out, "%d:'%.*s'\n", i, item->len, (const char*)item->data);
		}
	}
}


/** 申请关键词大小 
 * ptree:匹配树;count:关键词对象个数 */
static int acbmtree_keywords_alloc(acbmtree_t *ptree, int count) {
	if(ptree) {
		acbmtree_reclaim(ptree);//回收原有记录
		if((ptree->pattern_list = (acbmtree_key_t*)calloc(count, sizeof(acbmtree_key_t)))) {
			ptree->pattern_count = count;
			return 1;
		}
	}
	return 0;
}


/** 设置下标关键词 
 * ptree:匹配树;idx:关键词下标;key:关键词数据;par:附加参数 */
static int acbmtree_keywords_set(acbmtree_t *ptree, int idx, const char* key, void* par) {
	int ret = 0;
	if(ptree && key) {
		acbmtree_key_t *o= ((ptree->pattern_list)+idx);
		if((o->len = strlen(key)) >= ACTREE_PATTERN_LEN) {//处理超长的关键词
			o->len = ACTREE_PATTERN_LEN-1;
			memcpy(o->data, key, o->len);
			ret = 2;
		} else {
			strcpy((char*)o->data, key);
			ret = 1;
		}
		o->ptr = par;//赋值参数
	}
	return ret;
}


/** acbm树关键词重置 
 * ptree:匹配树;argc:关键词对象个数;argv:关键词对象列表 */
 int acbmtree4keywords(acbmtree_p ptree, int argc, const char* argv[]) {
	int i, count = 0, ret = 0;
	if(ptree && argv && argc> 0 && acbmtree_keywords_alloc(ptree, argc)) {
		for(i=0; i<argc; i++) {
			if(argv[i]  && acbmtree_keywords_set(ptree, i, argv[i], NULL)) {
				count++;
			}
		}
		if(acbmtree_compile(ptree)) {
			ret = count;
		}
		acbmtree_shifts_attach (ptree);
	}
	return ret;
}
/** 从文本文件中加载关键词，一行为一个关键词 */
int acbmtree4filelines(acbmtree_p o, const char* faddr) {
    FILE* fp;
	int ret=0, rlen = 0, flen=0, tln, idx = 0;//加载关键词个数
	char* buffer = NULL, *key= NULL, *next;
    if(o && faddr && (fp = fopen(faddr, "rb"))) {
    	const char* keywords[10240];//最大只支持10000个关键词
		fseek(fp, 0L, SEEK_END);
		flen = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		if((buffer = (char*)calloc(1, flen + 1))) {
			while(rlen < flen){
				tln = fread(buffer + rlen, 1, (flen - rlen), fp);
				rlen += tln;
			}
			key = buffer;
			memset(keywords, 0, sizeof(keywords));
			while(*key) {
				while('\t' == *key || '\v' == *key || 32 == *key) { key++; }//跳过行前空格
				if('#' != *key) {//跳过注示关键词
					keywords[idx++] = key;
				}            
				if(!(next = strstr(key, "\n"))) break;
				*next = 0;
				if('\r' == *(next - 1) || '\n' == *(next-1)) { *(next-1) = 0; }
				next++;
				while('\r' == *next || '\n' == *next) { next++; }
				key = next;
			}
			acbmtree4keywords(o, idx, keywords);
            free(buffer);
            ret = 1;
		}
		fclose(fp);
    }
    return ret;
}

/** acbm树关键词重置 
 * ptree:匹配树;fld:关键词对象的反射类型;records:固定大小对象关键词列表指针;recordcount:关键词对象个数; */
 int acbmtree4records (acbmtree_p ptree, reflect_p fld, const void* records, size_t recordcount) {
	void* r;
	const char* key = NULL;
	size_t i, count = 0, ret = 0;
	if(ptree && records && recordcount> 0 && acbmtree_keywords_alloc(ptree, recordcount)) {
		for(i=0; i<recordcount; i++) {
			r = (void*)(((const char*)records)+(i * fld->entire));
			key = reflect_getstring(fld, r);
			if(key  && acbmtree_keywords_set(ptree, i, key, r)) {
				count++;
			}
		}
		if(acbmtree_compile(ptree)) {
			ret = count;
		}
		acbmtree_shifts_attach (ptree) ;
	}
	return ret;
}