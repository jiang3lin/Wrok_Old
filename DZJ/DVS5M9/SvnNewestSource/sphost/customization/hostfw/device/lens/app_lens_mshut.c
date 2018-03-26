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
#include "middleware/proflog.h"
#include "sp5k_sensor_api.h"
#include "sp5k_global_api.h"
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_motor.h"
#include "app_lens_mshut.h"
#include "app_lens_api.h"
#include "app_ui_msg.h"
#include "app_calib_api.h"
#if KIT_LMI_CTRL
#include "api/sp5k_lmi_api.h"
#endif


#if KIT_WITH_MSHUT

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define MSHUT_MOTOR_ENGAGE_LAG		30	/* unit : ms */

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static BOOL gmshutIsBlocking = FALSE;
static BOOL gIsSendOpenMsg=FALSE;
static BOOL gIsMShutEngaged=FALSE;

#if MSH_GPIO_ISR_ENABLE
static UINT32 gmshDelaySet = 0;
#endif
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define mshutMotorPowerOn()		lensMotorPowerOn()
#define mshutMotorPowerOff()	lensMotorPowerOff()

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param);


/*-------------------------------------------------------------------------
 *  File Name : lensMShutLmiModeSet
 *------------------------------------------------------------------------*/
void
lensMShutLmiModeSet(
	UINT32 off
)
{

}


/*-------------------------------------------------------------------------
 *  File Name : lensMShutCBFIdleActuate
 *------------------------------------------------------------------------*/
