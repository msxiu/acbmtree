#include "acbmtree.h"

extern void acbmtree_destory_childs (acbmtree_node_t *root);
/** actree树关键词编译,@return 成功返回0,失败返回-1 
 * ptree:关键词初始化的树节点,即将关键词初始化到该树上 */
int acbmtree_compile (acbmtree_t *ptree) {
	unsigned char ch ;//当前字符
	int i, ch_i;
	int max_pattern_len = 0;
	int min_pattern_len = ACTREE_PATTERN_LEN ;
	acbmtree_node_t *root = NULL, *parent = NULL ;
	acbmtree_key_t *patterns;	//关键词对象列表
	int npattern;			//关键词对象个数

	if (ptree && ptree->pattern_list && (ptree->pattern_count > 0) && (root = (acbmtree_node_t*) malloc(sizeof (acbmtree_node_t)))) {
		patterns = ptree->pattern_list;
		npattern = ptree->pattern_count;
		memset (root, 0, sizeof (acbmtree_node_t)) ;
		root->label = -2 ;
		root->depth = 0 ;
		ptree->root = root ;

		for (i = 0 ; i < npattern ; i++)  {
			int pat_len ;//当前关键词的字节长度
			if ((pat_len = (patterns+i)->len) > 0) {
				if (pat_len > ACTREE_PATTERN_LEN) pat_len = ACTREE_PATTERN_LEN;
				if (pat_len > max_pattern_len) max_pattern_len = pat_len;
				if (pat_len < min_pattern_len) min_pattern_len = pat_len;
				parent = root ;
				//查找并跳过已预处理好的字节长度
				for (ch_i = 0 ; ch_i < pat_len ; ch_i++) {
					ch = tolower(((patterns+i)->data)[ch_i]);
					if (NULL == parent->childs[ch]) {
						break ;//发现未预处理的数据,跳出
					}
					parent = parent->childs[ch] ;
				}
				if (ch_i < pat_len) {//如果存在未预处理的字节数据
					for (; ch_i < pat_len ; ch_i++) {//遍历完成未预处理数据的预处理操作
						acbmtree_node_t *node = NULL ;
						ch = tolower(((patterns+i)->data)[ch_i]);
						if (NULL == (node = (acbmtree_node_t *) malloc(sizeof (acbmtree_node_t)))) {
							goto err ;
						}
						memset (node, 0, sizeof(acbmtree_node_t)) ;
						node->depth = ch_i + 1 ;
						node->ch = ch ;
						node->label = -1 ;
						parent->childs[ch] = node ;
						if ((ch >= 'a') && (ch <= 'z')) {//做忽略大小写处理
							parent->childs[ch-32] = node ;
						}
						parent->nchild++ ;
						parent->gonech = ch ;
						node->parent = parent ;
						parent = node ;//将当前节点设置成下一节点的父节点
					}
				}
			}
			parent->label = i ;//lable 记录字串来自于第几个输入字串
		}

		ptree->max_depth = max_pattern_len ;
		ptree->min_pattern_size = min_pattern_len ;

		return 1;
	}

err:// 出错处理,释放申请的空间
	if (ptree->root != NULL) {
		acbmtree_destory_childs (ptree->root) ;
		free (ptree->root) ;
		ptree->root = NULL ;
	}
	return 0;
}


/** 附加字母的最小关键词长度,对于没有出现关键词的字符,直接跳过最小关键词长度,有则计算最小长度
 * ptree:匹配树对象  */
static INLINE int acbmtree_bcshifts_attach (acbmtree_t *ptree) {
	unsigned char ch ;
	int i, j = 0 ;
	if(ptree) {
		for (i = 0 ; i < 256 ; i++) { ptree->bcshift[i] = ptree->min_pattern_size ; }//设置默认长度为最小关键词长度
		for (i = ptree->min_pattern_size - 1 ; i > 0 ; i--) {
			for (j = 0 ; j < ptree->pattern_count ; j++) {//遍历所有关键词
				ch = tolower((ptree->pattern_list+j)->data[i]) ;
				ptree->bcshift[ch] = i ;
				if ((ch > 'a') && (ch <'z')) {
					ptree->bcshift[ch-32] = i;
				}
			}
		}
	}
	return 0 ;
}


/**设置ptree树下关键词为key,深度为depth的gshift为shift, @return设置成功返回1,失败返回0
 * ptree：多模匹配树；key：设置关键词；depth：关键词的深度,即第几个字节；shift：设置的gshift值 */
