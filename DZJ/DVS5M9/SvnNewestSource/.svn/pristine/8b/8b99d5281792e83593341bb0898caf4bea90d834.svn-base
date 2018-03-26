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
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_motor.h"
#include "app_lens_zoom.h"
#include "app_lens_focus.h"
#include "app_lens_api.h"
#include "app_view_param.h"
#include "app_aaa.h"
#include "app_ui_para.h"

#define FOCUS_CONTROL_MOTOR_LED_POWER		0

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define EVENT_FOCUS_DONE		((ULONG)1)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct focusCtrlBlock_s {
//	UINT8						motorCmdIdx;
//	TYPE_FOCUS_MOTOR_CMD		motorCmd; /* UINT8 */

	TYPE_FOCUS_POS				curPos; /* INT16 */
} focusCtrlBlock_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if KIT_WITH_FOCUS
static TX_EVENT_FLAGS_GROUP evFocusCtrl; 	/* Focus op control event */
static TX_EVENT_FLAGS_GROUP evFocusCtrlAck; /* ACK for Focus NEAR/FAR ctrl */
static TX_EVENT_FLAGS_GROUP evFocusDone;	/* Focus op Done event */
static TX_MUTEX mtxFocusCtrl;

static focusCtrlBlock_t gfocusCb={.curPos=FOCUS_CLOSE_POS};

static UINT16 gfocusStepsCnt=0; /* store the steps to move */

static BOOL gIsFocusMoving=FALSE;
#endif
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if KIT_WITH_FOCUS
static void taskProc_FocusMoving(UINT8	focusDir,	UINT16	stepCnt);
#endif
/*-------------------------------------------------------------------------
 *  File Name : focusStepsGo
 *------------------------------------------------------------------------*/
