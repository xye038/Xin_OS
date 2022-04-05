#include "XinOS.h"

tTask tTask1;             //task1的结构体
tTask tTask2;             //task2的结构体
tTask tTask3;             //task3的结构体
tTask tTask4;             //task4的结构体

tTaskStack task1Env[1024]; //task1的栈空间
tTaskStack task2Env[1024]; //task2的栈空间
tTaskStack task3Env[1024]; //task3的栈空间
tTaskStack task4Env[1024]; //task4的栈空间


int task1Flag;
void task1Entry (void *param)
{	
		
	for(;;)
	{			
		task1Flag = 0;
		tTaskDelay(1);		
		task1Flag = 1;
		tTaskDelay(1);
	}
}


int task2Flag;
void task2Entry (void *param)
{	
	for(;;)
	{				
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
	}
}

int task3Flag;
void task3Entry (void *param)
{
	for(;;)
	{		
		task3Flag = 0;
		tTaskDelay(1);
		task3Flag = 1;
		tTaskDelay(1);
	}
}

int task4Flag;
void task4Entry (void *param)
{
	for(;;)
	{ 		
		task4Flag = 0;
		tTaskDelay(1);
		task4Flag = 1;
		tTaskDelay(1);		
	}
}

void tInitApp(void)
{
	tTaskInit(&tTask1, task1Entry,(void *)0x11111111, 0, task1Env, sizeof(task1Env));
	tTaskInit(&tTask2, task2Entry,(void *)0x22222222, 1, task2Env, sizeof(task2Env));
	tTaskInit(&tTask3, task3Entry,(void *)0x22222222, 1, task3Env, sizeof(task3Env));
	tTaskInit(&tTask4, task4Entry,(void *)0x44444444, 1, task4Env, sizeof(task4Env));
}
