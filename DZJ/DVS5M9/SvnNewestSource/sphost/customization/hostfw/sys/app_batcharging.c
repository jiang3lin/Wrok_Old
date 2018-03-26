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
#include "app_com_def.h"
#include "app_battery.h"
#include "app_battery_ctrl.h"
#include "app_timer.h"
#include "sp5k_gfx_api.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "app_pal.h"
#include "sp5k_usb_api.h"

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
static UINT32 ChargingTimer = TIMER_NULL;
static UINT32 count = 0;

#define LCD_SIZE_X					320                                                                                                                                                       
#define LCD_SIZE_Y					240
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern BOOL bBatteryIn;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appBatteryChargingInit(void)
{
    BOOL bsrc_PC = FALSE;
    
    bBatteryIn = appBatteryAdcValueGet() > 100 ? TRUE:FALSE;
    bsrc_PC = appBatteryCharge_LineStateGet();
    
	if(appBatteryCharge_SetCheckStatusCBF() != BATTERY_STATUS_FULL && bBatteryIn)
	{
	    if(bsrc_PC == BATTERY_DCSRC_ADAPTER || (bsrc_PC == BATTERY_DCSRC_PC && sp5kUsbMaxPowerCheck(SP5K_USB_MAXPOWER_HIGH))){
	        appBatteryCharge_SetStartCBF();
			RED_LED_ON;
		}
	}
	ChargingTimer = appTimerSet(1000, "Charging Timer");
}

void 
appBatteryChargingStateOnMsg(
    UINT32 msg,
    UINT32 param
)
{
    switch (msg) {
        case SP5K_MSG_TIMER:
            if (param == ChargingTimer)
            {
                if(count < 3)
                {
                    sp5kGfxRectFill(SP5K_GFX_PAGE_OSD_0, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, PAL_BLACK);
                    sp5kGfxIconDraw(SP5K_GFX_PAGE_OSD_0, LCD_SIZE_X/2, LCD_SIZE_Y/2, SP5K_GFX_ALIGN_TOP, ID_ICON_BATTERY_LOW+count);
                }
                count++;
                if(count >= 3)
                    count %= 3;
                if(appBatteryCharge_SetCheckStatusCBF() == BATTERY_STATUS_FULL)
                {
                    appBatteryCharge_SetStopCBF();
			RED_LED_OFF;
                    appTimerClear(&ChargingTimer);
                    sp5kGfxRectFill(SP5K_GFX_PAGE_OSD_0, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, PAL_BLACK);
                    sp5kGfxIconDraw(SP5K_GFX_PAGE_OSD_0, LCD_SIZE_X/2, LCD_SIZE_Y/2, SP5K_GFX_ALIGN_TOP, ID_ICON_BATTERY_FULL);
                }
            }
            break;
        default:
            break;
    }
}

void 
appBatteryChargingState(
	UINT32 msg,
	UINT32 param
)
{	
	switch (msg)
	{
		case APP_STATE_MSG_INIT:
			appBatteryChargingInit();
			break;
		case APP_STATE_MSG_CLOSE:
		    appStateCloseDone();
		    break;
		default:
		    appBatteryChargingStateOnMsg(msg,param);
			break;
	}
} 




