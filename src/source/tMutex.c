#include "XinOS.h"

#if XINOS_ENABLE_MUTEX == 1
void tMutexInit(tMutex * mutex)
{
	tEventInit(&mutex->event, tEventTypeMutex);
	mutex->lockedCount = 0;
	mutex->owner = (tTask *)0;
	mutex->ownerOriginalPrio = XinOS_PRO_COUNT;
}

uint32_t tMutexWait(tMutex * mutex, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
	if(mutex->lockedCount <=0) //若当前互斥信号量没有被锁定 任务就可以直接占有信号量
	{
		mutex->owner = currentTask;
		mutex->ownerOriginalPrio = currentTask->prio;
		mutex->lockedCount++;
		
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		if(mutex->owner == currentTask) //判断是不是自己锁定的
		{
			mutex->lockedCount++;
			tTaskExitCritical(status);
		  return tErrorNoError;
		}
		else
		{
			if(currentTask->prio < mutex->owner->prio) //若当前任务的优先级高于互斥量拥有者的优先级 则采用优先级继承机制
			{
				tTask * owner = mutex->owner;
				if(owner->state == XINOS_TASK_STATE_RDY)
				{
					tTaskSchedUnRdy(owner);
					owner->prio = currentTask->prio;
					tTaskSchedRdy(owner);
				}
				else
				{
					owner->prio = currentTask->prio;
				}
			}
			
			tEventWait(&mutex->event, currentTask, (void *)0, tEventTypeMutex, waitTicks);
			tTaskExitCritical(status);
			
			tTaskSched();
			return currentTask->waitEventResult;
		}
	}
}

uint32_t tMutexNoWaitGet(tMutex* mutex)
{	
	uint32_t status = tTaskEnterCritical();
	
	if(mutex->lockedCount <= 0)
	{
		mutex->owner = currentTask;
		mutex->ownerOriginalPrio = currentTask->prio;
		mutex->lockedCount++;
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		if(mutex->owner == currentTask) //判断是不是自己锁定的
		{
			mutex->lockedCount++;
			tTaskExitCritical(status);
		  return tErrorNoError;
		}
		
			tTaskExitCritical(status);
		  return tErrorResourceUnavailable;
	}
}

uint32_t tMutexNotify(tMutex * mutex)
{
	uint32_t status = tTaskEnterCritical();
	
	if(mutex->lockedCount <= 0)
	{
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	
	if (mutex->owner != currentTask) //互斥信号量的释放只能由占有者去释放
	{
		tTaskExitCritical(status);
		return tErrorOwner;
	}
	
	if(--mutex->lockedCount > 0)
	{
		tTaskExitCritical(status);	
		return tErrorNoError;
	}
	
	if(mutex->ownerOriginalPrio != mutex->owner->prio) //判断是否发生了优先级继承
	{
		if(mutex->owner->state == XINOS_TASK_STATE_RDY)
		{
			tTaskSchedUnRdy(mutex->owner);
			currentTask->prio = mutex->ownerOriginalPrio;
			tTaskSchedRdy(mutex->owner);
		}
		else
		{
			currentTask->prio = mutex->ownerOriginalPrio;
		}
	}
	
	if (tEventWaitCount(&mutex->event)>0)
	{
		tTask * task = tEventWakeUp(&mutex->event, (void *)0, tErrorNoError);
		
		mutex->owner = task;
		mutex->lockedCount++;
		mutex->ownerOriginalPrio = task->prio;
		
		if(task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	tTaskExitCritical(status);
	return tErrorNoError;
}

uint32_t tMutexDestroy(tMutex * mutex)
{
	uint32_t status = tTaskEnterCritical();
	uint32_t count;
	if(mutex->lockedCount > 0)
	{
		if(mutex->ownerOriginalPrio != mutex->owner->prio)
		{
			if(mutex->owner->state == XINOS_TASK_STATE_RDY)
			{
				tTaskSchedUnRdy(mutex->owner);
				mutex->owner->prio = mutex->ownerOriginalPrio;
				tTaskSchedRdy(mutex->owner);
			}
			else
			{
				mutex->owner->prio = mutex->ownerOriginalPrio;
			}
		}
		count = tEventRemoveAll(&mutex->event, (void *)0, tErrorDel);
		if(count>0)
		{
			tTaskSched();
		}
	}
	tTaskExitCritical(status);
	return count;
}

void tMutexGetInfo(tMutex * mutex, tMutexInfo * Info)
{
	uint32_t status = tTaskEnterCritical();
	
	Info->taskCount = tEventWaitCount(&mutex->event);
	Info->ownerPrio = mutex->ownerOriginalPrio;
	if(mutex->owner != (tTask *)0)
	{
		Info->inheritedPrio = mutex->owner->prio;
	}
	else
	{
		Info->inheritedPrio = XinOS_PRO_COUNT;
	}
	Info->owner = mutex->owner;
	Info->lockedCount = mutex->lockedCount;
	
	tTaskExitCritical(status);
}
#endif

