#include "XinOS.h"


/********************************************
**Function name : tTaskInit
**Descriptions  : 初始化任务结构体
**parameters    : task     要初始化的任务结构
**parameters    : entry    任务的入口参数
**parameters    : param    传递给任务的运行参数
**Returned value: 无
********************************************/
void tTaskInit(tTask * task, void(*entry)(void *), void *param, uint32_t prio, tTaskStack *stack, uint32_t size) //任务初始化函数需要参数有任务结构体指针，任务入口函数，任务入口函数所需参数，任务优先级，任务的栈地址
{
	uint32_t * stackTop;  //指向堆栈的末端指针
	
	task->stackBase = stack;
	task->stackSize = size;
	memset(stack, 0, size);
	
	stackTop = stack + size / sizeof(tTaskStack);
	*(--stackTop) = (unsigned long) (1 << 24);  //XPSR寄存器值 设置T标志位
	*(--stackTop) = (unsigned long)entry;       //PC寄存器值 设置为程序入口函数
	*(--stackTop) = (unsigned long)0x14;
	*(--stackTop) = (unsigned long)0x12;
	*(--stackTop) = (unsigned long)0x3;
	*(--stackTop) = (unsigned long)0x2;
	*(--stackTop) = (unsigned long)0x1;
	*(--stackTop) = (unsigned long)param;      //R0寄存器保存入口函数参数
	
	*(--stackTop) = (unsigned long)0x11;
	*(--stackTop) = (unsigned long)0x10;
	*(--stackTop) = (unsigned long)0x9;
	*(--stackTop) = (unsigned long)0x8;
	*(--stackTop) = (unsigned long)0x7;
	*(--stackTop) = (unsigned long)0x6;
	*(--stackTop) = (unsigned long)0x5;
	*(--stackTop) = (unsigned long)0x4;
	
	task->slice = XinOS_SLICE_MAX;
	task->stack = stackTop;
	task->delayTicks = 0;
	task->prio = prio;
	task->state = XINOS_TASK_STATE_RDY;
	task->suspendCount = 0;
	task->clean = (void(*)(void *))0;
	task->cleanParam = (void *)0;
	task->requestDeleteFlag = 0;
	
	tNodeInit(&(task->delayNode));      //延时节点初始化
	tNodeInit(&(task->linkNode));       //优先级节点初始化
	
	tTaskSchedRdy(task);  //将任务添加到就序列表中
	
#if XINOS_ENABLE_HOOKS == 1
	tHooksTaskInit(task);
#endif
}

/*
* 任务挂起函数
*/
void tTaskSuspend (tTask * task)
{
	uint32_t status = tTaskEnterCritical(); //进入临界保护区
	
	if(!(task->state & XINOS_TASK_STATE_DELAYED))
	{
		if(++task->suspendCount <=1) //防止重复将任务挂起
		{
			task->state |= XINOS_TASK_STATE_SUSPEND;
			tTaskSchedUnRdy(task);  //将任务从就绪列表中删除
			if(task == currentTask)
			{
				tTaskSched();
			}
		}
	}
	tTaskExitCritical(status); //退出临界区保护
}

/*
* 任务唤醒函数
*/
void tTaskWakeUp (tTask * task)
{
	uint32_t status = tTaskEnterCritical();  //进入临界区保护
	
	if (task->state & XINOS_TASK_STATE_SUSPEND)
	{
		if(--task->suspendCount == 0)
		{
			task->state &= ~XINOS_TASK_STATE_SUSPEND;
			tTaskSchedRdy(task);
			tTaskSched();
		}
	}
	tTaskExitCritical(status); //退出临界区保护
}

/*
* 配置任务删除回调函数
*/
void tTaskSetCleanCallFunc(tTask* task, void(*clean)(void * param), void *param)
{
	task->clean = clean;
	task->cleanParam = param;
}

/*
* 任务强制删除函数
*/
void tTaskForceDelete(tTask * task)
{
	uint32_t status = tTaskEnterCritical();
	
	if(task->state & XINOS_TASK_STATE_DELAYED)
	{
		tTimeTaskRemove(task);
	}
	else if(!(task->state & XINOS_TASK_STATE_SUSPEND))
	{
		tTaskSchedRemove(task);
	}
	
	if(task->clean)
	{
		task->clean(task->cleanParam); //调用删除函数
	}
	
	if(currentTask == task)
	{
		tTaskSched();
	}
	tTaskExitCritical(status);
}

/*
* 任务请求删除函数
*/
void tTaskRequestDelete(tTask * task)
{
  uint32_t status = tTaskEnterCritical();
	
	task->requestDeleteFlag = 1;
	
	tTaskExitCritical(status);
}

/*
* 任务判断是否删除函数
*/
uint8_t tTaskIsRequestedDeleted(void)
{
	uint8_t delete;
	
	uint32_t status = tTaskEnterCritical();
	
	delete = currentTask->requestDeleteFlag;
	
	tTaskExitCritical(status);
	
	return delete;
}

/*
* 任务自动删除函数
*/
void tTaskDeleteSelf (void)
{
	uint32_t status = tTaskEnterCritical();
	
	tTaskSchedRemove(currentTask);
	
	if(currentTask->clean)
	{
		currentTask->clean(currentTask->cleanParam); //调用删除函数
	}
	
	tTaskSched();
	
	tTaskExitCritical(status);
}

/*
* 获取任务信息函数
*/
void tTaskGetInfo(tTask * task, tTaskInfo * Info)
{
	uint32_t * stackEnd;
	uint32_t  status = tTaskEnterCritical();
	
	Info->delayTicks = task->delayTicks;
	Info->prio = task->prio;
	Info->state = task->state;
	Info->slice = task->slice;
	Info->suspendCount = task->suspendCount;
	Info->stackSize = task->stackSize;
	
	Info->stackFree = 0;
	stackEnd = task->stackBase;
	while((*stackEnd++ == 0)&&(stackEnd<=task->stackBase + task->stackSize / sizeof(tTaskStack)))
	{
		Info->stackFree++;
	}
	Info->stackFree *= sizeof(tTaskStack);
	
	tTaskExitCritical(status);
}
