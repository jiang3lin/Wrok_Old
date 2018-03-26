/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2006 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Stephen Hung                                                  *
 *                                                                        *
 **************************************************************************/
#ifndef _SP5K_OS_API_H_
#define _SP5K_OS_API_H_

#ifdef   __cplusplus
extern   "C" {
#endif

#include "common.h"
#include "os/_tx_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* Error code */
#define SP5K_SUCCESS                            0x00000000
#define SP5K_ERROR                              0x00000001

/* ==============================================
 -- Thread's priority --
=================================================*/
/* -- PRIORITY -- */
#define TX_PRIORITY_0                  0
#define TX_PRIORITY_1                  1
#define TX_PRIORITY_2                  2
#define TX_PRIORITY_3                  3
#define TX_PRIORITY_4                  4
#define TX_PRIORITY_5                  5
#define TX_PRIORITY_6                  6
#define TX_PRIORITY_7                  7
#define TX_PRIORITY_8                  8
#define TX_PRIORITY_9                  9
#define TX_PRIORITY_10                 10
#define TX_PRIORITY_11                 11
#define TX_PRIORITY_12                 12
#define TX_PRIORITY_13                 13
#define TX_PRIORITY_14                 14
#define TX_PRIORITY_15                 15
#define TX_PRIORITY_16                 16
#define TX_PRIORITY_17                 17
#define TX_PRIORITY_18                 18
#define TX_PRIORITY_19                 19
#define TX_PRIORITY_20                 20
#define TX_PRIORITY_21                 21
#define TX_PRIORITY_22                 22
#define TX_PRIORITY_23                 23
#define TX_PRIORITY_24                 24
#define TX_PRIORITY_25                 25
#define TX_PRIORITY_26                 26
#define TX_PRIORITY_27                 27
#define TX_PRIORITY_28                 28
#define TX_PRIORITY_29                 29
#define TX_PRIORITY_30                 30
#define TX_PRIORITY_31                 31

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/* Os Thread */
TX_THREAD *sp5kOsThreadCreate(char  *name_ptr, VOID  (*entry_function)(ULONG), ULONG entry_input, UINT  priority, UINT  preempt_threshold, ULONG time_slice_milli_sec, UINT  auto_start);
UINT32 sp5kOsThreadTerminate(TX_THREAD *thread_ptr);
UINT32 sp5kOsThreadDelete(TX_THREAD *thread_ptr);

/* Os queue */
UINT32 sp5kOsQueueCreate(TX_QUEUE *queue_ptr, CHAR *name_ptr, UINT message_size, VOID *queue_start, ULONG queue_size);
UINT32 sp5kOsQueueDelete(TX_QUEUE *queue_ptr);
UINT32 sp5kOsQueueReceive(TX_QUEUE *queue_ptr, VOID *destination_ptr, ULONG wait_option);
UINT32 sp5kOsQueueSend(TX_QUEUE *queue_ptr, VOID *source_ptr, ULONG wait_option);
UINT32 sp5kOsQueueCntGet(TX_QUEUE *queue_ptr, UINT32 *cnt, UINT32 *size);

/* Os Mutex */
UINT32 sp5kOsMutexCreate(TX_MUTEX *mutex_ptr, CHAR *name_ptr, UINT inherit);
UINT32 sp5kOsMutexDelete(TX_MUTEX *mutex_ptr);
UINT32 sp5kOsMutexGet(TX_MUTEX *mutex_ptr, ULONG wait_option);
UINT32 sp5kOsMutexPut(TX_MUTEX *mutex_ptr);

/* Os Event */
UINT32 sp5kOsEventFlagsCreate(TX_EVENT_FLAGS_GROUP *event_ptr, CHAR *name_ptr);
UINT32 sp5kOsEventFlagsDelete(TX_EVENT_FLAGS_GROUP *event_ptr);
UINT32 sp5kOsEventFlagsSet(TX_EVENT_FLAGS_GROUP *event_ptr, ULONG flags, ULONG set_option);
UINT32 sp5kOsEventFlagsGet(TX_EVENT_FLAGS_GROUP *event_ptr, ULONG req_flags, UINT get_option, ULONG *flags_ptr, ULONG wait_option);

/* semaphore */
UINT32 sp5kOsSemaphoreCreate(TX_SEMAPHORE *semaphore_ptr, CHAR *name_ptr, UINT32 init_val);
UINT32 sp5kOsSemaphoreGet(TX_SEMAPHORE *semaphore_ptr, ULONG wait_option);
UINT32 sp5kOsSemaphoreCntGet(TX_SEMAPHORE *semaphore_ptr, UINT32 *pcnt);
UINT32 sp5kOsSemaphorePut(TX_SEMAPHORE *semaphore_ptr);
UINT32 sp5kOsSemaphoreDestroy(TX_SEMAPHORE *semaphore_ptr);

/* Misc */
UINT32 sp5kOsTimeGet(void);
void*  sp5kMalloc(UINT32 size);
void   sp5kFree(void *pbuf);
void*  sp5kMallocCache(UINT32 size);
void*  sp5kMallocExt(UINT32 size);
void*  sp5kMallocCacheExt(UINT32 size);

#ifdef   __cplusplus
}
#endif

#endif /* _SP5K_OS_API_H_ */

