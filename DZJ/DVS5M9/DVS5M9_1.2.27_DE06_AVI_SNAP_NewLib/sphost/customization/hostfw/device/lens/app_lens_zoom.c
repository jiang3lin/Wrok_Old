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
#define HOST_DBG 0
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_ui_msg.h"
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_motor.h"
#include "app_lens_zoom.h"
#include "app_lens_api.h"

#include "middleware\proflog.h"

#if KIT_WITH_LENS
#define ZOOM_CONTROL_MOTOR_LED_POWER	0

#define ZOOM_BRAKE_CHK_BY_ZOOM_TASK		1

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ZOOM_BRAKE_INITIAL_SLEEP_MS		(100)
#define ZOOM_BRAKE_WATCHDOG_MS_PER_TICK	(10)
#define ZOOM_BRAKE_WATCHDOG_INTERVAL_MS	(20)//(60)
#define ZOOM_BRAKE_WATCHDOG_INTERVAL_TiCKS	(ZOOM_BRAKE_WATCHDOG_INTERVAL_MS/ZOOM_BRAKE_WATCHDOG_MS_PER_TICK)
#define ZOOM_BRAKE_ERROR_CHECK_CNT_MAX	(2000/ZOOM_BRAKE_WATCHDOG_INTERVAL_MS)
		/* the max value depends upon the polling interval during
		 * ZOOM_BRAKE period. It could be the watchdog timer or
		 * some other mechanism. At present, I set 1sec for timeout.
		 */
#define ZOOM_STOP_DONE_MIN_CHECK_TIME	(160)
		/* unit : ms
		 * Define the minimum time interval to confirm the Zoom has stop completely.
		 */
#define ZOOM_STOP_DONE_WATCHDOG_CHK_CNT	2 //(ZOOM_STOP_DONE_MIN_CHECK_TIME/ZOOM_BRAKE_WATCHDOG_INTERVAL_MS)

#define ZOOM_MOVING_CTRL_INTERVAL_MS	(5)
#define ZOOM_JAM_CHECK_CNT_MAX			(15*1000/(ZOOM_MOVING_CTRL_INTERVAL_MS*10))
		/* the max value depends upon the polling interval during
		 * ZOOM_MOVING_CTRL period. It could be the watchdog timer or
		 * some other mechanism. At present, I set 1.5 sec for timeout.
		 */
#define ZOOM_POS_NOTIFY_INTERVAL_MS	(60)

#define ZOOM_BACKLASH_STEP_CNT		40 // 80
#define ZOOM_DEFAULT_TELE_SLIP_CNT	54
#define ZOOM_DEFAULT_WIDE_SLIP_CNT	54
#define ZOOM_DEFAULT_SLIP_CNT		30

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum zoomWatchdogServiceID_s {
	WATCHDOG_ZOOM_MOVING,
	WATCHDOG_ZOOM_BRAKING,
	WATCHDOG_ZOOM_MAX
} zoomWatchdogServiceID_t;


/*-- zoom segment pos array index.
 * ZOOM_SEG_CLOSE is only used to mark the close pos,
 * do not use this index to retrieve zoom pos !!
 */
typedef enum zoomSegIndex_s {
	ZOOM_SEG_WIDE_STOP	= 0,
	ZOOM_SEG_TELE_STOP	= ZOOM_ZS_NUM-1,
	ZOOM_SEG_MAX		= ZOOM_ZS_NUM,
	ZOOM_SEG_CLOSE
} zoomSegIndex_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static TX_EVENT_FLAGS_GROUP evZoomCtrl; /* Zoom op control event */
static TX_MUTEX mtxZoomCtrl;
static zoomCtrlParam_t gzoomCb;			/* zoom global control param block */

#if (!ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
static UINT32 gzmWatchdogTbl[WATCHDOG_ZOOM_MAX]={0};
#endif

static BOOL	isRecurZMoving = FALSE;

static int 	gzmPosUpdateCnt=0;

#ifdef LENS_ZOOM_START_BOOST
static UINT8 zoomSpeedBackup=ZOOM_SPEED_TOTAL;
#endif


#if defined(LENS_MODEL_D160)
	static TYPE_ZOOM_POS gzmZSA[ZOOM_ZS_NUM] = {
		(ZOOM_WIDE_POS),		//0
		(ZOOM_WIDE_POS + 133),	//133
		(ZOOM_WIDE_POS + 214),	//214
		(ZOOM_WIDE_POS + 293),	//293
		(ZOOM_WIDE_POS + 377),	//377
		(ZOOM_TELE_POS),		//543
	};

	static UINT16 gzmDefaultSlipCnt[ZOOM_SLIP_SXN_TOTAL][ZOOM_SPEED_TOTAL] =
	{
		/*-- ZOOM_BOOT : FAST, NORMAL, SLOW */
		{10, 5, 5},
		/*-- ZOOM_CLOSE: FAST, NORMAL, SLOW */
		{10, 5, 5},
		/*-- ZOOM_TELEWIDE : FAST, NORMAL, SLOW */
		{10, 5, 5},
		/*-- ZOOM_BLASH: FAST, NORMAL, SLOW */
		{10, 5, 5},
	};
#else
	static TYPE_ZOOM_POS gzmZSA[ZOOM_ZS_NUM] = {
		(ZOOM_WIDE_POS),		//975
		(ZOOM_WIDE_POS + 117),	//1092
		(ZOOM_WIDE_POS + 233),	//1208
		(ZOOM_WIDE_POS + 361),	//1336
		(ZOOM_WIDE_POS + 514),	//1489
		(ZOOM_WIDE_POS + 703),	//1678
		(ZOOM_TELE_POS),		//1913
	};

	static UINT16 gzmDefaultSlipCnt[ZOOM_SLIP_SXN_TOTAL][ZOOM_SPEED_TOTAL] =
	{
		/*-- ZOOM_BOOT : FAST, NORMAL, SLOW */
		{58, 30, 16},
		/*-- ZOOM_CLOSE: FAST, NORMAL, SLOW */
		{58, 30, 16},
		/*-- ZOOM_TELEWIDE : FAST, NORMAL, SLOW */
		{58, 30, 30},
		/*-- ZOOM_BLASH: FAST, NORMAL, SLOW */
		{58, 30, 35},
	};
#endif

static BOOL isCountInBacklash = FALSE;
			/*-- Determine if the backlash has to be taken
			 * into account while calculating the stop pos.
			 * Typically, backlash will not be counted in while
			 * closing lens.
			 * This global is determined in lensZoomPosGo while
			 * going zoom-out.
			 */

static BOOL isDoingBacklash = FALSE;
			/*-- this global is used to determine stop pos while
			 * lens is doing backlash.
			 *    It is set to TRUE at the beginning when lens
			 * going wide direction, and clear to FALSE in
			 * taskProc_ZoomBrakingCheck() when lens stop to still.
			 *    zoomNextStopPosCalculate() need to consider this global
			 * while counting the next stop pos while going tele direction at
			 * backlash phase !
			 */

static BOOL isZPosUpdateEn = FALSE;

static BOOL gIsZoomCalibEnable=FALSE;
			/* indicating Zoom Calibration status */

static BOOL gIsEnable_ZHM_ISR=TRUE;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/*-- Zoom Motion state */
#define IS_ZOOM_MXN_TELE		(gzoomCb.motionStat==ZOOM_MXN_STATE_TELE)
#define IS_ZOOM_MXN_WIDE		(gzoomCb.motionStat==ZOOM_MXN_STATE_WIDE)
#define IS_ZOOM_MXN_BRAKING		(gzoomCb.motionStat==ZOOM_MXN_STATE_BRAKING)
#define IS_ZOOM_MOVING		(IS_ZOOM_MXN_TELE || IS_ZOOM_MXN_WIDE || IS_ZOOM_MXN_BRAKING)
#define IS_ZOOM_STOP		(!IS_ZOOM_MOVING)

/*-- Zoom motion orientation */
#define IS_ZOOM_TELE	(gzoomCb.motionDir==ZOOM_DIR_TELE)
#define IS_ZOOM_WIDE	(gzoomCb.motionDir==ZOOM_DIR_WIDE)

/*-- Zoom Led ZCLK count operation */
#define ZOOM_CLK_CNT_GET	(gzoomCb.irqZoomClkCnt) /* the steps count since the motor actuated */
#define ZOOM_CLK_CNT_CLEAR	do {gzoomCb.irqZoomClkCnt=0;} while(0)

/*-- Zoom check count */
#define ZOOM_CHK_CNT_CLEAR	do {gzoomCb.chkCnt=0;} while(0)

