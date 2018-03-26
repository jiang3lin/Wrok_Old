/**************************************************************************
 *
 *        Copyright (c) 2006-2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C. 
 *
 **************************************************************************/
#ifndef APP_LENS_PRIVATE_H
#define APP_LENS_PRIVATE_H

/* global control for lens private debug at development phase */
#define LENS_PRIVATE_DBG_MASTER		0

/* global control for lens log at official release */
#define LENS_PRIVATE_LOG_MASTER		1

/* global control for lens private debug for ISR/Callback
 * !!! MUST BE OFF except debugging ISR or CALLBACK !!!!
 */
#define LENS_ISR_DBG_ENABLE			(0 && LENS_PRIVATE_DBG_MASTER)

/* other dbg control for lens at development phase */
#define LENS_PRIVATE_DBG_FUNC_TRACE	0
#define LENS_PRIVATE_DBG_VERBOSE	0

/*-- Control of interleaving focus & zoom at boot time */
#define LENS_BOOT_FOCUS_EARLY		0

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "api/sp5k_os_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/*----------
 * Lens tasks configuration
 */
#define LENS_APP_TASK_PRIORITY			5
#define LENS_APP_TASK_TIME_SLICE		0

#define LENS_ZOOM_TASK_PRIORITY			4
#define LENS_ZOOM_TASK_TIME_SLICE		5 /* to share time slice for zoom & focus */

#define LENS_FOCUS_TASK_PRIORITY		4
#define LENS_FOCUS_TASK_TIME_SLICE		5 /* to share time slice for zoom & focus */



/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
/*----------
 * Lens Debug macros
 */
#define LENS_DBG_NULL_FUNC	do {} while(0)

#if LENS_PRIVATE_LOG_MASTER
#define lens_log(str, arg...)	printf(str, ##arg)
#else
#define lens_log(str, arg...)	LENS_DBG_NULL_FUNC
#endif

#if LENS_PRIVATE_DBG_MASTER
#define lens_printf(str, arg...)	printf(str, ##arg)
#else
#define lens_printf(str, arg...)	LENS_DBG_NULL_FUNC
#endif

#if LENS_PRIVATE_DBG_FUNC_TRACE
#define lens_func_trace(str, arg...)	lens_printf(str, ##arg)
#else
#define lens_func_trace(str, arg...)	LENS_DBG_NULL_FUNC
#endif

#if LENS_PRIVATE_DBG_VERBOSE
#define lens_verbose(str, arg...)		lens_printf(str, ##arg)
#else
#define lens_verbose(str, arg...)		LENS_DBG_NULL_FUNC
#endif

#if LENS_ISR_DBG_ENABLE
#define LENS_ISR_DBG(str, arg...)	lens_printf(str, ##arg)
#else
#define LENS_ISR_DBG(str, arg...)	LENS_DBG_NULL_FUNC
#endif

#define LENS_LOG(str, arg...)		lens_log(str, ##arg)
#define LENS_DBG(str, arg...)		lens_printf(str, ##arg)
#define LENS_VERBOSE(str, arg...)	lens_verbose(str, ##arg)
#define LENS_NOT_READY	printf("\n@@@@@@@@@@@ %s, %d : not implemented yet !! \n\n", __FUNCTION__, __LINE__)

#define LENS_FIN()				lens_func_trace(">>%s\n",__FUNCTION__)
#define LENS_FINA(str, arg...)	do { \
										lens_func_trace(">>%s: ",__FUNCTION__); \
										lens_func_trace(str, ##arg); \
										lens_func_trace("\n");	\
									} while(0)
#define LENS_FOUT()				lens_func_trace("\t<<%s",__FUNCTION__)

/*----------
 * Lens OS-specific macros
 */

/*-- Event */
#define lensOsEventGet(pev, wait_flag, pret_flag) \
		sp5kOsEventFlagsGet((TX_EVENT_FLAGS_GROUP *)pev, (ULONG)wait_flag, TX_OR_CLEAR, (ULONG *)pret_flag, TX_WAIT_FOREVER)

#define lensOsEventCheck(pev, wait_flag, pret_flag) \
		sp5kOsEventFlagsGet((TX_EVENT_FLAGS_GROUP *)pev, (ULONG)wait_flag, TX_OR_CLEAR, (ULONG *)pret_flag, TX_NO_WAIT)

#define lensOsEventSet(pev, flag)	\
		sp5kOsEventFlagsSet((TX_EVENT_FLAGS_GROUP *)pev, (ULONG)flag, TX_OR)

#define lensOsEventCreate(pev, label) \
		sp5kOsEventFlagsCreate((TX_EVENT_FLAGS_GROUP *)pev, (char *)label)

/*-- Mutex */
#define lensOsMutexPut(pmtx)			sp5kOsMutexPut((TX_MUTEX *)pmtx)
#define lensOsMutexGet(pmtx)			sp5kOsMutexGet((TX_MUTEX *)pmtx, TX_WAIT_FOREVER)
#define lensOsMutexCreate(pmtx, label)	sp5kOsMutexCreate((TX_MUTEX *)pmtx, (char *)label, TX_INHERIT)

/*-- Task */
typedef void (*LENS_TASK_PROC)(ULONG);
#define lensOsTaskCreate(label, proc, priority, tm_slice) \
		sp5kOsThreadCreate((char *)label, (LENS_TASK_PROC)proc, 0, priority, priority, tm_slice, TX_AUTO_START)

/*-- MSG */
#define LENS_POST_MSG(msg, param)		sp5kHostMsgSend((UINT32)msg, (UINT32)param)

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* APP_LENS_PRIVATE_H */


