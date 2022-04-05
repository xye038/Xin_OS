#ifndef TMEMBLOCK_H
#define TMEMBLOCK_H

#include "tEvent.h"

typedef struct _tMemBlock
{
	tEvent event;    //事件结构体
	void * memStart; //存储块起始地址
	uint32_t blockSize; //存储块大小
	uint32_t maxCount; //存储块数量
	tList blockList; //用于存放存储块的链表
}tMemBlock;       //存储块结构体

typedef struct _tMemBlockInfo
{
	uint32_t count;
	uint32_t maxCount;
	uint32_t blockSize;
	uint32_t taskCount;
}tMemBlockInfo;
void tMemBlockInit (tMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt); //存储块结构体初始化函数
uint32_t tMemBlockWait (tMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t tMemBlockNoWaitGet(tMemBlock * memBlock, void ** mem);
void tMemBlockNotify (tMemBlock * memBlock, uint8_t * mem);
void tMemBlockGetInfo (tMemBlock * memBlock, tMemBlockInfo * info);
uint32_t tMemBlockDestroy (tMemBlock * memBlock);
	
#endif
