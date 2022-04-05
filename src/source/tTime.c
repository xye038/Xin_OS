#include "XinOS.h"


void tTaskDelay (uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	tTimeTaskWait(currentTask, delay);//将目前正在执行的任务插入等待队列
	
	tTaskSchedUnRdy(currentTask);//从就绪列表中删除
	
	tTaskExitCritical(status);
	
	//进行任务切换，切换至另一个任务，或者空闲任务
	//delayTicks会在时钟中自动减1，当减至0时，会切换回来继续运行
	tTaskSched();
}
