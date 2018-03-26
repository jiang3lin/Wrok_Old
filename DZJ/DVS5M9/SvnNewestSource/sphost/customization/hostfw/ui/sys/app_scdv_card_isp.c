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
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_disp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"
#include "sp5k_utility_api.h"

#include "app_aaa.h"
#include "gpio_custom.h"
#include "app_ui_para.h" 
#include "app_icon_def.h"
#include "app_menu_tab.h"
#include "app_osd_api_lcm.h"
#include "app_osd_draw_lcm.h"
#include "app_scdv_ui_para_def.h"



extern UINT32 appFirmwareUpdate(void);
extern void sioCtrl(UINT32 opt);

void appScdvCardIspOsdShow(void)
{
	uiPara_t* puiPara = appUiParaGet();
	
	appLcmOsdClean();

	//Mode Icon
	appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SCDV_CARD_ISP, FALSE);

	//File Cnt
	appLcmOsdLargeStringShow(ID_STR_BURN, FALSE);

	//Mode Tips
	appLcmOsdModeStringShow(ID_STR_BURN, FALSE);

	//Tips
	appLcmOsdTipsStringShow(ID_STR_CARD_ISP, FALSE);
	
	//Battery
	appLcmOsdDrawBattery(TRUE, -1);
	
	appLcmRefresh();
}


void appSystemReboot(void)
{
	tmx_t rtc;
	UINT32 r,reliableCode;	
	r = sp5kRtcReliableGet(&reliableCode);
	sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
	rtc.tmx_sec += 1;
	
	if (rtc.tmx_sec >= 60) 
	{
		rtc.tmx_sec -= 60;
		rtc.tmx_min++;
		if (rtc.tmx_min >= 59) 
		{
			rtc.tmx_min -= 59;
			rtc.tmx_hour++;
		}
	}
	sp5kRtcDateTimeSet(SP5K_WAKEUP_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
	
	/* Powser off */
	sp5kPowerCtrl(SP5K_POWER_OFF, (UINT32)-1);

	//reboot???
	//DBG_ASSERT(0);
}

void appScdvCardIspStateClose(void)
{
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
	{
		appStateChange(APP_STATE_SPORTDV_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
	}
	else// if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_CAR)
	{
		appStateChange(APP_STATE_CARDV_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);
	}
}

void appScdvCardIsp(void)
{
	//button setting
	appBtnDisable(BTN_ALL);

	//upgreade fw from sd card
	LED_RULES_CARD_ISP;	//FW download tools set led blink
	sioCtrl(0);
	if(appFirmwareUpdate() == SUCCESS)
	{
		appSystemReboot();	//FW download tools disable auto poweroff
		//sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
	}
	else
	{
		appScdvCardIspStateClose();
	}
	LED_RULES_NORMAL;	

	//button setting
	appBtnEnable(BTN_ALL);
}

void appScdvCardIspStateInit(void)
{
	LED_RULES_NORMAL;
	LCD_BACKLIGHT_ON;

	appAutoPowerOffEnable(FALSE);
	appLcmPowerSaveEnable(FALSE);

	sp5kModeSet(SP5K_MODE_STANDBY);
	sp5kModeWait(SP5K_MODE_STANDBY);

	appScdvCardIspOsdShow();
	
	if(appScdvWaitForDiskReady() == TRUE)
	{
		appScdvCardIsp();
	}
	else if(!IS_CARD_EXIST)
	{
		sp5kHostMsgSend(APP_KEY_PRESS_OK, KEY_MSG_PARAM_DIRECT_RUN);
	}

	//button setting
	appBtnEnable(BTN_ALL);
}

void appScdvCardIspStateOnMsg(UINT32 msg, UINT32 param)
{
	switch (msg) 
	{
		case SP5K_MSG_DISK_MOUNT_COMPLETE:
		case APP_UI_MSG_VIEW_WAIT_DISK:
			if(appScdvWaitForDiskReady() == TRUE)
			{
				appScdvCardIsp();
			}
			break;
	}
}

void appScdvCardIspStateOnKey(UINT32 msg, UINT32 param)
{
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER: 
		case APP_KEY_PRESS_OK:
			appScdvCardIspStateClose();
			break;
	}
}

void appScdvCardIspState(UINT32 msg,UINT32 param)
{	
	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:		
			RPrintf("appScdvCardIspState");
			appScdvCardIspStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appStateCloseDone();
			break;	
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appScdvCardIspStateOnKey(msg,param);
			}
			else
			{
				appScdvCardIspStateOnMsg(msg,param);
			}
			break;
	}
} 

