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

_TR		*root=NULL,*last=NULL,*deep_last[3];
_TR		**s=NULL;
int count=0;//number of all nodes;use for calc normally deep
int cc=0,zz=0;
int lim[40];
int idx=0;

int tree_sort_list(_TR *t,_TR **save,int cnt);//traverse by value's size(only low to high)
int tree_balance();//balance by pointer
int tree_ins(_TR *t,int i);//created new node
int tree_b_mov();	//balance by value
int tree_v_mov();	//for test
int tree_bb_mov();	//test for tree_b_mov
int t_b_mov();
//{{{int main(int argc,char **argv)
int main(int argc,char **argv)
{
	_TR	*t=NULL;
	int i,j,k,l;
	char ch[1024];
	k=MINYCNT;l=0;
	if(argc == 2)
	{
		k=atoi(argv[1]);
		if((k <= 1) || k >= 33554432) //2^25
			k=MINYCNT;
	}
	deep_last[0]=deep_last[1]=deep_last[2]=NULL;
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
		/*if(tree_balance())
		{
			l=tree_v_mov(); //此时last有效
			if(l)
				break;
		}*/
		tree_balance();
		/*l=tree_sort_list(root,s,count);
		for(i=0;i<l;i++)
		{
			printf("%d\t",s[i]->vol);
		}
		printf("\n--end--\n");*/
		l=t_b_mov();
		if(l)
			break;
		//i=tree_sort_list(root,s,count);
		t++;
	}
	/*if(l)
	{
		k=open("aaa.txt",O_RDWR);
		if(k>0)
		{
			for(j=0;j<i;j++)
			{
				memset(ch,0,1024);
				snprintf(ch,1024,"<v=%d;t=%p;l=%p;r=%p>",s[j]->vol,s[j]->top,s[j]->left,s[j]->right);
				write(k,ch,strlen(ch));
			}
			memset(ch,0,1024);
			snprintf(ch,1024,"\n\n<top->vol=%d\tm1->vol=%d\tm2->vol=%d\n>",deep_last[0]->vol,deep_last[1]->vol,deep_last[2]->vol);
			write(k,ch,strlen(ch));
			close(k);
		}
	}*/
	printf("ldmax=%d\tldmin=%d\trdmax=%d\trdmin=%d\tcount=%d\tdeep=%d\n",root->ld,root->lm,root->rd,root->rm,count,idx+1);
	i=tree_sort_list(root,s,count);
	printf("list count=%d\tmoved times=%d\tbig move=%d\treturn=%d\n",i,cc,zz,l);
	free(p);
	free(q);
	printf("\n");
	return l;
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
		zz++;
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
			{printf("error001\n");return 1;}
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
//		if(k>10)
//			return 0;
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
//{{{int tree_bb_mov()
/*虽然这种方法不完美，但继续测试下。画个比较满意的句号吧*/
int tree_bb_mov()
{
	_TR *c,*c1,*t,*m1,*m2;
	int i,j,k,l,v1,v2;
	c=root;t=NULL;k=0;
	while(c != NULL)
	{
		i=c->ld >= c->rd?c->ld:c->rd;
		j=c->lm >= c->rm?c->rm:c->lm;
		if(i < (j+2))
			break;
		t=c;
		if(c->lm >= c->rd)
			c=c->right;
		else
			c=c->left;
	}
	if(t == NULL)
		return 0;
	m1=m2=NULL;c=t;
	while(c != NULL)
	{
		m1=c;
		if(c->ld > c->rd)
			c=c->left;
		else
			c=c->right;
	}
	c=t;
	while(c != NULL)
	{
		m2=c;
		if(c->lm >= c->rm)
			c=c->right;
		else
			c=c->left;
	}
	if(m1 == NULL || m2 == NULL)
	{printf("error001\n");return 1;}
	memset((void*)s,0,sizeof(_TR*)*(count+3));
	j=tree_sort_list(t,s,count);
	v1=v2=-1;
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
	if((v1 == -1) || (v2 == -1))
	{printf("error002\n");return 1;}
	if(v1 == v2)
	{printf("error003\n");return 1;}
	k=m1->vol;c=m1->top;
	if(c->left == m1)
	{c->left=NULL;c->ld=c->lm=1;}
	else
	{
		if(c->right == m1)
		{c->right=NULL;c->rd=c->rm=1;}
		else
		{printf("error004\n");return 1;}
	}
	if(v1 < v2)
	{
		for(i=(v1+1);i<v2;i++)
		{l=s[i]->vol;s[i]->vol=k;k=l;}
		if(m2->left == NULL)
		{m2->left=m1;m1->top=m2;m1->vol=k;}
		else
		{
			if(m2->right == NULL)
			{m2->right=m1;m1->top=m2;m1->vol=m2->vol;m2->vol=k;}
			else
			{printf("error005\n");return 1;}
		}
	}
	else
	{
		for(i=(v1-1);i>v2;i--)
		{l=s[i]->vol;s[i]->vol=k;k=l;}
		if(m2->right == NULL)
		{m2->right=m1;m1->top=m2;m1->vol=k;}
		else
		{
			if(m2->left == NULL)
			{m2->left=m1;m1->top=m2;m1->vol=m2->vol;m2->vol=k;}
			else
			{printf("error006\n");return 1;}
		}
	}
	while(c->top != NULL)
	{
		i=c->ld >= c->rd ?(c->ld+1):(c->rd+1);
		j=c->lm >= c->rm ?(c->rm+1):(c->lm+1);
		t=c;c=c->top;
		if(t == c->left)
		{c->ld=i;c->lm=j;}
		else
		{
			if(t == c->right)
			{c->rd=i;c->rm=j;}
			else
			{printf("error007\n");return 1;}
		}
	}
	while(m1->top != NULL)
	{
		i=m1->ld >= m1->rd ?(m1->ld+1):(m1->rd+1);
		j=m1->lm >= m1->rm ?(m1->rm+1):(m1->lm+1);
		t=m1;m1=m1->top;
		if(t == m1->left)
		{m1->ld=i;m1->lm=j;}
		else
		{
			if(t == m1->right)
			{m1->rd=i;m1->rm=j;}
			else
			{printf("error008\n");return 1;}
		}
	}
	cc++;
	return 0;
};
//}}}
//{{{int t_b_mov()
/*继续测试，ld-lm or rd-rm >=2的情形出错的原因*/
int t_b_mov()
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
	if(v1 == -1)
		return 32;
	for(i=0;i<j;i++)
	{
		if(s[i] == m2)
		{v2=i;break;}
	}
	if(v2 == -1)
		return 33;
	k=m1->vol;t=m1->top;
	if(t == NULL)
	{return 4;}
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




