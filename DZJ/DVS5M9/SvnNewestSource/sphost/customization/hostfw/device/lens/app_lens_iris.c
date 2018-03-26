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
#include "sp5k_sensor_api.h"
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_motor.h"
#include "app_lens_iris.h"
#include "app_lens_api.h"
#if KIT_LMI_CTRL
#include "api/sp5k_lmi_api.h" 
#endif


#if KIT_WITH_IRIS

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define IRIS_MOTOR_ENGAGE_LAG		30	/* unit : ms */

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static BOOL girisIsBlocking = FALSE;
static UINT8 girisStepPos = IRIS_STOP_LARGE;
static BOOL gIsIrisEngaged=FALSE;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define irisMotorPowerOn()		lensMotorPowerOn()
#define irisMotorPowerOff()		lensMotorPowerOff()

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param);


/*-------------------------------------------------------------------------
 *  File Name : lensIrisPowerOn
 *------------------------------------------------------------------------*/
void
lensIrisPowerOn(
	void
)
{
	LENS_DBG("%s \n\n", __FUNCTION__);
	glensPwrIsIrisBusy = TRUE;
	irisMotorPowerOn();
}

/*-------------------------------------------------------------------------
 *  File Name : lensIrisPowerOff
 *------------------------------------------------------------------------*/
void
lensIrisPowerOff(
	void
)
{
	LENS_DBG("%s \n\n", __FUNCTION__);
	glensPwrIsIrisBusy = FALSE;
	irisMotorPowerOff();
}

/*-------------------------------------------------------------------------
 *  File Name : lensIrisOpModeSet
 *------------------------------------------------------------------------*/
void
lensIrisOpModeSet(
	BOOL isBlocking
)
{
	LENS_VERBOSE("IRIS : (%s) \n", (isBlocking ? "BLOCKING" : "NONE_BLOCKING"));

	girisIsBlocking = (!!isBlocking);
}

/*-------------------------------------------------------------------------
 *  File Name : lensIrisSegPosQuery
 *------------------------------------------------------------------------*/
UINT32
lensIrisSegPosQuery(
	void
)
{
	return (UINT32)girisStepPos;
}

/*-------------------------------------------------------------------------
 *  File Name : lensIrisLmiModeSet
 *------------------------------------------------------------------------*/
void
lensIrisLmiModeSet(
	UINT32 Status
)
{

}


/*-------------------------------------------------------------------------
 *  File Name : lensIrisInit
 *------------------------------------------------------------------------*/
void
lensIrisInit(
	void
)
{
	LENS_VERBOSE("IRIS : Init \n");

	lensIrisOpModeSet(LENS_DO_NONE_BLOCKING);
	lensIrisLargeSet();
}
/*-------------------------------------------------------------------------
 *  File Name : lensIrisLargeActuate
 *------------------------------------------------------------------------*/
static
UINT32
lensIrisLargeActuate(
	void
)
{
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	#if KIT_LMI_CTRL
	if (gIsIrisEngaged==TRUE) {
		return FAIL;
	}
	gIsIrisEngaged=TRUE;
	lensIrisLmiModeSet(1);
	irisLargeActuate();
	return SUCCESS;
	#else
	if (gIsIrisEngaged==TRUE) {
		return FAIL;
	}
	gIsIrisEngaged=TRUE;

	lensIrisPowerOn();
	irisLargeActuate();

	return SUCCESS;
	#endif

}
/*-------------------------------------------------------------------------
 *  File Name : lensIrisSmallActuate
 *------------------------------------------------------------------------*/
static
UINT32
lensIrisSmallActuate(
	void
)
{
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	#if KIT_LMI_CTRL
	if (gIsIrisEngaged==TRUE) {
		return FAIL;
	}
	gIsIrisEngaged=TRUE;
	lensIrisLmiModeSet(0);
	irisSmallActuate();
	return SUCCESS;
	#else
	if (gIsIrisEngaged==TRUE) {
		return FAIL;
	}
	gIsIrisEngaged=TRUE;

	lensIrisPowerOn();
	irisSmallActuate();
	return SUCCESS;
	#endif
}
/*-------------------------------------------------------------------------
 *  File Name : lensIrisIdleActuate
 *------------------------------------------------------------------------*/
void
lensIrisIdleActuate(
	void
)
{
	LENS_DBG("\n%s ..\n\n", __FUNCTION__);
	#if KIT_LMI_CTRL
	gIsIrisEngaged=FALSE;
	irisIdleActuate();
	#else
	irisIdleActuate();
	lensIrisPowerOff();
	gIsIrisEngaged=FALSE;
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : lensIrisLargeSet
 *------------------------------------------------------------------------*/
UINT32
lensIrisLargeSet(
	void
)
{
	LENS_DBG("\n >>>> %s \n\n", __FUNCTION__);

	if (FAIL==lensMotorSafeActuateQuery(MOTOR_ACTUATE_QUERY_IRIS)) {
		LENS_LOG("\n >>>> %s: Motor driver busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==lensIrisLargeActuate()) {
		LENS_LOG("\n****%s: IRIS busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (girisIsBlocking==FALSE) {
		tmrWaitListInstall(2, IRIS_MOTOR_ENGAGE_LAG*1000, lensIrisIdleActuate, 0);
		/* unit : us */
	} else {
		LENS_SLEEP(IRIS_MOTOR_ENGAGE_LAG);
		lensIrisIdleActuate();
	}
	girisStepPos = IRIS_STOP_LARGE;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : lensIrisSmallSet
 *------------------------------------------------------------------------*/
UINT32
lensIrisSmallSet(
	void
)
{
	LENS_DBG("\n >>>> %s \n\n", __FUNCTION__);

	if (FAIL==lensMotorSafeActuateQuery(MOTOR_ACTUATE_QUERY_IRIS)) {
		LENS_LOG("\n >>>> %s: Motor driver busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==lensIrisSmallActuate()) {
		LENS_LOG("\n****%s: IRIS busy, ignored !!\n\n", __FUNCTION__);
		return FAIL;
	}

	if (girisIsBlocking==FALSE) {
		tmrWaitListInstall(2, IRIS_MOTOR_ENGAGE_LAG*1000, lensIrisIdleActuate, 0);
			/* unit : us */
	} else {
		LENS_SLEEP(IRIS_MOTOR_ENGAGE_LAG);
		lensIrisIdleActuate();
	}
	girisStepPos = IRIS_STOP_SMALL;

	return SUCCESS;
}
/*-------------------------------------------------------------------------
 *  File Name : lensIrisBusyQuery
 *------------------------------------------------------------------------*/
BOOL lensIrisBusyQuery(void)
{
	return gIsIrisEngaged;
}

/*-------------------------------------------------------------------------
 *  DEBUG FUNCTIONS : Export for cmdLine debug                            *
 -------------------------------------------------------------------------*/
#if IRIS_DBG_FUNCTION_EXPORT

#endif /* MSH_DBG_FUNCTION_EXPORT */

#endif /* KIT_WITH_IRIS */

