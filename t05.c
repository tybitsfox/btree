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
int xx=0;

int calc_deep(unsigned long c);
int tree_sort_list(_TR *t,_TR **save,int cnt);//traverse by value's size(only low to high)
int tree_balance();//point changed for balance
int tree_ins(_TR *t,int i);//created new node
int tree_max(_TR *t1);//for test
int tree_b_mov();	//value moved for balance
//{{{int main(int argc,char **argv)
int main(int argc,char **argv)
{
	_TR	*t=NULL;
	int i,j,k,l;
	k=MINYCNT;
	if(argc == 2)
	{
		k=atoi(argv[1]);
		if((k <= 1) || k >= 33554432) //2^25
			k=MINYCNT;
	}
	xx=k;
	unsigned char *p=malloc(sizeof(_TR)*(k+3));
	unsigned char *q=malloc(sizeof(_TR*)*(k+3));
	if(p == NULL)
		return 0;
	if(q == NULL)
	{free(p);return 0;}
	memset(p,0,sizeof(_TR)*(k+3));
	memset(q,0,sizeof(_TR*)*(k+3));
	t=(_TR *)p;s=(_TR **)q;
	srand((int)time(0));
//	for(i=1;i<=k-1;i++)
	while(count < k)
	{
		j=rand()%(k*5);
		if(tree_ins(t,j))
			continue;
		tree_balance();
		/*for(i=0;i<3;i++)
		{
			l=tree_b_mov();
			if(l)
				break;
		}*/
		t++;
	}
	i=calc_deep(count);
	printf("ldmax=%d\tldmin=%d\trdmax=%d\trdmin=%d\tcount=%d\tdeep=%d\n",root->ld,root->lm,root->rd,root->rm,count,i);
	//i=tree_sort_list(root,s,count);
	printf("list count=%d\tmoved times=%d\n",i,cc);
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
//{{{int tree_max(_TR *t1,_TR *t2)
int tree_max(_TR *t1)
{
	int i,j,k;
	_TR *c,*tmp,*v[10];
	memset((void*)v,0,sizeof(_TR *)*10);
	if(last == NULL)
		return 7;
	tmp=last->top;
	if(tmp == NULL)
		return 7;
	if((tmp->left != NULL) && (tmp->right != NULL))//单支的插入才考虑
		return 7;
	i=1;tmp=last;
	while(i<4)
	{
		tmp=tmp->top;
		if(tmp==NULL)
			return 7;
		i=(tmp->ld >= tmp->rd?(tmp->ld):(tmp->rd));
	}
	if(i != 4)
		return 7;
	j=(tmp->lm >= tmp->rm?(tmp->rm):(tmp->lm));
	if((i-j) != 2)
		return 7;//到这里就可以确定以tmp为顶点的子树总节点为7
	c=tmp->top;
	if(c == NULL)
		return 7;
	i=tree_sort_list(tmp,v,10);
	if(i == 7)
	{
		if(c->left == tmp)
			c->left=v[3];
		else
			c->right=v[3];
		v[3]->ld=v[3]->rd=3;v[3]->lm=v[3]->rm=3;
		v[3]->left=v[1];v[3]->right=v[5];v[3]->top=c;
		v[1]->top=v[3];v[1]->left=v[0];v[1]->right=v[2];
		v[1]->ld=v[1]->rd=2;v[1]->lm=v[1]->rm=2;
		v[0]->top=v[1];v[0]->left=v[0]->right=NULL;
		v[0]->ld=v[0]->rd=1;v[0]->lm=v[0]->rm=1;
		v[2]->top=v[1];v[2]->left=v[2]->right=NULL;
		v[2]->ld=v[2]->rd=1;v[2]->lm=v[2]->rm=1;
		v[5]->top=v[3];v[5]->left=v[4];v[5]->right=v[6];
		v[5]->ld=v[5]->rd=2;v[5]->lm=v[5]->rm=2;
		v[4]->top=v[5];v[4]->left=v[4]->right=NULL;
		v[4]->ld=v[4]->rd=1;v[4]->lm=v[4]->rm=1;
		v[6]->top=v[5];v[6]->left=v[6]->right=NULL;
		v[6]->ld=v[6]->rd=1;v[6]->lm=v[6]->rm=1;
		tmp=v[3];
		while(c != NULL)
		{
			if(c->left == tmp)
			{
				c->ld=(tmp->ld>=tmp->rd?(tmp->ld+1):(tmp->rd+1));
				c->lm=(tmp->lm>=tmp->rm?(tmp->rm+1):(tmp->lm+1));
			}
			else
			{
				c->rd=(tmp->ld>=tmp->rd?(tmp->ld+1):(tmp->rd+1));
				c->rm=(tmp->lm>=tmp->rm?(tmp->rm+1):(tmp->lm+1));
			}
			tmp=c;c=c->top;
		}
		xx++;
	}
	last=NULL;
	return i;	
};
//}}}
//{{{int tree_b_mov()
/*
   测试移动节点内的vol实现d，m差值大于等于2的情形,
   目前的调整，仅限于子树深度不大于7,即调整的节点数小于128
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
			if(i>=(j+2))
				t=c;
			else
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
	/*i=c->ld>=c->rd?c->ld:c->rd;
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
	i=255;
	memset((void*)s,0,sizeof(_TR*)*(xx+3));
	j=tree_sort_list(c1,s,i);//这里得到的是子树c1的节点数组，需要调整的只是min到max之间的节点
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
					break;
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
					break;
				if(min->right == NULL)
				{min->right=max;max->top=min;max->vol=k;}
				else
				{min->left=max;max->top=min;max->vol=min->vol;min->vol=k;}
				break;
			}
		}
	}
	k=0;
	while(k<=1)
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
/*	for(i=0;i<j;i++)
	{printf("%d\t",s[i]->vol);}
	if(i<=15)
		printf("\nld=%d\tlm=%d\trd=%d\trm=%d\tmax=%d\tmin=%d\tc1=%d\ti=%d\n",c1->ld,c1->lm,c1->rd,c1->rm,max->vol,min->vol,c1->vol,i);
	else
		printf("i=%d\n",i);*/
	cc++;
	return 0;
};
//}}}







