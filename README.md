# btree
a simple and perfect self balance tree code
一个简单高效的自平衡二叉树实现代码。效率不输红黑树 ;-)
SPDX-License-Identifier: GPL-2.0+
自平衡二叉树的完美实现
2022-7-25已经可以生成完美的自平衡二叉树，测试的2^n-1的数据量插入都可生成完美的n层二叉树，但大批量数据的连续插入效率还有待改善。
2022-8-18已完美实现自平衡二叉树的全部功能：节点插入，左右子树的平衡，节点的删除，树的遍历。其中树的平衡使用三个可选的函数实现：
btree_balance(旋转，移动节点实现)，tree_b_mov(自上而下移动节点内数值实现)和tree_v_mov(自下(新节点)而上的数值移动实现的平衡)。
这三个函数可以单独使用，也可节点移动的函数和其他两个数值移动的任意一个函数配合使用。单独使用tree_balance函数生成树的效率最高，但
无法生成完美的平衡二叉树（类似于红黑树）。单独使用tree_b_mov或tree_v_mov可以生成完美的平衡二叉树，但生成树的效率不高。我这里采用的
是同时使用tree_balance和tree_b_mov两个函数。效率折中，又可生成完美的平衡二叉树。
本方法实现的依据是将节点数据结构设计为含有层数计数的结构：ld=left deep；rd=right deep;lm=minimum left deep;rm=minimum right deep;
即ld和rd代表了当前节点最大的左子树和右子树层数，lm和rm代表了当前节点最小的左子树和右子树层数。计算方法是ld=左子结点的ld和rd的最大值+1
lm=左子结点的lm和rm的最小值+1。
这种实现方法我不知道是否有人做过。再写这个之前，我仅是查看了平衡二叉树、红黑树和B+树的概念介绍。
  Copyright (c) 2020-2022 tybitsfox <tybitsfox@163.com>
