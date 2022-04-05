#include "XinOS.h"
#include "ARMCM3.h"

tTask * currentTask; //当前任务的指针
tTask * nextTask;    //下一任务的指针
tTask * idleTask;    //空闲任务指针

tBitmap taskPrioBitmap; //优先级位图结构
tList taskTable[XinOS_PRO_COUNT];  //任务链表
	
uint8_t schedLockcount;  //调度锁计数器

uint32_t tickCount;      //时钟节拍发生的次数

tList tTaskDelayedList;  //延时队列

uint32_t idleCount;      //空闲任务计数器 用来计算CPU的使用率
uint32_t idleMaxCount;   //空闲任务最大的计数值

#if XINOS_ENABLE_CPUUSAGE_STAT == 1
static void initCpuUsageState(void);
static void checkCpuUsage (void);
static void cpuUsageSyncWithSysTick (void);
#endif


/******************************************
**Function name  :tTaskHighestReady
**Descriptions   :获取当前最高优先级且可运行的任务
**parameters     ：无
**Returned value : 优先级最高且可运行的任务
*******************************************/
tTask * tTaskHighestReady(void)  //获取最高优先级的任务
 {
	 uint32_t highestPrio = tBitmapGetFirstSet(&taskPrioBitmap);
	 tNode * node = tListFirst(&taskTable[highestPrio]); //获得最高优先级任务链表中的第一个任务
	 return tNodeParent(node, tTask, linkNode); //返回任务指针
 }
 
void tTaskSchedInit (void)
{
	int i;
	schedLockcount = 0;
	tBitmapInit(&taskPrioBitmap);  //任务优先级位图初始化
	for(i=0;i<XinOS_PRO_COUNT;i++)
	{
		tListInit(&taskTable[i]);
	}
}

void tTaskSchedDisable (void)  //调度锁上锁 禁止任务切换
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockcount < 255)
	{
		schedLockcount++;
	}
	tTaskExitCritical(status);
}

void tTaskSchedEnable (void)  //调度锁解锁 允许任务切换
{
	uint32_t status = tTaskEnterCritical();
	
	if(schedLockcount > 0)
	{
		if (--schedLockcount == 0)
		{
			tTaskSched();
		}
	}
	tTaskExitCritical(status);
}

/*********************************
**Function name :tTaskSchedRdy
**Dexcriptions  :将任务添加到就序列表中
**parameters    :任务结构体指针
**Returned Value:无
**********************************/
void tTaskSchedRdy (tTask * task)
{
	tListAddFirst(&(taskTable[task->prio]),&(task->linkNode));
	tBitmapSet(&taskPrioBitmap, task->prio);	
}

