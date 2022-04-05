#ifndef __TLIB_H_
#define __TLIB_H_

#include <stdint.h>

typedef struct
{
	uint32_t bitmap;
}tBitmap;   //位图结构体

void tBitmapInit(tBitmap * bitmap); //位图结构体初始化
uint32_t tBitmapPosCount (void);   //位图结构体位的数量
void tBitmapSet (tBitmap * bitmap, uint32_t pos); //位图结构体置一函数
void tBitmapClear (tBitmap * bitmap, uint32_t pos); //位图结构体清零函数
uint32_t tBitmapGetFirstSet (tBitmap * bitmap); //获取位图结构的第一个置一位的位置

typedef struct _tNode
{
	struct _tNode * preNode;
	struct _tNode * nextNode;
}tNode;//双向链表节点结构体指针

void tNodeInit (tNode * node); //节点初始化函数，将前后节点地址都指向自己

typedef struct _tList
{
	tNode headNode;        //头节点
	uint32_t nodeCount;    //结点数量
}tList;//双向链表结构体

#define tNodeParent(node, parent, name) (parent*)((uint32_t)node - (uint32_t)&((parent *)0)->name)

void tListInit (tList * list);       //链表初始化函数，将头节点的前后节点地址都指向自己，数量置为0
uint32_t tListCount (tList * list);  //返回链表的节点数量
tNode * tListFirst (tList * list);   //返回链表的第一个节点
tNode * tListLast (tList *list);     //返回链表的最后一个节点
tNode * tListPre (tList * list, tNode * node); //返回指定节点的前一个节点
tNode * tListNext (tList * list, tNode * node); //返回指定节点的下一个节点
void tListRemoveAll (tList * list);  //删除链表中的所有节点
void tListAddFirst (tList * list, tNode * node); //往链表的头部插入节点
void tListAddLast (tList * list, tNode * node);  //往链表的尾部插入节点
tNode * tListRemoveFirst (tList * list);  //删除链表第一个节点
void tListInsertAfter (tList * list, tNode * nodeAfter, tNode * nodeToInsert); //在指定的节点后面插入一个新的节点
void tListRemove (tList * list, tNode * node); //从链表中删除指定的节点
#endif
