#include "XinOS.h"

#if XINOS_ENABLE_MBOX == 1
void tMboxInit ( tMbox * mbox, void ** msgBuffer, uint32_t maxCount)
{
	tEventInit(&mbox->event,tEventTypeMbox);
	mbox->count = 0;
	mbox->maxCount = maxCount;
	mbox->msgBuffer = msgBuffer;
	mbox->write = 0;
	mbox->read = 0;
}

uint32_t tMboxWait(tMbox * mbox, void **msg, uint32_t waitTicks)
{
	uint32_t status = tTaskEnterCritical();
	
	if(mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
	else
	{
		tEventWait(&mbox->event, currentTask, (void *)0, tEventTypeMbox, waitTicks);
	  tTaskExitCritical(status);		
		tTaskSched();		
		*msg = currentTask->eventMsg;
		return currentTask->waitEventResult;
	}
}

uint32_t tMboxNoWaitGet (tMbox * mbox, void **msg)
{
	uint32_t status = tTaskEnterCritical();
	
	if(mbox->count > 0)
	{
		--mbox->count;
		*msg = mbox->msgBuffer[mbox->read++];
		if(mbox->read >= mbox->maxCount)
		{
			mbox->read = 0;
		}
		tTaskExitCritical(status);
		return tErrorNoError;
	}
  else
  {
		 tTaskExitCritical(status);
		return tErrorResourceUnavailable;
	}	
}

uint32_t tMboxNotify (tMbox * mbox, void *msg, uint32_t notifyOption)
{
	uint32_t status = tTaskEnterCritical();
	
	if(tEventWaitCount(&mbox->event) > 0)
	{
		tTask * task = tEventWakeUp(&mbox->event, (void *)msg, tErrorNoError);
		if(task->prio < currentTask->prio)
		{
			tTaskSched();
		}
	}
	else
  {
		if(mbox->count >= mbox->maxCount)
		{
			tTaskExitCritical(status);
			return tErrorResourceFull;
		}
		if(notifyOption & tMBOXSendFront)
		{
			if(mbox->read<=0)
			{
				mbox->read = mbox->maxCount-1;
			}
			else
      {
				--mbox->read;
			}
			mbox->msgBuffer[mbox->read] = msg;
		}
		else
		{
			mbox->msgBuffer[mbox->write++] = msg;
			if(mbox->write >= mbox->maxCount)
			{
				mbox->write = 0;
			}
		}
		mbox->count++;
	}
	tTaskExitCritical(status);
	return tErrorNoError;
}

void tMboxFlush (tMbox * mbox)  //清空邮箱接口 将邮箱中的消息队列清空
{
	uint32_t status = tTaskEnterCritical();
	//若邮箱中已经有任务在等待 就说明消息队列已经为空了 就不需要再次清空
	if(tEventWaitCount(&mbox->event)==0)
	{
		mbox->read = 0;
		mbox->write = 0;
		mbox->count = 0;
	}
	tTaskExitCritical(status);
}

uint32_t tMboxDestroy(tMbox * mbox)  //删除邮箱接口 将邮箱中的等待任务清空
{
		uint32_t status = tTaskEnterCritical();
	  uint32_t count = tEventRemoveAll(&mbox->event, (void *)0, tErrorDel);	
		tTaskExitCritical(status);
	  if(count > 0)
		{
			tTaskSched();
		}
     return count;
}

void tMboxGetInfo(tMbox * mbox, tMboxInfo * info)
{
	uint32_t status = tTaskEnterCritical();
	
	info->count = mbox->count;
	info->maxCount = mbox->maxCount;
	info->taskCount = tEventWaitCount(&mbox->event);
	tTaskExitCritical(status);
}
#endif

