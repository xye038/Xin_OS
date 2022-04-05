#ifndef TTASK_H
#define TTASK_H

#define XINOS_TASK_STATE_RDY               0         //任务就绪状态
#define XINOS_TASK_STATE_DESTROYED         (1 << 1)  //任务删除状态
#define XINOS_TASK_STATE_DELAYED           (1 << 2)  //任务延时状态
#define XINOS_TASK_STATE_SUSPEND           (1 << 3)  //任务挂起状态

#define XINOS_TASK_WAIT_MASK               (0xFF << 16)

struct _tEvent;
typedef uint32_t  tTaskStack;     //栈元素结构类型


typedef struct _tTask{
     tTaskStack * stack;          //指向栈的指针
	   uint32_t * stackBase;        //堆栈的起始位置
	   uint32_t  stackSize;         //堆栈的总大小
	   tNode  linkNode;             //优先级队列的链接节点
	   uint32_t  delayTicks;        //软延时计数器
	   tNode delayNode;             //延时节点，用来在有延时的时候将其插入延时队列中去
	   uint32_t  prio;              //优先级
	   uint32_t  state;             //任务状态 （就绪、非就绪）
	   uint32_t  slice;             //时间片的计数器
	   uint32_t  suspendCount;      //挂起计数器
	
	   void (*clean) (void *param); //任务删除回调函数
	   void *cleanParam;            //任务删除参数
	   uint8_t requestDeleteFlag;   //任务删除标志位
	
	   struct _tEvent * waitEvent;  //等待事件
	   void * eventMsg;             //等待信号
	   uint32_t waitEventResult;    //等待结果
	
	   uint32_t waitFlagsType;      //等待事件组的类型 
	   uint32_t eventFlags;         //等待事件组的标记
}tTask;                           //任务结构体

typedef struct _tTaskInfo{
     uint32_t delayTicks;
	   uint32_t prio;
	   uint32_t state;
	   uint32_t slice;
	   uint32_t suspendCount; 
	
	   uint32_t stackSize;          //堆栈的大小
	   uint32_t stackFree;          //空闲的堆栈大小
}tTaskInfo;                       //任务信息结构体

void tTaskInit(tTask * task, void(*entry)(void *), void *param, uint32_t prio, tTaskStack *stack, uint32_t size);
void tTaskSuspend (tTask * task);
void tTaskWakeUp (tTask * task);
void tTaskSetCleanCallFunc(tTask* task, void(*clean)(void * param), void *param);
void tTaskForceDelete(tTask * task);
void tTaskRequestDelete(tTask * task);
uint8_t tTaskIsRequestedDeleted(void);
void tTaskDeleteSelf (void);
void tTaskGetInfo(tTask * task, tTaskInfo * Info);


#endif
