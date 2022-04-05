#include "XinOS.h"

void tEventInit (tEvent * event, tEventType type)
{
	event->type = type;
	tListInit(&event->waitList);
}

void tEventWait (tEvent * event, tTask * task, void * msg, uint32_t state, uint32_t timeout)
{
	uint32_t status = tTaskEnterCritical();
	
	task->state |= state << 16;
	task->waitEvent = event;
	task->eventMsg = msg;
	task->waitEventResult = tErrorNoError;
	
	tTaskSchedUnRdy(task); //将任务从就绪列表中移除掉
	
	tListAddLast(&event->waitList, &task->linkNode);  //将任务插入到事件等待队列中的尾部
	
	if (timeout)
	{
		tTimeTaskWait(task, timeout);      //如果有超时将其插入到等待队列中
	}
	
	tTaskExitCritical(status);
}

tTask * tEventWakeUp (tEvent * event, void * msg, uint32_t result)
{
	tNode * node;
	tTask * task = (tTask *)0;
	
	uint32_t status = tTaskEnterCritical();
	
	if ((node = tListRemoveFirst(&event->waitList)) != (tNode *)0)
	{
		task = (tTask *)tNodeParent(node, tTask, linkNode);
		task->waitEvent = (tEvent *)0; //不再等待任何一个事件了
		task->eventMsg = msg;
		task->waitEventResult = result;
		task->state &= ~XINOS_TASK_WAIT_MASK;
		
		if(task->delayTicks !=0)
		{
			tTimeTaskWakeUp(task);
		}
		
		tTaskSchedRdy(task); //插入到就绪队列中
	}
	
	tTaskExitCritical(status);
	
	return task;
}

tTask * tEventWakeUpTask (tEvent * event, tTask * task, void * msg, uint32_t result)
{
	uint32_t status = tTaskEnterCritical();
	
	tListRemove(&event->waitList, &task->linkNode);
	task->waitEvent = (tEvent *)0; //不再等待任何一个事件了
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= ~XINOS_TASK_WAIT_MASK;
		
	if(task->delayTicks !=0)
	{
		tTimeTaskWakeUp(task);
	}
		
	tTaskSchedRdy(task); //插入到就绪队列中
	
	tTaskExitCritical(status);
	
	return task;
}


void tEventRemoveTask(tTask * task, void * msg, uint32_t result) //强制将任务从事件等待中移除
{
	uint32_t status = tTaskEnterCritical();
	
	tListRemove(&task->waitEvent->waitList, &task->linkNode);
	task->waitEvent = (tEvent *)0; //不再等待任何一个事件了
	task->eventMsg = msg;
	task->waitEventResult = result;
	task->state &= ~XINOS_TASK_WAIT_MASK;
	tTaskExitCritical(status);
}

uint32_t tEventRemoveAll(tEvent * event, void * msg, uint32_t result) //删除事件控制块中的所有任务，并返回原有的任务数量
{
	tNode * node;
	uint32_t count=0;
	uint32_t status = tTaskEnterCritical();
	count = tListCount(&event->waitList);
	
	while((node = tListRemoveFirst(&event->waitList))!= (tNode *)0)
	{
		tTask * task = (tTask *)tNodeParent(node,tTask, linkNode);
	  task->waitEvent = (tEvent *)0; //不再等待任何一个事件了
	  task->eventMsg = msg;
	  task->waitEventResult = result;
	  task->state &= ~XINOS_TASK_WAIT_MASK;		
		
		if(task->delayTicks !=0)
		{
			tTimeTaskWakeUp(task);
		}
				
		tTaskSchedRdy(task); //插入到就绪队列中
	}
	
	tTaskExitCritical(status);
	return count;	
}

uint32_t tEventWaitCount (tEvent * event)
{
	uint32_t count=0;
	uint32_t status = tTaskEnterCritical();
	count = tListCount(&event->waitList);	
	tTaskExitCritical(status);
	return count;	
}
