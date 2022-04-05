#ifndef TMUTEX_H
#define TMUTEX_H

#include "tEvent.h"

typedef struct _tMutex
{
	tEvent event;
	uint32_t lockedCount;
	tTask * owner;               //互斥量拥有者
	uint32_t ownerOriginalPrio;  //任务所有者的原始优先级
}tMutex; //互斥量结构体

typedef struct _tMutexInfo
{
	uint32_t taskCount;
	uint32_t ownerPrio; //拥有者的优先级
	uint32_t inheritedPrio; //继承的优先级
	tTask * owner;      //拥有者
	uint32_t lockedCount; //锁定次数
}tMutexInfo;

void tMutexInit(tMutex * mutex);
uint32_t tMutexWait(tMutex * mutex, uint32_t waitTicks);
uint32_t tMutexNoWaitGet(tMutex* mutex);
uint32_t tMutexNotify(tMutex * mutex);
uint32_t tMutexDestroy(tMutex * mutex);
void tMutexGetInfo(tMutex * mutex, tMutexInfo * Info);

#endif
