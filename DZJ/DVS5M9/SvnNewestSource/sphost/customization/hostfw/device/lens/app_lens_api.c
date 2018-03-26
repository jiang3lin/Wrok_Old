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
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_motor.h"
#include "app_lens_zoom.h"
#include "app_lens_focus.h"
#include "app_lens_iris.h"
#include "app_lens_mshut.h"
#include "app_lens_api.h"
#include "app_ui_msg.h"

#include "sp5k_lmi_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*-- LensAPP task main control block */
typedef struct lensAppCtrlBlock_s {
	UINT32 status;
	UINT32 errCode;
} lensAppCtrlBlock_t;


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if KIT_WITH_LENS

static BOOL gIsLensEverBoot = FALSE;
static BOOL gIsFocusInited = FALSE;


/*-- lensApp task control resources */
static TX_EVENT_FLAGS_GROUP evLensApp;
static TX_EVENT_FLAGS_GROUP evLensAppDone;
static TX_MUTEX mtxLensApp;
#if (LENS_BOOT_FOCUS_EARLY)
TX_EVENT_FLAGS_GROUP evLensBootFocusEarly;
#endif

lensAppCtrlBlock_t glensAppCb; /*-- LensAPP task global */

appLensStopNotifyCBF_t *plensStopNtfyFunc = NULL;

#endif /* KIT_WITH_LENS */

/*-- The following indicate whether if lensMotorPowerOff
 *   shall be ignored because MShut or Iris is busy.
 */
BOOL	glensPwrIsMShutBusy = FALSE;
BOOL	glensPwrIsIrisBusy = FALSE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#if KIT_WITH_LENS
/*-------------------------------------------------------------------------
 *  File Name : _appLensCrashReset
 *------------------------------------------------------------------------*/