static
void
lensMShutCBFIdleActuate(
	void
)
{
#if KIT_WITH_MSHUT
	mshutIdleActuate();
	lensMShutPowerOff();

	if (gIsSendOpenMsg) {
		LENS_MSG_POST(APP_UI_MSG_LENS_MSHUT_OPEN, 0);
		gIsSendOpenMsg = FALSE;
	}

	#if KIT_LMI_CTRL /*Edward 20090107*/
	lensMShutLmiModeSet(1); /* msh off */
	#endif
	gIsMShutEngaged=FALSE;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutCBFOpenActuate
 *------------------------------------------------------------------------*/
static
UINT32
lensMShutCBFOpenActuate(
	void
)
{
#if KIT_WITH_MSHUT
	if (gIsMShutEngaged==TRUE) {
		return FAIL;
	}
	lensMShutPowerOn();
	#if KIT_LMI_CTRL
	lensMShutLmiModeSet(0); /* msh on */
	#endif
	mshutOpenActuate();
	gIsMShutEngaged=TRUE;
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutCBFCloseActuate
 *------------------------------------------------------------------------*/
static
UINT32
lensMShutCBFCloseActuate(
	void
)
{
#if KIT_WITH_MSHUT
	if (gIsMShutEngaged==TRUE) {
		return FAIL;
	}
	lensMShutPowerOn();
	mshutCloseActuate();
	gIsMShutEngaged=TRUE;
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutSnapCloseCBF
 *------------------------------------------------------------------------*/
UINT32
lensMShutSnapCloseCBF(
	UINT32 V1,
	UINT32 V2,
	void* testp
)
{
	profLogAdd(0, "MShutSnapClose-s");

	if (FAIL==lensMotorSafeActuateQuery(MOTOR_ACTUATE_QUERY_MSHUT)) {
		LENS_LOG("\n >>>> %s: Motor driver busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==lensMShutCBFCloseActuate()) {
		LENS_DBG("\n*** %s: MShut busy, ignored!! \n\n", __FUNCTION__);
		return FAIL;
	}

	/*sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_FREX, 0);*/
	/*sp5kGpioWrite(0, 0x00000002, 0);*/


	if (gmshutIsBlocking==FALSE) {
		tmrWaitListInstall(2, MSHUT_MOTOR_ENGAGE_LAG*1000, lensMShutCBFIdleActuate, 0);
			/* unit : us */
	} else {
		LENS_SLEEP(30);
		lensMShutCBFIdleActuate();
	}
	profLogAdd(0, "MShutSnapClose-e");

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/
UINT32
lensMShutSnapOpenCBF(UINT32 V1, UINT32 V2, void* testp)
{
	profLogAdd(0, "MShutSnapOpen-s");

	#if MSH_GPIO_ISR_ENABLE
	sp5kGpioIntEnable(MSH_CLOSE_IRQ_ID, DISABLE);
	#endif

	if (FAIL==lensMotorSafeActuateQuery(MOTOR_ACTUATE_QUERY_MSHUT)) {
		LENS_LOG("\n >>>> %s: Motor driver busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==lensMShutCBFOpenActuate()) {
		LENS_DBG("\n*** %s: MShut busy, ignored!! \n\n", __FUNCTION__);
		return FAIL;
	}

	if (gmshutIsBlocking==FALSE) {
		tmrWaitListInstall(2, MSHUT_MOTOR_ENGAGE_LAG*1000, lensMShutCBFIdleActuate, 0);
		/* unit : us */
		gIsSendOpenMsg = TRUE;
	} else {
		LENS_SLEEP(30);
		lensMShutCBFIdleActuate();
		LENS_MSG_POST(APP_UI_MSG_LENS_MSHUT_OPEN, 0);
	}
	profLogAdd(0, "MShutSnapOpen-e");
	return SUCCESS;
}

#endif /* KIT_WITH_MSHUT */


/*-------------------------------------------------------------------------
 *  File Name : lensMShutConfig
 *------------------------------------------------------------------------*/
void
lensMShutInit(
	void
)
{
#if KIT_WITH_MSHUT

	LENS_VERBOSE("MSHUT : Init \n");
	/* For FastLensBoot, the Callback registration becomes too early to execute.
	 * So, move it to appLensMShutCallbackRegister().
	 * Power on flow will call appLensMShutCallbackRegister() at properiate time.
	 */
	#if 0
	/*-- Callback installation */
	sp5kSnapShutterCfgSet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, mshutSnapCloseCBF /*SnapShutterOffCb*/);
	sp5kSnapShutterCfgSet(SP5K_SNAP_SHUTTER_OPEN_READOUT_END, mshutSnapOpenCBF /*SnapShutterOnCb*/);

	/*-- Auto Dely configuration */
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, 3100);	/* unit : 10us */
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_OPEN_READOUT_END, 0, 8000); /* uint : 10us */
	#endif

	lensMShutOpModeSet(LENS_DO_NONE_BLOCKING);
	lensMShutOpen();
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appLensMShutCallbackRegister
 *------------------------------------------------------------------------*/
void
appLensMShutCallbackRegister(
	void
)
{
#if KIT_WITH_MSHUT
	#if 0
	appCalibData_t *pcalibData;
	UINT32 closeDelay;
	#endif

	/*-- Callback installation */
	sp5kSnapShutterCfgSet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, lensMShutSnapCloseCBF /*SnapShutterOffCb*/);
	sp5kSnapShutterCfgSet(SP5K_SNAP_SHUTTER_OPEN_FRAME_READOUT_END, lensMShutSnapOpenCBF /*SnapShutterOnCb*/);

#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutOpModeSet
 *------------------------------------------------------------------------*/
void
lensMShutOpModeSet(
	BOOL isBlocking
)
{
#if KIT_WITH_MSHUT
	LENS_VERBOSE("MShut : (%s) \n", (isBlocking ? "BLOCKING" : "NONE_BLOCKING"));

	gmshutIsBlocking = (!!isBlocking);
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : lensMShutPowerOn
 *------------------------------------------------------------------------*/
void
lensMShutPowerOn(
	void
)
{
#if KIT_WITH_MSHUT
	glensPwrIsMShutBusy = TRUE;
	mshutMotorPowerOn();
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutPowerOff
 *------------------------------------------------------------------------*/
void
lensMShutPowerOff(
	void
)
{
#if KIT_WITH_MSHUT
	glensPwrIsMShutBusy = FALSE;
	mshutMotorPowerOff();
#endif
}
/*-------------------------------------------------------------------------
 *  File Name : lensMShutOpenActuate
 *------------------------------------------------------------------------*/
UINT32 lensMShutOpenActuate(void)
{
/*s Modify by Aries 10/03/02*/
#if KIT_WITH_MSHUT
	#if KIT_LMI_CTRL
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	if (gIsMShutEngaged==TRUE) {
		return FAIL;
	}
	lensMShutLmiModeSet(0); /* msh on */
	mshutOpenActuate();
	gIsMShutEngaged=TRUE;
	return SUCCESS;
	#else
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	if (gIsMShutEngaged==TRUE) {
		return FAIL;
	}
	mshutOpenActuate();
	gIsMShutEngaged=TRUE;
	return SUCCESS;
	#endif
#else
	return SUCCESS;
#endif
/*e Modify by Aries 10/03/02*/
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutCloseActuate
 *------------------------------------------------------------------------*/
UINT32 lensMShutCloseActuate(void)
{
/*s Modify by Aries 10/03/02*/
#if KIT_WITH_MSHUT
	#if KIT_LMI_CTRL
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	if (gIsMShutEngaged==TRUE) {
		return FAIL;
	}
	lensMShutLmiModeSet(1); /* msh off */
	mshutCloseActuate();
	gIsMShutEngaged=TRUE;
	return SUCCESS;
	#else
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	if (gIsMShutEngaged==TRUE) {
		return FAIL;
	}
	mshutCloseActuate();
	gIsMShutEngaged=TRUE;
	return SUCCESS;
	#endif
#else
	return SUCCESS;
#endif
/*e Modify by Aries 10/03/02*/
}
/*-------------------------------------------------------------------------
 *  File Name : lensMShutIdleActuate
 *------------------------------------------------------------------------*/
void lensMShutIdleActuate(void)
{
#if KIT_WITH_MSHUT
	#if KIT_LMI_CTRL
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	mshutIdleActuate();
	gIsMShutEngaged=FALSE;
	#else
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	mshutIdleActuate();
	gIsMShutEngaged=FALSE;
	#endif
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : lensMShutOpen
 *------------------------------------------------------------------------*/
UINT32
lensMShutOpen(
	void
)
{
#if KIT_WITH_MSHUT
	LENS_DBG("\n >>>> %s \n\n", __FUNCTION__);
	if (FAIL==lensMotorSafeActuateQuery(MOTOR_ACTUATE_QUERY_MSHUT)) {
		LENS_LOG("\n >>>> %s: Motor driver busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==lensMShutOpenActuate()) {
		LENS_DBG("\n >>>> %s: MSHUT Busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (gmshutIsBlocking==FALSE) {
		tmrWaitListInstall(2, MSHUT_MOTOR_ENGAGE_LAG*1000, lensMShutIdleActuate, 0);
		/* unit : us */
	} else {
		LENS_SLEEP(30);
		lensMShutIdleActuate();
	}
	return SUCCESS;
#else
	_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_IRIS_N), 0);
	_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_SHUTTER_P), 1);

	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutClose
 *------------------------------------------------------------------------*/
UINT32
lensMShutClose(
	void
)
{
#if KIT_WITH_MSHUT
	LENS_DBG("\n >>>> %s \n\n", __FUNCTION__);
	if (FAIL==lensMotorSafeActuateQuery(MOTOR_ACTUATE_QUERY_MSHUT)) {
		LENS_LOG("\n >>>> %s: Motor driver busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==lensMShutCloseActuate()) {
		LENS_LOG("\n >>>> %s: MSHUT Busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (gmshutIsBlocking==FALSE) {
		tmrWaitListInstall(2, MSHUT_MOTOR_ENGAGE_LAG*1000, lensMShutIdleActuate, 0);
			/* unit : us */
	} else {
		LENS_SLEEP(30);
		lensMShutIdleActuate();
	}

	return SUCCESS;
#else

	return SUCCESS;
#endif
}
/*-------------------------------------------------------------------------
 *  File Name : lensMShutBusyQuery
 *------------------------------------------------------------------------*/
BOOL lensMShutBusyQuery(void)
{
#if KIT_WITH_MSHUT
	return gIsMShutEngaged;
#else
	return FALSE;
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutSnapCloseDelaySet
 *------------------------------------------------------------------------*/
void
lensMShutSnapCloseDelaySet(
	UINT32 delay /* unit : 10us */
)
{
#if KIT_WITH_MSHUT
	sp5kSnapShutterDelaySet(SP5K_SNAP_SHUTTER_CLOSE_EXP_END, 0, delay);
		/* unit : 10us */
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensMShutLMITrig
 *------------------------------------------------------------------------*/
void
lensMShutLMITrig(
	void
)
{
#if KIT_LMI_CTRL
	LENS_DBG("\n >>>> %s \n\n", __FUNCTION__);

	mshutOpenActuate();
	gIsMShutEngaged=TRUE;
	lensMShutIdleActuate();
#endif
}

#if MSH_GPIO_ISR_ENABLE
void
lensMshutCloseISRCBF(
	void
)
{
	profLogAdd(0,"msh close");
	lensMShutSnapCloseCBF(0,0,NULL);
}

void
lensMShutCloseISR(
	void
)
{
	profLogPrintf(0,"msh Isr : %d",gmshDelaySet);
	sp5kTimerUsOneShotReg(0, gmshDelaySet*10,lensMshutCloseISRCBF);
}
#endif

UINT32
appLensMShutDelaySet(
	mshutDelayMode_t mode,
	UINT32 delay	/* 10usec */
)
{
    return FALSE;       //Remove warning Lation@20121120
}
/*-------------------------------------------------------------------------
 *  File Name : appLensMShutLmiModeSet
 *------------------------------------------------------------------------*/
void
appLensMShutLmiModeSet(
	UINT32 off
)
{
#if KIT_WITH_MSHUT
	lensMShutLmiModeSet(off);
#endif
}
/*-------------------------------------------------------------------------
 *  DEBUG FUNCTIONS : Export for cmdLine debug                            *
 -------------------------------------------------------------------------*/
#if MSHUT_DBG_FUNCTION_EXPORT

#endif /* MSH_DBG_FUNCTION_EXPORT */