int acbmtree_gshift_set (acbmtree_t *ptree, unsigned char *key, int depth, int shift) {
	int i ;
	acbmtree_node_t *node;
	if (ptree && (node = ptree->root)) {
		for (i = 0 ; i < depth ; i++) {
			if(!(node = node->childs[key[i]])) {
				return 0;
			}
		}
		node->gshift = ((node->gshift < shift) ? node->gshift : shift);
		return 1;
	}
	return 0;
}

/** 计算每个模式串第一个字符的gshift 
 * ptree:匹配树;pat1:第一个关键词;pat1_len:第一个关键词的长度;pat2:第二个关键词;pat2_len:第二个关键词的长度 */
static int acbmtree_gshift_calculate(acbmtree_t *ptree, unsigned char *pat1, int pat1_len, unsigned char *pat2, int pat2_len) {
	unsigned char first_char ;
	int i, j;
	int pat1_index, pat2_index, offset ;

	if (!ptree || !pat1 || !pat2 || pat1_len < 0 || pat2_len < 0) return 0;
	if (pat1_len == 0 || pat2_len == 0) return 1;
	first_char = tolower(pat1[0]) ;//pat1首字母
	for (i = 1 ; i < pat2_len ; i++) {//查找与first_char不相等字符出现的位置
		if (tolower(pat2[i]) != first_char) break ;
	}
	acbmtree_gshift_set (ptree, pat1, 1, i);//计算每个模式串第一个字符的gshift
	i = 1 ;
	while (1){
		while ((i < pat2_len) && (tolower(pat2[i]) != first_char)){
			i++ ;//查找与first_char相同的字节i的位置
		}
		if (i == pat2_len || i > ptree->min_pattern_size) {
			break ;// pat2剩余字符中未发现有pat1首字符相同字符,退出循环
		}
		offset = (pat2_index = i);//计算gshift值
		pat1_index = 0 ;
		while (pat2_index < pat2_len && pat1_index < pat1_len) {//比较pat1是pat2的pat2_index为开始的子串
			if (tolower(pat1[pat1_index]) != tolower(pat2[pat2_index])) break ;
			pat1_index++ ;//是比较位的字符的深度
			pat2_index++ ;
		}
		if (pat2_index == pat2_len) {// 关键字pat1前缀是关键字pat2后缀
			for (j = pat1_index ; j < pat1_len ; j++) {
				acbmtree_gshift_set (ptree, pat1, j+1, offset);
			}
		} else {
			acbmtree_gshift_set (ptree, pat1, pat1_index+1, offset) ;
		}
		i++ ;
	}
	return 1;
}

/**遍历关键词每个字母的跳转节数*/
static int acbmtree_gshift_attach (acbmtree_t *ptree) {
	int i = 0, j = 0 ;
	if(ptree) {
		for (i = 0 ; i < ptree->pattern_count ; i++) {//双重循环调用pattern_list每个关键词
			for (j = 0 ; j < ptree->pattern_count ; j++) {
				acbmtree_gshift_calculate (ptree,
						(ptree->pattern_list+i)->data, (ptree->pattern_list+i)->len,
						(ptree->pattern_list+j)->data, (ptree->pattern_list+j)->len
				) ;
			}
		}
	}
	return 0 ;
}
/**gshift查找初始化,设置每个节点的gshift默认值为shift*/
static int acbmtree_gshift_init (acbmtree_node_t *root, int shift) {
	int i ;
	if(root) {
		if (root->label != -2) root->gshift = shift;
		for (i = 0 ; i < 256 ; i++) {
			if (!((i >= 'A') && (i <= 'Z')) && (NULL != root->childs[i])) {
				acbmtree_gshift_init (root->childs[i], shift);
			}
		}
	}
	return 0 ;
}
/**计算ACBM多模匹配节点树的（好后缀/坏字符）跳转逻辑 
 * ptree:预处理好的匹配树 */
int acbmtree_shifts_attach (acbmtree_t *ptree) {
	if(ptree) {
		acbmtree_bcshifts_attach (ptree) ;//计算坏字节的跳过长度
		acbmtree_gshift_init (ptree->root, ptree->min_pattern_size);//循环赋值child->gshift=min_pattern_size
		acbmtree_gshift_attach (ptree) ;
		return 1;
	}
	return 0;
}

