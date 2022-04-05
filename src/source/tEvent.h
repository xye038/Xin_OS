#ifndef TEVENT_H
#define TEVENT_H

#include "tLib.h"
#include "tTask.h"

typedef enum _tEventType{
    tEventTypeUnknow, 
	  tEventTypeSem, //计数信号量类型
	  tEventTypeMbox, //邮箱信号量类型
	  tEventTypeMemBlock, //存储块事件类型
	  tEventTypeFlagGroup, //事件组类型
	  tEventTypeMutex, //互斥信号量类型
}tEventType;           //事件类型定义 枚举变量

typedef struct _tEvent{
    tEventType type;  //事件类型
    tList waitList;   //等待队列	
}tEvent;  //事件结构体

void tEventInit (tEvent * event, tEventType type);
void tEventWait (tEvent * event, tTask * task, void * msg, uint32_t state, uint32_t timeout);
tTask * tEventWakeUp (tEvent * event, void * msg, uint32_t result);
tTask * tEventWakeUpTask (tEvent * event, tTask * task, void * msg, uint32_t result);
void tEventRemoveTask(tTask * task, void * msg, uint32_t result);
uint32_t tEventRemoveAll(tEvent * event, void * msg, uint32_t result);
uint32_t tEventWaitCount (tEvent * event);
#endif