/*-- Zoom Position set */
#define ZOOM_POS_SET(a)	do {gzoomCb.irqZoomPos = (TYPE_ZOOM_POS)a; } while(0)

/*-- Zoom Park Flag operation */
#define ZOOM_PARK_FLAG_CLEAR	do { gzoomCb.parkFlag=ZOOM_PARK_NONE; } while(0)
#define ZOOM_PARK_FLAG_SET(a)	do { gzoomCb.parkFlag = a; } while(0)

/*-- Zoom Error code */
#define ZOOM_ERR_CODE_SET(a)	do { gzoomCb.errCode = (UINT32)a; } while(0)
#define ZOOM_ERR_CODE_GET		(gzoomCb.errCode)

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
static void taskProc_ZoomTeleMoving(void);
static void taskProc_ZoomWideMoving(void);
static void taskProc_ZoomBrakingCheck(void);

static void zoomCreate(void);
static void zoomDestroy(void);

static void ledZCLK_IRQConfig(BOOL en, sp5kTrigLvl_t trigLevel);
static void ledZHM_IRQConfig(BOOL en, sp5kTrigLvl_t trigLevel);

#if (!ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
static void zoomWatchdogRelease(UINT32 id);
static UINT32 zoomWatchdogRegister(UINT32 id);
static void watchdog_ZoomBrakingService(void);
#endif

/*-------------------------------------------------------------------------
 *  File Name : zoomEmergencyService
 *------------------------------------------------------------------------*/
static
void
zoomEmergencyService(
	void
)
{
	LENS_DBG("%s\n", __FUNCTION__);

	isRecurZMoving = FALSE;

	#if (!ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
	zoomWatchdogRelease(WATCHDOG_ZOOM_BRAKING);
	#endif

	zoomDestroy();
}

/*-------------------------------------------------------------------------
 *  File Name : zoomDynamicSlipAdjust
 *------------------------------------------------------------------------*/
static
void
zoomDynamicSlipAdjust(
	TYPE_ZOOM_POS curZClkCnt
)
{
#if defined(LENS_ZOOM_SLIP_DYNAMIC_ADJ)
	if (IS_ZOOM_TELE) {
		if (curZClkCnt < 100) {
			gzoomCb.teleSlipCnt = 17;
		} else if (curZClkCnt < 200) {
			gzoomCb.teleSlipCnt = 90;
		} else if (curZClkCnt < 300) {
			gzoomCb.teleSlipCnt = 95;
		} else if (curZClkCnt < 500) {
			gzoomCb.teleSlipCnt = 100;
		} else if (curZClkCnt < 700) {
			gzoomCb.teleSlipCnt = 100;
		} else if (curZClkCnt < 800) {
			gzoomCb.teleSlipCnt = 100;
		} else {
			gzoomCb.teleSlipCnt = 120;
		}
		LENS_VERBOSE("%s: teleSlipCnt(%d) \n", __FUNCTION__, gzoomCb.teleSlipCnt);
	} else {
		if (curZClkCnt < 60) {
			gzoomCb.wideSlipCnt = 70;
		} else if (curZClkCnt < 80) {
			gzoomCb.wideSlipCnt = 90;
		} else if (curZClkCnt < 110) {
			gzoomCb.wideSlipCnt = 95;
		} else if (curZClkCnt < 160) {
			gzoomCb.wideSlipCnt = 100;
		} else {
			gzoomCb.wideSlipCnt = 120;
		}
		LENS_VERBOSE("%s: wideSlipCnt(%d) \n", __FUNCTION__, gzoomCb.wideSlipCnt);
	}
#endif
	return;
}

/*-------------------------------------------------------------------------
 *  File Name : zoomSegmentUpdate
 *------------------------------------------------------------------------*/
