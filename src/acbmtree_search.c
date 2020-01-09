#include "llqvarymem.h"
#include "acbmtree.h"



/** 查找matcheds结果集中是否包含key 
 * key:查找关键词;matcheds:查找匹配的结果集;nmatched:已匹配到的结果个数 */
static int matcheds_haskey(acbmtree_key_t* key, acbmtree_match_p matcheds, size_t nmatched) {
	int i=0;
	acbmtree_match_t* item = NULL;
	if(matcheds && key) {
		for(i=0;i<nmatched;i++){
			if((key->len == item->pattern->len) && (0 == memcmp(key->data, item->pattern->data, key->len))) {
				return 1;
			}
		}
	}
	return 0;
}
static int acbmtree_node_GoodSuffix(acbmtree_node_p node) {//求好后缀的最大长度
	if (node->nchild > 0) {
		return (node->childs[node->gonech] ? node->childs[node->gonech]->gshift : 0);
	}
	return -1;
}



/** 从指定的多模匹配树中搜索关键词;返回匹配到的关键词个数
 * original：查找关键词的内容数据；tlen：查找内容长度;results:匹配到的关键词结果,由搜索函数申请内存，外部负责释放
 * nreply:匹配结果中是否去掉重复告警,0:产生重复,1:不产生重复 */
int acbmtree_search(acbmtree_t *ptree, const void *original, int tlen, acbmtree_match_p* results, int nreply) {
	int nmatched = 0;//匹配到的结果个数
	unsigned char txtchidx = 0;//当前位置的字符
	register int base_index = 0;//字符串比较的当前位置
	register int cur_index = 0 ;//关键词查找的当前文本位置
	register int gs_shift = 0;//好后缀跳过的字数
	register int bc_shift = 0;//坏字符跳过的字数
	acbmtree_node_t *node = NULL ;
	const unsigned char* text = (const unsigned char*)original;

	if (ptree && original && ptree->min_pattern_size < tlen) {//只有当文本内容大于最小关键词长度时才比较
		base_index = (tlen - ptree->min_pattern_size );//从尾部开始比较
		uint32_t rcount=0, ridx = 0;//匹配结果申请内存总大小
		while (base_index >= 0) {//逐个遍历文本中的数据
			cur_index = base_index ;
			node = ptree->root ;
			while (NULL != node->childs[(txtchidx = text[cur_index])]) {//获取文本的当前字节数据
				node = node->childs[txtchidx] ;
				if (node->label >= 0) {//表示查找到关键词
					acbmtree_key_t* key=  (ptree->pattern_list + node->label);
					if(!nreply || !matcheds_haskey(key, *results, nmatched)) {
						acbmtree_match_t item = {key, (unsigned long)base_index};
						varyrecordset_append((void**)results, &item, sizeof(acbmtree_match_t), &rcount, &ridx);
						nmatched = (int)ridx;
					}
				}
				cur_index++;
				if (cur_index >= tlen)break;
			}
			if(-1 != (gs_shift = acbmtree_node_GoodSuffix(node))) {
				bc_shift =  (cur_index < tlen) ? (ptree->bcshift[txtchidx] + base_index - cur_index) : 1;
				base_index -= (gs_shift > bc_shift) ? gs_shift : bc_shift;//取大者跳转
			} else {
				base_index-- ;
			}
		}
	}
	return nmatched;
}


/** 匹配数据内容是否以多模匹配树中搜索关键词开头，返回找到的关键词的位置,未找到返回-1
 * original：查找关键词的内容数据；tlen：查找内容长度;matchedkey:匹配到的关键词对象，无需释放
 * maxoffset:从头开始的最大偏移字节数 */
int acbmtree_startwith(acbmtree_t *ptree, const void *original, int tlen, acbmtree_key_p *matchedkey, int maxoffset) {
	unsigned char txtchidx = 0;//当前位置的字符
	register int base_index = 0;//字符串比较的当前位置
	register int cur_index = 0 ;//关键词查找的当前文本位置
	register int gs_shift = 0;//好后缀跳过的字数
	register int bc_shift = 0;//坏字符跳过的字数
	acbmtree_node_p node = NULL ;
	const unsigned char* text = (const unsigned char*)original;

	if (ptree && original && ptree->min_pattern_size < tlen) {//只有当文本内容大于最小关键词长度时才比较
		base_index = 0;
		while (base_index <= maxoffset) {//逐个遍历文本中的数据
			cur_index = base_index ;
			node = ptree->root ;
			while (NULL != node->childs[(txtchidx = text[cur_index])]) {//获取文本的当前字节数据
				node = node->childs[txtchidx] ;
				if (node->label >= 0) {//表示查找到关键词
					*matchedkey = (ptree->pattern_list + node->label);
					return base_index;
				}
				cur_index++;
				if (cur_index >= tlen) break;
			}
			if(-1 != (gs_shift = acbmtree_node_GoodSuffix(node))) {
				bc_shift =  (cur_index < tlen) ? (ptree->bcshift[txtchidx] + base_index - cur_index) : 1;
				base_index += (gs_shift > bc_shift) ? gs_shift : bc_shift;//取大者跳转
			} else {
				base_index++;
			}
		}
	}
	return -1;
}

/** 匹配数据内容是否以多模匹配树中搜索关键词结尾，返回找到的关键词的位置,未找到返回-1
 * original：查找关键词的内容数据；tlen：查找内容长度;matchedkey:匹配到的关键词对象，无需释放
 * maxoffset:从头开始的最大偏移字节数 */
int acbmtree_endwith(acbmtree_t *ptree, const void *original, int tlen, acbmtree_key_p *matchedkey, int maxoffset) {
	unsigned char txtchidx = 0;//当前位置的字符
	register int base_index = 0;//字符串比较的当前位置
	register int cur_index = 0 ;//关键词查找的当前文本位置
	register int gs_shift = 0;//好后缀跳过的字数
	register int bc_shift = 0;//坏字符跳过的字数
	acbmtree_node_t *node = NULL ;
	const unsigned char* text = (const unsigned char*)original;

	if (ptree && original && ptree->min_pattern_size < tlen) {//只有当文本内容大于最小关键词长度时才比较
		base_index = (tlen - ptree->min_pattern_size );//从尾部开始比较
		while (base_index >= 0) {//逐个遍历文本中的数据
			cur_index = base_index ;
			node = ptree->root ;
			while (NULL != node->childs[(txtchidx = text[cur_index])]) {//获取文本的当前字节数据
				node = node->childs[txtchidx] ;
				if (node->label >= 0) {//表示查找到关键词
					*matchedkey = (ptree->pattern_list + node->label);
					return base_index;
				}
				cur_index++;
				if (cur_index >= tlen)break;
			}
			if(-1 != (gs_shift = acbmtree_node_GoodSuffix(node))) {
				bc_shift =  (cur_index < tlen) ? (ptree->bcshift[txtchidx] + base_index - cur_index) : 1;
				base_index -= (gs_shift > bc_shift) ? gs_shift : bc_shift;//取大者跳转
			} else {
				base_index-- ;
			}
		}
	}
	return -1;
}
