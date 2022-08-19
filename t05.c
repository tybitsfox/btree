/* SPDX-License-Identifier: GPL-2.0+
 * 自平衡二叉树的完美实现
 * 2022-7-25已经可以生成完美的自平衡二叉树，测试的2^n-1的数据量插入都可生成完美的n层二叉树，但大批量数据的连续插入效率还有待改善。
 * 2022-8-18已完美实现自平衡二叉树的全部功能：节点插入，左右子树的平衡，节点的删除，树的遍历。其中树的平衡使用三个可选的函数实现：
 * btree_balance(旋转，移动节点实现)，tree_b_mov(自上而下移动节点内数值实现)和tree_v_mov(自下(新节点)而上的数值移动实现的平衡)。
 * 这三个函数可以单独使用，也可节点移动的函数和其他两个数值移动的任意一个函数配合使用。单独使用tree_balance函数生成树的效率最高，但
 * 无法生成完美的平衡二叉树（类似于红黑树）。单独使用tree_b_mov或tree_v_mov可以生成完美的平衡二叉树，但生成树的效率不高。我这里采用的
 * 是同时使用tree_balance和tree_b_mov两个函数。效率折中，又可生成完美的平衡二叉树。
 * 本方法实现的依据是将节点数据结构设计为含有层数计数的结构：ld=left deep；rd=right deep;lm=minimum left deep;rm=minimum right deep;
 * 即ld和rd代表了当前节点最大的左子树和右子树层数，lm和rm代表了当前节点最小的左子树和右子树层数。计算方法是ld=左子结点的ld和rd的最大值+1
 * lm=左子结点的lm和rm的最小值+1。
 * 这种实现方法我不知道是否有人做过。再写这个之前，我仅是查看了平衡二叉树、红黑树和B+树的概念介绍。
 *  Copyright (c) 2020-2022 tybitsfox <tybitsfox@163.com>
 */


#include"clsscr.h"
#include<time.h>
//2^17-1
#define	MINYCNT				131071
#define EXTBUF				100
typedef struct _TREE
{
	int vol;
	struct _TREE *top;
	struct _TREE *left;
	struct _TREE *right;
/*	union{ unsigned int deep;
		unsigned char ld[4];
	}; */
	unsigned char ld,lm;
	unsigned char rd,rm;
} _TR;

_TR		*root=NULL,*last=NULL;
_TR		**s=NULL;
int count=0;//number of all nodes;use for calc normally deep
int cc=0,zz=0; //for test
int lim[40];
int idx=0;

