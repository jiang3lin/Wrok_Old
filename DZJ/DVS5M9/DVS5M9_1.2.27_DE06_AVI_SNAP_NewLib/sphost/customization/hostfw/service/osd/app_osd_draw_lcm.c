/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2010 by DXG Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of DXG        *
 *  Technology Co., Ltd. All rights are reserved by DXG Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of DXG Technology Co., Ltd.                       *
 *                                                                        *
 *  DXG Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  DXG Technology Co., Ltd.                                          *
 *  MIN ZHI IND., PARK,					            *
 *  LONG HUA,BAOAN,SHEN ZHEN,P.R.O.C.                                           *
 *                                                                        *
 *  Author: Wujf                                                  *
 *                                                                         *
 **************************************************************************/
#define HOST_DBG 0

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_otrack_api.h"
#include "sp5k_usb_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_media_api.h"
#include "sp5k_af_api.h"
#include "Sp5k_usb_api.h"

#include "app_osd_draw_lcm.h"
#include "app_osd_api_lcm.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"

#include "app_battery.h"
#include "gpio_custom.h"

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

void appLcmOsdDrawPowerOnLogo(void)
{
	profLogAdd(0, "appLcmOsdDrawPowerOnLogo s");

	appLcmOsdClean();
	//appLcmOsdTest(FALSE);
	appLcmOsdTextShow(LCM_OSD_CENTER_X, LCM_OSD_CENTER_Y, SP5K_GFX_ALIGN_CENTER, "HELLO", TRUE, FALSE);
	appLcmRefresh();

	LCD_BACKLIGHT_ON; 

	profLogAdd(0, "appLcmOsdDrawPowerOnLogo e");
}

void appLcmOsdDrawPowerOffLogo(void)
{
	appLcmOsdClean();
	//appLcmOsdTest(FALSE);
	//appLcmOsdTextShow(LCM_OSD_CENTER_X, LCM_OSD_CENTER_Y, SP5K_GFX_ALIGN_CENTER, "BYE...", TRUE, FALSE);
	appLcmOsdTextShow(LCM_OSD_CENTER_X, LCM_OSD_CENTER_Y, SP5K_GFX_ALIGN_CENTER, "MEDION", TRUE, FALSE);
	appLcmRefresh();
}

void appLcmOsdDrawBattery(UINT8 bShow, UINT8 idx)
{
	if (bShow && appActiveStateGet() != APP_STATE_POWER_ON) //power on state refresh battery will white screen
	{
		switch(idx)
		{
			case APP_BATT_LEVEL_0:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_000, FALSE);
				break;
			case APP_BATT_LEVEL_1:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_001, FALSE);
				break;
			case APP_BATT_LEVEL_2:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_011, FALSE);
				break;
			case APP_BATT_LEVEL_3:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_111, FALSE);
				break;
			default:
				appLcmOsdDrawBattery(TRUE, appBatteryLevelGet());
				break;
		}
		appLcmRefresh();
	}
	else
	{

	}	
}

void appRecLcmOsdDrawBatterySecond(UINT8 bShow, UINT8 idx)
{
	if (bShow && appActiveStateGet() != APP_STATE_POWER_ON) //power on state refresh battery will white screen
	{
		switch(idx)
		{
			case APP_BATT_LEVEL_0:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_000, FALSE);
				break;
			case APP_BATT_LEVEL_1:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_001, FALSE);
				break;
			case APP_BATT_LEVEL_2:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_011, FALSE);
				break;
			case APP_BATT_LEVEL_3:
				appLcmOsdBatteryIconShow(ID_ICON_BATTERY_111, FALSE);
				break;
			default:
				appLcmOsdDrawBattery(TRUE, appBatteryLevelGet());
				break;
		}
		//appLcmRefresh();
	}
	else
	{

	}	
}