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
#ifndef APP_LENS_FOCUS_H
#define APP_LENS_FOCUS_H

#define FOCUS_DBG_FUNCTION_EXPORT	0	/* only for debug release */

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "gpio_custom.h"


#if defined(LENS_MODEL_D160)
#include "app_lens_d160.h"
#else
#include "app_lens_dcu10.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef SINT16	TYPE_FOCUS_POS;

/*-- Focus moving direction */
typedef enum focusDir_s {
	FOCUS_DIR_STILL,
	FOCUS_DIR_FAR,
	FOCUS_DIR_NEAR,
	FOCUS_DIR_MAX
} focusDir_t;

/*----------
 * Focus events
 */

/* focus task event */
typedef enum focusTaskEventDef_s {
	EVENT_FOCUS_TASK_INIT			= (((UINT32)1)<<0),
	EVENT_FOCUS_FAR_END_CTRL		= (((UINT32)1)<<1),
	EVENT_FOCUS_NEAR_END_CTRL		= (((UINT32)1)<<2),
	EVENT_FOCUS_CTRL_ACK			= (((UINT32)1)<<3),
	EVENT_FOCUS_TASK_DEL			= (((UINT32)1)<<31)
} focusTaskEventDef_t;


/*-- Zoom Query ID */
typedef enum focusQueryID_s {
	FOCUS_QUERY_ID_MOTION_STATE,
	FOCUS_QUERY_CURRENT_POS,
	FOCUS_QUERY_ID_HOME_POS,
	FOCUS_QUERY_ID_IS_IO_HOME,
	FOCUS_QUERY_ID_MAX
} focusQueryID_t;



/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void lensFocusCreate(void);
void lensFocusDestroy(void);
UINT32 lensFocusReset(void);
UINT32 lensFocusBoot(void);
UINT32 lensFocusCrashRecover(void);
BOOL lensFocusMovingCheck(void);
UINT32 lensFocusDefaultPosGo(void);
TYPE_FOCUS_POS lensFocusDefaultPosGet(void);

void lensFocusStopWait(void);
UINT32 lensFocusPosGo(TYPE_FOCUS_POS pos);
UINT32 lensFocusJob(TYPE_FOCUS_POS pos);
UINT32 lensFocusQuery(UINT32 queryId);
void lensFocusTask(ULONG init_data);
void lensFocusTaskInit(void);


/*-------------------------------------------------------------------------
 *  DEBUG FUNCTION DECLARATIONS                                           *
 -------------------------------------------------------------------------*/
#define DBG_FOCUS_NULL_FUNC		do { } while(0)
#if FOCUS_DBG_FUNCTION_EXPORT
void dbg_focusCreate(void);
void dbg_focusDestroy(void);
void dbg_focusParamDump(void);
void dbg_focusMotorFarNext(void);
void dbg_focusMotorNearNext(void);
void dbg_focusStepsGo(UINT8	focusDir,	UINT16	stepCnt);
UINT32 dbg_focusHomeGo(void);
#else
#define dbg_focusCreate()					DBG_FOCUS_NULL_FUNC
#define dbg_focusDestroy()					DBG_FOCUS_NULL_FUNC
#define dbg_focusParamDump()				DBG_FOCUS_NULL_FUNC
#define dbg_focusMotorFarNext()				DBG_FOCUS_NULL_FUNC
#define dbg_focusMotorNearNext()			DBG_FOCUS_NULL_FUNC
#define dbg_focusStepsGo(focusDir, stepCnt)	DBG_FOCUS_NULL_FUNC
#define dbg_focusHomeGo(focusDir, stepCnt)	DBG_FOCUS_NULL_FUNC
#endif

#endif  /* APP_LENS_FOCUS_H */