int tree_sort_list(_TR *t,_TR **save,int cnt);//traverse by value's size(only low to high)
int tree_balance();//balance by pointer
int tree_ins(_TR *t,int i);	//created new node
int tree_b_mov();			//balance by value,from root to leaf
int tree_v_mov();			//balance by value,from new leaf to root
int tree_del(_TR *t);		//delete a node
int test_all(int m);		//testing code
//{{{int main(int argc,char **argv)
int main(int argc,char **argv)
{
	_TR	*t=NULL;
	int i,j,k,l,m;
	k=MINYCNT;l=0;m=0;
	switch(argc)
	{
		case 2:
			k=atoi(argv[1]);
			if((k <= 1) || k >= 33554432) //2^25
				k=MINYCNT;
			break;
		case 3:
			k=atoi(argv[1]);
			if((k <= 1) || k >= 33554432) //2^25
				k=MINYCNT;
			m=atoi(argv[2]);
			if(m != 0)
				m=1;
	};
	unsigned char *p=malloc(sizeof(_TR)*(k+EXTBUF));
	unsigned char *q=malloc(sizeof(_TR*)*(k+EXTBUF));
	if(p == NULL)
		return 0;
	if(q == NULL)
	{free(p);return 0;}
	memset(p,0,sizeof(_TR)*(k+EXTBUF));
	memset(q,0,sizeof(_TR*)*(k+EXTBUF));
	t=(_TR *)p;s=(_TR **)q;j=1;
	for(i=0;i<28;i++)
	{
		j*=2;
		lim[i]=j-1;
	}
	srand((int)time(0));
	while(count < k)	//保证生成指定数量的节点
	{
		j=rand()%(k*5);
		if(tree_ins(t,j))
			continue;
		if(count > lim[idx])
			idx++;
		/*if(tree_balance()) //adjust by value
		{
			l=tree_v_mov(); //此时last有效
			if(l)
				break;
		}*/
		tree_balance();
		l=tree_b_mov();
		t++;
	}
	test_all(m);
	free(p);
	free(q);
	printf("\n");
	return 0;
};
//}}}
//{{{int tree_sort_list(_TR *t,_TR **save,int cnt)
/*传入参数：
t:		待排序的树的root
save:	保存的完成排序的_TR指针的队列，需先分配好内存空间
cnt:	待排序的树的元素个数
返回值：=0 错误；否则=cnt
*/
int tree_sort_list(_TR *t,_TR **save,int cnt)
{
	_TR *cur,*tmp;
	int i;
	if((t == NULL) || (save == NULL))
		return 0;
	cur=t;i=0;
	while(1)
	{
		if(cur->left == NULL)
		{save[i]=cur;i++;}
		else
		{
			while(cur->left != NULL)
			{cur=cur->left;}
			continue;
		}
		if(cur->right == NULL)//返回
		{
			while(1)
			{
				tmp=cur;
				cur=cur->top;
				if(cur == t->top)
					return i;
				if(cur->left == tmp) //左返回
				{
					save[i]=cur;i++;
					if(cur->right != NULL)
					{
						cur=cur->right;
						break;
					}
				}//右返回则继续循环
				else//右返回
				{
					if(cur == t)
						return i;
				}
			}
		}
		else
		{cur=cur->right;}
	}
	return 0; //can't be run at here
};
//}}}
//{{{int tree_balance()
/*调整的原则：左右子树的深度大于等于2即开始调整*/
int tree_balance()
{
	_TR *c1,*c2,*c3,*c4,*tmp;
	int i,j,k;
	if(last == NULL)
		return 0;
	c1=last;
	while(c1->top != NULL)
	{
		tmp=c1;c1=c1->top;
		i=(c1->ld>=c1->rd?(c1->ld-c1->rd):(c1->rd-c1->ld));
		if(i>=2)
		{
			c2=c1->top;
			if(c1->ld > c1->rd)//left
			{
				if(tmp->ld >= tmp->rd)//left,equal is impossible
				{
					c3=tmp->right;
					if(c3 != NULL)
						c3->top=c1;
					c1->top=tmp;c1->left=c3;c1->ld=tmp->rd;c1->lm=tmp->rm;
					tmp->top=c2;tmp->right=c1;tmp->rd=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
					tmp->rm=(c1->lm >= c1->rm?(c1->rm+1):(c1->lm+1));
				}
				else//right
				{
					c3=tmp;tmp=tmp->right;c4=tmp->left;
					if(c4 != NULL)
						c4->top=c3;
					c3->right=tmp->left;c3->top=tmp;c3->rd=tmp->ld;c3->rm=tmp->lm;c4=tmp->right;
					if(c4 != NULL)
						c4->top=c1;
					c1->left=tmp->right;c1->top=tmp;c1->ld=tmp->rd;c1->lm=tmp->rm;
					tmp->top=c2;tmp->left=c3;tmp->right=c1;
					tmp->ld=(c3->ld >= c3->rd?(c3->ld+1):(c3->rd+1));
					tmp->rd=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
					tmp->lm=(c3->lm >= c3->rm?(c3->rm+1):(c3->lm+1));
					tmp->rm=(c1->lm >= c1->rm?(c1->rm+1):(c1->lm+1));
				}
			}
			else//right
			{
				if(tmp->ld >= tmp->rd)//left,equal is impossible
				{
					c3=tmp;tmp=tmp->left;c4=tmp->right;
					if(c4 != NULL)
						c4->top=c3;
					c3->left=tmp->right;c3->top=tmp;c3->ld=tmp->rd;c3->lm=tmp->rm;c4=tmp->left;
					if(c4 != NULL)
						c4->top=c1;
					c1->right=tmp->left;c1->top=tmp;c1->rd=tmp->ld;c1->rm=tmp->lm;
					tmp->top=c2;tmp->left=c1;tmp->right=c3;
					tmp->ld=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
					tmp->rd=(c3->ld >= c3->rd?(c3->ld+1):(c3->rd+1));
					tmp->lm=(c1->lm >= c1->rm?(c1->rm+1):(c1->lm+1));
					tmp->rm=(c3->lm >= c3->rm?(c3->rm+1):(c3->lm+1));
				}
				else//right
				{
					c3=tmp->left;
					if(c3 != NULL)
						c3->top=c1;
					c1->top=tmp;c1->right=c3;c1->rd=tmp->ld;c1->rm=tmp->lm;
					tmp->top=c2;tmp->left=c1;tmp->ld=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
					tmp->lm=(c1->lm >= c1->rm?(c1->rm+1):(c1->lm+1));
				}
			}
			if(c2 == NULL)
			{root=tmp;}
			else
			{
				if(c2->left == c1)
				{
					c2->left=tmp;
					c2->ld=tmp->ld >= tmp->rd?(tmp->ld+1):(tmp->rd+1);
					c2->lm=tmp->lm >= tmp->rm?(tmp->rm+1):(tmp->lm+1);
				}
				else
				{
					c2->right=tmp;
					c2->rd=tmp->ld >= tmp->rd?(tmp->ld+1):(tmp->rd+1);
					c2->rm=tmp->lm >= tmp->rm?(tmp->rm+1):(tmp->lm+1);
				}
				while(c2->top != NULL)
				{
					tmp=c2;c2=c2->top;
					if(c2->left == tmp)
					{
						c2->ld=(tmp->ld >= tmp->rd?(tmp->ld+1):(tmp->rd+1));
						c2->lm=(tmp->lm >= tmp->rm?(tmp->rm+1):(tmp->lm+1));
					}
					else
					{
						c2->rd=(tmp->ld >= tmp->rd?(tmp->ld+1):(tmp->rd+1));
						c2->rm=(tmp->lm >= tmp->rm?(tmp->rm+1):(tmp->lm+1));
					}
				}
			}
			//tree_max(deep_last);
			last=NULL;
			return 0;
		}
	}
	return 1;
};
//}}}
//{{{int tree_ins(_TR *t,int i)
int tree_ins(_TR *t,int i)
/*改进的插入函数，记录节点的深度值，且深度值自下向上累加，即叶节点深度为1*/	
{
	_TR *c1,*tmp;
	int d,k;
	if(root == NULL)
	{root=t;t->vol=i;root->ld=root->rd=1;root->lm=root->rm=1;last=t;count++;return 0;}
	c1=root;k=0;
	while(c1 != NULL)
	{
		tmp=c1;
		if(c1->vol == i)//剔除相同项
		{last=NULL;return 1;}
		c1=((c1->vol > i)?(c1->left):(c1->right));
	}
	t->vol=i;t->top=tmp;t->ld=t->rd=1;count++;last=t;c1=t;
	c1->lm=c1->rm=1;
	if(tmp->vol > i)
		tmp->left=c1;
	else
		tmp->right=c1;
	while(tmp != NULL)
	{
		k=(c1->ld >= c1->rd?(c1->ld + 1):(c1->rd + 1));
		d=(c1->lm >= c1->rm?(c1->rm+1):(c1->lm+1));
		if(c1 == tmp->left)
		{tmp->ld=k;tmp->lm=d;}
		else
		{tmp->rd=k;tmp->rm=d;}
		c1=tmp;tmp=tmp->top;
	}
	return 0;
};
//}}}
//{{{int tree_v_mov()
/*这是一个自叶向根的平衡移动，好处是尽量减少每次移动的数据量，但移动次数要多*/
int tree_v_mov()
{
	_TR *c,*c1,*t,*max,*min;
	int i,j,k,l,v1,v2;
	if(last == NULL)
		return 0;
	c=last;t=NULL;k=0;min=NULL;max=last;l=count;//2^10+6
	while(c->top != NULL)
	{
		t=c;c=c->top;k++;
		if(c->left == t)//左返回
		{
			if(c->ld < (c->rm+2))
				continue;
			t=c;c=c->right;
			while(c != NULL)
			{
				min=c;
				if(c->lm > c->rm)
					c=c->right;
				else
					c=c->left;
			}
			if(min == NULL)
			{printf("error001\n");return 1;}
			memset((void*)s,0,sizeof(_TR*)*l);
			j=tree_sort_list(t,s,l);
			k=max->vol;t=max->top;v1=v2=-1;
			for(i=0;i<j;i++)
			{
				if(s[i] == max)
					v1=i;
				if(s[i] == min)
					v2=i;
			}
			if((v1 == -1) || (v2 == -1))
			{
				printf("error 005 \n");
				return 1;
			}
			k=0;
			for(i=v1;i<v2;i++)
			{
				l=s[i]->vol;s[i]->vol=k;k=l;
			}
			if(min->left == NULL)
			{min->left=max;max->top=min;max->vol=k;min->ld=min->lm=2;}
			else
			{min->right=max;max->top=min;max->vol=min->vol;min->vol=k;min->rd=min->rm=2;}
			if(t->left == max)
			{t->left=NULL;t->ld=t->lm=1;}
			else
			{t->right=NULL;t->rd=t->rm=1;}
			v1=1;break;
		}
		if(c->right == t) //右返回
		{
			if(c->rd < (c->lm+2))
				continue;
			t=c;c=c->left;
			while(c != NULL)
			{
				min=c;
				if(c->lm > c->rm)
					c=c->right;
				else
					c=c->left;
			}
			if(min == NULL)
			{printf("error007");return 1;}
			memset((void*)s,0,sizeof(_TR*)*l);
			j=tree_sort_list(t,s,l);
			k=max->vol;t=max->top;v1=v2=-1;
			for(i=0;i<j;i++)
			{
				if(s[i] == max)
					v1=i;
				if(s[i] == min)
					v2=i;
			}
			k=0;
			for(i=v1;i>v2;i--)
			{l=s[i]->vol;s[i]->vol=k;k=l;}
			if(min->right == NULL)
			{min->right=max;max->top=min;max->vol=k;min->rd=min->rm=2;}
			else
			{min->left=max;max->top=min;max->vol=min->vol;min->vol=k;min->ld=min->lm=2;}
			if(t->left == max)
			{t->left=NULL;t->ld=t->lm=1;}
			else
			{t->right=NULL;t->rd=t->rm=1;}
			v1=1;break;
		}
	}
	if((t == NULL) || (v1 != 1))
		return 0;
	k=0;
	while(k<=1) //调整两个子树的层数值
	{
		while(t->top != NULL)
		{
			i=t->ld>=t->rd?(t->ld+1):(t->rd+1);
			j=t->lm>=t->rm?(t->rm+1):(t->lm+1);
			c=t;t=t->top;
			if(t->left==c)
			{t->ld=i;t->lm=j;}
			else
			{t->rd=i;t->rm=j;}
		}
		t=max;k++;
	}
	cc++;
	return 0;
};
//}}}
//{{{int tree_b_mov()
/*自顶向下实现值移动的完美实现*/
int tree_b_mov()
{
	_TR *c,*t,*m1,*m2;
	int i,j,k,v1,v2;
	t=root;c=NULL;k=0;
	while(t != NULL)
	{
		i=t->ld - t->lm; j=t->rd - t->rm;
		if(i >= 2)
		{k++;t=t->left;continue;}
		if(j >= 2)
		{k++;t=t->right;continue;}
		c=t;
		break;//此时,t为分支点
	}
	if(k == 0)
	{
		c=root;
		i=c->ld > c->rd ?c->ld:c->rd;
		j=c->lm > c->rm ?c->rm:c->lm;
		if(i<(j+2))
			return 0;
		zz++;
	}
	m1=c;
	while(m1 != NULL)
	{
		if((m1->ld == 1) && (m1->rd == 1))
			break;
		if(m1->ld > m1->rd)
			m1=m1->left;
		else
			m1=m1->right;
	}
	m2=c;
	while(m2 != NULL)
	{
		if((m2->lm == 1) || (m2->rm == 1))
			break;
		if(m2->lm > m2->rm)
			m2=m2->right;
		else
			m2=m2->left;
	}
	v1=v2=-1;
	memset(s,0,sizeof(_TR*)*count);
	j=tree_sort_list(c,s,count);
	for(i=0;i<j;i++)
	{
		if(s[i] == m1)
		{v1=i;break;}
	}
	for(i=0;i<j;i++)
	{
		if(s[i] == m2)
		{v2=i;break;}
	}
	k=m1->vol;t=m1->top;
	if(t->left == m1)
	{t->left=NULL;t->ld=t->lm=1;}
	else
	{t->right=NULL;t->rd=t->rm=1;}
	if(v1 < v2)
	{
		for(i=(v1+1);i<v2;i++)
		{j=s[i]->vol;s[i]->vol=k;k=j;}
		if(m2->left == NULL)
		{m2->left=m1;m1->top=m2;m1->vol=k;}
		else
		{m2->right=m1;m1->top=m2;m1->vol=m2->vol;m2->vol=k;}
	}
	else
	{
		for(i=(v1-1);i>v2;i--)
		{j=s[i]->vol;s[i]->vol=k;k=j;}
		if(m2->right == NULL)
		{m2->right=m1;m1->top=m2;m1->vol=k;}
		else
		{m2->left=m1;m1->top=m2;m1->vol=m2->vol;m2->vol=k;}
	}
	k=0;
	while(k<2)
	{
		while(t->top != NULL)
		{
			i=(t->ld >= t->rd ? (t->ld+1):(t->rd+1));
			j=(t->lm >= t->rm ? (t->rm+1):(t->lm+1));
			c=t;t=t->top;
			if(c == t->left)
			{t->ld=i;t->lm=j;}
			else
			{t->rd=i;t->rm=j;}
		}
		k++;t=m1;
	}
	cc++;
	return 0;
};
//}}}
//{{{int tree_del(_TR *t)
/*节点的删除函数，测试次数不多*/
int tree_del(_TR *t)
{
	_TR *c,*m,*c1,*c2,*c3;
	int i,j,k;
	c=t;m=c->top;
	if(c->ld >= c->rd) //调整大的子树
	{
		c1=c->left;c3=NULL;
		if(c1 == NULL) //c is a leaf
		{
			if(m==NULL)
			{root=NULL;count=0;return 0;}
			if(m->left==c)
			{m->left=NULL;m->ld=m->lm=1;}
			else
			{m->right=NULL;m->rd=m->rm=1;}
			goto del_01;
		}
		while(c1->right !=NULL)
		{c1=c1->right;}
		c2=c1->left;c3=c1->top;
		if(c3 == c)
			goto del_02;
		if(c2 != NULL)
		{
			c3->right=c2;c2->top=c3;
			c3->rd=(c2->ld > c2->rd ?(c2->ld+1):(c2->rd+1));
			c3->rm=(c2->lm > c2->rm ?(c2->rm+1):(c2->lm+1));
		}
		else
		{c3->right=NULL;c3->rd=c3->rm=1;}
		c->vol=c1->vol; //删除节点c1,将c1的数值移动至c节点，层数无需调整c节点，后面统一调整
		m=c3;
		goto del_01;
	}
	else
	{
		c1=c->right;c3=NULL;
		while(c1->left != NULL)
			c1=c1->left;
		c2=c1->right;c3=c1->top;
		if(c3 == c)
			goto del_02;
		if(c2 != NULL)
		{
			c3->left=c2;c2->top=c3;
			c3->ld=(c2->ld > c2->rd ?(c2->ld+1):(c2->rd+1));
			c3->lm=(c2->rd > c2->rm ?(c2->rm+1):(c2->lm+1));
		}
		else
		{c3->left=NULL;c3->ld=c3->lm=1;}
		c->vol=c1->vol;
		m=c3;
		goto del_01;
	}
del_02:
	if(m == NULL) //root
	{root=c1;c1->top=NULL;count--;return 0;}
	if(m->left==c)
	{
		m->left=c1;c1->top=m;
		m->ld=(c1->ld > c1->rd ?(c1->ld+1):(c1->rd+1));
		m->lm=(c1->lm > c1->rd ?(c1->rm+1):(c1->lm+1));
	}
	else
	{
		m->right=c1;c1->top=m;
		m->rd=c1->ld > c1->rd ? (c1->ld+1):(c1->rd+1);
		m->rm=c1->lm > c1->rm ? (c1->rm+1):(c1->lm+1);
	}
del_01:	
	while(m != root)
	{
		i=m->ld > m->rd ? (m->ld+1):(m->rd+1);
		j=m->lm > m->rm ? (m->rm+1):(m->lm+1);
		c1=m;m=m->top;
		if(m->left == c1)
		{m->ld=i;m->lm=j;}
		else
		{m->rd=i;m->rm=j;}
	}
	count--;
	return 0;
};
//}}}
//{{{int test_all(int m) 所有的测试代码移动到这个函数中
int test_all(int m)
{
	char ch[1024];
	int i,j,k,l;
	if(m == 0)
	{
		printf("ldmax=%d\tldmin=%d\trdmax=%d\trdmin=%d\tcount=%d\tdeep=%d\n",root->ld,root->lm,root->rd,root->rm,count,idx+1);
		i=tree_sort_list(root,s,count);
		printf("list count=%d\tmoved times=%d\tbig move=%d\n",i,cc,zz);
		return 0;
	}
	if(m == 1)
	{
		for(i=0;i<EXTBUF;i++)
		{
			printf("press 'q' to exit,a number of index to delete this node: ");
			memset(ch,0,sizeof(ch));
			fgets(ch,80,stdin);
			if(ch[0] == 'q')
				break;
			k=atoi(ch);
			if(k<0 || k >= count)
			{
				printf("error the index of tree\n");
				continue;
			}
			j=tree_sort_list(root,s,count);
			if(j>100)
			{printf("please to create a tree to less than 100 nodes for test!\n");return 0;}
			for(l=0;l<j;l++)
			{printf("<%d,%d,%d>\t",s[l]->vol,s[l]->ld,s[l]->rd);}
			printf("\nTarget delete index=%d value=%d\n",k,s[k]->vol);
			tree_del(s[k]);
			tree_b_mov();
			j=tree_sort_list(root,s,count);
			for(l=0;l<j;l++)
			{printf("%d\t",s[l]->vol);}
			printf("\n\n");
			memset(s,0,sizeof(_TR*)*(count+3));
		}
		return 0;
	}
	return 0;
};
//}}}


