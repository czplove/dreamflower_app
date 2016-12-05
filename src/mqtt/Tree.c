/*******************************************************************************
 * Copyright (c) 2009, 2013 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution. 
 *
 * The Eclipse Public License is available at 
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at 
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Ian Craggs - initial implementation and documentation
 *******************************************************************************/

/** @file
 * \brief functions which apply to tree structures.
 *
 * These trees can hold data of any sort, pointed to by the content pointer of the
 * Node structure.
 * */
 
/*
二叉排序树，红黑树，这些都是二叉树
主要为排序和检索的效率。
举几个应用的方面：
哈夫曼编解码，搜索二叉树，红黑树(STL中map的基础)

红黑树（Red Black Tree） 是一种自平衡二叉查找树，是在计算机科学中用到的一种数据结构，典型的用途是实现关联数组。
由于红黑树也是二叉查找树，它们当中每一个节点的比较值都必须大于或等于在它的左子树
中的所有节点，并且小于或等于在它的右子树中的所有节点。这确保红黑树运作时能够快速
的在树中查找给定的值。
红黑树在函数式编程中也特别有用，在这里它们是最常用的持久数据结构之一，它们用来构
造关联数组和集合，在突变之后它们能保持为以前的版本。除了O(log n)的时间之外，红黑
树的持久版本对每次插入或删除需要O(log n)的空间。
左边的数 < 右边的数
*/ 

#define NO_HEAP_TRACKING 1

#include "Tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#include "Heap.h"


void TreeInitializeNoMalloc(Tree* aTree, int(*compare)(void*, void*, int))	//-开辟一个节点
{
	memset(aTree, '\0', sizeof(Tree));
	aTree->heap_tracking = 1;		//-一个标志位,表示是否有跟踪功能
	aTree->index[0].compare = compare;
	aTree->indexes = 1;	//-最初的时候索引号为1
}

/**
 * Allocates and initializes a new tree structure.
 * @return a pointer to the new tree structure
 */
Tree* TreeInitialize(int(*compare)(void*, void*, int))	//-创建和初始化了一个tree结构体元素
{
#if defined(UNIT_TESTS)
	Tree* newt = malloc(sizeof(Tree));
#else
	Tree* newt = mymalloc(__FILE__, __LINE__, sizeof(Tree));	//-__FILE__用以指示本行语句所在源文件的文件名;__LINE__用以指示本行语句在源文件中的位置信息
#endif
	TreeInitializeNoMalloc(newt, compare);
	return newt;
}


void TreeAddIndex(Tree* aTree, int(*compare)(void*, void*, int))	//-这里是增加索引号,是一个分支,而不是一个简单的成员
{
	aTree->index[aTree->indexes].compare = compare;
	++(aTree->indexes);	//-增加索引号
}


void TreeFree(Tree* aTree)	//-释放一个节点
{
#if defined(UNIT_TESTS)
	free(aTree);
#else
	(aTree->heap_tracking) ? myfree(__FILE__, __LINE__, aTree) : free(aTree);
#endif
}


#define LEFT 0
#define RIGHT 1
#if !defined(max)
#define max(a, b) (a > b) ? a : b;
#endif



int isRed(Node* aNode)	//-判断是否有,有返回1
{
	return (aNode != NULL) && (aNode->red);
}


int isBlack(Node* aNode)	//-判断是否空,空返回1
{
	return (aNode == NULL) || (aNode->red == 0);
}

//-先序遍历
//-首先访问根，再先序遍历左（右）子树，最后先序遍历右（左）子树
int TreeWalk(Node* curnode, int depth)	//-这是一个很值得去学习的函数,自己调用自己,递归调用
{
	if (curnode)
	{
		int left = TreeWalk(curnode->child[LEFT], depth+1);
		int right = TreeWalk(curnode->child[RIGHT], depth+1);
		depth = max(left, right);
		if (curnode->red)
		{
			/*if (isRed(curnode->child[LEFT]) || isRed(curnode->child[RIGHT]))
			{
				printf("red/black tree violation %p\n", curnode->content);
				exit(-99);
			}*/;
		}
	}
	return depth;
}