static
void
zoomSegmentUpdate(
	TYPE_ZOOM_POS pos
)
{
	int i;

	LENS_VERBOSE("%s: pos(%d) \n", __FUNCTION__, pos);

	for (i=0; i<ZOOM_SEG_MAX; i++) {
		if (pos==gzmZSA[i]) {
			gzoomCb.curZSeg = i;
			break;
		}
	}

	if (i==ZOOM_SEG_MAX && pos!=ZOOM_CLOSE_POS) {
		LENS_LOG("\n%s: !! WARNING !! ZSeg Not Found @ ZPos(%d) !!\n\n", __FUNCTION__, pos);
		gzoomCb.curZSeg = ZOOM_SEG_MAX;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : zoomParkFlagUpdate
 *------------------------------------------------------------------------*/
static
void
zoomParkFlagUpdate(
	TYPE_ZOOM_POS pos
)
{
	LENS_VERBOSE("%s: pos(%d) \n", __FUNCTION__, pos);

	if (pos==ZOOM_TELE_POS) {
		ZOOM_PARK_FLAG_SET(ZOOM_PARK_TELE);
	} else if (pos==ZOOM_WIDE_POS) {
		ZOOM_PARK_FLAG_SET(ZOOM_PARK_WIDE);
	} else if (pos==ZOOM_CLOSE_POS) {
		ZOOM_PARK_FLAG_SET(ZOOM_PARK_CLOSE);
	} else {
		ZOOM_PARK_FLAG_SET(ZOOM_PARK_NONE);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : zoomNextStopPosCalculate
 *------------------------------------------------------------------------*/
static
TYPE_ZOOM_POS
zoomNextStopPosCalculate(
	BOOL isDirTele
)
{
	TYPE_ZOOM_POS curZPos, zsPos, deltaErr;
	int i=0;

	if (gzoomCb.irqZoomClkCnt<20 && IS_ZOOM_MOVING) {
		/*-- Force to next ZSeg */
		UINT32 nextSeg;
		if (isDirTele) {
			nextSeg = (gzoomCb.curZSeg<ZOOM_SEG_TELE_STOP) ? (gzoomCb.curZSeg+1) : ZOOM_SEG_TELE_STOP;
		} else {
			nextSeg = (gzoomCb.curZSeg>ZOOM_SEG_WIDE_STOP) ? (gzoomCb.curZSeg-1) : ZOOM_SEG_WIDE_STOP;
		}
		zsPos = gzmZSA[nextSeg];
		LENS_VERBOSE("%s: next ZS=%d ZPos(%d) \n", __FUNCTION__, nextSeg, zsPos);
		return zsPos;
	}

	curZPos = lensZoomCurPosGet();

	LENS_DBG("%s : curZPos(%d) \n", __FUNCTION__, curZPos);

	deltaErr = 0;

	if (isDirTele) {
		/*-- Going TELE */
		zsPos = ZOOM_TELE_POS;
		if (curZPos > gzmZSA[ZOOM_SEG_TELE_STOP-1]+deltaErr) {
			/* almost approching TELE_STOP */
			zsPos = ZOOM_TELE_POS;
			LENS_DBG("%s: (T) next @ TELE_STOP ZPos(%d) \n", __FUNCTION__, zsPos);
		} else {
			for (i=ZOOM_SEG_TELE_STOP-1; i>=0; i--) {
				if (curZPos > (gzmZSA[i]+deltaErr)) {
					zsPos = gzmZSA[i+1];
					LENS_DBG("%s: (T) next ZS=%d ZPos(%d) \n", __FUNCTION__, i+1, zsPos);
					break;
				}
			}
			if (i<0) {
				zsPos = gzmZSA[0];
				//LENS_DBG("%s:%d: !!ERROR!!, Check Me !! \n", __FUNCTION__, __LINE__);
			}
		}
	} else {
		/*-- Going WIDE */
		zsPos = ZOOM_WIDE_POS;

		if (curZPos < gzmZSA[1]-deltaErr) {
			/* almost approching WIDE_STOP */
			zsPos = ZOOM_WIDE_POS;
			LENS_DBG("%s: (W) next @ WIDE_STOP ZPos(%d) \n", __FUNCTION__, zsPos);
		} else {
			for (i=2; i<ZOOM_SEG_TELE_STOP; i++) {
				if (curZPos < gzmZSA[i]-deltaErr) {
					zsPos = gzmZSA[i-1];
					LENS_DBG("%s: (W) next ZS=%d ZPos(%d) \n", __FUNCTION__, i-1, zsPos);
					break;
				}
			}
			if (i==(ZOOM_SEG_TELE_STOP)) {
				zsPos = gzmZSA[ZOOM_SEG_TELE_STOP-1];
				LENS_DBG("%s: (W) next ZS=%d ZPos(%d) \n", __FUNCTION__, ZOOM_SEG_TELE_STOP-1, zsPos);
			}
		}

	}

	LENS_VERBOSE("%s: next stop ZPos(%d)\n", __FUNCTION__, zsPos);

	return zsPos;
}

/*-------------------------------------------------------------------------
 *  File Name : zoomPos2Segment
 *------------------------------------------------------------------------*/
static
UINT32
zoomPos2Segment(
	UINT32 zmPos
)
{
	int i;
	UINT32 ret;

	if (IS_ZOOM_TELE) {
		for (i=ZOOM_SEG_WIDE_STOP+1; i<=ZOOM_SEG_TELE_STOP; i++) {
			if (zmPos < gzmZSA[i]) break;
		}
		ret = (i>ZOOM_SEG_TELE_STOP) ? ZOOM_SEG_TELE_STOP : i-1;
	} else {
		for (i=ZOOM_SEG_TELE_STOP-1; i>=ZOOM_SEG_WIDE_STOP; i--) {
			if (zmPos > gzmZSA[i]) break;
		}
		ret = (i<ZOOM_SEG_WIDE_STOP) ? ZOOM_SEG_WIDE_STOP : i+1;
	}

	//LENS_VERBOSE("SEG = %d \n", ret);

	return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : zoomPosUpdateParamGet
 *------------------------------------------------------------------------*/
static
UINT32
zoomPosUpdateParamGet(
	BOOL isStopUpdate
)
{
	UINT32 ret, pos, seg;

	pos = (UINT32) lensZoomCurPosGet();

	if (IS_ZOOM_TELE) {
		ret = (pos >= gzoomCb.tgtStopPos) ? gzoomCb.tgtStopPos : pos;
	} else {
		ret = (pos <= gzoomCb.tgtStopPos) ? gzoomCb.tgtStopPos : pos;
	}

	ret = (UINT32)(ret-ZOOM_WIDE_POS);
	ret = 0x000000FF & ((ret*100)/(ZOOM_TELE_POS-ZOOM_WIDE_POS));
	//LENS_VERBOSE("... percent(%d) \n", ret);

	if (isStopUpdate) {
		seg = gzoomCb.curZSeg + 0x00000100;
	} else {
	seg = zoomPos2Segment(pos);
	}

	ret += ((0x00000FFF & seg)<<8);
	//LENS_VERBOSE("... seg(%d) \n", seg);

	LENS_DBG("%s: >>>> pos(%d), ret(0x%08x) \n", __FUNCTION__, pos, ret);

	return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : zoomCreate
 *------------------------------------------------------------------------*/
static
void
zoomCreate(
	void
)
{
	sp5kTrigLvl_t trigLevel;

	LENS_DBG("%s\n", __FUNCTION__);

	#if ZOOM_CONTROL_MOTOR_LED_POWER
	/*- supposedly, caller shall guarantee these, but... */
	lensMotorPowerOn();
	lensLedPowerOn(LENS_LED_ID_ZCLK | LENS_LED_ID_ZHM);
	#endif

	ZOOM_ERR_CODE_SET(ZOOM_ERROR_NONE);
	ZOOM_PARK_FLAG_CLEAR;

	/*-- config ZCLK IRQ */
	if (lensLedRead(LENS_LED_ID_ZCLK)) {  //	if (IO_LENS_LED_ZCLK_GET) {
		trigLevel = SP5K_TRIG_LVL_RISE;
	} else {
		trigLevel = SP5K_TRIG_LVL_FALL;
	}
	ledZCLK_IRQConfig(ENABLE, trigLevel);
	ZOOM_CLK_CNT_CLEAR; /* clear zclk cnt */
	ZOOM_CHK_CNT_CLEAR; /* clear jam/brake chk cnt */

	/*-- config ZHM IRQ */
	#if LENS_CLOSE_ZHM_LED_LEVEL_HI
	trigLevel = SP5K_TRIG_LVL_FALL;
	#else
	trigLevel = SP5K_TRIG_LVL_RISE;
	#endif
	ledZHM_IRQConfig(ENABLE, trigLevel);

}

/*-------------------------------------------------------------------------
 *  File Name : zoomDestroy
 *------------------------------------------------------------------------*/
static
void
zoomDestroy(
	void
)
{
	LENS_DBG("%s\n", __FUNCTION__);

	/*-- config ZCLK IRQ */
	ledZCLK_IRQConfig(DISABLE, SP5K_TRIG_LVL_BOTH);
	ZOOM_CLK_CNT_CLEAR;

	/*-- config ZHM IRQ */
	ledZHM_IRQConfig(DISABLE, SP5K_TRIG_LVL_BOTH);

	#if ZOOM_CONTROL_MOTOR_LED_POWER
	/*- supposedly, caller shall guarantee these, but... */
	lensLedPowerOff(LENS_LED_ID_ZCLK | LENS_LED_ID_ZHM);
	lensMotorPowerOff();
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : zoomGo
 *------------------------------------------------------------------------*/
static
UINT32
zoomGo(
	zoomMotionDir_t dir
)
{

	LENS_DBG("%s: %s \n",__FUNCTION__, (dir==ZOOM_DIR_TELE ? "-->>" : "<<--"));

	/*-- Validity check !!  */
	if (dir!=ZOOM_DIR_TELE && dir!=ZOOM_DIR_WIDE) {
		LENS_VERBOSE("%s: !! ERROR !! Illegal zoom dir(%d) \n", __FUNCTION__, dir);
		return FAIL;
	}

	if (IS_ZOOM_MOVING) {
		LENS_DBG("%s: skipped, still moving !!\n", __FUNCTION__);
		ZOOM_ERR_CODE_SET(ZOOM_ERROR_STILL_MOVING);
		return FAIL;
	}

	if (lensZoomCloseParked() && dir==ZOOM_DIR_WIDE && gzoomCb.tgtStopPos==ZOOM_CLOSE_POS) {
		LENS_DBG("%s: skipped, C-Parked already !!\n", __FUNCTION__);
		return SUCCESS;
	}

	if (lensZoomWideParked() && dir==ZOOM_DIR_WIDE && gzoomCb.tgtStopPos==ZOOM_WIDE_POS) {
		LENS_DBG("%s: skipped, W-Parked already !!\n", __FUNCTION__);
		return SUCCESS;
	}

	if (lensZoomTeleParked() && dir==ZOOM_DIR_TELE) {
		LENS_DBG("%s: skipped, T-Parked already !!\n", __FUNCTION__);
		return SUCCESS;
	}

	/*-- Config Zoom session */
	zoomCreate();

	#ifdef LENS_ZOOM_START_BOOST
	lensZoomBoostStart();
	#endif

	/*-- Actuate Zoom Motor */
	if (dir==ZOOM_DIR_TELE) {
		zoomMotorTele();
		gzoomCb.motionStat = ZOOM_MXN_STATE_TELE;
		gzoomCb.motionDir = ZOOM_DIR_TELE;
	} else {
		zoomMotorWide();
		gzoomCb.motionStat = ZOOM_MXN_STATE_WIDE;
		gzoomCb.motionDir = ZOOM_DIR_WIDE;
	}

	/*-- Trigger Zoom Task */
	gzmPosUpdateCnt = 0;
	lensOsEventSet(&evZoomCtrl, EVENT_ZOOM_MOVING_CTRL);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomDefaultSlipGet
 *------------------------------------------------------------------------*/
void
lensZoomPosUpdateEnable(
	BOOL isEn
)
{
	isZPosUpdateEn = isEn;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomCalibSessionControl
 *------------------------------------------------------------------------*/
void
lensZoomCalibSessionControl(
	BOOL isCalibOn
)
{
	gIsZoomCalibEnable=(!!isCalibOn);
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomDefaultSlipCalibWrite
 *------------------------------------------------------------------------*/
void
lensZoomDefaultSlipCalibWrite(
	zoomSlipCntSxN_t zmSxn, /* Ref : zoomSlipCntSxN_t */
	zoomMotorSpeed_t zmSpeed,
	UINT16 newSlip
)
{
	LENS_VERBOSE("Setting SLIP of zmSxn(%d), zmSpeed(%d) ...  slip(%d) \n",
			zmSxn, zmSpeed, newSlip);

	if (!gIsZoomCalibEnable) {
		return;
	}

	if (zmSpeed>=ZOOM_SPEED_TOTAL || zmSxn>=ZOOM_SLIP_SXN_TOTAL) {
		LENS_DBG("%s: !!WARNING!!, Undefined arguments !!\n", __FUNCTION__);
		return;
	}

	gzmDefaultSlipCnt[zmSxn][zmSpeed] = newSlip;

	LENS_VERBOSE("SLIP of zmSxn(%d), zmSpeed(%d) changed >>> slip(%d) \n",
			zmSxn, zmSpeed, gzmDefaultSlipCnt[zmSxn][zmSpeed]);
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomDefaultSlipGet
 *------------------------------------------------------------------------*/
UINT16
lensZoomDefaultSlipGet(
	zoomSlipCntSxN_t zmSxn, /* Ref : zoomSlipCntSxN_t */
	zoomMotorSpeed_t zmSpeed
)
{
	UINT16 retSlip;

	LENS_DBG("%s: %d \n", __FUNCTION__, zmSpeed);

	if (zmSpeed>=ZOOM_SPEED_TOTAL || zmSxn>=ZOOM_SLIP_SXN_TOTAL) {
		LENS_DBG("%s: !!WARNING!!, Undefined arguments !!\n", __FUNCTION__);
		return ZOOM_DEFAULT_SLIP_CNT;
	}

	retSlip = gzmDefaultSlipCnt[zmSxn][zmSpeed];

	LENS_DBG("%s: Default Slip (%d) \n", __FUNCTION__, retSlip);
	return retSlip;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomTeleSlipCntSet
 *------------------------------------------------------------------------*/
void
lensZoomTeleSlipCntSet(
	UINT8 slipCnt
)
{
	gzoomCb.teleSlipCnt = slipCnt;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomWideSlipCntSet
 *------------------------------------------------------------------------*/
void
lensZoomWideSlipCntSet(
	UINT8 slipCnt
)
{
	gzoomCb.wideSlipCnt = slipCnt;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomBacklashSlipCntSet
 *------------------------------------------------------------------------*/
void
lensZoomBacklashSlipCntSet(
	UINT8 slipCnt
)
{
	gzoomCb.blashSlipCnt = slipCnt;
}

#ifdef LENS_ZOOM_START_BOOST
/*-------------------------------------------------------------------------
 *  File Name : lensZoomBoostStart
 *------------------------------------------------------------------------*/
void
lensZoomBoostStart(
	 void
)
{
	if (isZPosUpdateEn) {
		printf("\n>>>>>> ZOOM Boost Start \n\n");
		/*-- Do not apply ZoomBoost mechanism at LensOpen/Boot/Close.
		 * The variable isZPosUpdateEn indicates the above situations.
		 */
		zoomSpeedBackup = zoomMotorSpeedGet();
		lensZoomSpeedSet(ZOOM_SPEED_FAST);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : lensZoomBoostRestore
 *------------------------------------------------------------------------*/
void
lensZoomBoostRestore(
	 void
)
{
	if (isZPosUpdateEn && zoomSpeedBackup<ZOOM_SPEED_TOTAL) {
		/*-- Do not apply ZoomBoost mechanism at LensOpen/Boot/Close.
		 * The variable isZPosUpdateEn indicates the above situations.
		 */
		lensZoomSpeedSet(zoomSpeedBackup);
			/*-- NOTICE: this function is to be called from ISR.
			 *           BE CAREFUL while adding printf or delay !!!
			 */
		zoomSpeedBackup = ZOOM_SPEED_TOTAL;
	}
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : lensZoomSpeedGet
 *------------------------------------------------------------------------*/
UINT8
lensZoomSpeedGet(
	 void
)
{
	return zoomMotorSpeedGet();
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomSpeedSet
 *------------------------------------------------------------------------*/
void
lensZoomSpeedSet(
	UINT8 zoom_speed
)
{
	LENS_ISR_DBG("%s: %d \n", __FUNCTION__, zoom_speed);
		/*-- It would be called by ledZClK_ISR while restoring
		 * ZOOM Lens Speed.
		 */

	zoomMotorSpeedSet(zoom_speed);
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomMovingCheck
 *------------------------------------------------------------------------*/
BOOL
lensZoomMovingCheck(
	void
)
{
	return (IS_ZOOM_MOVING);
}


/*-------------------------------------------------------------------------
 *  File Name : lensZoomTeleParked
 *------------------------------------------------------------------------*/
BOOL
lensZoomTeleParked(
	void
)
{
	return ((gzoomCb.parkFlag & ZOOM_PARK_TELE) ? TRUE : FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomWideParked
 *------------------------------------------------------------------------*/
BOOL
lensZoomWideParked(
	void
)
{
	return ((gzoomCb.parkFlag & ZOOM_PARK_WIDE) ? TRUE : FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomCloseParked
 *------------------------------------------------------------------------*/
BOOL
lensZoomCloseParked(
	void
)
{
	return ((gzoomCb.parkFlag & ZOOM_PARK_CLOSE) ? TRUE : FALSE);
}


/*-------------------------------------------------------------------------
 *  File Name : lensZoomParkFlagGet
 *------------------------------------------------------------------------*/
UINT8
lensZoomParkFlagGet(
	void
)
{
	return gzoomCb.parkFlag;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomQuery
 *------------------------------------------------------------------------*/
UINT32
lensZoomQuery(
	UINT32 queryId,
	UINT32 qryParam
)
{
	UINT32 qryResult;

	switch (queryId) {
	default :
		LENS_LOG("\n%s: !! ERROR !!, illegal id(%d) \n\n", __FUNCTION__, queryId);
		qryResult = 0;
		break;
	case ZOOM_QUERY_MOTION_STATE:
		qryResult = gzoomCb.motionStat;
		break;
	case ZOOM_QUERY_SEG_POS:
		qryResult = gzoomCb.curZSeg;
		break;
	case ZOOM_QUERY_STEP_POS:
		qryResult = (UINT32)lensZoomCurPosGet();
		break;
	case ZOOM_QUERY_SEG_NUM:
		qryResult = ZOOM_ZS_NUM;
		break;
	case ZOOM_QUERY_SEG2POS:
		if (qryParam<ZOOM_ZS_NUM) {
			qryResult = (UINT32)gzmZSA[qryParam];
		} else {
			LENS_LOG("\n%s: Illegal ZSEG(%d) \n\n", __FUNCTION__, qryParam);
			qryResult = gzoomCb.irqZoomPos;
		}
		break;
	case ZOOM_QUERY_BRAKE_POS:
		qryResult = gzoomCb.posBrake;
		break;
	}

	return qryResult;

}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomCurSegGet
 *------------------------------------------------------------------------*/
UINT16
lensZoomCurSegGet(
	void
)
{
	/*-- to retrieve the current segment idx  */
	//LENS_DBG("\n>>> ZOOM SEG : %d \n", gzoomCb.curZSeg);
	return gzoomCb.curZSeg;
}


/*-------------------------------------------------------------------------
 *  File Name :lensZoomStopPosGet
 *------------------------------------------------------------------------*/
TYPE_ZOOM_POS
lensZoomStopPosGet(
	void
)
{
	return gzoomCb.tgtStopPos;
}

/*-------------------------------------------------------------------------
 *  File Name :lensZoomStopPosSet
 *------------------------------------------------------------------------*/
void
lensZoomStopPosSet(
	TYPE_ZOOM_POS tgtPos
)
{
	gzoomCb.tgtStopPos = tgtPos;
}

/*-------------------------------------------------------------------------
 *  File Name :lensZoomCurPosGet
 *------------------------------------------------------------------------*/
TYPE_ZOOM_POS
lensZoomCurPosGet(
	void
)
{
	if (IS_ZOOM_MOVING) {
		if (IS_ZOOM_TELE) {
			return (gzoomCb.irqZoomPos+gzoomCb.irqZoomClkCnt);
		} else {
			return (gzoomCb.irqZoomPos-gzoomCb.irqZoomClkCnt);
		}
	} else {
		return gzoomCb.irqZoomPos;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : lensZoomNextStopPosSet
 *------------------------------------------------------------------------*/
TYPE_ZOOM_POS
lensZoomNextStopPosSet(
	void
)
{
	//LENS_VERBOSE("\n%s: curPos(%d) \n", __FUNCTION__, lensZoomCurPosGet());
	gzoomCb.tgtStopPos = zoomNextStopPosCalculate(IS_ZOOM_TELE);
	LENS_DBG("%s: next stop ZPos(%d) \n", __FUNCTION__, gzoomCb.tgtStopPos);

	return gzoomCb.tgtStopPos;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomCurPosSet
 *------------------------------------------------------------------------*/
void
lensZoomCurPosSet(
	TYPE_ZOOM_POS	pos
)
{
	LENS_VERBOSE("%s: pos(%d) \n", __FUNCTION__, pos);
	gzoomCb.irqZoomPos = pos;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomResetConfig
 *------------------------------------------------------------------------*/
void
lensZoomResetConfig(
	void
)
{
	LENS_DBG("%s ...\n", __FUNCTION__);

	/*--
	 * This function is used only when ZOOM is reset from unknow to close
	 * It will config zoom param while reset to CLOSE state.
	 */
	memset(&gzoomCb, 0, sizeof(zoomCtrlParam_t));
	gzoomCb.motionDir = ZOOM_DIR_STOP;
	gzoomCb.motionStat = ZOOM_MXN_STATE_STOP;
	gzoomCb.parkFlag = ZOOM_PARK_CLOSE;
	gzoomCb.errCode = ZOOM_ERROR_NONE;
	lensZoomCurPosSet(ZOOM_CLOSE_POS);
	lensZoomSpeedSet(ZOOM_SPEED_FAST);
	gzoomCb.teleSlipCnt = gzoomCb.wideSlipCnt = lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_BOOT, lensZoomSpeedGet());
	gzoomCb.curZSeg = ZOOM_SEG_CLOSE;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomCrashReset
 *------------------------------------------------------------------------*/
UINT32
lensZoomCrashReset(
	void
)
{
	UINT32	startedTime;
	BOOL 	isLensError=FALSE;

	/*-- Prerequesite !!
	 * Assume all lens power and leds has been powered
	 */

	/*-- go for Zoom lens */
	startedTime = appMsTimeGet();
	LENS_VERBOSE("%s: zoom started @ %u ms \n", __FUNCTION__, startedTime*10);

	LENS_VERBOSE("--Widing\n");
	zoomMotorWide();

	#ifdef LENS_MODEL_D160
	/*-- D160 is quite strange that the reading of ZHM
	 * at the most TELE position will tell the same as
	 * the reading of HOME/CLOSE position.
	 * It will mislead the Lens FW to regard it has discover
	 * the HOME PI !!
	 * So, we put off couple of time to see if it is a fake HOME PI.
	 */
	APP_SLEEP_MS(100);
	#endif

	while(!IS_LENS_CLOSE) {
		LENS_VERBOSE("--Widing\n");
		APP_SLEEP_MS(100);

		/* timeout mechanism */
		if ( (appMsTimeGet()-startedTime) >= 3000) {
			isLensError = TRUE;
		}
	}

	LENS_VERBOSE("--Braking\n");
	zoomMotorBrake();
	APP_SLEEP_MS(250);

#ifdef EVB_CFG_COUGAR /*-- D160 Debug purpose */
{
	UINT32 ioval;
	sp5kGpioRead(SP5K_GPIO_GRP_TGL, 1<<2, &ioval);
	printf("\n***************\nZOOM Home (%d) \n******************\n\n", ioval);
}
#endif

	LENS_VERBOSE("--Standby\n");
	zoomMotorStandby();
	APP_SLEEP_MS(100);

	/* check if error !! */
	if (isLensError) {
		printf("\n\n !!! Lens Reset ERROR !!! \n\n");
		return FAIL;
	}

	lensZoomResetConfig();

	LENS_DBG("\n\n*** LENS is reset to close @ %u !!\n\n", appMsTimeGet());

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomBacklash
 *------------------------------------------------------------------------*/
UINT32
lensZoomBacklash(
	void
)
{
	LENS_LOG("\nZOOM BACKLASH !!\n\n");

	isDoingBacklash = TRUE;
		/* set TRUE shall ahead of calling
		 * zoomNextStopPosCalculate(TRUE) because it needs this
		 * variable to determined stop pos.
		 * Probably, I will work out a beter way.
		 */
	gzoomCb.tgtStopPos = zoomNextStopPosCalculate(TRUE/*Tele direction*/);
	LENS_DBG("%s: curPos(%d) --> tgtPos(%d) \n", __FUNCTION__, lensZoomCurPosGet(), gzoomCb.tgtStopPos);

	return zoomGo(ZOOM_DIR_TELE);
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomPosGo
 *------------------------------------------------------------------------*/
UINT32
lensZoomPosGo(
	TYPE_ZOOM_POS	tgtPos
)
{
	TYPE_ZOOM_POS curPos;
	UINT32 err=SUCCESS;

	if (IS_ZOOM_MOVING) {
		printf("%s: WARNING, zoom is moving !!\n", __FUNCTION__);
		return FAIL;
	}

	curPos=lensZoomCurPosGet();

	LENS_DBG("%s: tgtPos(%d) from curPos(%d)\n", __FUNCTION__, tgtPos, curPos);

	if (curPos==tgtPos) {
		LENS_DBG("%s: skipped, alreay @ target pos !!\n", __FUNCTION__);
		zoomParkFlagUpdate(tgtPos);
		return SUCCESS;
	}

	/* store stop position */
	gzoomCb.tgtStopPos = tgtPos;

	if (curPos < tgtPos) {
		/*-- Zoom IN --*/
		LENS_VERBOSE("%s: curPos(%d) --> tgtPos(%d) \n", __FUNCTION__, curPos, tgtPos);
		err = zoomGo(ZOOM_DIR_TELE);
	} else {
		/*-- Zoom OUT --*/
		isCountInBacklash = (tgtPos==ZOOM_CLOSE_POS) ? FALSE : TRUE;
		LENS_VERBOSE("%s: tgtPos(%d) <-- curPos(%d)\n", __FUNCTION__, tgtPos, curPos);
		err = zoomGo(ZOOM_DIR_WIDE);
	}
	return err;

}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomStopNotifyCBFRegister
 *------------------------------------------------------------------------*/
void
lensZoomStopNotifyCBFRegister(
	zoomStopNotifyCBF_t *stopNotifyCBF
)
{
	if (!stopNotifyCBF) {
		gzoomCb.stopNotifyCBF = NULL;
	} else {
		gzoomCb.stopNotifyCBF = stopNotifyCBF;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomHomeISREnable
 *------------------------------------------------------------------------*/
void lensZoomHomeISREnable(void)
{
	gIsEnable_ZHM_ISR = TRUE;
}
/*-------------------------------------------------------------------------
 *  File Name : lensZoomHomeISRDisable
 *------------------------------------------------------------------------*/
void lensZoomHomeISRDisable(void)
{
	gIsEnable_ZHM_ISR = FALSE;
}

/*-------------------------------------------------------------------------
 *  File Name : zoomStopNotifyDefaultCBF
 *------------------------------------------------------------------------*/
void
zoomStopNotifyDefaultCBF(
	TYPE_ZOOM_POS stopPos,
	UINT16 stopSt
)
{
	/* stopPos : the pos where zoom stopped at */
	/* stopSt : ZOOM_ERROR_NONE or ZOOM_ERROR_JAMMED */

	if (gzoomCb.stopNotifyCBF) {
		(gzoomCb.stopNotifyCBF)(stopPos, stopSt);
		return;
	}

	//LENS_MSG_POST(APP_UI_MSG_ZOOM_STOP_NOTIFY, 0);
}
/*-------------------------------------------------------------------------
 *  File Name : lensZoomBrakeTrigWait
 *------------------------------------------------------------------------*/
UINT32
lensZoomBrakeTrigWait(
	void
)
{
	if (!IS_ZOOM_MOVING) {
		LENS_VERBOSE("\n%s: !!WARNING!! Not MOVING state !! \n\n", __FUNCTION__);
		return FAIL;
	}

	/* blocking wait !! */
	do {
		LENS_VERBOSE("%s : sleeping !! \n", __FUNCTION__);
		LENS_SLEEP(ZOOM_BRAKE_WATCHDOG_INTERVAL_MS);
	} while (!IS_ZOOM_MXN_BRAKING);

	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : lensZoomStopWait
 *------------------------------------------------------------------------*/
UINT32
lensZoomStopWait(
	BOOL	isBlockingWait
)
{
	BOOL isForceBlocking;
	UINT32 err=SUCCESS;

	gzoomCb.isAskStopCB = FALSE;

	/*-- checkup blocking wait */
	if (!isBlockingWait) {
		if (!gzoomCb.stopNotifyCBF) {
			/* asking Non-Blocking but no notify callback !! */
			LENS_DBG("%s: Force to blocking wait\n", __FUNCTION__);
			isForceBlocking = TRUE;
		}
	}
	isForceBlocking = FALSE;

	/*-- check if Zoom has stopped */
	if (IS_ZOOM_STOP) {
		if (isBlockingWait) {
			err = SUCCESS;
		} else {
			if (isForceBlocking) {
				/* call default stop notify CBF, and return FAIL */
				err = FAIL;
			} else {
				/* call user's notify CBF, and return SUCCESS */
				err = SUCCESS;
			}

			if (gzoomCb.errCode==ZOOM_ERROR_JAMMED) {
				zoomStopNotifyDefaultCBF(gzoomCb.irqZoomPos, ZOOM_ERROR_JAMMED);
			} else {
				zoomStopNotifyDefaultCBF(gzoomCb.irqZoomPos, ZOOM_ERROR_NONE);
			}
		}

		return err;
	}

	/*-- Zoom is still moving */
	if (isBlockingWait || isForceBlocking) {
		/* blocking wait or force-blocking */
		do {
			LENS_VERBOSE("%s : sleeping !! \n", __FUNCTION__);
			LENS_SLEEP(ZOOM_BRAKE_WATCHDOG_INTERVAL_MS);
		} while (!IS_ZOOM_STOP);

		/* err(FAIL) indicating non-blocking is forced to blocking */
		err = (isForceBlocking) ? FAIL : SUCCESS;
	} else {
		/* it is non-block wait */
		gzoomCb.isAskStopCB = TRUE;
		err = SUCCESS;
	}

	LENS_VERBOSE("%s : exit !! \n", __FUNCTION__);

	return err;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomClkCntClear
 *------------------------------------------------------------------------*/
void
lensZoomClkCntClear(
	void
)
{
	ZOOM_CLK_CNT_CLEAR;
}

/*-------------------------------------------------------------------------
 *  File Name :ledZCLK_ISR
 *------------------------------------------------------------------------*/
#define DBG_ZOOM_CLK_LED_IRQ	0
static
UINT32
ledZCLK_ISR(
	void
)
{
	gzoomCb.irqZoomClkCnt++;

	#ifdef LENS_ZOOM_START_BOOST
	if (gzoomCb.irqZoomClkCnt>30) {
		lensZoomBoostRestore();
	}
	#endif

	#if DBG_ZOOM_CLK_LED_IRQ
	/* just for debug purpose */
	if ((gzoomCb.irqZoomClkCnt & 0x00ff)==0) {
		LENS_DBG("(%d, %d) ", gzoomCb.irqZoomClkCnt, lensZoomCurPosGet());
	}
	#endif

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
static
void
ledZCLK_IRQConfig(
	BOOL en,
	sp5kTrigLvl_t trigLevel
)
{
	#if DBG_ZOOM_CLK_LED_IRQ
	LENS_ISR_DBG("%s: en(%d), trig_level(%d)\n", __FUNCTION__, en, trigLevel);
	#endif

	sp5kCntrCfgSet(SP5K_CNTR0, 13/* GENGPIO13 */, SP5K_TRIG_LVL_BOTH, 0xfff);
	sp5kCntrTargetValSet(SP5K_CNTR0, 1);
	sp5kCntrCallbackSet(ledZCLK_ISR);

	if (en) {
		sp5kCntrEnable(SP5K_CNTR0, ENABLE);
	} else {
		sp5kCntrEnable(SP5K_CNTR0, DISABLE);
	}
}


/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
#define DBG_ZOOM_HOME_LED_IRQ	0
static
void
ledZHM_ISR(
	void
)
{
	if (gIsEnable_ZHM_ISR==FALSE) {
		LENS_ISR_DBG("\n\n%s: Ignored !! \n\n", __FUNCTION__);
		return;
	}

	#if 0 /* %REVIEW% : would be not needed */
	sp5kTrigLvl_t trigLevel;

	/*-- config ZCLK IRQ */
	if (IO_LENS_LED_ZCLK_GET) {
		trigLevel = SP5K_TRIG_LVL_RISE;
	} else {
		trigLevel = SP5K_TRIG_LVL_FALL;
	}
	ledZCLK_IRQConfig(ENABLE, trigLevel);
	#endif

	if (IS_ZOOM_TELE) {
		ZOOM_CLK_CNT_CLEAR;
		ZOOM_POS_SET(ZOOM_HOME_POS);

		#if DBG_ZOOM_HOME_LED_IRQ
		/* JUST for DEBUG  */
		LENS_ISR_DBG("\n\n -->>> ZHOME_T@(%d) \n\n", lensZoomCurPosGet());
		#endif
	} else if (IS_ZOOM_WIDE) 	{
		ZOOM_CLK_CNT_CLEAR;
		ZOOM_POS_SET(ZOOM_HOME_POS - 5); /* %REVIEW% : what is the purpose of -5 */

		#if DBG_ZOOM_HOME_LED_IRQ
		/* JUST for DEBUG  */
		LENS_ISR_DBG("\n\n <<<-- ZHOME_W@(%d) \n\n", lensZoomCurPosGet());
		#endif
	}

	ledZHM_IRQConfig(DISABLE, SP5K_TRIG_LVL_BOTH);
	//MOTOR_ZOOM_PWM_SET(20);

}


/*-------------------------------------------------------------------------
 *  File Name : zoomLedHomeIrqConfig
 *------------------------------------------------------------------------*/
static
void
ledZHM_IRQConfig(
	BOOL en,
	sp5kTrigLvl_t trigLevel
)
{
	/*-- DO NOT Printf in this function !!
	 * ledZHM_ISR will call to disable ledZHM_IRQ once ZHM is detected.
	 */
	#if DBG_ZOOM_HOME_LED_IRQ
	LENS_ISR_DBG("%s: en(%d), trig_level(%d)\n", __FUNCTION__, en, trigLevel);
	#endif

	/* %REVIEW% :
	 * shall check sp5k1c has the irq reset problem as specified below.
	 */
	/*-- Reset previous GPIO Irq config */
	sp5kGpioIsrReg(ZOOM_HOME_LED_SP5K_IRQ_ID, SP5K_TRIG_LVL_RISE, ledZHM_ISR);
	sp5kGpioIntEnable(ZOOM_HOME_LED_SP5K_IRQ_ID, DISABLE);
	sp5kGpioIsrReg(ZOOM_HOME_LED_SP5K_IRQ_ID, SP5K_TRIG_LVL_FALL, ledZHM_ISR);
	sp5kGpioIntEnable(ZOOM_HOME_LED_SP5K_IRQ_ID, DISABLE);
	sp5kGpioIsrReg(ZOOM_HOME_LED_SP5K_IRQ_ID, SP5K_TRIG_LVL_BOTH, ledZHM_ISR);
	sp5kGpioIntEnable(ZOOM_HOME_LED_SP5K_IRQ_ID, DISABLE);

	/* %REVIEW% :
	 * The following flow is quite weird to me.
	 * Why does the trigLevel would be different between IsrReg and IntEnable ?
	 */
	/*-- Register GPIO Irq ISR */
	sp5kGpioIsrReg(ZOOM_HOME_LED_SP5K_IRQ_ID, trigLevel, ledZHM_ISR);

	if (en) {
		if (trigLevel == SP5K_TRIG_LVL_RISE) {
			sp5kGpioIntEnable(ZOOM_HOME_LED_SP5K_IRQ_ID, SP5K_TRIG_LVL_RISE);
		} else {
			/* %REVIEW%, why both ? */
			sp5kGpioIntEnable(ZOOM_HOME_LED_SP5K_IRQ_ID, SP5K_TRIG_LVL_BOTH );
		}
	} else {
		sp5kGpioIntEnable(ZOOM_HOME_LED_SP5K_IRQ_ID, DISABLE);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomTask
 *------------------------------------------------------------------------*/
void
lensZoomTask(
	ULONG init_data
)
{
	UINT32 ret_flag=0;
	UINT32 wait_flag;

	wait_flag = EVENT_ZOOM_TASK_INIT | \
				EVENT_ZOOM_BRAKING_CTRL | \
				EVENT_ZOOM_MOVING_CTRL;

	while (1)
	{
		// wait for Focus event
		lensOsEventGet(&evZoomCtrl, wait_flag, &ret_flag);

		/* task init */
		if (ret_flag & EVENT_ZOOM_TASK_INIT) {
			LENS_DBG("event : EVENT_ZOOM_TASK_INIT\n");
			/* init zoom global variables */
			memset(&gzoomCb, 0, sizeof(zoomCtrlParam_t));
			gzoomCb.motionDir = ZOOM_DIR_STOP;
			gzoomCb.motionStat = ZOOM_MXN_STATE_STOP;
			gzoomCb.parkFlag = ZOOM_PARK_NONE;
			gzoomCb.errCode = ZOOM_ERROR_NONE;
			lensZoomSpeedSet(ZOOM_SPEED_FAST);
			gzoomCb.teleSlipCnt = gzoomCb.wideSlipCnt = lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_BOOT, lensZoomSpeedGet());
			continue;
		}

		/* handle braking */
		if (ret_flag & EVENT_ZOOM_BRAKING_CTRL) {
//profLogAdd(0,"*zm task:brake evt");
			if (IS_ZOOM_MXN_BRAKING) {
				LENS_VERBOSE("event : EVENT_ZOOM_BRAKING_CTRL\n");
				/*-- Check ZOOM_BRAKE */
				taskProc_ZoomBrakingCheck();

				/*-- Update ZOOM POS  */
				if (!isDoingBacklash && IS_ZOOM_MOVING && isZPosUpdateEn) {
					//LENS_VERBOSE("PosUpdate: Braking %u \n", appMsTimeGet());
					LENS_POST_MSG(APP_UI_MSG_LENS_POS_UPDATE, zoomPosUpdateParamGet(FALSE));
				}
			}
			continue;
		}

		/* handle zooming */
		if (ret_flag & EVENT_ZOOM_MOVING_CTRL) {
			if (IS_ZOOM_MXN_TELE || IS_ZOOM_MXN_WIDE) {
				LENS_VERBOSE("event : EVENT_ZOOM_MOVING_CTRL\n");
				if (IS_ZOOM_TELE) {
					taskProc_ZoomTeleMoving();
				} else {
					taskProc_ZoomWideMoving();
				}

				if (isRecurZMoving) {
					/*-- Update ZOOM Pos */
					if (!isDoingBacklash && IS_ZOOM_MOVING && isZPosUpdateEn) {
						gzmPosUpdateCnt++;
						if (gzmPosUpdateCnt > (ZOOM_POS_NOTIFY_INTERVAL_MS/ZOOM_MOVING_CTRL_INTERVAL_MS)) {
							LENS_POST_MSG(APP_UI_MSG_LENS_POS_UPDATE, zoomPosUpdateParamGet(FALSE));
							gzmPosUpdateCnt=0;
						}
					}

					/*-- Self activate MOVING_CTRL */
					LENS_SLEEP(ZOOM_MOVING_CTRL_INTERVAL_MS);
					lensOsEventSet(&evZoomCtrl, EVENT_ZOOM_MOVING_CTRL);
				}
			}
		}
		continue;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomTaskInit
 *------------------------------------------------------------------------*/
void
lensZoomTaskInit(
	void
)
{
	/* Create & init task resources */
 	lensOsEventCreate(&evZoomCtrl, "evZoomCtrl");
 	lensOsMutexCreate(&mtxZoomCtrl, "mtxZoomCtrl");

	lensOsEventSet(&evZoomCtrl, EVENT_ZOOM_TASK_INIT);
	lensOsMutexPut(&mtxZoomCtrl);

	/* Create Lens common control task */
	LENS_DBG("creating Zoom task\n");
	lensOsTaskCreate("lensZoom-Task", lensZoomTask, LENS_ZOOM_TASK_PRIORITY, LENS_ZOOM_TASK_TIME_SLICE);
}

/*-------------------------------------------------------------------------
 *  File Name : taskProc_ZoomJamCheck
 *------------------------------------------------------------------------*/
static
BOOL
taskProc_ZoomJamCheck(
	void
)
{
	LENS_VERBOSE("%s \n", __FUNCTION__);

	if (gzoomCb.lastChkPos==lensZoomCurPosGet()) {
		/*-- zoom seems not moving */
		gzoomCb.chkCnt++;

		if (gzoomCb.chkCnt > ZOOM_JAM_CHECK_CNT_MAX) {
			/* Lens has jammed !! */

			LENS_DBG("%s: Lens has jammed !!\n", __FUNCTION__);
			LENS_DBG("%s: curPos(%d), lastChkPos(%d), chkCnt(%d) \n", __FUNCTION__, lensZoomCurPosGet(), gzoomCb.lastChkPos, gzoomCb.chkCnt);

			zoomMotorStandby();

			gzoomCb.chkCnt=0;
			gzoomCb.motionStat = ZOOM_MXN_STATE_STOP;
			gzoomCb.motionDir = ZOOM_DIR_STOP;
			gzoomCb.errCode = ZOOM_ERROR_NONE;

			zoomDestroy();

			/*-- zoom stop notify callback */
			if (gzoomCb.isAskStopCB) {
				if (!gzoomCb.stopNotifyCBF) {
					zoomStopNotifyDefaultCBF(lensZoomCurPosGet(), ZOOM_ERROR_JAMMED);
				}

				/* Shall reset this flag !! */
				gzoomCb.isAskStopCB = FALSE;
			}

			return TRUE;
		} else {
			return FALSE;
		}
	} else {
		gzoomCb.chkCnt=0;
		return FALSE;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : taskProc_ZoomBrakingCheck
 *------------------------------------------------------------------------*/
static
void
taskProc_ZoomBrakingCheck(
	void
)
{
	TYPE_ZOOM_POS curPos;

	LENS_VERBOSE("%s\n", __FUNCTION__);

	gzoomCb.chkCnt++;
	/*-- check if lens failed to brake and stop */
	if (gzoomCb.chkCnt > ZOOM_BRAKE_ERROR_CHECK_CNT_MAX) {
		LENS_LOG("\n%s: !!! FATAL !!!, Zoom seems fail to stop !!\n\n", __FUNCTION__);
		zoomEmergencyService();
	}

	/*-- check if zoom is stop */
	curPos = lensZoomCurPosGet();
	if (curPos==gzoomCb.lastChkPos) {
		gzoomCb.brkConfirmCnt++;
	}

	if (gzoomCb.brkConfirmCnt>=ZOOM_STOP_DONE_WATCHDOG_CHK_CNT) {
		/*-- lens has stopped */
		zoomMotorStandby();

		#if (!ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
		zoomWatchdogRelease(WATCHDOG_ZOOM_BRAKING);
		#endif

		gzoomCb.chkCnt=0;
		gzoomCb.motionStat = ZOOM_MXN_STATE_STOP;
		gzoomCb.motionDir = ZOOM_DIR_STOP;
		gzoomCb.errCode = ZOOM_ERROR_NONE;
		gzoomCb.irqZoomClkCnt = 0;
		gzoomCb.irqZoomPos = curPos;

		/* must use target pos, dont use curPos */
		LENS_VERBOSE("%s: tgtStopPos(%d) \n", __FUNCTION__, gzoomCb.tgtStopPos);
		zoomParkFlagUpdate(gzoomCb.tgtStopPos);
		zoomSegmentUpdate(gzoomCb.tgtStopPos);

		/*-- Update ZOOM POS  */
		if (!isDoingBacklash && isZPosUpdateEn) {
			LENS_POST_MSG(APP_UI_MSG_LENS_POS_UPDATE, zoomPosUpdateParamGet(TRUE));
		}

		isDoingBacklash = FALSE;

		LENS_LOG("\nZM Stopped@(%d), ZSEG(%d), ParkFlag(%d) !!\n\n", curPos, gzoomCb.curZSeg, gzoomCb.parkFlag);

		zoomDestroy();

		/*-- zoom still notify callback */
		if (gzoomCb.isAskStopCB) {
			if (!gzoomCb.stopNotifyCBF) {
				zoomStopNotifyDefaultCBF(curPos, ZOOM_ERROR_NONE);
			}

			/* Shall reset this flag !! */
			gzoomCb.isAskStopCB = FALSE;
		}

		return;
	}

	gzoomCb.lastChkPos = curPos;
	#if (ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
	LENS_SLEEP(ZOOM_BRAKE_WATCHDOG_INTERVAL_MS);
	lensOsEventSet(&evZoomCtrl, EVENT_ZOOM_BRAKING_CTRL);
	#endif
}


/*-------------------------------------------------------------------------
 *  File Name : zoomTeleWideBrakeStart
 *------------------------------------------------------------------------*/
static
void
zoomTeleWideBrakeStart(
	TYPE_ZOOM_POS curZPos
)
{
	/*-- Trigger Motor Brake ASAP !! */
	zoomMotorBrake();

	/*-- Dismiss recursive MovingCheck */
	isRecurZMoving=FALSE;

	gzoomCb.brkConfirmCnt=0;
	gzoomCb.posBrake = curZPos;

	gzoomCb.motionStat = ZOOM_MXN_STATE_BRAKING;
	gzoomCb.chkCnt = 0;

	#if (LENS_BOOT_FOCUS_EARLY)
	lensOsEventSet(&evLensBootFocusEarly, EVENT_LENS_APP_LENS_BOOT_FOCUS_EARLY);
	#endif

	#if (ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
	LENS_SLEEP(ZOOM_BRAKE_INITIAL_SLEEP_MS);
	lensOsEventSet(&evZoomCtrl, EVENT_ZOOM_BRAKING_CTRL);
	#else
	/* enable BRAKING watchdog service to wait zoom lens go stopped */
	if (zoomWatchdogRegister(WATCHDOG_ZOOM_BRAKING)==FAIL) {
		LENS_LOG("%s: !!! FATAL !!!, failing wathdog reg!!\n",__FUNCTION__);
		zoomEmergencyService();
		return;
	}
	#endif

}
/*-------------------------------------------------------------------------
 *  File Name : taskProc_ZoomTeleMoving
 *------------------------------------------------------------------------*/
static
void
taskProc_ZoomTeleMoving(
	void
)
{
	TYPE_ZOOM_POS tgtStopPos, curZPos;
	UINT16 slipCnt;

	LENS_VERBOSE("%s \n", __FUNCTION__);

	curZPos = lensZoomCurPosGet();
	tgtStopPos = gzoomCb.tgtStopPos;

	/*-- dynamically adjust slip steps, if necessary */
	zoomDynamicSlipAdjust(ZOOM_CLK_CNT_GET);

	slipCnt = (isDoingBacklash) ? gzoomCb.blashSlipCnt : gzoomCb.teleSlipCnt;

	LENS_VERBOSE("%s: curPos(%d), tgtStop(%d), slipCnt(%d)\n", __FUNCTION__, curZPos, tgtStopPos, slipCnt);

	if (curZPos < (tgtStopPos-slipCnt)) {

		LENS_VERBOSE("%s: curPose(%d), lastChkPos(%d), chkCnt(%d) \n", __FUNCTION__, curZPos, gzoomCb.lastChkPos, gzoomCb.chkCnt);

		if (taskProc_ZoomJamCheck()==TRUE) {
			LENS_LOG("\n%s: !!! FATAL !!!, Zoom JAMMED !!\n\n", __FUNCTION__);
			zoomEmergencyService();
			LENS_MSG_POST(APP_UI_MSG_LENS_ERR, 0);
		} else {
			gzoomCb.lastChkPos = curZPos;
			/* %REVIEW% : shall use watchdog service ?? */
			/* Can not use watchdog service, because Moving need
			 * intensive position check. It would be too much overhead
			 * to use TimerISR.
			 */
			isRecurZMoving=TRUE;
		}

		return;
	}

	/*--
	 * about to brake zoom lens
	 */
	profLogAdd(0,"**zoom brake");
	zoomTeleWideBrakeStart(curZPos);
	LENS_VERBOSE("ZMing(T): lastChkPos(%d)\n", gzoomCb.lastChkPos);
	LENS_LOG("\nZMing(T): About to brake @cur(%d), ZSeg(%d), tgt(%d), slip(%d)\n", curZPos, gzoomCb.curZSeg, tgtStopPos, slipCnt);
	LENS_DBG("\nZMing(T): ZClkCnt(%d), curZSeg(%d) !!\n", ZOOM_CLK_CNT_GET, gzoomCb.curZSeg);
}

/*-------------------------------------------------------------------------
 *  File Name : taskProc_ZoomWideMoving
 *------------------------------------------------------------------------*/
static
void
taskProc_ZoomWideMoving(
	void
)
{
	TYPE_ZOOM_POS tgtStopPos, curZPos;
	TYPE_ZOOM_POS brakePos;	/* the real pos to start braking while counting backlash in */

	LENS_VERBOSE("%s \n", __FUNCTION__);

	curZPos = lensZoomCurPosGet();
	tgtStopPos = gzoomCb.tgtStopPos;

	/*-- dynamically adjust slip steps, if necessary */
	zoomDynamicSlipAdjust(ZOOM_CLK_CNT_GET);
	if (isCountInBacklash) {
		brakePos = tgtStopPos - ZOOM_BACKLASH_STEP_CNT + gzoomCb.wideSlipCnt;
	} else {
		brakePos = tgtStopPos + (tgtStopPos==ZOOM_CLOSE_POS ? 0 : gzoomCb.wideSlipCnt);
	}

	if (brakePos < ZOOM_CLOSE_POS) {
		brakePos = ZOOM_CLOSE_POS;
	}

	LENS_VERBOSE("%s: curPos(%d), tgtStop(%d), wideSlipCnt(%d)\n", __FUNCTION__, curZPos, tgtStopPos, gzoomCb.wideSlipCnt);

	if (curZPos > brakePos) {

		LENS_VERBOSE("%s: curPose(%d), lastChkPos(%d), chkCnt(%d) \n", __FUNCTION__, curZPos, gzoomCb.lastChkPos, gzoomCb.chkCnt);

		if (taskProc_ZoomJamCheck()==TRUE) {
			LENS_LOG("\n%s: !!! FATAL !!!, Zoom JAMMED !!\n\n", __FUNCTION__);
			zoomEmergencyService();
			LENS_MSG_POST(APP_UI_MSG_LENS_ERR, 0);
		} else {
			gzoomCb.lastChkPos = curZPos;
			/* %REVIEW% : shall use watchdog service ?? */
			/* Can not use watchdog service, because Moving need
			 * intensive position check. It would be too much overhead
			 * to use TimerISR.
			 */
			isRecurZMoving=TRUE;
		}

		return;
	}

	/*--
	 * about to brake zoom lens
	 */
	zoomTeleWideBrakeStart(curZPos);
	LENS_LOG("\nZMing(W): About to brake @cur(%d), tgt(%d), brakePos(%d), slip(%d)\n", curZPos, tgtStopPos, brakePos, gzoomCb.wideSlipCnt);
	LENS_DBG("\nZMing(W): ZClkCnt(%d)!!\n", ZOOM_CLK_CNT_GET);
	LENS_VERBOSE("ZMing(W): lastChkPos(%d)\n", gzoomCb.lastChkPos);
}

#if (!ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
/*-------------------------------------------------------------------------
 *  File Name : zoomWatchdogRelease
 *------------------------------------------------------------------------*/
static
void
zoomWatchdogRelease(
	UINT32 id
)
{
	if (id>=WATCHDOG_ZOOM_MAX) {
		LENS_DBG("%s: Illegal index(%d) \n", __FUNCTION__, id);
		return;
	}

	if (id==WATCHDOG_ZOOM_BRAKING) {
		appTimerIsrUnreg(&gzmWatchdogTbl[id]);
	}

	gzmWatchdogTbl[id] = 0;
}
#endif

#if (!ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
/*-------------------------------------------------------------------------
 *  File Name : zoomWatchdogRegister
 *------------------------------------------------------------------------*/
UINT32
zoomWatchdogRegister(
	UINT32	id
)
{
	UINT32 serviceTag;

	if (id>=WATCHDOG_ZOOM_MAX) {
		LENS_DBG("%s: Illegal index(%d) \n", __FUNCTION__, id);
		return FAIL;
	}

	if (gzmWatchdogTbl[id]) {
		/* already registered !! */
		return SUCCESS;
	}

	serviceTag = TIMER_NULL;
	if (id==WATCHDOG_ZOOM_BRAKING) {

	//profLogAdd(0,"**zm tmr Isr reg");

		serviceTag = appTimerIsrReg(watchdog_ZoomBrakingService, ZOOM_BRAKE_WATCHDOG_INTERVAL_TiCKS); /* Ref ZOOM_BRAKE_WATCHDOG_MS_PER_TICK for time unit */
	}

	if (serviceTag==TIMER_NULL) {
		printf("\n%s: !!! FATAL !!!, watchdog service error\n\n", __FUNCTION__);
		return FAIL;
	}

	gzmWatchdogTbl[id]=serviceTag;
	return SUCCESS;
}
#endif

#if (!ZOOM_BRAKE_CHK_BY_ZOOM_TASK)
/*-------------------------------------------------------------------------
 *  File Name : watchdog_ZoomBrakingService
 *------------------------------------------------------------------------*/
static
void
watchdog_ZoomBrakingService(
	void
)
{
	//profLogAdd(0,"*send brake evt");
	lensOsEventSet(&evZoomCtrl, EVENT_ZOOM_BRAKING_CTRL);
}
#endif

/*-------------------------------------------------------------------------
 *  DEBUG FUNCTIONS : Export for cmdLine debug                            *
 -------------------------------------------------------------------------*/
#if ZOOM_DBG_FUNCTION_EXPORT

void dbg_zoomMotorTele(void)
{
	zoomMotorTele();
}

void dbg_zoomMotorWide(void)
{
	zoomMotorWide();
}

void dbg_zoomMotorStandby(void)
{
	zoomMotorStandby();
}

void dbg_zoomMotorBrake(void)
{
	zoomMotorBrake();
}
#endif /* ZOOM_DBG_FUNCTION_EXPORT */

#endif /* KIT_WITH_LENS */
