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
#include "app_com_def.h"
#include "app_com_api.h"

#include "app_strobe.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define STROBE_RDY_POLLING_TRUE_IO		1

#define STROBE_CHG_CHK_INTERVAL		40 /* unit: ms */
#define STROBE_CHG_CHK_TICK_CNT		(STROBE_CHG_CHK_INTERVAL/appTimerIsrTickIntervalGet())

#define STROBE_CHG_TIMEOUT_TIME			25 /* unit : sec */
#define STROBE_CHG_TIMEOUT_CNT_MAX		((STROBE_CHG_TIMEOUT_TIME*1000)/STROBE_CHG_CHK_INTERVAL)

// unit: 40ms, 0 is disable. 
// For a900 the charge ready is not avaible yet in this moment.
// We use timer tick to count a constant time for ready.
// We should remove this when ready sigle is ok and available.
#define A900_TIMER_FOR_NONREADY 	125 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define STROBE_MAIN_POWER_ON		IO_STROBE_PWR_ON
#define STROBE_MAIN_POWER_OFF		IO_STROBE_PWR_OFF

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if KIT_FLASH_ENABLE
static BOOL 	gIsStbChgEnabled = FALSE;
#endif

static BOOL 	gIsStbPowerOn = FALSE;

#if KIT_FLASH_ENABLE
static BOOL 	gIsStbResCreated = FALSE;
static UINT32	gStrobeWatchdog = TIMER_NULL;
static UINT32	gStbChgTimeoutCnt=0;
static BOOL		gIsStbRdy = FALSE;
static TX_MUTEX mtxStbRdy;
#endif

#if A900_TIMER_FOR_NONREADY
	// Here we just add this for non-"ready" single of a900
	UINT32 gIsReadyInTimer=0;
	UINT32 SomeoneAskedReady=A900_TIMER_FOR_NONREADY;// unit is 40ms
	
	#ifdef IS_STROBE_RDY
	#undef IS_STROBE_RDY
	#endif
	#define IS_STROBE_RDY gIsReadyInTimer
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

void tmrWaitListInstall(UINT32 tmrid, UINT32 delay, void (*pfunc)(), void *param);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if KIT_FLASH_ENABLE 
static void strobeResourceDestroy(void);
static UINT32 strobeResourceCreate(void);
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeChargeRdyNotify
 *------------------------------------------------------------------------*/
static
void
strobeChargeRdyNotify(
	void
)
{
	printf("\nStrbChg - OK !! \n\n");
	sp5kHostMsgSend(APP_UI_MSG_STROBE_CHARGE_READY, 0);
}
#endif


#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeChargeTimeoutNotify
 *------------------------------------------------------------------------*/
static
void
strobeChargeTimeoutNotify(
	void
)
{
	printf("\nStrbChg - ERR !! \n\n");
	sp5kHostMsgSend(APP_UI_MSG_STROBE_CHARGE_ERR, 0);
}

#endif

/*-------------------------------------------------------------------------
 *  File Name : strobePowerOn
 *------------------------------------------------------------------------*/
static
void
strobePowerOn(
	void
)
{
	/*-- power on charger circuit */
	STROBE_MAIN_POWER_ON;
	gIsStbPowerOn = TRUE;
	APP_SLEEP_MS(2);
}

/*-------------------------------------------------------------------------
 *  File Name : strobePowerOff
 *------------------------------------------------------------------------*/
static
void
strobePowerOff(
	void
)
{
	/*-- power off charger circuit */
	STROBE_MAIN_POWER_OFF;
	gIsStbPowerOn = FALSE;
}

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeChargeEnable
 *------------------------------------------------------------------------*/
static
void
strobeChargeEnable(
	void
)
{
	IO_STROBE_CHARGE_ON; APP_SLEEP_MS(1);
	IO_STROBE_CHARGE_OFF; APP_SLEEP_MS(2);
	IO_STROBE_CHARGE_ON; APP_SLEEP_MS(1);
	gIsStbChgEnabled = TRUE;
}
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeChargeDisable
 *------------------------------------------------------------------------*/
static
void
strobeChargeDisable(
	void
)
{
	IO_STROBE_CHARGE_OFF;
	gIsStbChgEnabled = FALSE;
}
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeReadyFlagSet
 *------------------------------------------------------------------------*/
static
void
strobeReadyFlagSet(
	BOOL isRdy
)
{
	DBG_PRINT("%s: isRdy(%d) \n", __FUNCTION__, isRdy);

	sp5kOsMutexGet(&mtxStbRdy, TX_WAIT_FOREVER);
	gIsStbRdy = isRdy;
	sp5kOsMutexPut(&mtxStbRdy);
}
#endif

#if (!STROBE_RDY_POLLING_TRUE_IO)
/*-------------------------------------------------------------------------
 *  File Name : strobeReadyFlagGet
 *------------------------------------------------------------------------*/
static
BOOL
strobeReadyFlagGet(
	void
)
{
	BOOL isRdy;
	DBG_PRINT("%s \n", __FUNCTION__);

	sp5kOsMutexGet(&mtxStbRdy, TX_WAIT_FOREVER);
	isRdy = gIsStbRdy;
	sp5kOsMutexPut(&mtxStbRdy);

	return isRdy;
}
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeInit
 *------------------------------------------------------------------------*/