int TreeMaxDepth(Tree *aTree)
{
	int rc = TreeWalk(aTree->index[0].root, 0);	//-把树遍历一遍
	/*if (aTree->root->red)
	{
		printf("root node should not be red %p\n", aTree->root->content);
		exit(-99);
	}*/
	return rc;
}


void TreeRotate(Tree* aTree, Node* curnode, int direction, int index)	//-二叉树旋转平衡的问题
{
	Node* other = curnode->child[!direction];

	curnode->child[!direction] = other->child[direction];
	if (other->child[direction] != NULL)
		other->child[direction]->parent = curnode;
	other->parent = curnode->parent;
	if (curnode->parent == NULL)
		aTree->index[index].root = other;
	else if (curnode == curnode->parent->child[direction])
		curnode->parent->child[direction] = other;
	else
		curnode->parent->child[!direction] = other;
	other->child[direction] = curnode;
	curnode->parent = other;
}


Node* TreeBAASub(Tree* aTree, Node* curnode, int which, int index)
{
	Node* uncle = curnode->parent->parent->child[which];

	if (isRed(uncle))
	{
		curnode->parent->red = uncle->red = 0;
		curnode = curnode->parent->parent;
		curnode->red = 1;
	}
	else
	{
		if (curnode == curnode->parent->child[which])
		{
			curnode = curnode->parent;
			TreeRotate(aTree, curnode, !which, index);
		}
		curnode->parent->red = 0;
		curnode->parent->parent->red = 1;
		TreeRotate(aTree, curnode->parent->parent, which, index);
	}
	return curnode;
}


void TreeBalanceAfterAdd(Tree* aTree, Node* curnode, int index)
{
	while (curnode && isRed(curnode->parent) && curnode->parent->parent)
	{
		if (curnode->parent == curnode->parent->parent->child[LEFT])
			curnode = TreeBAASub(aTree, curnode, RIGHT, index);
		else
			curnode = TreeBAASub(aTree, curnode, LEFT, index);
  }
  aTree->index[index].root->red = 0;
}


/**
 * Add an item to a tree
 * @param aTree the list to which the item is to be added
 * @param content the list item content itself
 * @param size the size of the element
 */
void* TreeAddByIndex(Tree* aTree, void* content, int size, int index)
{
	Node* curparent = NULL;
	Node* curnode = aTree->index[index].root;	//-记录了当前树的根节点
	Node* newel = NULL;
	int left = 0;
	int result = 1;
	void* rc = NULL;

	while (curnode)
	{
		result = aTree->index[index].compare(curnode->content, content, 1);	//-content目前传递过来的是一个指针值,这个空间里存储的是地址值
		left = (result > 0);
		if (result == 0)
			break;	//-寻找到相同的退出
		else
		{
			curparent = curnode;	//-父节点
			curnode = curnode->child[left];	//-字节点
		}
	}
	
	if (result == 0)
	{//-找到了一样的
		if (aTree->allow_duplicates)
			exit(-99);
		{
			newel = curnode;
			rc = newel->content;
			if (index == 0)
				aTree->size += (size - curnode->size);
		}
	}
	else
	{
		#if defined(UNIT_TESTS)
			newel = malloc(sizeof(Node));
		#else
			newel = (aTree->heap_tracking) ? mymalloc(__FILE__, __LINE__, sizeof(Node)) : malloc(sizeof(Node));
		#endif
		memset(newel, '\0', sizeof(Node));
		if (curparent)
			curparent->child[left] = newel;
		else
			aTree->index[index].root = newel;
		newel->parent = curparent;
		newel->red = 1;
		if (index == 0)
		{
			++(aTree->count);
			aTree->size += size;
		}
	}
	newel->content = content;
	newel->size = size;
	TreeBalanceAfterAdd(aTree, newel, index);
	return rc;
}


