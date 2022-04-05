#include "XinOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL            0xE000ED04
#define NVIC_PENDSVSET           0x10000000
#define NVIC_SYSPR12             0xE000ED22
#define NVIC_PENDSV_PRI          0x000000FF

#define MEM32(addr)              *(volatile unsigned long *)(addr)
#define MEM8(addr)               *(volatile unsigned char *)(addr)
	
uint32_t tTaskEnterCritical (void)  //进入临界区保护关闭中断
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void tTaskExitCritical (uint32_t status) //退出临界区保护恢复中断
{
	__set_PRIMASK(status);
}

__asm void PendSV_Handler (void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP
	CBZ R0, PendSV_Handler_nosave
	STMDB R0!,{R4-R11}     //将当前任务的运行状态进行保存，并且将保存数据后的地址存放在R0中
	LDR R1, =currentTask
	LDR R1,[R1]
	STR R0,[R1]           //将保存的R0地址写入currentTask中
	
PendSV_Handler_nosave
	LDR R0, =currentTask   //加载currentTask到R0
	LDR R1, =nextTask      //加载nextTask到R1
	LDR R2, [R1]           //加载nextTask的值到R2
	STR R2, [R0]           //将R2的内容写入到currentTask    即将nextTask的值写入到currentTask中去
	
	LDR R0, [R2]           //将currentTask的值加载到R0
	LDMIA R0!, {R4-R11}    //将栈中保存的值加载到内核寄存器中
	
	MSR PSP, R0     
	ORR LR, LR, #0x04     //返回后使用进程的堆栈
	BX LR
}

void tTaskRunFirst()
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPR12) = NVIC_PENDSV_PRI;    //配置PENDSV中断优先级为最低
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;   //触发PENDSV中断
}

void tTaskSwitch()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;   //触发PENDSV中断	
}
