#ifndef TIMER_H
#define TIMER_H
#include "tEvent.h"

typedef enum _tTimerState
{
	tTimerCreated,
	tTimerStarted,
	tTimerRunning,
	tTimerStopped,
	tTimerDestroyed
}tTimerState;

typedef struct _tTimer
{
	tNode linkNode;  //定时器节点
	uint32_t startDelayTicks;   //初始延时计数值
  uint32_t durationTicks;     //循环执行计数值
  uint32_t delayTicks;        //当前计数值
	void (*timerFunc) (void *arg);  //定时器的回调处理函数
	void *arg;   //回调处理函数参数
	uint32_t config;   //配置参数
	
	tTimerState state;  //定时器状态
}tTimer;

typedef struct _tTimerInfo
{
	uint32_t startDelayTicks;   //初始延时计数值
  uint32_t durationTicks;     //循环执行计数值
	void (*timerFunc) (void *arg);  //定时器的回调处理函数
	void *arg;   //回调处理函数参数
	uint32_t config;   //配置参数	
	tTimerState state;  //定时器状态
}tTimerInfo;

#define TIMER_CONFIG_TYPE_HARD       (1 << 0)  //在中断服务程序中处理任务
#define TIMER_CONFIG_TYPE_SOFT       (0 << 0)  //在线程服务函数中处理任务

void tTimerInit (tTimer * timer, uint32_t delayTicks, uint32_t durationTicks, 
void (*TimerFunc)(void *arg), void * arg, uint32_t config);
void tTimerInitTask (void);
void tTimerStart (tTimer * timer);
void tTimerStop (tTimer * timer);
void tTimerDestroy (tTimer * timer);
void tTimerModuleTickNotify(void);
void tTimerModuleInit(void);
void tTimerGetInfo(tTimer * timer, tTimerInfo * info);

#endif