void* TreeAdd(Tree* aTree, void* content, int size)	//-在tree结构中增加一个成员
{
	void* rc = NULL;
	int i;

	for (i = 0; i < aTree->indexes; ++i)
		rc = TreeAddByIndex(aTree, content, size, i);	//-这里的目录Index,一个应该代表一个树

	return rc;
}

//-查找应该是这样的原理
//-用目标值(key),去和第一个节点(root)比较,由于所有的节点都是按照规律排列的,所以
//-可以很快找到(左边的<右边的)
Node* TreeFindIndex1(Tree* aTree, void* key, int index, int value)	//-key是寻找的目标
{
	int result = 0;
	Node* curnode = aTree->index[index].root;	//-记录的是整个树的信息

	while (curnode)
	{
		result = aTree->index[index].compare(curnode->content, key, value);
		if (result == 0)
			break;	//-相等就找到了
		else
			curnode = curnode->child[result > 0];	//-key小于节点值,那么就取节点左边的子节点,否则取右边的
	}
	return curnode;
}


Node* TreeFindIndex(Tree* aTree, void* key, int index)
{
	return TreeFindIndex1(aTree, key, index, 0);
}


Node* TreeFindContentIndex(Tree* aTree, void* key, int index)
{
	return TreeFindIndex1(aTree, key, index, 1);
}


Node* TreeFind(Tree* aTree, void* key)
{
	return TreeFindIndex(aTree, key, 0);
}


Node* TreeMinimum(Node* curnode)
{
	if (curnode)
		while (curnode->child[LEFT])
			curnode = curnode->child[LEFT];
	return curnode;
}


Node* TreeSuccessor(Node* curnode)
{
	if (curnode->child[RIGHT])
		curnode = TreeMinimum(curnode->child[RIGHT]);
	else
	{
		Node* curparent = curnode->parent;
		while (curparent && curnode == curparent->child[RIGHT])
		{
			curnode = curparent;
			curparent = curparent->parent;
		}
		curnode = curparent;
	}
	return curnode;
}


Node* TreeNextElementIndex(Tree* aTree, Node* curnode, int index)
{
	if (curnode == NULL)
		curnode = TreeMinimum(aTree->index[index].root);
	else
		curnode = TreeSuccessor(curnode);
	return curnode;
}


Node* TreeNextElement(Tree* aTree, Node* curnode)
{
	return TreeNextElementIndex(aTree, curnode, 0);
}


Node* TreeBARSub(Tree* aTree, Node* curnode, int which, int index)
{
	Node* sibling = curnode->parent->child[which];

	if (isRed(sibling))
	{
		sibling->red = 0;
		curnode->parent->red = 1;
		TreeRotate(aTree, curnode->parent, !which, index);
		sibling = curnode->parent->child[which];
	}
	if (!sibling)
		curnode = curnode->parent;
	else if (isBlack(sibling->child[!which]) && isBlack(sibling->child[which]))
	{
		sibling->red = 1;
		curnode = curnode->parent;
	}
	else
	{
		if (isBlack(sibling->child[which]))
		{
			sibling->child[!which]->red = 0;
			sibling->red = 1;
			TreeRotate(aTree, sibling, which, index);
			sibling = curnode->parent->child[which];
		}
		sibling->red = curnode->parent->red;
		curnode->parent->red = 0;
		sibling->child[which]->red = 0;
		TreeRotate(aTree, curnode->parent, !which, index);
		curnode = aTree->index[index].root;
	}
	return curnode;
}


void TreeBalanceAfterRemove(Tree* aTree, Node* curnode, int index)
{
	while (curnode != aTree->index[index].root && isBlack(curnode))
	{
		/* curnode->content == NULL must equal curnode == NULL */
		if (((curnode->content) ? curnode : NULL) == curnode->parent->child[LEFT])
			curnode = TreeBARSub(aTree, curnode, RIGHT, index);
		else
			curnode = TreeBARSub(aTree, curnode, LEFT, index);
    }
	curnode->red = 0;
}


