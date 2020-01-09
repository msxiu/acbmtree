#include "llqvarymem.h"

#include "libproperties.h"
#include "llqparameters.h"
#include "acbmtree.h"

static acbmtree_t acbm;//ACBM匹配对象

/** 做acbm算法匹配 */
static void acbmtree_content(const char* original) {
    int nmatched = 0, i=0;
    acbmtree_match_p pmatcheds = NULL, item = NULL;//匹配结果
    if((nmatched = acbmtree_search(&acbm, original, strlen(original), &pmatcheds, 0))>0) {//执行AC查找
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
            acbmtree4filelines(&acbm, rulefile);//加载规则文件
            for(;i<argc; i++) {//遍历文件,提交处理
                acbmtree_file(argv[i]);
            }
            acbmtree_reclaim(&acbm);
        }
    }
    return 0;
}