/*********************************
**Function name :tTaskSchedUnRdy
**Dexcriptions  :将任务从就绪列表中删除
**parameters    :任务结构体指针
**Returned Value:无
**********************************/
void tTaskSchedUnRdy (tTask * task)
{
	tListRemove(&(taskTable[task->prio]),&(task->linkNode));
	if(tListCount(&taskTable[task->prio])==0)
	{
	tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

/*********************************
**Function name :tTaskSchedRemove
**Dexcriptions  :将任务从优先级队列中移除
**parameters    :任务结构体指针
**Returned Value:无
**********************************/
void tTaskSchedRemove (tTask * task)
{
	tListRemove(&(taskTable[task->prio]),&(task->linkNode));
	if(tListCount(&taskTable[task->prio])==0)
	{
	tBitmapClear(&taskPrioBitmap, task->prio);
	}
}

void tTaskSched(void)
{
	tTask * tempTask;
	uint32_t status = tTaskEnterCritical();
	
	if (schedLockcount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
		
   tempTask = tTaskHighestReady();
	 if(tempTask != currentTask)
	 {
		 nextTask = tempTask;
		 
#if XINOS_ENABLE_HOOKS == 1
		 tHooksTaskSwitch(currentTask,nextTask);
#endif
		 tTaskSwitch();
	 }
	 
	 tTaskExitCritical(status);
}

/*********************************
**Function name :tTaskDelayedInit
**Dexcriptions  :延时队列初始化操作函数
**parameters    :无
**Returned Value:无
**********************************/
void tTaskDelayedInit(void)
{
	tListInit(&tTaskDelayedList);
}

/*********************************
**Function name :tTimeTaskWait
**Dexcriptions  :将任务延时节点插入延时队列中
**parameters    :任务结构体指针
**parameters    :延时的tick数
**Returned Value:无
**********************************/
void tTimeTaskWait(tTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;
	tListAddLast(&tTaskDelayedList,&(task->delayNode));
	task->state |= XINOS_TASK_STATE_DELAYED;
}

/*********************************
**Function name :tTimeTaskWakeUp
**Dexcriptions  :将任务从延时队列中删除
**parameters    :任务结构体指针
**Returned Value:无
**********************************/
void tTimeTaskWakeUp(tTask * task)
{
	tListRemove(&tTaskDelayedList,&(task->delayNode));
	task->state &= ~XINOS_TASK_STATE_DELAYED;
}

/*********************************
**Function name :tTimeTaskRemove
**Dexcriptions  :将任务从延时队列中移除
**parameters    :任务结构体指针
**Returned Value:无
**********************************/
void tTimeTaskRemove(tTask * task)
{
	tListRemove(&tTaskDelayedList,&(task->delayNode));
//	task->state &= ~XINOS_TASK_STATE_DESTROYED;
}

void tTimerTickInit (void)
{
	tickCount = 0;
}
/*********************************
**Function name :tTaskSystemTickHandler
**Dexcriptions  :系统时钟节拍处理
**parameters    :无
**Returned Value:无
**********************************/
void tTaskSystemTickHandler()
{
  tNode * node;
	
	uint32_t status = tTaskEnterCritical();
	
	for(node = tTaskDelayedList.headNode.nextNode; node != &(tTaskDelayedList.headNode); node = node->nextNode)
	{
		tTask * task = tNodeParent(node, tTask, delayNode);
		if(--task->delayTicks == 0)
		{
			if(task->waitEvent) //如果任务在事件等待队列中超时，将任务从等待队列中移除
			{
				tEventRemoveTask(task, (void *)0, tErrorTimeout);
			}
			
			tTimeTaskWakeUp(task); //从等待队列中删除
			
			tTaskSchedRdy(task);   //插入到就绪列表中
		}
	}
	
	if(--currentTask->slice == 0) //判断当前任务的时间片是否已经用完
	{
		if(tListCount(&taskTable[currentTask->prio])>0) //判断此优先级链表下是否还有别的任务
		{
			tListRemoveFirst(&taskTable[currentTask->prio]);//将优先级链表的表头任务添加至表尾
			tListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));
            currentTask->slice=XinOS_SLICE_MAX;			
		}
	}
	tickCount++;
	
#if XINOS_ENABLE_CPUUSAGE_STAT == 1
	checkCpuUsage();  //检查CPU使用率
#endif
	
	tTaskExitCritical(status);

#if XINOS_ENABLE_TIMER == 1 
	tTimerModuleTickNotify(); //
#endif
	
#if XINOS_ENABLE_HOOKS == 1
	tHooksSysTick();
#endif
	//这个过程中可能有任务延时完毕（delayTicks = 0），进行一次调度。
	tTaskSched();
}

#if XINOS_ENABLE_CPUUSAGE_STAT == 1
static float cpuUsage;
static uint32_t enableCpuUsageState;

static void initCpuUsageState(void)
{
	idleCount = 0;
	idleMaxCount = 0;
	cpuUsage = 0.0f;
	enableCpuUsageState = 0;
}

static void checkCpuUsage (void)
{
	if(enableCpuUsageState == 0)
	{
		enableCpuUsageState = 1;
		tickCount=0;
		return;
	}
	
	if(tickCount == TICKS_PER_SEC)
	{
		idleMaxCount = idleCount;
		idleCount = 0;
		
		tTaskSchedEnable();
	}
	else if(tickCount && TICKS_PER_SEC == 0) //每隔1秒检测CPU利用率
	{
		cpuUsage = 100 - (idleCount*100.0 / idleMaxCount);
		idleCount = 0;
	}
}

static void cpuUsageSyncWithSysTick (void)
{
	while (enableCpuUsageState == 0)
	{
		;;
	}
}

float tCpuUsageGet (void)
{
	float usage = 0;
	
	uint32_t status = tTaskEnterCritical();
	usage = cpuUsage;
	tTaskExitCritical(status);
	
	return usage;
	
}
#endif

tTask tTaskIdle;          //空闲任务
tTaskStack idleTaskEnv[XinOS_IDLE_STACK_SIZE];  //空闲任务的栈空间

void idleTaskEntry(void *param)
{
	tTaskSchedDisable();   //关闭调度锁
	
	tInitApp();	
	
#if XINOS_ENABLE_TIMER == 1
	tTimerInitTask();
#endif
	
	tSetSysTickPeriod(XINOS_SYSTICK_MS);
#if XINOS_ENABLE_CPUUSAGE_STAT == 1
	cpuUsageSyncWithSysTick();
#endif	
	for(;;)
	{
		uint32_t status = tTaskEnterCritical();
		idleCount++;
		tTaskExitCritical(status);
#if XINOS_ENABLE_HOOKS == 1
	  tHooksCpuIdle();
#endif
	}
}

int main()
{
	tTaskSchedInit();
	
	tTaskDelayedInit();

#if XINOS_ENABLE_TIMER == 1	
	tTimerModuleInit();
#endif
	
	tTimerTickInit();
	
#if XINOS_ENABLE_CPUUSAGE_STAT == 1	
	initCpuUsageState();
#endif
	
	tTaskInit(&tTaskIdle, idleTaskEntry,(void *)0x0, XinOS_PRO_COUNT-1, idleTaskEnv, XinOS_IDLE_STACK_SIZE);
    idleTask = &tTaskIdle;
	
	nextTask = tTaskHighestReady();

	tTaskRunFirst();
	
	return 0;
}