/**
 * Remove an item from a tree
 * @param aTree the list to which the item is to be added
 * @param curnode the list item content itself
 */
void* TreeRemoveNodeIndex(Tree* aTree, Node* curnode, int index)
{
	Node* redundant = curnode;
	Node* curchild = NULL;
	int size = curnode->size;
	void* content = curnode->content;

	/* if the node to remove has 0 or 1 children, it can be removed without involving another node */
	if (curnode->child[LEFT] && curnode->child[RIGHT]) /* 2 children */
		redundant = TreeSuccessor(curnode); 	/* now redundant must have at most one child */

	curchild = redundant->child[(redundant->child[LEFT] != NULL) ? LEFT : RIGHT];
	if (curchild) /* we could have no children at all */
		curchild->parent = redundant->parent;

	if (redundant->parent == NULL)
		aTree->index[index].root = curchild;
	else
	{
		if (redundant == redundant->parent->child[LEFT])
			redundant->parent->child[LEFT] = curchild;
		else
			redundant->parent->child[RIGHT] = curchild;
	}

	if (redundant != curnode)
	{
		curnode->content = redundant->content;
		curnode->size = redundant->size;
	}

	if (isBlack(redundant))
	{
		if (curchild == NULL)
		{
			if (redundant->parent)
			{
				Node temp;
				memset(&temp, '\0', sizeof(Node));
				temp.parent = (redundant) ? redundant->parent : NULL;
				temp.red = 0;
				TreeBalanceAfterRemove(aTree, &temp, index);
			}
		}
		else
			TreeBalanceAfterRemove(aTree, curchild, index);
	}

#if defined(UNIT_TESTS)
	free(redundant);
#else
	(aTree->heap_tracking) ? myfree(__FILE__, __LINE__, redundant) : free(redundant);
#endif
	if (index == 0)
	{
		aTree->size -= size;
		--(aTree->count);
	}
	return content;
}


/**
 * Remove an item from a tree
 * @param aTree the list to which the item is to be added
 * @param curnode the list item content itself
 */
void* TreeRemoveIndex(Tree* aTree, void* content, int index)
{
	Node* curnode = TreeFindContentIndex(aTree, content, index);

	if (curnode == NULL)
		return NULL;

	return TreeRemoveNodeIndex(aTree, curnode, index);
}


void* TreeRemove(Tree* aTree, void* content)
{
	int i;
	void* rc = NULL;

	for (i = 0; i < aTree->indexes; ++i)
		rc = TreeRemoveIndex(aTree, content, i);

	return rc;
}


void* TreeRemoveKeyIndex(Tree* aTree, void* key, int index)
{
	Node* curnode = TreeFindIndex(aTree, key, index);
	void* content = NULL;
	int i;

	if (curnode == NULL)
		return NULL;

	content = TreeRemoveNodeIndex(aTree, curnode, index);
	for (i = 0; i < aTree->indexes; ++i)
	{
		if (i != index)
			content = TreeRemoveIndex(aTree, content, i);
	}
	return content;
}


void* TreeRemoveKey(Tree* aTree, void* key)
{
	return TreeRemoveKeyIndex(aTree, key, 0);
}


int TreeIntCompare(void* a, void* b, int content)	//-比较整数大小 a > b返回-1;a == b返回0;a < b返回1
{
	int i = *((int*)a);
	int j = *((int*)b);

	//printf("comparing %d %d\n", *((int*)a), *((int*)b));
	return (i > j) ? -1 : (i == j) ? 0 : 1;
}


int TreePtrCompare(void* a, void* b, int content)
{
	return (a > b) ? -1 : (a == b) ? 0 : 1;
}


int TreeStringCompare(void* a, void* b, int content)
{
	return strcmp((char*)a, (char*)b);
}

//-看到没有,这里也隐含了一个编程方法
#if defined(UNIT_TESTS)