#if KIT_WITH_FOCUS
static
void
focusStepsGo(
	UINT8	focusDir,
	UINT16	stepCnt
)
{
	void (*funcMotorNext)(void);

	LENS_VERBOSE("%s: dir(%d), stepCnt(%d) \n", __FUNCTION__, focusDir, stepCnt);

	if (stepCnt==0 || stepCnt>FOCUS_MOVE_STEPS_MAX) {
		printf("\n%s: !! ERROR !!, Illegal stepCnt(%d) !\n\n", __FUNCTION__, stepCnt);
		return;
	}

	if (!(focusDir==FOCUS_DIR_FAR || focusDir==FOCUS_DIR_NEAR)) {
		printf("\n%s: !! ERROR !!, Illegal focusDir(%d) !\n\n", __FUNCTION__, focusDir);
		return;
	}

	if (focusDir==FOCUS_DIR_FAR) {
		LENS_DBG("%s: focusMotorFarNext (%d)\n", __FUNCTION__, stepCnt);
		funcMotorNext = focusMotorFarNext;
	} else {
		LENS_DBG("%s: focusMotorNearNext (%d)\n", __FUNCTION__, stepCnt);
		funcMotorNext = focusMotorNearNext;
	}

	gIsFocusMoving = TRUE;

	do {
		(*funcMotorNext)();
		/*-- Update current pos */
		gfocusCb.curPos = (focusDir==FOCUS_DIR_FAR) ? (gfocusCb.curPos-1) : (gfocusCb.curPos+1);
		stepCnt--;
	} while(stepCnt);

	gIsFocusMoving = FALSE;

	LENS_DBG("%s: finished @ curPos(%d) \n", __FUNCTION__, gfocusCb.curPos);
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : lensFocusCreate
 *------------------------------------------------------------------------*/
void
lensFocusCreate(
	void
)
{
#if KIT_WITH_FOCUS
	/*--
	 * Considering accompany with ZOOM,
	 * we request the lens power shall be controlled by caller.
	 * --> dont power-off lens in Focus session !!
	 */

	#if FOCUS_CONTROL_MOTOR_LED_POWER
	/*-- Power on lens motor, just in case */
	lensMotorPowerOn();
	lensLedPowerOn(LENS_LED_ID_FHM);
	#endif

	/*-- To excite focus motor from inertia state */
	focusInertiaExcite();

	LENS_DBG("%s: curPos(%d) \n", __FUNCTION__, gfocusCb.curPos);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusDestroy
 *------------------------------------------------------------------------*/
void
lensFocusDestroy(
	void
)
{
#if KIT_WITH_FOCUS
	/*--
	 * Considering accompany with ZOOM,
	 * we request the lens power shall be controlled by caller.
	 * --> dont power-off lens in Focus session !!
	 */

	/*-- To hold focus state before release */
	focusReleaseHold();

	#if FOCUS_CONTROL_MOTOR_LED_POWER
	/*-- Power off lens motor, just in case */
	lensMotorPowerOff();
	lensLedPowerOff(LENS_LED_ID_FHM);
	#endif

	LENS_DBG("%s: curPos(%d) \n", __FUNCTION__, gfocusCb.curPos);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : focusNearTrig
 *------------------------------------------------------------------------*/
#if KIT_WITH_FOCUS
static
void
focusNearTrig(
	void
)
{
	ULONG waitEvent;

	gIsFocusMoving = TRUE;
	lensOsEventCheck(&evFocusCtrlAck, EVENT_FOCUS_CTRL_ACK, &waitEvent);
		/*-- To clear evFocusCtrlAck before submit FOCUS command */
	lensOsEventSet(&evFocusCtrl, EVENT_FOCUS_NEAR_END_CTRL);
	lensOsEventGet(&evFocusCtrlAck, EVENT_FOCUS_CTRL_ACK, &waitEvent);
}

/*-------------------------------------------------------------------------
 *  File Name : focusFarTrig
 *------------------------------------------------------------------------*/
static
void
focusFarTrig(
	void
)
{
	ULONG waitEvent;

	gIsFocusMoving = TRUE;
	lensOsEventCheck(&evFocusCtrlAck, EVENT_FOCUS_CTRL_ACK, &waitEvent);
		/*-- To clear evFocusCtrlAck before submit FOCUS command */
	lensOsEventSet(&evFocusCtrl, EVENT_FOCUS_FAR_END_CTRL);
	lensOsEventGet(&evFocusCtrlAck, EVENT_FOCUS_CTRL_ACK, &waitEvent);
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : lensFocusCrashRecover
 *------------------------------------------------------------------------*/
UINT32
lensFocusCrashRecover(
	void
)
{
#if KIT_WITH_FOCUS
	UINT16 scanCnt, scanMax, i;

	LENS_VERBOSE("\n%s:... start\n\n", __FUNCTION__);

	/*-- Inertial Excitation */
	focusMotorReset();
	lensFocusCreate();

	/*-- Scan HOME to initial focus pos */
	if (!IS_LENS_FOCUS_HOME) {
		/*-- focus is at open (near) end
		 * (1) Heading far end to hunt the HOME reverse.
		 * (2) Heading near end to hunt the HOME reverse.
		 */
		LENS_DBG("%s: FCS @ Out !! \n", __FUNCTION__);
		scanCnt=0;
		scanMax=FOCUS_MOVE_STEPS_MAX;
		do {
			focusMotorFarNext();
			scanCnt++;
			if (scanCnt>scanMax) {
				break;
			}
		} while(!IS_LENS_FOCUS_HOME);

		if (scanCnt>scanMax) {
			printf("%s: scan failed (1)\n", __FUNCTION__);
			return FAIL;
		}

		/* move more steps to confirm Focus Lens into CLOSE */
		scanMax = 5;
		for (i=0; i<scanMax; i++) {
			focusMotorFarNext();
		}
		gfocusCb.curPos = FOCUS_HP_POS-scanMax;
	}

	lensFocusDestroy();
	LENS_DBG("\n%s ... exit, curPos(%d), cmdIdx(%d), cmd(0x%x)\n\n",
				__FUNCTION__,
				gfocusCb.curPos,
				focusMotorCmdIdxGet(),
				focusMotorCmdGet());

#endif
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : lensFocusResetBoot
 *------------------------------------------------------------------------*/
#if KIT_WITH_FOCUS
static
UINT32
lensFocusResetBoot(
	BOOL isBoot2Default
)
{

	UINT16 scanCnt, scanMax, i;

	LENS_VERBOSE("\n%s: %d ... start\n\n", __FUNCTION__, isBoot2Default);

	/*-- Inertial Excitation */
	focusMotorReset(); //gfocusCb.motorCmdIdx = 0;
	lensFocusCreate();

	/*-- Scan HOME to initial focus pos */
	if (!IS_LENS_FOCUS_HOME) {
		/*-- focus is at open (near) end
		 * (1) Heading far end to hunt the HOME reverse.
		 * (2) Heading near end to hunt the HOME reverse.
		 */
		LENS_DBG("%s: FCS Out Of HOME !! \n", __FUNCTION__);
		scanCnt=0;
		scanMax=FOCUS_MOVE_STEPS_MAX;
		do {
			focusMotorFarNext();
			scanCnt++;
			if (scanCnt>scanMax) {
				break;
			}
		} while(!IS_LENS_FOCUS_HOME);

		if (scanCnt>scanMax) {
			printf("%s: scan failed (1)\n", __FUNCTION__);
			return FAIL;
		}

		gfocusCb.curPos = FOCUS_HP_POS;
		/* move more steps for backlash */
		for (i=0; i<FOCUS_BACKLASH_CNT; i++) {
			focusMotorFarNext();
		}

	}

	/*-- focus is at close (far) end
	 * Heading near end to hunt the HOME reverse.
	 */
	LENS_DBG("%s: FCS @ HOME !! \n", __FUNCTION__);
	scanCnt=0;
	scanMax=FOCUS_HP_POS-FOCUS_CLOSE_POS+300;

	LENS_VERBOSE("%s: scanMax(%d) !! \n", __FUNCTION__, scanMax);
	do {
		focusMotorNearNext();
		scanCnt++;
		if (scanCnt>scanMax) {
			break;
		}
	} while(IS_LENS_FOCUS_HOME);

	if (scanCnt>scanMax) {
		printf("%s: scan failed(2), scanCnt(%d) \n", __FUNCTION__, scanCnt);
		return FAIL;
	}
	LENS_VERBOSE("%s: FHome found @ scanCnt(%d) !! \n", __FUNCTION__, scanCnt);

	gfocusCb.curPos = FOCUS_HP_POS + 1;

	if (isBoot2Default) {
		lensFocusPosGo(FOCUS_BOOT_POS);
		lensFocusStopWait();
		/*-- alex, 0125
		 * change lensFocusPosGo() to non-blocking to
		 * consist with lensZoomPosGo().
		 * Now, caller has to wait focus completion with
		 * lensFocusStopWait by itself.
		 */
	}

	lensFocusDestroy();
	LENS_DBG("\n%s ... exit, curPos(%d), cmdIdx(%d), cmd(0x%x)\n\n", __FUNCTION__, gfocusCb.curPos, focusMotorCmdIdxGet(), focusMotorCmdGet());

	return SUCCESS;
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : lensFocusReset
 *------------------------------------------------------------------------*/
UINT32
lensFocusReset(
	void
)
{
#if KIT_WITH_FOCUS
	LENS_DBG("%s \n", __FUNCTION__);
	return lensFocusResetBoot(FALSE);
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusQuery
 *------------------------------------------------------------------------*/
UINT32
lensFocusQuery(
	UINT32 queryId
)
{
#if KIT_WITH_FOCUS
	UINT32 ret;

	switch (queryId) {
	default :
		LENS_DBG("\n%s: !! ERROR !!, illegal id(%d) \n\n", __FUNCTION__, queryId);
		ret = 0xFFFFFFFF;
		break;
	case FOCUS_QUERY_ID_MOTION_STATE:
		ret = (gIsFocusMoving) ? 1 : 0;
		break;
	case FOCUS_QUERY_CURRENT_POS:
		ret = (UINT32)gfocusCb.curPos;
		break;
	case FOCUS_QUERY_ID_HOME_POS :
		ret = (UINT32)FOCUS_HP_POS;
		break;
	case FOCUS_QUERY_ID_IS_IO_HOME:
		ret = (UINT32)IS_LENS_FOCUS_HOME;
		break;
	}
	return ret;
#else
	return 0;
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : lensFocusBoot
 *------------------------------------------------------------------------*/
UINT32
lensFocusBoot(
	void
)
{
#if KIT_WITH_FOCUS
	LENS_DBG("%s \n", __FUNCTION__);
	return lensFocusResetBoot(TRUE);
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusStopWait
 *------------------------------------------------------------------------*/
void
lensFocusStopWait(
	void
)
{
#if KIT_WITH_FOCUS
	ULONG waitEvent;

	LENS_DBG("%s \n", __FUNCTION__);
	/*--
	 * if Focus has stopped, return is ZERO.
	 */
	if (gIsFocusMoving) {
		LENS_DBG("%s: focus moving... wait event \n", __FUNCTION__);
		lensOsEventGet(&evFocusDone, EVENT_FOCUS_DONE, &waitEvent);
	} else {
		/*-- Test and Clear Event !! */
		LENS_DBG("%s: focus standby... clear event \n", __FUNCTION__);
		lensOsEventCheck(&evFocusDone, EVENT_FOCUS_DONE, &waitEvent);
	}

	LENS_DBG("%s exit \n", __FUNCTION__);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusPosGo
 *------------------------------------------------------------------------*/
TYPE_FOCUS_POS
lensFocusDefaultPosGet(
	void
)
{
#if KIT_WITH_FOCUS
	uiPara_t* puiPara = appUiParaGet();

	return ((TYPE_FOCUS_POS) appFocusInitPosGet(puiPara->af));
#else
	return 0;
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : lensFocusPosGo
 *------------------------------------------------------------------------*/
UINT32
lensFocusDefaultPosGo(
	void
)
{
#if KIT_WITH_FOCUS
	TYPE_FOCUS_POS defFPos;
	UINT32 err;

	LENS_DBG("%s \n", __FUNCTION__);

	defFPos = lensFocusDefaultPosGet();
	err = lensFocusJob(defFPos);
	return err;
#else
	return SUCCESS;
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : lensFocusPosGo
 *------------------------------------------------------------------------*/
UINT32
lensFocusPosGo(
	TYPE_FOCUS_POS tgtPos
)
{
#if KIT_WITH_FOCUS
	LENS_DBG("%s: tgtPos(%d), curPos(%d) \n", __FUNCTION__, tgtPos, gfocusCb.curPos);

	if (gIsFocusMoving) {
		LENS_LOG("\n\n\n >>>>>>>> FOCUS Busy, pos(%d) !! \n\n\n", tgtPos);
	}

	if (tgtPos==gfocusCb.curPos) {
		gfocusStepsCnt = 0;
		lensOsEventSet(&evFocusDone, EVENT_FOCUS_DONE);
		return SUCCESS;
	}

	if (tgtPos>gfocusCb.curPos) {
		/*-- Going Near end */
		LENS_DBG("%s: Near end\n", __FUNCTION__);

		/*-- Do Near */
		gfocusStepsCnt = tgtPos - gfocusCb.curPos;
		focusNearTrig();
		//lensFocusStopWait();
		/*-- alex, 0125
		 * change lensFocusPosGo() to non-blocking to
		 * consist with lensZoomPosGo().
		 * Now, caller has to wait focus completion with
		 * lensFocusStopWait by itself.
		 */
	} else {
		/*-- Going Far end, need backlash */
		LENS_DBG("%s: Far end\n", __FUNCTION__);
		#if 0 /* original */
		/*-- Do Far(FAR+BACKLASH) */
		if (tgtPos<FOCUS_CLOSE_POS+FOCUS_BACKLASH_CNT) {
			gfocusStepsCnt = (gfocusCb.curPos - FOCUS_CLOSE_POS + 5);
		} else {
			gfocusStepsCnt = FOCUS_BACKLASH_CNT + (gfocusCb.curPos - tgtPos);
		}

		focusFarTrig();
		lensFocusStopWait();

		/*-- Do Near(BACKLASH) */
		gfocusStepsCnt = FOCUS_BACKLASH_CNT;
		focusNearTrig(); //lensOsEventSet(&evFocusCtrl, EVENT_FOCUS_NEAR_END_CTRL);
		//lensFocusStopWait();
		/*-- alex, 0125
		 * change lensFocusPosGo() to non-blocking to
		 * consist with lensZoomPosGo().
		 * Now, caller has to wait focus completion with
		 * lensFocusStopWait by itself.
		 */
		#else
		/*-- Do Far(FAR+BACKLASH) */
		if (tgtPos<=FOCUS_PARK_POS) {
			/*-- going to park, skip backlash */
			gfocusStepsCnt = gfocusCb.curPos - FOCUS_PARK_POS;
			focusFarTrig();
			/*-- since we dont do backlash, do not wait stop here !!
			 *   this function is non-blocking.
			 *   see explanation below.
			 */
		} else if (tgtPos<=FOCUS_CLOSE_POS) {
			/*-- between CLOSE & PARK, just retrack ?, do backlash */
			gfocusStepsCnt = gfocusCb.curPos - FOCUS_PARK_POS;
			focusFarTrig();
			lensFocusStopWait();

			/*-- Do Near(BACKLASH) to CLOSE pos */
			gfocusStepsCnt = FOCUS_CLOSE_POS - gfocusCb.curPos;
			focusNearTrig();
			//lensOsEventSet(&evFocusCtrl, EVENT_FOCUS_NEAR_END_CTRL);
			//lensFocusStopWait();
			/*-- alex, 0125
			 * change lensFocusPosGo() to non-blocking to
			 * consist with lensZoomPosGo().
			 * Now, caller has to wait focus completion with
			 * lensFocusStopWait by itself.
			 */
		} else {
			gfocusStepsCnt = FOCUS_BACKLASH_CNT + (gfocusCb.curPos - tgtPos);
			focusFarTrig();
			lensFocusStopWait();

			/*-- Do Near(BACKLASH) */
			gfocusStepsCnt = FOCUS_BACKLASH_CNT;
			focusNearTrig(); //lensOsEventSet(&evFocusCtrl, EVENT_FOCUS_NEAR_END_CTRL);
			//lensFocusStopWait();
			/*-- alex, 0125
			 * change lensFocusPosGo() to non-blocking to
			 * consist with lensZoomPosGo().
			 * Now, caller has to wait focus completion with
			 * lensFocusStopWait by itself.
			 */
		}
		#endif
	}
#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusJob
 *------------------------------------------------------------------------*/
UINT32
lensFocusJob(
	SINT16 pos
)
{
#if KIT_WITH_FOCUS

	UINT32 err;

	#if FOCUS_CONTROL_MOTOR_LED_POWER
	lensMotorPowerOn();
	lensLedPowerOn(LENS_LED_ID_FHM);
	#endif

	lensFocusCreate();

	err = lensFocusPosGo(pos);
	lensFocusStopWait();
	/*-- alex, 0125
	 * change lensFocusPosGo() to non-blocking to
	 * consist with lensZoomPosGo().
	 * Now, caller has to wait focus completion with
	 * lensFocusStopWait() by itself.
	 */

	lensFocusDestroy();

	return err;
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusTask
 *------------------------------------------------------------------------*/
void
lensFocusTask(
	ULONG init_data
)
{
#if KIT_WITH_FOCUS
	UINT32 ret_flag=0;
	UINT32 wait_flag;

	wait_flag = EVENT_FOCUS_TASK_INIT | \
				EVENT_FOCUS_NEAR_END_CTRL | \
				EVENT_FOCUS_FAR_END_CTRL;

	while (1)
	{
		// wait for Focus event
		lensOsEventGet(&evFocusCtrl, wait_flag, &ret_flag);

		/* task init */
		if (ret_flag & EVENT_FOCUS_TASK_INIT) {
			LENS_DBG("event : EVENT_FOCUS_TASK_INIT\n");
			continue;
		}

		/* handle focus moving */
		if (ret_flag & EVENT_FOCUS_NEAR_END_CTRL) {
			LENS_DBG("event : EVENT_FOCUS_NEAR_END_CTRL\n");
			taskProc_FocusMoving(FOCUS_DIR_NEAR, gfocusStepsCnt);
		} else if (ret_flag & EVENT_FOCUS_FAR_END_CTRL) {
			LENS_DBG("event : EVENT_FOCUS_FAR_END_CTRL\n");
			taskProc_FocusMoving(FOCUS_DIR_FAR, gfocusStepsCnt);
		}
	}
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusTaskInit
 *------------------------------------------------------------------------*/
void
lensFocusTaskInit(
	void
)
{
#if KIT_WITH_FOCUS
	/* Create & init task resources */
 	lensOsEventCreate(&evFocusCtrl, "evFocusCtrl");
 	lensOsEventCreate(&evFocusDone, "evFocusDone");
 	lensOsEventCreate(&evFocusCtrlAck, "evFocusCtrlAck");
 	lensOsMutexCreate(&mtxFocusCtrl, "mtxFocusCtrl");

	lensOsEventSet(&evFocusCtrl, EVENT_FOCUS_TASK_INIT);
	lensOsMutexPut(&mtxFocusCtrl);

	//InitShutterDelay();

	/* Create Lens common control task */
	LENS_DBG("creating Focus task\n");
	lensOsTaskCreate("Focus-Task", lensFocusTask, LENS_FOCUS_TASK_PRIORITY, LENS_FOCUS_TASK_TIME_SLICE);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensFocusTaskInit
 *------------------------------------------------------------------------*/
#if KIT_WITH_FOCUS
static
void
taskProc_FocusMoving(
	UINT8	focusDir,
	UINT16	stepCnt
)
{
	ULONG waitEvent;

	/*--
	 * Focus could be control by both AF and Host tasks.
	 * Shall guarantee exclusive operation.
	 */
	lensOsMutexGet(&mtxFocusCtrl);

	lensOsEventCheck(&evFocusDone, EVENT_FOCUS_DONE, &waitEvent);
			/*-- To clear evFocusDone event */

	focusStepsGo(focusDir, stepCnt);

	lensOsEventSet(&evFocusCtrlAck, EVENT_FOCUS_CTRL_ACK);
			/*-- To acknowledge the FOCUS request */

	lensOsEventSet(&evFocusDone, EVENT_FOCUS_DONE);
			/*-- To notify the completion of FOCUS request */

	lensOsMutexGet(&mtxFocusCtrl);
}
#endif
void cmdFocus(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
#if KIT_WITH_FOCUS
	if (argc==1) {
		printf("focus usage:\n"
			" focus xxx : move foucus lens to xxx position \n"
			" focus far nnn : go far end nnn steps \n"
			" focus near mmm : go near end mmm steps \n"
			" focus info : report focus lens information \n"
		);
	} else if (strcmp(arg[1], "info")==0) {
		printf("focus info : \n");
		printf("curr pos : %d \n", gfocusCb.curPos);
		printf("\n");
	} else if (strcmp(arg[1], "far")==0) {
		printf("focus far %d\n",v[2]);
		/*-- Power on lens motor */
		lensMotorPowerOn();
		lensLedPowerOn(LENS_LED_ID_FHM);

		lensFocusJob(gfocusCb.curPos-v[2]);

		/*-- Power off lens motor */
		lensMotorPowerOff();
		lensLedPowerOff(LENS_LED_ID_FHM);
		printf("current pos : %d \n", gfocusCb.curPos);
	} else if (strcmp(arg[1], "near")==0) {
		printf("focus near %d\n",v[2]);
		/*-- Power on lens motor */
		lensMotorPowerOn();
		lensLedPowerOn(LENS_LED_ID_FHM);

		lensFocusJob(gfocusCb.curPos+v[2]);

		/*-- Power off lens motor */
		lensMotorPowerOff();
		lensLedPowerOff(LENS_LED_ID_FHM);
		printf("current pos : %d \n", gfocusCb.curPos);
	}else {
		printf("focus [%d] --> [%d] \n", gfocusCb.curPos, v[1]);

		/*-- Power on lens motor */
		lensMotorPowerOn();
		lensLedPowerOn(LENS_LED_ID_FHM);

		lensFocusJob(v[1]);

		/*-- Power off lens motor */
		lensMotorPowerOff();
		lensLedPowerOff(LENS_LED_ID_FHM);
		printf("current pos : %d \n", gfocusCb.curPos);
	}
#else
	DBG_PRINT("Sorry, Focus not supported !! \n");
#endif
}

