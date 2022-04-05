#ifndef __TCONFIG_H_
#define __TCONFIG_H_

#define XinOS_PRO_COUNT    32
#define XinOS_SLICE_MAX    10
#define XinOS_IDLE_STACK_SIZE 1024

#define XinOS_TIMERTASK_STACK_SIZE 1024
#define XinOS_TIMERTASK_PRIO       1

#define XINOS_SYSTICK_MS           10

#define XINOS_ENABLE_SEM           1
#define XINOS_ENABLE_MUTEX         1
#define XINOS_ENABLE_FLAGGROUP     1
#define XINOS_ENABLE_MBOX          1
#define XINOS_ENABLE_MEMBLOCK      1
#define XINOS_ENABLE_TIMER         1
#define XINOS_ENABLE_CPUUSAGE_STAT 1
#define XINOS_ENABLE_HOOKS         1

#endif