int check(Tree *t)
{
	Node* curnode = NULL;
	int rc = 0;

	curnode = TreeNextElement(t, curnode);
	while (curnode)
	{
		Node* prevnode = curnode;

		curnode = TreeNextElement(t, curnode);

		if (prevnode && curnode && (*(int*)(curnode->content) < *(int*)(prevnode->content)))
		{
			printf("out of order %d < %d\n", *(int*)(curnode->content), *(int*)(prevnode->content));
			rc = 99;
		}
	}
	return rc;
}


int traverse(Tree *t, int lookfor)
{
	Node* curnode = NULL;
	int rc = 0;

	printf("Traversing\n");
	curnode = TreeNextElement(t, curnode);
	//printf("content int %d\n", *(int*)(curnode->content));
	while (curnode)
	{
		Node* prevnode = curnode;

		curnode = TreeNextElement(t, curnode);
		//if (curnode)
		//	printf("content int %d\n", *(int*)(curnode->content));
		if (prevnode && curnode && (*(int*)(curnode->content) < *(int*)(prevnode->content)))
		{
			printf("out of order %d < %d\n", *(int*)(curnode->content), *(int*)(prevnode->content));
		}
		if (curnode && (lookfor == *(int*)(curnode->content)))
			printf("missing item %d actually found\n", lookfor);
	}
	printf("End traverse %d\n", rc);
	return rc;
}


int test(int limit)
{
	int i, *ip, *todelete;
	Node* current = NULL;
	Tree* t = TreeInitialize(TreeIntCompare);	//-创建了一个tree的结构体成员
	int rc = 0;

	printf("Tree initialized\n");

	srand(time(NULL));

	ip = malloc(sizeof(int));
	*ip = 2;
	TreeAdd(t, (void*)ip, sizeof(int));

	check(t);

	i = 2;
	void* result = TreeRemove(t, (void*)&i);
	if (result)
		free(result);

	int actual[limit];
	for (i = 0; i < limit; i++)
	{
		void* replaced = NULL;

		ip = malloc(sizeof(int));
		*ip = rand();
		replaced = TreeAdd(t, (void*)ip, sizeof(int));
		if (replaced) /* duplicate */
		{
			free(replaced);
			actual[i] = -1;
		}
		else
			actual[i] = *ip;
		if (i==5)
			todelete = ip;
		printf("Tree element added %d\n",  *ip);
		if (1 % 1000 == 0)
		{
			rc = check(t);
			printf("%d elements, check result %d\n", i+1, rc);
			if (rc != 0)
				return 88;
		}
	}

	check(t);

	for (i = 0; i < limit; i++)
	{
		int parm = actual[i];

		if (parm == -1)
			continue;

		Node* found = TreeFind(t, (void*)&parm);
		if (found)
			printf("Tree find %d %d\n", parm, *(int*)(found->content));
		else
		{
			printf("%d not found\n", parm);
			traverse(t, parm);
			return -2;
		}
	}

	check(t);

	for (i = limit -1; i >= 0; i--)
	{
		int parm = actual[i];
		void *found;

		if (parm == -1) /* skip duplicate */
			continue;

		found = TreeRemove(t, (void*)&parm);
		if (found)
		{
			printf("%d Tree remove %d %d\n", i, parm, *(int*)(found));
			free(found);
		}
		else
		{
			int count = 0;
			printf("%d %d not found\n", i, parm);
			traverse(t, parm);
			for (i = 0; i < limit; i++)
				if (actual[i] == parm)
					++count;
			printf("%d occurs %d times\n", parm, count);
			return -2;
		}
		if (i % 1000 == 0)
		{
			rc = check(t);
			printf("%d elements, check result %d\n", i+1, rc);
			if (rc != 0)
				return 88;
		}
	}
	printf("finished\n");
	return 0;
}

int main(int argc, char *argv[])
{
	int rc = 0;

	while (rc == 0)
		rc = test(999999);
}

#endif





