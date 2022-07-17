/* SPDX-License-Identifier: GPL-2.0+
 * 自平衡二叉树的完美实现
 *
 *  Copyright (c) 2020-2022 tybitsfox <tybitsfox@163.com>
 */


#include"clsscr.h"
#include<time.h>
//2^24
#define	MINYCNT				16888800
typedef struct _TREE
{
	int vol;
	struct _TREE *top;
	struct _TREE *left;
	struct _TREE *right;
	unsigned short ld,lm;
	unsigned short rd,rm;
} _TR;

_TR		*root=NULL,*last=NULL,*deep_last=NULL;
_TR		**s=NULL;
int count=0;//numbers of all point;use for calc normally deep
int cc=0;

int calc_deep(unsigned long c);
int tree_sort_list(_TR *t,_TR **save,int cnt);
int tree_balance();
int tree_ins(_TR *t,int i);
int tree_max(_TR *t1);
//{{{int main(int argc,char **argv)
int main(int argc,char **argv)
{
	_TR	*t=NULL;
	int i,j,k;
	k=MINYCNT;
	if(argc == 2)
	{
		k=atoi(argv[1]);
		if((k <= 1) || k >= 33554432) //2^25
			k=MINYCNT;
	}
	unsigned char *p=malloc(sizeof(_TR)*k);
	unsigned char *q=malloc(sizeof(_TR*)*k);
	if(p == NULL)
		return 0;
	if(q == NULL)
	{free(p);return 0;}
	memset(p,0,sizeof(_TR)*k);
	memset(q,0,sizeof(_TR*)*k);
	t=(_TR *)p;s=(_TR **)q;
	srand((int)time(0));
	for(i=1;i<=k-1;i++)
	{
		j=rand()%(k*3);
		tree_ins(t,j);
		tree_balance();
		t++;
	}
	i=calc_deep(count);
	printf("ldmax=%d\tldmin=%d\trdmax=%d\trdmin=%d\tcount=%d\tdeep=%d\n",root->ld,root->lm,root->rd,root->rm,count,i);
	//i=tree_sort_list(root,s,count);
	printf("list count=%d\tneed=%d\n",i,cc);
	tree_max(last);
	/*for(j=1;j<=i;j++)
	{
		printf("%d\t",(s[j-1])->vol);
		if(j%20 == 0)
			printf("\n");
	}*/
	free(p);
	free(q);
	printf("\n");
	return 0;
};
//}}}
//{{{int calc_deep(unsigned long c)
//参数c=当前节点总数
int calc_deep(unsigned long c)
{
	int i;
	unsigned long m=2;
	if(root != NULL)
		i=1;
	else
		return 0;
	while(i<=31)	//set the max count of point;currently support 4G
	{
		if(c <= (m-1))
			return i;
		m=m*2;i++;
	}
	return 32;
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
		if(i>=cnt)
			return i;
		if(cur->left == NULL)
		{tmp=cur;save[i]=cur;i++;}
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
				if(i>=cnt)
					return i;
				tmp=cur;
				cur=cur->top;
				if(cur == NULL)
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
			deep_last=tmp;
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
			break;
		}
	}
	return 0;
};
//}}}
//{{{int tree_ins(_TR *t,int i)
int tree_ins(_TR *t,int i)
/*改进的插入函数，不再需要子结点的计数值，改为记录节点的深度值，且深度值自下向上累加，即叶节点深度为0*/	
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
		{last=NULL;return 0;}
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
		{
		/*	if(tmp->ld < k)
				tmp->ld=k;
			if(tmp->lm < d)
				tmp->lm=d;*/
			tmp->ld=k;tmp->lm=d;
		}
		else
		{
		/*	if(tmp->rd < k)
				tmp->rd=k;
			if(tmp->rm < d)
				tmp->rm=d;*/
			tmp->rd=k;tmp->rm=d;
		}
		c1=tmp;tmp=tmp->top;
	}
	return 0;
};
//}}}
//{{{int tree_max(_TR *t1,_TR *t2)
int tree_max(_TR *t1)
{
	int i,j,k;
	_TR *c,*tmp;
	j=root->ld-root->lm;
	k=root->rd-root->rm;
	i=j>k?j:k;
	if(i<=2)
		return 0;
	if(j>k)
		c=root->left;
	else
		c=root->right;
	k=2;
	while(c != NULL)
	{
		tmp=c;
		if(tmp->lm>tmp->rm)
			c=c->right;
		else
			c=c->left;
		printf("deep=%d\tld=%d\tlm=%d\trd=%d\trm=%d\n",k,tmp->ld,tmp->lm,tmp->rd,tmp->rm);
		k++;
	}
	return 0;	
};
//}}}