static
void
strobeInit(
	void
)
{
	static BOOL isStrobeInit=FALSE;

	if (!isStrobeInit) {
		sp5kOsMutexCreate((TX_MUTEX *)&mtxStbRdy, (char *)"StbRdy", TX_INHERIT);
		sp5kOsMutexPut(&mtxStbRdy);
		isStrobeInit=TRUE;
	}
}
#endif

#if KIT_FLASH_ENABLE

/*-------------------------------------------------------------------------
 *  File Name : watchdogISR_StrobeChargeRdy
 *------------------------------------------------------------------------*/
static
void
watchdogISR_StrobeChargeRdy(
	void
)
{
	#if HOST_DBG /*-- Only for DEBUG */
	static UINT32 i=0;
	i++;
	if (i>(2000/STROBE_CHG_CHK_INTERVAL)) {
		printf("\nwdog_StrobeChargeRdy @ ostime(%u) \n\n", appMsTimeGet());
		i = 0;
	}
	#endif

	if (gIsStbChgEnabled==FALSE) {
		return;
	}

#if A900_TIMER_FOR_NONREADY
	if(SomeoneAskedReady) {
		SomeoneAskedReady--;
	}	
	
	if(SomeoneAskedReady==0) {
		gIsReadyInTimer=1;
#else 
	if (IS_STROBE_RDY) {
#endif		
		/*-- Strobe Charge has completed
		 */
		DBG_PRINT("\n\nSTRB : READY !! \n\n");

		/*-- release strobe control resource */
		strobeResourceDestroy();

		/*-- update Rdy flag */
		strobeReadyFlagSet(TRUE);

		/*-- notify charge ready */
		strobeChargeRdyNotify();
	}
	else {
#if A900_TIMER_FOR_NONREADY
		gIsReadyInTimer=0;
		DBG_PRINT("\n\nSTRB : notREADY !! \n\n");
#endif		
		/*-- Strobe Charge progressing
		 */
		gStbChgTimeoutCnt++;

		if(gStbChgTimeoutCnt > STROBE_CHG_TIMEOUT_CNT_MAX) {
			/* !! Charge has timed out !!
			*/
			DBG_PRINT("\n\nSTRB : !! CHG TIMEOUT (cnt=%d) !! \n\n", STROBE_CHG_TIMEOUT_CNT_MAX);

			/* Turn off charger circuit */
			strobeChargeDisable();

			/*-- Release strobe resources */
			strobeResourceDestroy();

			/* notify charge ERROR */
			strobeChargeTimeoutNotify();
		}
	}
}
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeWatchdogRegister
 *------------------------------------------------------------------------*/
static
UINT32
strobeWatchdogRegister(
	void
)
{
	DBG_PRINT("%s \n", __FUNCTION__);

	if (gStrobeWatchdog!=TIMER_NULL) {
		/* already registered !! */
		DBG_PRINT("\n%s: !! Skipped !!, already register\n\n", __FUNCTION__);
		return SUCCESS;
	}

	gStrobeWatchdog = appTimerIsrReg(watchdogISR_StrobeChargeRdy, STROBE_CHG_CHK_TICK_CNT);

	if (gStrobeWatchdog==TIMER_NULL) {
		printf("\n%s: !!! FATAL !!!, watchdog service error\n\n", __FUNCTION__);
		return FAIL;
	}

	DBG_PRINT("STRB: Watchdog registered, check per (%d) ticks \n", STROBE_CHG_CHK_TICK_CNT);
	return SUCCESS;
}
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeWatchdogRelease
 *------------------------------------------------------------------------*/
static
void
strobeWatchdogRelease(
	void
)
{
	appTimerIsrUnreg(&gStrobeWatchdog);
	gStrobeWatchdog = TIMER_NULL;
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : strobeTrigControl
 *------------------------------------------------------------------------*/
#if 0
static
void
strobeTrigControl(
	BOOL isEnable
)
{
	/*-- this function is used while the hardware
	 * has implemented the switch circuit to disable/enable
	 * the strobe trigger function.
	 */
	return;
}
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeResourceCreate
 *------------------------------------------------------------------------*/
static
UINT32
strobeResourceCreate(
	void
)
{
	DBG_PRINT("\n%s !!\n\n", __FUNCTION__);

	if (gIsStbResCreated==TRUE) {
		DBG_PRINT("\n%s:__IGNORE__, Res created. \n\n", __FUNCTION__);
		return SUCCESS;
	}

	/*-- Register watchdog timer to monitor charging progress */
	if (strobeWatchdogRegister()==FAIL) {
		DBG_PRINT("\n%s: !! FATAL !!, failed watchdog \n\n", __FUNCTION__);
		return FAIL;
	}

	/*-- charge timeout count */
	gStbChgTimeoutCnt = 0;

	gIsStbResCreated = TRUE;
	return SUCCESS;
}
#endif

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeResourceDestroy
 *------------------------------------------------------------------------*/
static
void
strobeResourceDestroy(
	void
)
{
	DBG_PRINT("\n%s !!\n\n", __FUNCTION__);

	/*-- charge timeout count */
	gStbChgTimeoutCnt = 0;

	/*-- Release watchdog timer */
	strobeWatchdogRelease();

	gIsStbResCreated = FALSE;
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appStrobeQueryReady
 *------------------------------------------------------------------------*/
BOOL
appStrobeQueryReady(
	void
)
{
#if KIT_FLASH_ENABLE
	BOOL isRdy = FALSE;
	#if STROBE_RDY_POLLING_TRUE_IO /*-- direct IO polling */
	if (gIsStbChgEnabled) {
		isRdy = IS_STROBE_RDY;
	} else {
		strobePowerOn();
		strobeChargeEnable();
		isRdy = IS_STROBE_RDY;
		strobeChargeDisable();
		strobePowerOff();
	}
	#else /*-- program flag polling */
	isRdy = strobeReadyFlagGet();
	#endif

	return isRdy;
#else
	return TRUE;
#endif /* KIT_FLASH_ENABLE */
}
/*-------------------------------------------------------------------------
 *  File Name : appStrobeQueryChargeEnable
 *------------------------------------------------------------------------*/
BOOL
appStrobeQueryChargeEnable(
	void
)
{
#if KIT_FLASH_ENABLE
	return gIsStbChgEnabled;
#else
	return TRUE;
#endif /* KIT_FLASH_ENABLE */
}
/*-------------------------------------------------------------------------
 *  File Name : appStrobeQueryPowerEnable
 *------------------------------------------------------------------------*/
BOOL
appStrobeQueryPowerEnable(
	void
)
{
#if KIT_FLASH_ENABLE
	return gIsStbPowerOn;
#else
	return TRUE;
#endif /* KIT_FLASH_ENABLE */
}
/*-------------------------------------------------------------------------
 *  File Name : appStrobePowerOn
 *------------------------------------------------------------------------*/
void
appStrobePowerOn(
	void
)
{
	strobePowerOn();
}
/*-------------------------------------------------------------------------
 *  File Name : appStrobePowerOff
 *------------------------------------------------------------------------*/
void
appStrobePowerOff(
	void
)
{
	strobePowerOff();
}

/*-------------------------------------------------------------------------
 *  File Name : appStrobeChargeStart
 *------------------------------------------------------------------------*/
UINT32
appStrobeChargeStart(
	void
)
{
	DBG_PRINT("STRB: (CHG) START \n");
	printf("STRB: (CHG) START \n");

#if KIT_FLASH_ENABLE
	/*--
	 * Strobe Init
	 */
	strobeInit();

	/*--
	 * Reset StrobeRdy flag
	 */
	strobeReadyFlagSet(FALSE);

	/*--
	 * Power on Strobe circuitry
	 */
	if (!gIsStbPowerOn) {
		printf("\n%s: Force ST_PWR On !!!\n\n", __FUNCTION__);
		appStrobePowerOn();
	}

	/*--
	 * Create Strobe resources
	 */
	if (FAIL==strobeResourceCreate()) {
		printf("Strobe RES error !!\n");
		return FAIL;
	}

	/*--
	 * Turn on ST_CHG
	 */
	strobeChargeEnable();


	printf("STRB: (CHG) End\n");

#endif /* KIT_FLASH_ENABLE */
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appStrobeChargeStop
 *------------------------------------------------------------------------*/
void
appStrobeChargeStop(
	void
)
{
	DBG_PRINT("STRB: (CHG) STOP \n");

#if KIT_FLASH_ENABLE
	/*--
	 * Turn off ST_CHG
	 */
	strobeChargeDisable();

	/*--
	 * 	Destroy Strobe resources,
	 */
	strobeResourceDestroy();
#endif /* KIT_FLASH_ENABLE */
}

#if 1 /*-- Only for DEBUG */

#if KIT_FLASH_ENABLE
/*-------------------------------------------------------------------------
 *  File Name : strobeTrigOffCBF
 *------------------------------------------------------------------------*/
static
void
strobeTrigOffCBF(
	void
)
{
	IO_STROBE_TRIG_OFF;
}

#endif
/*-------------------------------------------------------------------------
 *  File Name : appStrobeTrig
 *------------------------------------------------------------------------*/
void
appStrobeTrig(
	UINT32 durationUS
)
{
#if KIT_FLASH_ENABLE
	DBG_PRINT("STRB: TRIG(%d) \n", durationUS);

	STROBE_MAIN_POWER_ON;
	IO_STROBE_CHARGE_OFF;
	IO_STROBE_TRIG_ON;
	tmrWaitListInstall(2, durationUS, strobeTrigOffCBF, 0);
#endif /* KIT_FLASH_ENABLE */
}

/*-------------------------------------------------------------------------
 *  File Name : appStrobeChargeTimerReset
 *  This is only for a900 ready signle not available.
 *------------------------------------------------------------------------*/
void
appStrobeChargeTimerReset(
	void
)
{
#if A900_TIMER_FOR_NONREADY
	SomeoneAskedReady=A900_TIMER_FOR_NONREADY;
#endif 
}

#endif

