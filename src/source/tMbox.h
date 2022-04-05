#ifndef TMBOX_H
#define TMBOX_H

#include "tEvent.h"

#define tMBOXSendNormal      0x00
#define tMBOXSendFront       0x01

typedef struct _tMbox
{
	tEvent event;      //事件结构体
	uint32_t count;    //数量
	uint32_t read;     //读指引
	uint32_t write;    //写指引
	uint32_t maxCount; //最大数量
	void ** msgBuffer; //存放消息变量地址的地址	
}tMbox;//邮箱结构体

typedef struct _tMboxInfo
{
	uint32_t count;    //数量
  uint32_t maxCount; //最大数量
	uint32_t taskCount; //当前等待的任务数量
}tMboxInfo; //邮箱信息结构体

void tMboxInit ( tMbox * mbox, void ** msgBuffer, uint32_t maxCount);  //邮件结构体初始化函数
uint32_t tMboxWait(tMbox * mbox, void **msg, uint32_t waitTicks); //邮箱等待函数
uint32_t tMboxNoWaitGet (tMbox * mbox, void **msg); //邮箱获取信息但不等待
uint32_t tMboxNotify (tMbox * mbox, void *msg, uint32_t notifyOption);//邮箱通知函数
void tMboxFlush (tMbox * mbox);  //清空邮箱接口 将邮箱中的消息队列清空
uint32_t tMboxDestroy(tMbox * mbox);  //删除邮箱接口 将邮箱中的等待任务清空
void tMboxGetInfo(tMbox * mbox, tMboxInfo * info); //邮箱的状态查询接口

#endif
