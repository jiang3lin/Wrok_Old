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
#ifndef APP_LENS_ZOOM_H
#define APP_LENS_ZOOM_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "gpio_custom.h"

typedef SINT16	TYPE_ZOOM_POS;


#if defined(LENS_MODEL_D160)
	#include "app_lens_d160.h"
#else
	#include "app_lens_dcu10.h"
#endif


/**************************************************************************
 *                        C O M M O N                                     *
 **************************************************************************/
/*-- Zoom Wait still notifiy callback function prototype */
#define ZOOM_OP_BLOCKING		((BOOL)1)
#define ZOOM_OP_NON_BLOCKING	((BOOL)0)

typedef void zoomStopNotifyCBF_t(TYPE_ZOOM_POS stopPos, UINT16 stopSt);

/*-- Zoom Error Code */
typedef enum zoomErrorCode_s {
	ZOOM_ERROR_NONE,
	ZOOM_ERROR_JAMMED,
	ZOOM_ERROR_STILL_MOVING,
	ZOOM_ERROR_MAX
} zoomErrorCode_t;

/*-- Zoom Motion State */
typedef enum zoomMotionState_s {
	ZOOM_MXN_STATE_STOP	,
	ZOOM_MXN_STATE_TELE		,
	ZOOM_MXN_STATE_WIDE		,
	ZOOM_MXN_STATE_BRAKING	,
	ZOOM_MXN_STATE_JAMMED	,
	ZOOM_MXN_STATE_MAX
} zoomMotionState_t;

/*-- Zoom Motion orientation */
typedef enum zoomMotionDir_s {
	ZOOM_DIR_STOP,
	ZOOM_DIR_TELE,
	ZOOM_DIR_WIDE,
	ZOOM_DIR_MAX
} zoomMotionDir_t;


/*-- Zoom Park Flag */
typedef enum zoomParkFlag_s {
	/* Be careful of the order */
	ZOOM_PARK_NONE		= ((UINT8)0),
	ZOOM_PARK_TELE		= ((UINT8)1)<<0,
	ZOOM_PARK_WIDE		= ((UINT8)1)<<1,
	ZOOM_PARK_CLOSE		= ((UINT8)1)<<2,
	ZOOM_PARK_MAX		= ((UINT8)1)<<7
} zoomParkFlag_t;

/*----------
 * general Zoom operation control block
 */
typedef struct zoomCtrlParam_s {
	TYPE_ZOOM_POS irqZoomPos;
	TYPE_ZOOM_POS irqZoomClkCnt;

	TYPE_ZOOM_POS tgtStopPos;	/* the position to be stopped */
	TYPE_ZOOM_POS lastChkPos;	/* last brake/jam check position  */

	UINT16	teleSlipCnt;	/* slip step cnt for zoom tele */
	UINT16	wideSlipCnt;	/* slip step cnt for zoom wide */

	UINT16	curZSeg;		/* current Zoom Segment */
	UINT16	posBrake;		/* the step postion while BRAKE starts */

	UINT16 	blashSlipCnt;	/* slip step cnt for backlash */
//	UINT8	motorSpeed;		/* control the motor speed */
	UINT8	brkConfirmCnt;	/* check count that lastPos==curPos during braking */

	UINT8 	motionDir;	/* TELE/WIDE */
	UINT8 	parkFlag;		/* indicating zoom lens special stop state, CLOSE/TELE/WIDE */
	UINT16	chkCnt;		/* the count during waiting lens to stop or Jam check */

	zoomStopNotifyCBF_t *stopNotifyCBF;

	UINT8 	motionStat;		/* zoomMotionState_t : TELE/WIDE/STILL/BRAKING/JAMMED */
	UINT8 	isAskStopCB;	/* TRUE: waitStill w/ non-blocking and CBF has registered ! */

	UINT32 errCode;	//UINT8 nLensErr;

} zoomCtrlParam_t;

/*-- Zoom Query ID */
typedef enum zoomQueryID_s {
	ZOOM_QUERY_MOTION_STATE,
	ZOOM_QUERY_SEG_POS,
	ZOOM_QUERY_SEG_NUM,
	ZOOM_QUERY_SEG2POS,
	ZOOM_QUERY_STEP_POS,
	ZOOM_QUERY_BRAKE_POS,
	ZOOM_QUERY_ID_MAX
} zoomQueryID_t;

/*-- Zoom Motor Speed */
typedef enum zoomMotorSpeed_s {
	ZOOM_SPEED_FAST,
	ZOOM_SPEED_NORMAL,
	ZOOM_SPEED_SLOW,
	ZOOM_SPEED_TOTAL,
} zoomMotorSpeed_t;

