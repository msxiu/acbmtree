# AcbmTree 使用说明
ACBM算法是在AC自动机的基础之上，引入了BM算法的多模扩展，实现的高效的多模匹配。和AC自动机不同的是，ACBM算法不需要扫描目标文本串中的每一个字符，可以利用本次匹配不成功的信息，跳过尽可能多的字符，实现高效匹配。

### 编译与安装
````
chmod +x configure
./configure
make
make install
````
### 使用步骤
1. 定义对象并初始化
````
/** acbm树关键词重置 
 * ptree:匹配树;argc:关键词对象个数;argv:关键词对象列表 */
extern int acbmtree4keywords(acbmtree_p ptree, int argc, const char* argv[]);
/** 从文本文件中加载关键词，一行为一个关键词 */
extern int acbmtree4filelines(acbmtree_p o, const char* faddr);
/** acbm树关键词重置 
 * ptree:匹配树;fld:关键词对象的反射类型;records:固定大小对象关键词列表指针;recordcount:关键词对象个数; */
extern int acbmtree4records (acbmtree_p ptree, reflect_p fld, const void* records, size_t recordcount);
````
2. 使用ACBM树进行数据匹配查找
````
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
````
3. 回收资源
````
/**回收资源,不回收对象指针*/
extern void acbmtree_reclaim(acbmtree_t *ptree);
/**回收资源,并回收对象指针 */
extern void acbmtree_destory (acbmtree_t *ptree);
````

### 简单示例
````
#include "libacbmtree.h"

static acbmtree_t acbm;//1. 定义ACBM匹配对象

/** 做acbm算法匹配 */
static void acbmtree_content(const char* original) {
    int nmatched = 0, i=0;
    acbmtree_match_p pmatcheds = NULL, item = NULL;//匹配结果
    if((nmatched = acbmtree_search(&acbm, original, strlen(original), &pmatcheds, 0))>0) {//2. 使用ACBM树，执行AC查找
        for(i=0; i<nmatched; i++) {
            item = pmatcheds+i;
            fprintf(stdout, "\n{position:%lu,key:'%s'},", item->offset, item->pattern->data);
        }
        free(pmatcheds);//回收资源
    }
}
/** 读取文件中内容，提交acbm算法处理 */
static void acbmtree_file(const char* addr) {
	FILE *fp;
	char* buffer = NULL;
	int rlen = 0, flen=0, tln;
	if(addr && (fp = fopen(addr, "rb"))) {//成功打开文件
		fseek(fp, 0L, SEEK_END);
		flen = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
		if((buffer = (char*)calloc(1, flen + 10))) {
			while(rlen < flen){
				tln = fread(buffer + rlen, 1, (flen - rlen), fp);
				rlen += tln;
			}
            fprintf(stdout, "--%s---------------------------\n", addr);
            acbmtree_content(buffer);
		}
		fclose(fp);
		free(buffer);
	}
}

int main(int argc, const char* argv[]) {
    int i = 0;
    const char* rulefile;
    if(parameters_find(argc, argv, "-c", &rulefile)) {//指定了规则文件
        if((i = parameters_indexof(argc, argv, "-f", 0))) {// 指定了过滤文件列表需要处理
            acbmtree4filelines(&acbm, rulefile);//1. 加载规则文件,初始化ACBM树
            for(;i<argc; i++) {//遍历文件,提交处理
                acbmtree_file(argv[i]);
            }
            acbmtree_reclaim(&acbm);//3. 回收ACBM资源
        }
    }
    return 0;
}
````