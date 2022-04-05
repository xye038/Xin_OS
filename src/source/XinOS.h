#ifndef XINOS_H
#define XINOS_H

#include <stdint.h>
#include "string.h"
#include "tLib.h"
#include "tConfig.h"
#include "tEvent.h"
#include "tTask.h"
#include "tSem.h"
#include "tMbox.h"
#include "tMemBlock.h"
#include "tFlagGroup.h"
#include "tMutex.h"
#include "tTimer.h"
#include "tHooks.h"

#define TICKS_PER_SEC         (1000 / XINOS_SYSTICK_MS)

typedef enum _tError {
	  tErrorNoError = 0,
	  tErrorTimeout,
	  tErrorResourceUnavailable,
	  tErrorDel,
	  tErrorResourceFull,
	  tErrorOwner,
}tError;

extern tTask * currentTask;
extern tTask * nextTask;

void tTaskRunFirst (void);
void tTaskSwitch (void);
uint32_t tTaskEnterCritical (void);
void tTaskExitCritical (uint32_t status);
void tTaskSchedInit (void);
void tTaskSchedDisable (void); 
void tTaskSchedEnable (void); 
void tTaskSchedRdy (tTask * task);
void tTaskSchedUnRdy (tTask * task);
void tTaskSchedRemove (tTask * task);
void tTaskSched(void);
void tTimeTaskWait(tTask * task, uint32_t ticks);
void tTimeTaskWakeUp(tTask * task);
void tTimeTaskRemove(tTask * task);
void tTaskSystemTickHandler(void);
void tTaskDelay (uint32_t delay);
void tSetSysTickPeriod (uint32_t ms);
void tInitApp(void);

float tCpuUsageGet (void);



#endif
