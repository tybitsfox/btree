/* SPDX-License-Identifier: GPL-2.0+
 * 自平衡二叉树的完美实现
 *
 *  Copyright (c) 2020-2022 tybitsfox <tybitsfox@163.com>
 */


#include"clsscr.h"
#include<time.h>

typedef struct _TREE
{
	int vol;
	struct _TREE *top;
	struct _TREE *left;
	struct _TREE *right;
	unsigned short ld;
	unsigned short rd;
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
	unsigned char *p=malloc(sizeof(_TR)*80000);
	unsigned char *q=malloc(sizeof(_TR*)*80000);
	if(p == NULL)
		return 0;
	if(q == NULL)
	{free(p);return 0;}
	memset(p,0,sizeof(_TR)*80000);
	memset(q,0,sizeof(_TR*)*80000);
	t=(_TR *)p;s=(_TR **)q;
	srand((int)time(0));
	for(i=1;i<=52300;i++)
	{
		j=rand()%150000;
		tree_ins(t,j);
		tree_balance();
		t++;
	}
	i=calc_deep(count);
	printf("ldmax=%d\trdmax=%d\tcount=%d\tdeep=%d\n",root->ld,root->rd,count,i);
	i=tree_sort_list(root,s,count);
	printf("list count=%d\tneed=%d\n",i,cc);
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
					c1->top=tmp;c1->left=c3;c1->ld=tmp->rd;
					tmp->top=c2;tmp->right=c1;tmp->rd=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
				}
				else//right
				{
					c3=tmp;tmp=tmp->right;c4=tmp->left;
					if(c4 != NULL)
						c4->top=c3;
					c3->right=tmp->left;c3->top=tmp;c3->rd=tmp->ld;c4=tmp->right;
					if(c4 != NULL)
						c4->top=c1;
					c1->left=tmp->right;c1->top=tmp;c1->ld=tmp->rd;
					tmp->top=c2;tmp->left=c3;tmp->right=c1;
					tmp->ld=(c3->ld >= c3->rd?(c3->ld+1):(c3->rd+1));
					tmp->rd=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
				}
			}
			else//right
			{
				if(tmp->ld >= tmp->rd)//left,equal is impossible
				{
					c3=tmp;tmp=tmp->left;c4=tmp->right;
					if(c4 != NULL)
						c4->top=c3;
					c3->left=tmp->right;c3->top=tmp;c3->ld=tmp->rd;c4=tmp->left;
					if(c4 != NULL)
						c4->top=c1;
					c1->right=tmp->left;c1->top=tmp;c1->rd=tmp->ld;
					tmp->top=c2;tmp->left=c1;tmp->right=c3;
					tmp->ld=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
					tmp->rd=(c3->ld >= c3->rd?(c3->ld+1):(c3->rd+1));
				}
				else//right
				{
					c3=tmp->left;
					if(c3 != NULL)
						c3->top=c1;
					c1->top=tmp;c1->right=c3;c1->rd=tmp->ld;
					tmp->top=c2;tmp->left=c1;tmp->ld=(c1->ld >= c1->rd?(c1->ld+1):(c1->rd+1));
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
				}
				else
				{
					c2->right=tmp;
					c2->rd=tmp->ld >= tmp->rd?(tmp->ld+1):(tmp->rd+1);
				}
				while(c2->top != NULL)
				{
					tmp=c2;c2=c2->top;
					if(c2->left == tmp)
						c2->ld=(tmp->ld >= tmp->rd?(tmp->ld+1):(tmp->rd+1));
					else
						c2->rd=(tmp->ld >= tmp->rd?(tmp->ld+1):(tmp->rd+1));
				}
			}
			tree_max(deep_last);
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
	{root=t;t->vol=i;last=t;count++;return 0;}
	c1=root;k=0;
	while(c1 != NULL)
	{
		if(c1->vol == i)//剔除相同项
		{last=NULL;return 0;}
		if(c1->vol > i)//left
		{tmp=c1;c1=c1->left;}
		else
		{tmp=c1;c1=c1->right;}
	}
	t->vol=i;t->top=tmp;count++;last=t;
	if(tmp->vol > i)
	{tmp->left=t;tmp->ld=1;}
	else
	{tmp->right=t;tmp->rd=1;}
	k=tmp->ld>=tmp->rd?tmp->ld:tmp->rd;
	while(tmp->top != NULL)
	{
		k++;c1=tmp;tmp=tmp->top;
		if(c1 == tmp->left)
		{
			if(tmp->ld == k)
				return 0;
			else
				tmp->ld=k;
		}
		else
		{
			if(tmp->rd == k)
				return 0;
			else
				tmp->rd=k;
		}
	}
	return 0;
};
//}}}
//{{{int tree_max(_TR *t1,_TR *t2)
int tree_max(_TR *t1)
{
	int i,k;
	_TR *c,*tmp;
	if(t1 == NULL)
		return 0;
	c=t1->left;
	k=c->ld >= c->rd?(c->ld-c->rd):(c->rd-c->ld);
	if(k>=2)
		cc++;
	c=t1->right;
	k=c->ld >= c->rd?(c->ld-c->rd):(c->rd-c->ld);
	if(k>=2)
		cc++;
	return 0;	
};
//}}}