typedef enum {
	ZOOM_SLIP_SXN_BOOT,
	ZOOM_SLIP_SXN_CLOSE,
	ZOOM_SLIP_SXN_TELEWIDE,
	ZOOM_SLIP_SXN_BLASH,
	ZOOM_SLIP_SXN_TOTAL,
} zoomSlipCntSxN_t;

/*----------
 * Zoom common events
 */

/* zoom task event */
typedef enum zoomTaskEventDef_s {
	EVENT_ZOOM_TASK_INIT			= (((UINT32)1)<<0),
	EVENT_ZOOM_MOVING_CTRL			= (((UINT32)1)<<1),
	EVENT_ZOOM_BRAKING_CTRL			= (((UINT32)1)<<2),
	EVENT_ZOOM_TASK_DEL				= (((UINT32)1)<<31)
} zoomTaskEventDef_t;



void lensZoomTask(ULONG init_data);
void lensZoomTaskInit(void);
void lensZoomPosUpdateEnable(BOOL isEn);
UINT16 lensZoomDefaultSlipGet(zoomSlipCntSxN_t zmSxn, zoomMotorSpeed_t zmSpeed);
void lensZoomTeleSlipCntSet(UINT8 slipCnt);
void lensZoomWideSlipCntSet(UINT8 slipCnt);
void lensZoomBacklashSlipCntSet(UINT8 slipCnt);
#ifdef LENS_ZOOM_START_BOOST
void lensZoomBoostStart(void);
void lensZoomBoostRestore(void);
#endif
UINT8 lensZoomSpeedGet(void);
void lensZoomSpeedSet(UINT8 zoomSpd);
BOOL lensZoomTeleParked(void);
BOOL lensZoomWideParked(void);
BOOL lensZoomCloseParked(void);
BOOL lensZoomMovingCheck(void);

void lensZoomClkCntClear(void);
UINT32 lensZoomQuery(UINT32 queryId, UINT32 qryParam);
TYPE_ZOOM_POS lensZoomNextStopPosSet(void);
TYPE_ZOOM_POS lensZoomStopPosGet(void);
void lensZoomStopPosSet(TYPE_ZOOM_POS tgtPos);
TYPE_ZOOM_POS lensZoomCurPosGet(void);
void lensZoomCurPosSet(TYPE_ZOOM_POS pos);

UINT32 lensZoomBacklash(void);
UINT16 lensZoomCurSegGet(void);
UINT32 lensZoomPosGo(TYPE_ZOOM_POS tgtPos);
void lensZoomResetConfig(void);
UINT32 lensZoomCrashReset(void);
void lensZoomHomeISREnable(void);
void lensZoomHomeISRDisable(void);
void lensZoomStopNotifyCBFRegister(zoomStopNotifyCBF_t stopNotifyCBF);
void zoomStopNotifyDefaultCBF( TYPE_ZOOM_POS stillPos, UINT16 stillSt);
UINT32 lensZoomStopWait(BOOL isBlockingWait);
UINT32 lensZoomBrakeTrigWait(void);
void lensZoomDefaultSlipCalibWrite(
	zoomSlipCntSxN_t zmSxn, /* Ref : zoomSlipCntSxN_t */
	zoomMotorSpeed_t zmSpeed,
	UINT16 newSlip
);
void lensZoomCalibSessionControl(BOOL isCalibOn);
#if 0
/*-------------------------------------------------------------------------
 *  DEBUG FUNCTION DECLARATIONS                                           *
 -------------------------------------------------------------------------*/
#define ZOOM_DBG_FUNCTION_EXPORT		1
#define DBG_ZOOM_NULL_FUNC		do { } while(0)
#if ZOOM_DBG_FUNCTION_EXPORT
void dbg_zoomMotorTele(void);
void dbg_zoomMotorWide(void);
void dbg_zoomMotorBrake(void);
void dbg_zoomMotorStandby(void);
#else
#define dbg_zoomMotorTele()		DBG_ZOOM_NULL_FUNC
#define dbg_zoomMotorWide()		DBG_ZOOM_NULL_FUNC
#define dbg_zoomMotorBrake()	DBG_ZOOM_NULL_FUNC
#define dbg_zoomMotorStandby()	DBG_ZOOM_NULL_FUNC
#endif /* DBG_ZOOM_NULL_FUNC */
#endif

#endif  /* APP_LENS_ZOOM_H */

