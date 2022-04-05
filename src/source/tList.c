#include "tLib.h"

/*
节点初始化函数，将前后节点地址都指向自己
*/
void tNodeInit (tNode * node)
{
	node->preNode = node;
	node->nextNode = node;
}

#define  firstNode   headNode.nextNode
#define  lastNode    headNode.preNode

/*
链表初始化函数，将头节点的前后节点地址都指向自己，数量置为0
*/
void tListInit (tList * list)
{
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

/*
返回链表的节点数量
*/
uint32_t tListCount (tList * list)
{
	return list->nodeCount;
}

/*
返回链表的第一个节点
*/
tNode * tListFirst (tList * list)
{
	tNode * node = (tNode *)0;
	if(list->nodeCount != 0)
	{
		node = list->firstNode;
	}
	return node;
}

/*
返回链表的最后一个节点
*/
tNode * tListLast (tList *list)
{
	tNode * node = (tNode *)0;
	if(list->nodeCount != 0)
	{
		node = list->lastNode;
	}
	return node;
}

/*
返回指定节点的前一个节点
*/
tNode * tListPre (tList * list, tNode * node)
{
	if (node->preNode == node)
	{
		return (tNode *)0;
	}
	else
	{
		return node->preNode;
	}
}

/*
返回指定节点的下一个节点
*/
tNode * tListNext (tList * list, tNode * node)
{
		if (node->preNode == node)
	{
		return (tNode *)0;
	}
	else
	{
		return node->nextNode;
	}
}

/*
删除链表中的所有节点
*/
void tListRemoveAll (tList * list)
{
	uint32_t count;
	tNode * nextNode;
	
	nextNode = list->firstNode;
	for(count = list->nodeCount; count != 0; count--)
	{
		tNode * currentNode = nextNode;
		nextNode = nextNode->nextNode;
		
		currentNode->nextNode = currentNode;
		currentNode->preNode = currentNode;
	}
	
	list->firstNode = &(list->headNode);
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

/*
往链表的头部插入节点
*/
void tListAddFirst (tList * list, tNode * node)
{
	node->preNode = list->firstNode->preNode;
	node->nextNode = list->firstNode;
	
	list->firstNode->preNode = node;
	list->firstNode = node;
	list->nodeCount++;
}

/*
往链表的尾部插入节点
*/
void tListAddLast (tList * list, tNode * node)
{
	node->nextNode = &(list->headNode);
	node->preNode = list->lastNode;
	
	list->lastNode->nextNode = node;
	list->lastNode = node;
	list->nodeCount++;
}

/*
删除链表第一个节点
*/
tNode * tListRemoveFirst (tList * list)
{
	tNode * node = (tNode *)0;
	
	if(list->nodeCount != 0)
	{
		node = list->firstNode;
		
		node->nextNode->preNode = &(list->headNode);
		list->firstNode = node->nextNode;
		list->nodeCount--;
	}
	
	return node;
}

/*
在指定的节点后面插入一个新的节点
*/
void tListInsertAfter (tList * list, tNode * nodeAfter, tNode * nodeToInsert)
{
	nodeToInsert->preNode = nodeAfter;
	nodeToInsert->nextNode = nodeAfter->nextNode;
	
	nodeAfter->nextNode->preNode = nodeToInsert;
	nodeAfter->nextNode = nodeToInsert;
	
	list->nodeCount++;
}

/*
从链表中删除指定的节点
*/
void tListRemove (tList * list, tNode * node)
{
	node->preNode->nextNode = node->nextNode;
	node->nextNode->preNode = node->preNode;
	list->nodeCount--;
}