static
UINT32
_appLensCrashReset(
	void
)
{
	LENS_DBG("%s \n", __FUNCTION__);

	/*-- Prerequesite !!
	 * Assume all lens power and leds has been powered
	 */
	if (lensFocusCrashRecover()==SUCCESS) {
		gIsFocusInited = TRUE;

		/*-- go for Zoom lens */
		return lensZoomCrashReset();
	} else {
		LENS_LOG("\n**********%s: Focus recovery failed !!!!\n\n", __FUNCTION__);
		return FAIL;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : lensZoomFocusReset
 *------------------------------------------------------------------------*/
static
UINT32
lensZoomFocusReset (
	void
)
{
	UINT32 err;

	/*-- check if Lens is closed ? */
	lensZoomClkCntClear();
	if (IS_LENS_CLOSE) {
		/*--
		 * Lens is at CLOSE side
		 */
		LENS_DBG("\n%s: LENS is closed ! \n\n", __FUNCTION__);

		/*-- give a initial zoom position */
		lensZoomCurPosSet(ZOOM_CLOSE_POS);

		if (IS_LENS_FOCUS_HOME) {
			/*--
			 * NORMAL case : Zoom & Focus has been in close state !!
			 */
			LENS_DBG("LENS RST: case A-1\n");
			err = SUCCESS;
			gIsFocusInited = FALSE;
				/*-- because Focus is already home,
				 * we will do focus reset to locate curPos later on
				 * while boot lens to wide-stop !
				 */
		} else {
			/*-- %TODO%
			 * CRASH case : Focus could has jammed !!
			 * (1) Move Zoom to open side
			 * (2) Move Focus to close side
			 * (3) Move Zoom to close side
			 */
			LENS_DBG("\n*****************************************\n");
			LENS_DBG("LENS RST: !!!FATAL!!! case A-2 !!!!");
			LENS_DBG("\n*****************************************\n");
			err = lensZoomPosGo(ZOOM_WIDE_POS);
			if (err==SUCCESS) {
				err = lensZoomStopWait(ZOOM_OP_BLOCKING);
			}

			if (err==SUCCESS) {
				err = lensFocusReset();
				gIsFocusInited = TRUE;
			}

			if (err==SUCCESS) {
				err = lensZoomPosGo(ZOOM_CLOSE_POS);
			}

			if (err==SUCCESS) {
				err = lensZoomStopWait(ZOOM_OP_BLOCKING);
			}
		}
		lensZoomResetConfig();
	} else {
		/*--
		 * Lens is at OPEN side
		 */
		LENS_DBG("\n%s: LENS is open !! \n\n", __FUNCTION__);

		/* Crash case !!
		 * Lens pos is unknown, shall go special flow !!
		 */
		err = _appLensCrashReset();
	}

	return err;
}

/*-------------------------------------------------------------------------
 *  File Name : lensBootReset
 *------------------------------------------------------------------------*/
static
UINT32
lensBootReset(
	void
)
{
	LENS_VERBOSE("%s \n", __FUNCTION__);

	/*--
	 * Check & reset Zoom-Focus lens to close state
	 */
	if (lensZoomFocusReset()==FAIL) {
		/* %TODO%
		 * (1) shutdown lens immediately
		 * (2) check lens error code
		 * (3) call alert handler
		 */
		LENS_DBG("\n%s: !!! FATAL !!! Lens reset failed !!\n\n", __FUNCTION__);
		return FAIL;
	}
	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : lensBootOpen
 *------------------------------------------------------------------------*/
static
UINT32
lensBootOpen (
	void
)
{
	/*-- Prerequesite :
	 * Shall assure both Zoom & Focus are reset at HOME side !!
	 */
	LENS_VERBOSE("%s \n", __FUNCTION__);

	profLogAdd(0, "*zoom start");

#if (LENS_BOOT_FOCUS_EARLY)
	ULONG waitEvent;
	/*-- (1) Move Zoom Lens to Wide Stop */
	if (FAIL==lensZoomPosGo(ZOOM_WIDE_POS)) {
		return FAIL;
	}
	lensOsEventGet(&evLensBootFocusEarly, EVENT_LENS_APP_LENS_BOOT_FOCUS_EARLY, &waitEvent);
	profLogAdd(0, "*focus trig");

	/*-- (2) Focus to default pos */
	if (!gIsFocusInited) {
		if (FAIL==lensFocusBoot()) {
			return FAIL;
		}
	} else {
		lensFocusJob(FOCUS_BOOT_POS);
	}
	profLogAdd(0, "*focus stop");

	/*-- (3) Confirm ZOOM Stopped */
	UINT32 qryVal;
	qryVal=lensZoomQuery(ZOOM_QUERY_MOTION_STATE, 0);
	if (qryVal != ZOOM_MXN_STATE_STOP) {
		lensZoomStopWait(ZOOM_OP_BLOCKING);
	}
	profLogAdd(0, "*zoom stop");
#else
	/*-- (1) Move Zoom Lens to Wide Stop */
	if (FAIL==lensZoomPosGo(ZOOM_WIDE_POS)) {
		return FAIL;
	}
	lensZoomStopWait(ZOOM_OP_BLOCKING);
	profLogAdd(0, "*zoom stop");

	/*-- (2) Focus to default pos */
	if (!gIsFocusInited) {
		if (FAIL==lensFocusBoot()) {
			return FAIL;
		}
	} else {
		lensFocusJob(FOCUS_BOOT_POS);
	}
	profLogAdd(0, "*focus stop");
#endif

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : lensStatusMark
 *------------------------------------------------------------------------*/
static
void
lensStatusMark (
	UINT32 statusMsk
)
{
	LENS_VERBOSE("%s : msk(0x%08x) \n", __FUNCTION__, statusMsk);
	glensAppCb.status |= statusMsk;
	LENS_VERBOSE("%s : status(0x%08x) \n", __FUNCTION__, glensAppCb.status);
}

/*-------------------------------------------------------------------------
 *  File Name : lensStatusClear
 *------------------------------------------------------------------------*/
static
void
lensStatusClear (
	UINT32 statusMsk
)
{
	LENS_VERBOSE("%s : msk(0x%08x) \n", __FUNCTION__, statusMsk);
	glensAppCb.status &= (~statusMsk);
	LENS_VERBOSE("%s : status(0x%08x) \n", __FUNCTION__, glensAppCb.status);
}
/*-------------------------------------------------------------------------
 *  File Name : _appLensLedPowerCtrl
 *------------------------------------------------------------------------*/
static void _appLensLedPowerCtrl(BOOL isOn)
{
	LENS_DBG("\n**** LensLED : -- %s --\n\n", (!!isOn ? "ON":"OFF"));

	if (isOn) {
		lensLedPowerOn(LENS_LED_ID_ALL);
	} else {
		lensLedPowerOff(LENS_LED_ID_ALL);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : _appLensPowerCtrl
 *------------------------------------------------------------------------*/
static void _appLensPowerCtrl(BOOL isOn)
{
	LENS_DBG("\n**** LensPWR : -- %s --\n\n", (!!isOn ? "ON":"OFF"));

	if (isOn) {
		lensMotorPowerOn();
	} else {
		lensMotorPowerOff();
	}
}
/*-------------------------------------------------------------------------
 *  File Name : _appLensStopProc
 *------------------------------------------------------------------------*/
static void _appLensStopProc(void)
{
	LENS_VERBOSE("%s ...\n", __FUNCTION__);

	/*-- Notify LENS STOP */
	LENS_POST_MSG(APP_UI_MSG_LENS_STOP_NOTIFY, (0x01<<16));
	/*-- Lens Stop Notify Callback */
	if (plensStopNtfyFunc != NULL) {
		(*plensStopNtfyFunc)();
	}
}
#endif /* KIT_WITH_LENS */
#if 0
/*-------------------------------------------------------------------------
 *  File Name : appLensSafeActuateCheck
 *------------------------------------------------------------------------*/
static
UINT32
appLensSafeActuateCheck(
	UINT32 whoisit
)
{
#if KIT_WITH_LENS
	return lensMotorSafeActuateQuery();
#else
	return SUCCESS;
#endif
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appLMI_CTRL_Init
 *------------------------------------------------------------------------*/
UINT32
appLMI_CTRL_Init(
	void
)
{
#if KIT_LMI_CTRL
	UINT32 freq,unit;
	UINT32 delay[]={240,2}; /* 30ms delay for 1st state */
	freq = 1920000; /* LMI clock = 1.92MHz */
	unit = 240; 	/* unit = 240/1.92MHz = 1/8 ms */
	freq = sp5kLmiFreqSet(freq);
	sp5kLmiDelaySet(unit, 1, COUNT_OF(delay), delay); 
	sp5kLmiPinSet(0xF0); /* LMI 4 & 5 & 6 & 7 */ 
	return SUCCESS;
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appMShutOpen
 *------------------------------------------------------------------------*/
UINT32
appMShutOpen(
	void
)
{
#if KIT_WITH_MSHUT
	return lensMShutOpen();
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appMShutClose
 *------------------------------------------------------------------------*/
UINT32
appMShutClose(
	void
)
{
#if KIT_WITH_MSHUT
	return lensMShutClose();
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appIrisInit
 *------------------------------------------------------------------------*/
void
appIrisInit(
	void
)
{
#if KIT_WITH_IRIS
	lensIrisInit();
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appIrisLargeSet
 *------------------------------------------------------------------------*/
UINT32
appIrisLargeSet(
	void
)
{
#if KIT_WITH_IRIS
	return lensIrisLargeSet();
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appIrisSmallSet
 *------------------------------------------------------------------------*/
UINT32
appIrisSmallSet(
	void
)
{
#if KIT_WITH_IRIS
	return lensIrisSmallSet();
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appFocusPosGo
 *------------------------------------------------------------------------*/
UINT32
appFocusPosGo(
	SINT16 pos
)
{
	LENS_VERBOSE("%s: pos(%d) \n", __FUNCTION__, pos);
#if KIT_WITH_LENS
	return lensFocusPosGo(pos);
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appFocusStopWait
 *------------------------------------------------------------------------*/
void
appFocusStopWait(
	void
)
{
	LENS_VERBOSE("%s \n", __FUNCTION__);
#if KIT_WITH_LENS
	lensFocusStopWait();
#endif
}
/*-------------------------------------------------------------------------
 *  File Name : appLensAFCreate
 *------------------------------------------------------------------------*/
void
appLensAFCreate(
	void
)
{
#if KIT_WITH_LENS
	LENS_VERBOSE("\n >>> AF CREATE !!\n\n");
	/*-- Power On Lens Motor */
	_appLensPowerCtrl(ENABLE);
	/*-- Initial excitation Focus lens */
	lensFocusCreate();
#endif
}
/*-------------------------------------------------------------------------
 *  File Name : appLensAFDestroy
 *------------------------------------------------------------------------*/
void
appLensAFDestroy(
	void
)
{
#if KIT_WITH_LENS
	/*-- Braking excitation Focus lens */
	lensFocusDestroy();
	/*-- Power Off Lens Motor */
	_appLensPowerCtrl(DISABLE);
	LENS_VERBOSE("\n <<< AF DESTROY !!\n\n");
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensCreate
 *------------------------------------------------------------------------*/
void
appLensCreate(
	void
)
{
#if KIT_WITH_LENS
	LENS_VERBOSE("\n >>> Lens CREATE !!\n\n");
	/*-- Power On Lens Motor */
	_appLensPowerCtrl(ENABLE);
	/*-- Power on Zoom and Focus Home/Clk leds */
	_appLensLedPowerCtrl(ENABLE);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensDestroy
 *------------------------------------------------------------------------*/
void
appLensDestroy(
	void
)
{
#if KIT_WITH_LENS
	LENS_VERBOSE("\n <<< Lens DESTROY !!\n\n");
	/*-- Power off Zoom and Focus Home/Clk leds */
	_appLensLedPowerCtrl(DISABLE);
	/*-- Power Off Lens Motor */
	_appLensPowerCtrl(DISABLE);
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : appLensQuery
 *------------------------------------------------------------------------*/
UINT32
appLensQuery(
	UINT32 queryId
)
{
	UINT32 ret, param;

	ret = 0xCCCCAAAA;

	/*-- Reserve the Most Significant 16-bit to carry parameter if necessary */
	param = (queryId & 0xFFFF0000)>>16;
#if KIT_WITH_LENS
	switch (queryId & 0x0000FFFF) {
	default :
		ret = 0xCCCCAAAA;
		LENS_DBG("\n%s: !! ERROR !!, illegal id(%d) \n\n", __FUNCTION__, queryId);
		break;
	case LENS_QUERY_BOOT_DONE:
		if (gIsLensEverBoot) ret = 1; /*-- Boot completed */
		else ret = 0; /*-- Never boot yet, or boot busy. */
		break;
	case LENS_QUERY_MOTION_STATE:
		if (glensAppCb.status & LENS_ST_MSK_MOTION_STATE) {
			ret = 1; /*-- still moving */
		} else {
			ret = 0; /*-- lens stop */
		}
		break;
	case LENS_QUERY_LENS_CLOSED:
		/*-- %REVIEW%: Well, it is dangerous !! */
		if (lensLedPowerQuery(LENS_LED_ID_ZHM)==FALSE) {
			lensLedPowerOn(LENS_LED_ID_ZHM);
			ret = (IS_LENS_CLOSE) ? 1 : 0;
			lensLedPowerOff(LENS_LED_ID_ZHM);
		} else {
			ret = (IS_LENS_CLOSE) ? 1 : 0;
		}
		break;
	case LENS_QUERY_ZOOM_TELE_PARKED:
		ret = lensZoomTeleParked();
		break;
	case LENS_QUERY_ZOOM_WIDE_PARKED:
		ret = lensZoomWideParked();
		break;
	case LENS_QUERY_IRIS_SEG_POS:
		ret = lensIrisSegPosQuery();
		if (ret==IRIS_STOP_LARGE) ret = 0;
		else ret = 1;
		break;		
	case LENS_QUERY_ZOOM_SEG_POS:
		ret = (UINT32)lensZoomCurSegGet();
		break;
	case LENS_QUERY_ZOOM_SEG_NUM:
		ret = lensZoomQuery(ZOOM_QUERY_SEG_NUM, 0);
		break;
	case LENS_QUERY_ZOOM_SEG2POS:
		/*-- Use the Most Significant 16-bit to carry the ZSEG */
		ret = lensZoomQuery(ZOOM_QUERY_SEG2POS, param);
		break;
	case LENS_QUERY_ZOOM_STEP_POS:
		ret = (UINT32)lensZoomCurPosGet();
		break;
	case LENS_QUERY_FOCUS_MOTION_STATE:
		ret = (UINT32)lensFocusQuery(FOCUS_QUERY_ID_MOTION_STATE);
		break;
	case LENS_QUERY_FOCUS_HOME_POS:
		ret = (UINT32)lensFocusQuery(FOCUS_QUERY_ID_HOME_POS);
		break;
	case LENS_QUERY_FOCUS_IS_HOME:
		ret = (UINT32)lensFocusQuery(FOCUS_QUERY_ID_IS_IO_HOME);
		break;
	case LENS_QUERY_FOCUS_STEP_POS:
		ret = (UINT32)lensFocusQuery(FOCUS_QUERY_CURRENT_POS);
		break;
	}

	return ret;
#else
	/*-- KIT w/o lens */
	switch (queryId & 0x0000FFFF) {
	default :
		ret = 0xCCCCAAAA;
		LENS_DBG("\n%s: !! ERROR !!, illegal id(%d) \n\n", __FUNCTION__, queryId);
		break;
	case LENS_QUERY_BOOT_DONE:
	case LENS_QUERY_LENS_CLOSED:
	case LENS_QUERY_FOCUS_HOME_POS:
	case LENS_QUERY_FOCUS_IS_HOME:
		ret = 1;
		break;
	case LENS_QUERY_MOTION_STATE:
	case LENS_QUERY_ZOOM_TELE_PARKED:
	case LENS_QUERY_ZOOM_WIDE_PARKED:
	case LENS_QUERY_ZOOM_SEG_POS:
	case LENS_QUERY_ZOOM_SEG_NUM:
	case LENS_QUERY_ZOOM_SEG2POS:
	case LENS_QUERY_ZOOM_STEP_POS:
	case LENS_QUERY_FOCUS_MOTION_STATE:
		ret = 0; /*-- lens stop */
		break;
	#if KIT_WITH_IRIS		
	case LENS_QUERY_IRIS_SEG_POS:
		ret = lensIrisSegPosQuery();
		if (ret==IRIS_STOP_LARGE) ret = 0;
		else ret = 1;
		break;
	#else
	case LENS_QUERY_IRIS_SEG_POS:
		ret = 0; /*-- lens stop */
		break;

	#endif
	}
	return ret;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensStopWait
 *------------------------------------------------------------------------*/
UINT32
appLensStopWait(
	void
)
{
#if KIT_WITH_LENS

	ULONG waitEvent;
	UINT32 isMoving;
	profLogAdd(0,"lens stop wait:s");
	LENS_LOG("%s \n", __FUNCTION__);
	isMoving = appLensQuery(LENS_QUERY_MOTION_STATE);

	/*--
	 * if Lens has stopped, return is ZERO.
	 */
	if (isMoving) {
		//LENS_VERBOSE(">> Lens Moving ... wait event \n");
		lensOsEventGet(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK, &waitEvent);
	} else {
		/*-- Test and Clear Event !! */
		//LENS_VERBOSE(">> Lens Standby ... clr event \n");
		lensOsEventCheck(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK, &waitEvent);
	}

	LENS_LOG("%s exit \n", __FUNCTION__);
	profLogAdd(0,"lens stop wait:e");

#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appLensStop
 *------------------------------------------------------------------------*/
void
appLensStop(
	void
)
{
#if KIT_WITH_LENS
	UINT32 qryResult;

	LENS_LOG("%s \n", __FUNCTION__);

	qryResult = appLensQuery(LENS_QUERY_MOTION_STATE);
	/*--
	 * if Lens has stopped return is ZERO.
	 */
	if (!qryResult) return;

	/*-- Dont submit command to lensAPP task !!
	 * All jobs in lensAPP task is excuting in blocking way.
	 * You dont have a chance to interrupt current Tele/Wide job.
	 */

	/*- if Zoom moving, set stop pos.
	 * !! If Zoom is braking, dont do anything but return.
	 *    It is because that the stop pos has been determined if
	 *    Zoom motion state is BRAKING, DONT override it !!!!
	 */
	qryResult = lensZoomQuery(ZOOM_QUERY_MOTION_STATE, 0);
	if (qryResult==ZOOM_MXN_STATE_WIDE || qryResult==ZOOM_MXN_STATE_TELE) {
		/*-- Calculate & Set Next Stop pos */
		lensZoomNextStopPosSet();
			/*- Do not need to wait lens stop.
			 * taskProc_LensTeleWide will do it and
			 * send event to appLensStopWait().
			 * Caller shall call appLesStopWait() to
			 * check lens has stopped successfully.
			 */
	}

	/*- What if the Focus is running ?
	 *  It is doing default focusing after new zoom stop.
	 *  Just let free run and complete, otherwise you
	 *  might get a blur image since zoom stop has changed.
	 */

	return;
#endif
}
/*-------------------------------------------------------------------------
 *  File Name : appLensStopCallbackSet
 *------------------------------------------------------------------------*/
void appLensStopCallbackSet(appLensStopNotifyCBF_t *pcallbackFunc)
{
#if KIT_WITH_LENS
	plensStopNtfyFunc = pcallbackFunc;
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : appLensZoomTele
 *------------------------------------------------------------------------*/
UINT32
appLensTele(
	void
)
{
#if KIT_WITH_LENS
	UINT32 qryResult;
	qryResult = appLensQuery(LENS_QUERY_MOTION_STATE);
	/*--
	 * if Lens has stopped return is ZERO.
	 */
	if (qryResult) {
		LENS_LOG("\n%s: Lens moving, ignored \n\n", __FUNCTION__);
		return FAIL;
	}

	if (lensZoomTeleParked()) return SUCCESS;

	lensOsMutexGet(&mtxLensApp);

	ULONG waitEvent;
	lensOsEventCheck(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK, &waitEvent);
		/*-- 20080508: Bug resolve
		 *	For the case :
		 *  Lens is boot to CALIB, HOST might not call appLensStopWait()
		 *  to clear the EVENT_LENS_APP_LENS_STOP_ACK event.
		 *  Lens will stop automatically at WIDE stop, and EVENT_LENS_APP_LENS_STOP_ACK
		 *  is to be set.
		 *  There would cause problem while checking Lens stop by appLensStopWait() later.
		 *  The appLensStopWait() will misread the last STOP_ACK in PB !!
		 *  >>> Resolution : Clear the STOP_ACK before Lens Open.
		 */

	lensStatusMark(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensApp, EVENT_LENS_APP_TELE);

	lensOsMutexPut(&mtxLensApp);
#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appLensWide
 *------------------------------------------------------------------------*/
UINT32
appLensWide(
	void
)
{
#if KIT_WITH_LENS
	UINT32 qryResult;
	qryResult = appLensQuery(LENS_QUERY_MOTION_STATE);
	/*--
	 * if Lens has stopped return is ZERO.
	 */
	if (qryResult) {
		LENS_LOG("\n%s: Lens moving, ignored \n\n", __FUNCTION__);
		return FAIL;
	}

	if (lensZoomWideParked()) return SUCCESS;

	lensOsMutexGet(&mtxLensApp);

	ULONG waitEvent;
	lensOsEventCheck(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK, &waitEvent);
		/*-- 20080508: Bug resolve
		 *	For the case :
		 *  Lens is boot to CALIB, HOST might not call appLensStopWait()
		 *  to clear the EVENT_LENS_APP_LENS_STOP_ACK event.
		 *  Lens will stop automatically at WIDE stop, and EVENT_LENS_APP_LENS_STOP_ACK
		 *  is to be set.
		 *  There would cause problem while checking Lens stop by appLensStopWait() later.
		 *  The appLensStopWait() will misread the last STOP_ACK in PB !!
		 *  >>> Resolution : Clear the STOP_ACK before Lens Open.
		 */

	lensStatusMark(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensApp, EVENT_LENS_APP_WIDE);

	lensOsMutexPut(&mtxLensApp);
#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appLensZoomClose
 *------------------------------------------------------------------------*/
UINT32
appLensClose(
	void
)
{
#if KIT_WITH_LENS
	ULONG waitEvent;
	UINT32 qryResult;

	LENS_LOG("%s \n", __FUNCTION__);

	qryResult = appLensQuery(LENS_QUERY_MOTION_STATE);
	/*--
	 * if Lens has stopped return is ZERO.
	 */
	if (qryResult) {
		LENS_LOG("\n%s: Lens moving, ignored \n\n", __FUNCTION__);
		return FAIL;
	}

	if (lensZoomCloseParked() || IS_LENS_CLOSE) {
		LENS_LOG("\n%s: CLOSE Parked, ignored \n\n", __FUNCTION__);
		return SUCCESS;
	}

	lensOsMutexGet(&mtxLensApp);

	lensStatusMark(LENS_ST_MSK_MOTION_STATE);

	/*-- Test and Clear Event !!
	 * To resolve the bug that HOST had trigger Lens operation, such as
	 * BOOT but did not invoke appLensStopWait() to check
	 * EVENT_LENS_APP_LENS_STOP_ACK.
	 * In this case, the next call to appLensStopWait() of the Lens move
	 * operation followed will misadjust the Lens has stopped to still while
	 * the Lens is still moving in fact.
	 */
	lensOsEventCheck(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK, &waitEvent);

	lensOsEventSet(&evLensApp, EVENT_LENS_APP_CLOSE);

	lensOsMutexPut(&mtxLensApp);
#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appLensBoot
 *------------------------------------------------------------------------*/
void
appLensBoot (
	void
)
{
#if KIT_WITH_LENS
	profLogAdd(0, "lens boot");

	lensOsMutexGet(&mtxLensApp);

	lensStatusMark(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensApp, EVENT_LENS_APP_BOOT);

	lensOsMutexPut(&mtxLensApp);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensOpen
 *------------------------------------------------------------------------*/
void
appLensOpen (
	void
)
{
#if KIT_WITH_LENS
	ULONG waitEvent;

	LENS_LOG("%s \n", __FUNCTION__);

	lensOsMutexGet(&mtxLensApp);

	lensOsEventCheck(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK, &waitEvent);
		/*-- 20080415: Bug resolve
		 *	For the case that Lens is closed in PB mode, PB wont invoke
		 *  appLensStopWait() to clear the EVENT_LENS_APP_LENS_STOP_ACK event.
		 *  While re-entering VIEW mode, this will result in the bug that
		 *  VIEW will misread the Lens has stopped at the following operation:
		 *  --> appLensOpen(); --> appLensStopWait();
		 *  The appLensStopWait() will misread the last STOP_ACK in PB !!
		 *  >>> Resolution : Clear the STOP_ACK before Lens Open.
		 */
	lensStatusMark(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensApp, EVENT_LENS_APP_OPEN);

	lensOsMutexPut(&mtxLensApp);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensCrashRecover
 *------------------------------------------------------------------------*/
UINT32
appLensCrashRecover(
	void
)
{
#if (KIT_WITH_LENS)
	UINT32 ret;

	appLensCreate();

	if (IS_LENS_CLOSE) {
		LENS_DBG("\n**** %s: Lens@Close, ignored !! \n\n", __FUNCTION__);
		appLensDestroy();
		return SUCCESS;
	}

	LENS_LOG("\n**** Lens Recovering !! \n\n");
	ret = _appLensCrashReset();
	LENS_LOG("\n**** Lens Recover OK !! \n\n");

	appLensDestroy();

	return ret;
#else
	return SUCCESS;	
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensInit
 *------------------------------------------------------------------------*/
void
appLensInit(
	void
)
{
#if KIT_WITH_LENS
	static UINT32 isLensEverInit=FALSE;

	#if (defined(_EVB_HW_) && defined(EVB_CFG_COUGAR))
	lensMotorGpioConfig();
		/*-- TEMP for motor port */
	#endif

	if (isLensEverInit) {
		return;
	}

	LENS_VERBOSE("%s \n", __FUNCTION__);

	profLogAdd(0, "lens init");

	isLensEverInit = TRUE;

	/*-- Lens Tasks init */
	lensAppTaskInit();
	lensZoomTaskInit();
	lensFocusTaskInit();

	#ifndef EVB_CFG_COUGAR
	lensIrisInit();
	lensMShutInit();
	#endif

#else

	/*s Modify by Aries 09/07/27*/
	#if KIT_WITH_MSHUT
		#if KIT_WITH_IRIS
			#if KIT_LMI_CTRL
				profLogAdd(0, "lens init");
				appLMI_CTRL_Init();
				lensMotorPowerOn();
				appMShutOpen();
				appIrisLargeSet();
				profLogAdd(0, "lens init end");
			#endif
		#else
			#if KIT_LMI_CTRL
				profLogAdd(0, "lens init");
				appLMI_CTRL_Init();
				lensMotorPowerOn();
				appMShutOpen();
				profLogAdd(0, "lens init end");
			#endif
		#endif
	#endif
	/*e Modify by Aries 09/07/27*/

#endif
}

/*-------------------------------------------------------------------------
 *  File Name : calibLensZoomSegGo
 *------------------------------------------------------------------------*/
void
calibLensZoomSegGo(
	UINT32 zmSeg
)
{
#if KIT_WITH_LENS
	UINT32 curZSeg;
	TYPE_ZOOM_POS tgtPos;

	LENS_VERBOSE("%s: zmSeg(%d) \n", __FUNCTION__, zmSeg);

	/*-- Note: Lens shall have been boot successfully.
	 */
	/*-- Query current Zoom SEG to decide Tele/Wide */
	curZSeg = appLensQuery(LENS_QUERY_ZOOM_SEG_POS);
	if (curZSeg == zmSeg) {
		LENS_DBG("\n%s: Already parked @ zmSeg(%d) !!\n\n", __FUNCTION__, zmSeg);
		return;
	}

	/*-- Get target pos */
	tgtPos = (TYPE_ZOOM_POS)appLensQuery(LENS_QUERY_ZOOM_SEG2POS+(zmSeg<<16));

	if (zmSeg > curZSeg) {
		/*-- Shall Tele */
		appLensTele();
	} else {
		/*-- Shall Wide */
		appLensWide();
	}
	LENS_SLEEP(1);
	lensZoomStopPosSet(tgtPos);

	appLensStopWait();
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensZoomTeleWideSlipCalib
 *------------------------------------------------------------------------*/
UINT32
appLensZoomTeleWideSlipCalib(
	UINT32 startSeg,
	UINT32 stopSeg
)
{
#if KIT_WITH_LENS
	UINT32 curZPos, brkZPos;

	/*-- Enable ZOOM Calibration */
	lensZoomCalibSessionControl(TRUE);

	/*-- Close Lens */
	appLensClose();
	APP_SLEEP_MS(2000);

	/*-- Boot Lens */
	appLensBoot();
	appLensStopWait();
	APP_SLEEP_MS(2000);

	if (startSeg>0) {
		calibLensZoomSegGo(startSeg);
		APP_SLEEP_MS(2000);
	}

	calibLensZoomSegGo(stopSeg);
	APP_SLEEP_MS(2000);

	/*-- Calculate Slip steps */
	curZPos = lensZoomQuery(ZOOM_QUERY_STEP_POS, 0);
	brkZPos = lensZoomQuery(ZOOM_QUERY_BRAKE_POS, 0);
	LENS_VERBOSE("BootSlip: curZPos(%d), brkZPos(%d) >>> Slip(%d) \n", curZPos, brkZPos, curZPos-brkZPos);

	/*-- Shut ZOOM Calibration */
	lensZoomCalibSessionControl(FALSE);

	return (curZPos-brkZPos);
#else
	return 0;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensZoomBlashSlipCalib
 *------------------------------------------------------------------------*/
UINT32
appLensZoomBlashSlipCalib(
	UINT32 startSeg,
	UINT32 stopSeg
)
{
#if KIT_WITH_LENS
	UINT32 curZPos, brkZPos;

	/*-- Enable ZOOM Calibration */
	lensZoomCalibSessionControl(TRUE);

	/*-- Close Lens */
	appLensClose();
	APP_SLEEP_MS(2000);

	/*-- Boot Lens */
	appLensBoot();
	appLensStopWait();
	APP_SLEEP_MS(2000);

	calibLensZoomSegGo(startSeg);
	APP_SLEEP_MS(2000);

	calibLensZoomSegGo(stopSeg);
	APP_SLEEP_MS(2000);

	/*-- Calculate Slip steps */
	curZPos = lensZoomQuery(ZOOM_QUERY_STEP_POS, 0);
	brkZPos= lensZoomQuery(ZOOM_QUERY_BRAKE_POS, 0);
	LENS_VERBOSE("BootSlip: curZPos(%d), brkZPos(%d) >>> Slip(%d) \n", curZPos, brkZPos, curZPos-brkZPos);

	/*-- Shut ZOOM Calibration */
	lensZoomCalibSessionControl(FALSE);

	return (curZPos-brkZPos);
#else
	return 0;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensZoomBootSlipCalib
 *------------------------------------------------------------------------*/
UINT32
appLensZoomBootSlipCalib(
	void
)
{
#if KIT_WITH_LENS
	UINT32 curZPos, brkZPos;

	/*-- Enable ZOOM Calibration */
	lensZoomCalibSessionControl(TRUE);

	/*-- Close Lens */
	appLensClose();
	APP_SLEEP_MS(2000);

	/*-- Boot Lens */
	appLensBoot();
	appLensStopWait();
	APP_SLEEP_MS(2000);

	/*-- Calculate Slip steps */
	curZPos = lensZoomQuery(ZOOM_QUERY_STEP_POS, 0);
	brkZPos = lensZoomQuery(ZOOM_QUERY_BRAKE_POS, 0);
	LENS_VERBOSE("BootSlip: curZPos(%d), brkZPos(%d) >>> Slip(%d) \n", curZPos, brkZPos, curZPos-brkZPos);

	/*-- Shut ZOOM Calibration */
	lensZoomCalibSessionControl(FALSE);

	return (curZPos-brkZPos);
#else
	return 0;
#endif

}


/*-------------------------------------------------------------------------
 *  File Name : taskProc_LensTeleWide
 *------------------------------------------------------------------------*/
#if KIT_WITH_LENS
static
void
taskProc_LensTeleWide (
	BOOL isTele
)
{
	LENS_VERBOSE("%s: %s \n", __FUNCTION__, (isTele ? "TELE" : "WIDE"));

	/*-- Create LENS session */
	appLensCreate();

	lensZoomHomeISRDisable();
		/*-- Disable HOME_ISR for Lens like D160 */

	/*- (1) Tele/Wide Zoom lens */
	if (isTele) {
		lensZoomPosGo(ZOOM_TELE_POS);
		lensZoomStopWait(ZOOM_OP_BLOCKING);
	} else {
		lensZoomPosGo(ZOOM_WIDE_POS);
		lensZoomStopWait(ZOOM_OP_BLOCKING);
		/*-- Do backlash */
		lensZoomBacklash();
		lensZoomStopWait(ZOOM_OP_BLOCKING);
	}

	/*- (2) Focus to default pos @ current Zoom stop */
	lensFocusDefaultPosGo();
	//lensFocusStopWait();
	/*-- Do not need to wait focus, because lensFocusDefaultPosGo
	 * will call lensFocusJob which is blocking operation by itself.
	 */

	/*-- Complete LENS session */
	appLensDestroy();

	_appLensStopProc();

	lensStatusClear(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK);
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : taskProc_LensClose
 *------------------------------------------------------------------------*/
#if KIT_WITH_LENS
static
void
taskProc_LensClose(
	void
)
{
	UINT16 slipDefault;

	/*-- Create LENS session */
	appLensCreate();

	lensZoomPosUpdateEnable(DISABLE);

	lensZoomHomeISRDisable();
		/*-- Disable HOME_ISR for Lens like D160 */

	/*- (1) Close focus first */
	lensFocusJob(FOCUS_CLOSE_POS);

	/*- (2) Close up zoom */
	lensZoomSpeedSet(ZOOM_SPEED_FAST);
	lensZoomWideSlipCntSet(lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_CLOSE, ZOOM_SPEED_FAST));
	lensZoomPosGo(ZOOM_CLOSE_POS);
	lensZoomStopWait(ZOOM_OP_BLOCKING);

	/*- (3) Reset Slip and speed for next open */
	lensZoomSpeedSet(ZOOM_SPEED_FAST);
	slipDefault = lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_BOOT, ZOOM_SPEED_FAST);
	lensZoomTeleSlipCntSet(slipDefault);
	lensZoomWideSlipCntSet(slipDefault);

	/*-- Complete LENS session */
	appLensDestroy();

	lensStatusClear(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK);
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : taskProc_LensBoot
 *------------------------------------------------------------------------*/
#if KIT_WITH_LENS
static
void
taskProc_LensBoot(
	void
)
{
	BOOL isResetOK;
	UINT16 slipDefault;

	gIsLensEverBoot = TRUE;

	/*-- Create LENS session */
	appLensCreate();

	lensZoomPosUpdateEnable(DISABLE);

	lensZoomHomeISREnable();

	/*-- reset lens :
	 * to check & move lens to close state,if necessary
	 */
	isResetOK = TRUE;
	if (lensBootReset()==FAIL) {
		printf("\n\n%s: !!! FATAL !!!, BootReset failed !!\n\n", __FUNCTION__);
		isResetOK = FALSE;
	}

	/*-- open default :
	 * to move lens to open default pos (WidePos)
	 */
	if (isResetOK) {
		lensBootOpen();
	}

	/*-- Modify LENS motor speed and default slipCnt */
	lensZoomSpeedSet(ZOOM_SPEED_SLOW);
	slipDefault = lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_TELEWIDE, ZOOM_SPEED_SLOW);
	lensZoomTeleSlipCntSet(slipDefault);
	lensZoomWideSlipCntSet(slipDefault);
	slipDefault = lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_BLASH, ZOOM_SPEED_SLOW);
	lensZoomBacklashSlipCntSet(slipDefault);

	lensZoomHomeISRDisable();

	#ifdef EVB_CFG_COUGAR
	lensIrisLargeSet();
	LENS_SLEEP(100);
	appMShutOpen();
	LENS_SLEEP(100);
	#endif

	/*-- Complete LENS session */
	appLensDestroy();

	lensZoomPosUpdateEnable(ENABLE);

	lensStatusClear(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK);

	_appLensStopProc();

	profLogAdd(0, "*lens rdy");
}
#endif


/*-------------------------------------------------------------------------
 *  File Name : taskProc_LensOpen
 *------------------------------------------------------------------------*/
#if KIT_WITH_LENS
static
void
taskProc_LensOpen(
	void
)
{
	UINT16 slipDefault;

	/*-- Create LENS session */
	appLensCreate();

	lensZoomPosUpdateEnable(DISABLE);

	lensZoomHomeISREnable();

	/*-- open default :
	 * to move lens to open default pos (WidePos)
	 */
	lensBootOpen();

	/*-- Modify LENS motor speed and default slipCnt */
	lensZoomSpeedSet(ZOOM_SPEED_SLOW);
	slipDefault = lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_TELEWIDE, ZOOM_SPEED_SLOW);
	lensZoomTeleSlipCntSet(slipDefault);
	lensZoomWideSlipCntSet(slipDefault);
	slipDefault = lensZoomDefaultSlipGet(ZOOM_SLIP_SXN_BLASH, ZOOM_SPEED_SLOW);
	lensZoomBacklashSlipCntSet(slipDefault);

	/*-- Complete LENS session */
	appLensDestroy();

	lensZoomPosUpdateEnable(ENABLE);

	lensZoomHomeISRDisable();

	lensStatusClear(LENS_ST_MSK_MOTION_STATE);
	lensOsEventSet(&evLensAppDone, EVENT_LENS_APP_LENS_STOP_ACK);
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : taskProc_LensInit
 *------------------------------------------------------------------------*/
#if KIT_WITH_LENS
static
void
taskProc_LensInit(
	void
)
{
	lensStatusClear(LENS_ST_MSK_CLR_ALL);
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : lensAppTask
 *------------------------------------------------------------------------*/
void
lensAppTask(
	ULONG init_data
)
{
#if KIT_WITH_LENS
	UINT32 ret_flag=0;
	UINT32 wait_flag;

	wait_flag = EVENT_LENS_APP_TASK_INIT | \
				EVENT_LENS_APP_BOOT	| \
				EVENT_LENS_APP_OPEN	| \
				EVENT_LENS_APP_TELE | \
				EVENT_LENS_APP_WIDE | \
				EVENT_LENS_APP_CLOSE;

	while (1)
	{
		// wait for Focus event
		lensOsEventGet(&evLensApp, wait_flag, &ret_flag);

profLogAdd(0,"*$$lensApp");
		/* task init */
		if (ret_flag & EVENT_LENS_APP_TASK_INIT) {
			LENS_DBG("event : EVENT_LENS_APP_TASK_INIT\n");
			taskProc_LensInit();
			continue;
		}

		if (ret_flag & EVENT_LENS_APP_BOOT) {
			LENS_DBG("event : EVENT_LENS_APP_BOOT\n");
			taskProc_LensBoot();
			continue;
		}

		if (ret_flag & EVENT_LENS_APP_OPEN) {
			LENS_DBG("event : EVENT_LENS_APP_OPEN\n");
			taskProc_LensOpen();
			continue;
		}

		if (ret_flag & EVENT_LENS_APP_CLOSE) {
			LENS_DBG("event : EVENT_LENS_APP_CLOSE\n");
			taskProc_LensClose();
			continue;
		}

		if (ret_flag & EVENT_LENS_APP_TELE) {
			LENS_DBG("event : EVENT_LENS_APP_TELE\n");
			taskProc_LensTeleWide(1);
			continue;
		}

		if (ret_flag & EVENT_LENS_APP_WIDE) {
			LENS_DBG("event : EVENT_LENS_APP_WIDE\n");
			taskProc_LensTeleWide(0);
			continue;
		}

		LENS_SLEEP(1);
	}
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensAppTaskInit
 *------------------------------------------------------------------------*/
void
lensAppTaskInit(
	void
)
{
#if KIT_WITH_LENS
	/* Create & init task resources */
 	lensOsEventCreate(&evLensApp, "evLensApp");
 	lensOsEventCreate(&evLensAppDone, "evLensAppDone");
 	lensOsMutexCreate(&mtxLensApp, "mtxLensApp");
 	#if (LENS_BOOT_FOCUS_EARLY)
 	lensOsEventCreate(&evLensBootFocusEarly, "evLensBootFocusEarly");
 	#endif

	lensOsEventSet(&evLensApp, EVENT_LENS_APP_TASK_INIT);
	lensOsMutexPut(&mtxLensApp);

	/* Create Lens common control task */
	LENS_DBG("creating lensApp task\n");
	lensOsTaskCreate("lensApp-Task", lensAppTask, LENS_APP_TASK_PRIORITY, LENS_APP_TASK_TIME_SLICE);
#endif
}


