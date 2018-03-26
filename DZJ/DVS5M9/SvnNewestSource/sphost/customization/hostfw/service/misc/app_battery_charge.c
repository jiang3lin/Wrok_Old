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
#include "Gpio_custom.h"
#include "app_battery.h"
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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 
/**************************************************************************
 *  Function Name: appBatteryCharge_Start                   *
 **************************************************************************/
void appBatteryCharge_Start(void)
{
    //sp5kGpioWrite(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_BATCHARGER_ENn),GPIO_PIN_VAL_LO(CFG_BATCHARGER_ENn));
   // printf("appBatteryCharge_Start\n");
   
//	BATTERY_CHARGE_ON;
}

/**************************************************************************
 *  Function Name: appBatteryCharge_Stop                   *
 **************************************************************************/
void appBatteryCharge_Stop(void)
{
    //sp5kGpioWrite(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_BATCHARGER_ENn),GPIO_PIN_VAL_HI(CFG_BATCHARGER_ENn));
  //  printf("appBatteryCharge_Stop\n");

//	BATTERY_CHARGE_OFF;
}

/**************************************************************************
 *  Function Name: appBatteryCharge_CheckStatus                   *
 status: BATTERY_STATUS_NON_FULL/BATTERY_STATUS_FULL
 **************************************************************************/
UINT8 appBatteryCharge_CheckStatus(void)
{
    UINT32 ucBatteryStatus = BATTERY_STATUS_NON_FULL;
    UINT32 BATCHARGER_CHARGING_STATE ;
    sp5kGpioRead(SP5K_GPIO_GRP_LMI,1<<CFG_BATCHARGER_CHARGINGn,&BATCHARGER_CHARGING_STATE);
    if( BATCHARGER_CHARGING_STATE != BATTERY_STATUS_NON_FULL)
        ucBatteryStatus = BATTERY_STATUS_FULL ;
  //  printf("appBatteryCharge_CheckStatus\n");

	if(IS_BATTERY_CHARGING == TRUE)
	{
		ucBatteryStatus = BATTERY_STATUS_NON_FULL ;
	}
	else
	{
		ucBatteryStatus = BATTERY_STATUS_FULL ;
	}
  
    return ucBatteryStatus;
}
