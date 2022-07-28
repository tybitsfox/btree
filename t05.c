/* SPDX-License-Identifier: GPL-2.0+
 * 自平衡二叉树的完美实现
 * 2022-7-25已经可以生成完美的自平衡二叉树，测试的2^n-1的数据量插入都可生成完美的n层二叉树，但大批量数据的连续插入效率还有待改善。
 *
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
	unsigned short ld,lm;
	unsigned short rd,rm;
} _TR;

_TR		*root=NULL,*last=NULL,*deep_last=NULL;
_TR		**s=NULL;
int count=0;//number of all nodes;use for calc normally deep
int cc=0;
int xx=0;
int calc_deep(unsigned long c);
int tree_sort_list(_TR *t,_TR **save,int cnt);//traverse by value's size(only low to high)
int tree_balance();//balance by pointer
int tree_ins(_TR *t,int i);//created new node
int tree_b_mov();	//balance by value
int tree_v_mov();
//{{{int main(int argc,char **argv)
int main(int argc,char **argv)
{
	_TR	*t=NULL;
	int i,j,k,l;
	char ch[80];
	k=MINYCNT;
	if(argc == 2)
	{
		k=atoi(argv[1]);
		if((k <= 1) || k >= 33554432) //2^25
			k=MINYCNT;
	}
	unsigned char *p=malloc(sizeof(_TR)*(k+EXTBUF));
	unsigned char *q=malloc(sizeof(_TR*)*(k+EXTBUF));
	if(p == NULL)
		return 0;
	if(q == NULL)
	{free(p);return 0;}
	memset(p,0,sizeof(_TR)*(k+EXTBUF));
	memset(q,0,sizeof(_TR*)*(k+EXTBUF));
	t=(_TR *)p;s=(_TR **)q;
	srand((int)time(0));
	while(count < k)	//保证生成指定数量的节点
	{
		j=rand()%(k*5);
		if(tree_ins(t,j))
			continue;
		//tree_balance();
		l=tree_v_mov();	//测试表明，在树生成后统一使用该函数调整，效率更低。
		if(l)
			break;
		t++;
	}
	j=calc_deep(count);
	printf("ldmax=%d\tldmin=%d\trdmax=%d\trdmin=%d\tcount=%d\tdeep=%d\n",root->ld,root->lm,root->rd,root->rm,count,j);
//	i=tree_sort_list(root,s,count);
	printf("list count=%d\tmoved times=%d\t nei loop=%d\n",j,cc,xx);
//	for(j=0;j<i;j++)
//		printf("%d\t",s[j]->vol);
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
			last=NULL;
			break;
		}
	}
	return 0;
};
//}}}
//{{{int tree_ins(_TR *t,int i)
int tree_ins(_TR *t,int i)
/*改进的插入函数，不再需要子结点的计数值，改为记录节点的深度值，且深度值自下向上累加，即叶节点深度为1*/	
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
//{{{int tree_b_mov()
/*
   测试移动节点内的vol实现d，m差值大于等于2的情形,但是这个函数到后期可能会移动大量的数据。是大批量数据插入的一个效率瓶颈。
   因此，该函数中有个阀值，可以限定两个待移动数据之间的距离，这样可以极大的提高插入效率，可是这样生成的二叉树并不完美，某些情况下
   可以先快速插入，待完成后再关闭阀值，通过该函数逐步调整至完美状态。
   返回值：0 正常返回，否则错误。
 */
