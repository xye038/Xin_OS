#include "XinOS.h"
#include "ARMCM3.h"

#define NVIC_INT_CTRL            0xE000ED04
#define NVIC_PENDSVSET           0x10000000
#define NVIC_SYSPR12             0xE000ED22
#define NVIC_PENDSV_PRI          0x000000FF

#define MEM32(addr)              *(volatile unsigned long *)(addr)
#define MEM8(addr)               *(volatile unsigned char *)(addr)
	
uint32_t tTaskEnterCritical (void)  //�����ٽ��������ر��ж�
{
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	return primask;
}

void tTaskExitCritical (uint32_t status) //�˳��ٽ��������ָ��ж�
{
	__set_PRIMASK(status);
}

__asm void PendSV_Handler (void)
{
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP
	CBZ R0, PendSV_Handler_nosave
	STMDB R0!,{R4-R11}     //����ǰ���������״̬���б��棬���ҽ��������ݺ�ĵ�ַ�����R0��
	LDR R1, =currentTask
	LDR R1,[R1]
	STR R0,[R1]           //�������R0��ַд��currentTask��
	
PendSV_Handler_nosave
	LDR R0, =currentTask   //����currentTask��R0
	LDR R1, =nextTask      //����nextTask��R1
	LDR R2, [R1]           //����nextTask��ֵ��R2
	STR R2, [R0]           //��R2������д�뵽currentTask    ����nextTask��ֵд�뵽currentTask��ȥ
	
	LDR R0, [R2]           //��currentTask��ֵ���ص�R0
	LDMIA R0!, {R4-R11}    //��ջ�б����ֵ���ص��ں˼Ĵ�����
	
	MSR PSP, R0     
	ORR LR, LR, #0x04     //���غ�ʹ�ý��̵Ķ�ջ
	BX LR
}

void tTaskRunFirst()
{
	__set_PSP(0);
	
	MEM8(NVIC_SYSPR12) = NVIC_PENDSV_PRI;    //����PENDSV�ж����ȼ�Ϊ���
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;   //����PENDSV�ж�
}

void tTaskSwitch()
{
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;   //����PENDSV�ж�	
}
