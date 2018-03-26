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
#define HOST_DBG 1

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "math.h"
#include "common.h"
#include "app_battery_ctrl.h"
#include "app_battery.h"
#include "sp5k_usb_api.h"
#include "app_com_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appBatteryChargeStartCBF_t *gBatteryChargeStartCBF;
appBatteryChargeStopCBF_t *gBatteryChargeStopCBF;
appBatteryCheckStatusCBF_t *gBatteryCheckStatusCBF;

UINT8 gucBatteryStatus=BATTERY_STATUS_NON_FULL;

UINT8 gChargerStatus=FALSE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
/**************************************************************************
 *  Function Name: appBatteryCharge_SetStartCBFRegister                   *
 **************************************************************************/
void appBatteryCharge_SetStartCBFRegister(
	appBatteryChargeStartCBF_t *aBatteryChargeStartCBF
)
{
    if (!aBatteryChargeStartCBF)
	{
		gBatteryChargeStartCBF = NULL;
	} 
	else 
	{
		gBatteryChargeStartCBF = aBatteryChargeStartCBF;
	}
}

/**************************************************************************
 *  Function Name: appBatteryCharge_SetStartCBF                   *
 **************************************************************************/
void appBatteryCharge_SetStartCBF(void)
{

	if (gBatteryChargeStartCBF){
		gBatteryChargeStartCBF();
		gChargerStatus=TRUE;
	}
}

/**************************************************************************
 *  Function Name: appBatteryCharge_SetStopCBFRegister                   *
 **************************************************************************/
void appBatteryCharge_SetStopCBFRegister(
	appBatteryChargeStopCBF_t *aBatteryChargeStopCBF
)
{
    if (!aBatteryChargeStopCBF)
	{
		gBatteryChargeStopCBF = NULL;
	} 
	else 
	{
		gBatteryChargeStopCBF = aBatteryChargeStopCBF;
	}
}

/**************************************************************************
 *  Function Name: appBatteryCharge_SetStopCBF                   *
 **************************************************************************/
void appBatteryCharge_SetStopCBF(void)
{
	if (gBatteryChargeStopCBF){
		gBatteryChargeStopCBF();
		gChargerStatus=FALSE;
	}
}

/**************************************************************************
 *  Function Name: appBatteryCharge_SetCheckStatusCBFRegister                   *
 **************************************************************************/
void appBatteryCharge_SetCheckStatusCBFRegister(
	appBatteryCheckStatusCBF_t *aBatteryCheckStatusCBF
)
{
    if (!aBatteryCheckStatusCBF)
	{
		gBatteryCheckStatusCBF = NULL;
	} 
	else 
	{
		gBatteryCheckStatusCBF = aBatteryCheckStatusCBF;
	}
}

/**************************************************************************
 *  Function Name: appBatteryCharge_SetCheckStatusCBF                   *
 **************************************************************************/
UINT8 appBatteryCharge_SetCheckStatusCBF(void)
{
	if (gBatteryCheckStatusCBF) 
		gucBatteryStatus = gBatteryCheckStatusCBF();
	else
	    gucBatteryStatus = BATTERY_STATUS_NON_FULL;
	
	return gucBatteryStatus;
}

/**************************************************************************
 *  Function Name: appBatteryCharge_CBRegister                   *
 **************************************************************************/
UINT8 appBatteryCharge_CBRegister(void)
{
#if COMPEL_CHARGING
	appBatteryCharge_SetStartCBFRegister(appBatteryCharge_Start);
	appBatteryCharge_SetStopCBFRegister(appBatteryCharge_Stop);
	appBatteryCharge_SetCheckStatusCBFRegister(appBatteryCharge_CheckStatus);
#endif
	return SUCCESS;

}


/**************************************************************************
 *  Function Name: appBatteryCharge_LineStateGet                   *
 **************************************************************************/
appDCSRC_e appBatteryCharge_LineStateGet(void)
{

	if( sp5kUsbLineStateGet()== 0x0f )
	{
		return BATTERY_DCSRC_ADAPTER;
	}
	else
	{
		UINT8 i = 0;
		for(i = 0; i < 3; i ++)
		{
			appOsDelayMs(3); 
			if(sp5kUsbLineStateGet()== 0x0f)	//re-check
			{
				return BATTERY_DCSRC_ADAPTER;
			}
		}
		return BATTERY_DCSRC_PC;
	}
}


/**************************************************************************
 *  Function Name: appBatteryCharge_Init                   *
 **************************************************************************/
UINT8 appBatteryCharge_Init(void)
{
	appBatteryCharge_CBRegister();
	
	return SUCCESS;

}

/**************************************************************************
 *  Function Name: appBatteryCharge_ChargerStatusGet                   *
 **************************************************************************/
UINT8 appBatteryCharge_ChargerStatusGet(void)
{

	return gChargerStatus;

}