int tree_b_mov()
{
	_TR *c,*t,*c1,*max,*min;
	int i,j,k,l;
	c=root;t=NULL;
	while(c != NULL)
	{
		i=c->ld-c->lm;j=c->rd-c->rm;
		if(i>=2)
		{t=c;c=c->left;continue;}
		if(j>=2)
		{t=c;c=c->right;continue;}
		break;
	}//t is root's top for adjusted,or error
	if(t == NULL)
	{
		c=root;
		if(c != NULL)
		{
			i=c->ld>c->rd?(c->ld):(c->rd);
			j=c->lm>c->rm?c->rm:c->lm;
			if(i<(j+2))
				return 0;
		}
	}
	else
	{
		if((t->ld-t->lm) >= 2)
			c=t->left; //root,到这里，最大和最小已经在不同的分支上了
		else
		{
			if((t->rd-t->rm) >= 2)
				c=t->right;
			else
				return 0;
		}
	}
	/*i=c->ld>=c->rd?c->ld:c->rd; //阀值！！
	if(i>10) //超过6层不调整了，即调整范围是128个节点内,1024
		return 0;*/
	c1=c;
	while(c != NULL)
	{
		max=c;
		if(c->ld >= c->rd)
			c=c->left;
		else
			c=c->right;
	}
	c=c1;
	while(c != NULL)
	{
		min=c;
		if(c->lm >= c->rm)
			c=c->right;
		else
			c=c->left;
	}
	memset((void*)s,0,sizeof(_TR*)*(count+3));
	j=tree_sort_list(c1,s,count);//这里得到的是子树c1的节点数组，需要调整的只是min到max之间的节点
	if((max->ld != 1) || (max->rd != 1))//verified 
		return 0;
	if((min->lm != 1) && (min->rm != 1))
		return 0;
	if(max->vol < min->vol) //从左至右调整
	{
		k=max->vol;t=max->top;
		if(t->left == max)
		{t->left=NULL;t->ld=t->lm=1;}
		else
		{t->right=NULL;t->rd=t->rm=1;}
		for(i=0;i<(j-1);i++)
		{
			if(s[i]->vol <= max->vol)
				continue;
			l=s[i]->vol;s[i]->vol=k;k=l;
			if(s[i+1]->vol >= min->vol)
			{
				if(s[i+1]->vol > min->vol)
				{
					printf("errooooor\n");
					return 1;
				}
				if(min->left == NULL)
				{min->left=max;max->top=min;max->vol=k;}
				else
				{min->right=max;max->top=min;max->vol=min->vol;min->vol=k;}
				break;
			}
		}//开始调整层数：t，max
	}
	else //从右至左调整
	{
		k=max->vol;t=max->top;
		if(t->left == max)
		{t->left=NULL;t->ld=t->lm=1;}
		else
		{t->right=NULL;t->rd=t->rm=1;}
		for(i=(j-1);i>0;i--)
		{
			if(s[i]->vol >= max->vol)
				continue;
			l=s[i]->vol;s[i]->vol=k;k=l;
			if(s[i-1]->vol <= min->vol)
			{
				if(s[i-1]->vol < min->vol)
				{
					printf("errorrrr\n");
					return 1;
				}
				if(min->right == NULL)
				{min->right=max;max->top=min;max->vol=k;}
				else
				{min->left=max;max->top=min;max->vol=min->vol;min->vol=k;}
				break;
			}
		}
	}
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
//{{{int tree_v_mov()
int tree_v_mov()
{
	_TR	*c,*t,*c1,*max,*min;
	int i,j,k,l;
	c=root;t=NULL;k=0;
	while(c != NULL)
	{
		i=c->ld>=c->rd?c->ld:c->rd;
		j=c->lm>=c->rm?c->rm:c->lm;
		if(i>=(j+2))
			t=c;
		if(c->lm>=c->rm)
			c=c->right;
		else
			c=c->left;
	}
	if(t == NULL)
		return 0;
	i=t->ld>=t->rd?(t->ld-t->rd):(t->rd-t->ld);
	if(i>=2)
	{
		c=t;
		while(c != NULL)
		{
			last=c;
			if(c->ld >= c->rd)
			   c=c->left;
			else
				c=c->right;
		}
		tree_balance();
		xx++;
		return 0;
	}
	c=t;c1=t;
	while(c != NULL)
	{
		max=c;
		if(c->ld >= c->rd)
			c=c->left;
		else
			c=c->right;
	}
	c=t;
	while(c != NULL)
	{
		min=c;
		if(c->lm >= c->rm)
			c=c->right;
		else
			c=c->left;
	}
	c=t;
	memset((void*)s,0,sizeof(_TR*)*(count+3));
	j=tree_sort_list(c,s,count);
	if((max->ld != 1) || (max->rd != 1))//verified 
		return 0;
	if((min->lm != 1) && (min->rm != 1))
		return 0;
	if(max->vol < min->vol) //从左至右调整
	{
		k=max->vol;t=max->top;
		if(t->left == max)
		{t->left=NULL;t->ld=t->lm=1;}
		else
		{t->right=NULL;t->rd=t->rm=1;}
		for(i=0;i<(j-1);i++)
		{
			if(s[i]->vol <= max->vol)
				continue;
			if(s[i+1]->vol > min->vol)
			{
				printf("errooooor\n");
				return 1;
			}
			l=s[i]->vol;s[i]->vol=k;k=l;
			if(s[i+1]->vol == min->vol)
			{
				if(min->left == NULL)
				{min->left=max;max->top=min;max->vol=k;}
				else
				{min->right=max;max->top=min;max->vol=min->vol;min->vol=k;}
				break;
			}
		}//开始调整层数：t，max
	}
	else //从右至左调整
	{
		k=max->vol;t=max->top;
		if(t->left == max)
		{t->left=NULL;t->ld=t->lm=1;}
		else
		{t->right=NULL;t->rd=t->rm=1;}
		for(i=(j-1);i>0;i--)
		{
			if(s[i]->vol >= max->vol)
				continue;
			if(s[i-1]->vol < min->vol)
			{
				printf("errorrrr\n");
				return 1;
			}
			l=s[i]->vol;s[i]->vol=k;k=l;
			if(s[i-1]->vol == min->vol)
			{
				if(min->right == NULL)
				{min->right=max;max->top=min;max->vol=k;}
				else
				{min->left=max;max->top=min;max->vol=min->vol;min->vol=k;}
				break;
			}
		}
	}
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






