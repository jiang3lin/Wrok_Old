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
 
/**************************************************************************
 *                        H E A D E R   F I L E S                            *
 **************************************************************************/
#include "common.h"
#include <stdio.h>

#include "app_com_def.h" 

#include "app_menu.h" 
#include "app_menu_core.h" 
#include "app_menu_tab.h" 
#include "app_menu_disp.h" 
#include "app_menu_event.h"
#include "app_ui_para.h" 
#include "app_setup_lib.h" 
#include "app_user_setting.h"


#include "sp5k_cdsp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_af_api.h"
#include "sp5k_rsvblk_api.h"
#include "customization\app_init.h"	
#include "app_com_api.h"
#include "app_com_def.h"
#include "app_osd.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_playback_def.h"
#include "app_zoom_api.h"
#include "app_util.h"

#include "app_still.h"
#include "app_video.h"

#include "app_strobe.h"
#include "app_led.h"
#include "app_aaa.h"
#include "app_exif.h"
#include "app_gSensor.h"
#include "app_view_osd.h"
#include "app_face_detect_ex.h"
#include "app_usb_dpslib.h"
#include "app_osd_api.h"
#include "app_playback_def.h"
#include "app_playback_uiparam.h"
#include "app_calib_api.h"
#include "app_init.h"
#include "api/sp5k_awb_api.h"
#include "app_playback_osd.h"
#include "app_gSensor_def.h"
#include "middleware/sysg_resource_def.h"
#include "app_md.h"

#include "app_scdv_ui_para_def.h"
#include "app_scdv_menu_disp.h"
#include "app_cdfs_api.h"
#include "app_osd_draw_lcm.h"
#include "app_osd_api_lcm.h"
#include "app_version.h"
#include "app_rf_def.h"

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
extern void appScdvMenuStateMenuExit(void);
extern UINT32 appScdvMenuParentMenuEnter(UINT32 msg, UINT32 param);
extern UINT32 appScdvMenuSubMenuEnter(UINT32 msg, UINT32 param, UINT32 bBackParent);

UINT32 appMenuEvt_SdvFirstItemProcess (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SdvLastItemProcess (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ScdvLayer4Process (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	menuCtrl_t *pMenu = NULL;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			nSPDVSwitchResolution=1;
			//printf("--appMenuEvt_ScdvLayer4Process=%d\n",nSPDVSwitchResolution);
			menuProcParaSet(layer, item);
			menuProcParent ();
			menuProcParent ();
			appScdvMenuDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeSportDV (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			menuProcCurrPosSet(MENU_MARK_SYSTEM1);
			appScdvMenuDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SdvVideoModeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SdvVideoModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcParaSetParent(layer, item);
			break;
	}	
	return ret; 
}

UINT32 appMenuEvt_SdvVideoNormalSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_SdvVideoSlowTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_SdvPhotoModeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SdvPhotoModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcParaSetParent(layer, item);
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SdvPhotoNormalSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_SdvPhotoLapseTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_SdvPhotoSelfTimerSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_SdvPhotoBurstTimeSet (UINT8 layer, 
	UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_ScdvSensorFlipSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SdvCarDvSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			if(item == UI_SCDV_CONFIRM_OK)
			{
				puiPara->ScdvMainMode = UI_SCDV_MAIN_MODE_CAR;
				puiPara->ReturnSetting = 0x11;//
				menuProcCurrPosSet (MENU_MARK_CAR_DV);
				appScdvMenuDisplay();
				appCdfsInit();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SdvExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appScdvMenuParentMenuEnter(msg, para);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeCarDV (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			menuProcCurrPosSet(MENU_MARK_SYSTEM1);
			appScdvMenuDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvVideoModeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvVideoModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcParaSetParent(layer, item);
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvVideoNormalSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_CdvVideoLapseSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	return appMenuEvt_ScdvLayer4Process(layer, item, msg, para);
}

UINT32 appMenuEvt_CdvLoopingTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvGsensorTriggerSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvGsensorSensitivitySet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appGsensorSensortivitySet();
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvMicPhoneSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvSportDvSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			if(item == UI_SCDV_CONFIRM_OK)
			{
				puiPara->ScdvMainMode = UI_SCDV_MAIN_MODE_SPORT;
				puiPara->ReturnSetting = 0x10;
				menuProcCurrPosSet (MENU_MARK_SPORT_DV);
				appScdvMenuDisplay();
				appCdfsInit();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_CdvExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appScdvMenuParentMenuEnter(msg, para);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeSystem1 (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemDisSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemWdrSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemLedIndicatorSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcParaSet(layer, item);
			appLedIndicatorSet();
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemOsdFlipSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcParaSet(layer, item);
			appOsdFlipSet();
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemOsdBrightnessSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	menuCtrl_t *pMenu;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			menuProcMoveDown();
			appScdvMenuDisplay();
			if(menuHdlGet(&pMenu) == SUCCESS)
			{
				menuProcParaSet(pMenu->layer, pMenu->item);
				appOsdBrightnessSet();
			}			
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_OK:
			menuProcParaSet(layer, item);
			appOsdBrightnessSet();
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemBeepVolumeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcParaSet(layer, item);
			appBuzzerEnableSet();
			appBuzzerPlay(BUZZER_BEEP);
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemDateStampSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemAutoPowerOffSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_System1ExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appScdvMenuParentMenuEnter(msg, para);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeSystem2 (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemDateTimeInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	appRTC_t rtcTime;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appRtcGet(&rtcTime);
			puiPara->SystemDateTimeYear = rtcTime.year - APP_RTC_MIN_YEAR ;
			puiPara->SystemDateTimeMonth = rtcTime.mon - 1 ;
			puiPara->SystemDateTimeDay = rtcTime.day - 1;
			puiPara->SystemDateTimeHour = rtcTime.hour;
			puiPara->SystemDateTimeMinute = rtcTime.min;
			puiPara->SystemDateTimeSecond = rtcTime.sec;			
			menuProcChild(0);
			appScdvMenuSystemDateTimeDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemDateTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	appRTC_t rtcTime;
	
	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			menuProcMoveDown();
			appScdvMenuSystemDateTimeDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_OK:
			if(item == UI_SCDV_SYSTEM_DATE_TIME_EXIT)
			{
				//don't save and exit
				menuProcParent();
				appScdvMenuDisplay();
				ret = MENU_IGNORE_DEFAULT;
			}
			else if(item == UI_SCDV_SYSTEM_DATE_TIME_OK)
			{
				//save and exit
				rtcTime.year = puiPara->SystemDateTimeYear + APP_RTC_MIN_YEAR;
				rtcTime.mon = puiPara->SystemDateTimeMonth + 1;
				rtcTime.day = puiPara->SystemDateTimeDay + 1;
				rtcTime.hour = puiPara->SystemDateTimeHour;
				rtcTime.min = puiPara->SystemDateTimeMinute;
				rtcTime.sec = puiPara->SystemDateTimeSecond;				
				appRtcSet(&rtcTime);
				
				menuProcParent();
				appScdvMenuDisplay();
				ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				//adjust value
				menuProcChild (menuProcParaGet(layer, item));
				menuProcMoveDown();
				layer = menuProcLayerGet();
				item = menuProcCurrItemGet(layer);
				menuProcParaSet(layer, item);
				menuProcParent();
				layer = menuProcLayerGet();
				item = menuProcCurrItemGet(layer);

				rtcTime.year = puiPara->SystemDateTimeYear + APP_RTC_MIN_YEAR;
				rtcTime.mon = puiPara->SystemDateTimeMonth + 1;
				rtcTime.day = puiPara->SystemDateTimeDay + 1;
				rtcTime.hour = puiPara->SystemDateTimeHour;
				rtcTime.min = puiPara->SystemDateTimeMinute;
				rtcTime.sec = puiPara->SystemDateTimeSecond;				
				if(appRtcRangeCheck(&rtcTime))
				{
					puiPara->SystemDateTimeYear = rtcTime.year - APP_RTC_MIN_YEAR ;
					puiPara->SystemDateTimeMonth = rtcTime.mon - 1 ;
					puiPara->SystemDateTimeDay = rtcTime.day - 1;
					puiPara->SystemDateTimeHour = rtcTime.hour;
					puiPara->SystemDateTimeMinute = rtcTime.min;
					puiPara->SystemDateTimeSecond = rtcTime.sec;
				}
				
				appScdvMenuSystemDateTimeDisplay();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemRemoterSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			if(item == UI_SCDV_REMOTER_ON)
			{
				menuProcParaSet(layer, item);
				appStateChange(APP_STATE_SCDV_RF_PAIRING, STATE_PARAM_NORMAL_INIT);
				ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				menuProcParaSet(layer, item);
				appRfRelease();
			}
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemFrequnecySet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemFormatSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			if(item == UI_SCDV_CONFIRM_OK)
			{
				appBtnDisable(BTN_ALL);	
				if(appDiskStatusGet() == APP_DISK_STATUS_MOUNTED_OK || appDiskStatusGet() == APP_DISK_STATUS_MOUNTED_FAIL)
				{
					LED_RULES_BUSY;
					
					DBG_PRINT("Format disk%d start ..\n",appActiveDiskGet());	
					if(appDiskFormat(appActiveDiskGet()) == SUCCESS)
					{
						uiPara_t* puiPara = appUiParaGet();
						if (puiPara->fileNumber  == FILENUM_RESET) 
						{
							puiPara->dcfDirKey = 0;
							puiPara->dcfFileKey = 0;
						}
						appDcfNexDcfKeySet((puiPara->fileNumber  == FILENUM_RESET),puiPara->dcfDirKey,puiPara->dcfFileKey);
						appPbBackdoorEventCBF(PB_APP_EVENT_DISK_FORMAT);
						
						//cdfs init???
						appCdfsInit();						
					}
					///appDiskStatusSet(APP_DISK_STATUS_MOUNTED_OK);
					DBG_PRINT("Format disk%d done ..\n",appActiveDiskGet());
					
					LED_RULES_NORMAL;
				}
				else
				{
					appBuzzerStop(TRUE);
					appBuzzerPlay(BUZZER_WARNING);
				}
				appBtnEnable(BTN_ALL);
			}			
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemDefaultSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			if(item == UI_SCDV_CONFIRM_OK)
			{
				appFactorySettingApply();
			}
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemWifiPasswordInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcChild(0);
			appScdvMenuSystemWifiPassWordDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemWifiPasswordSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			menuProcMoveDown();
			appScdvMenuSystemWifiPassWordDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_OK:
			if(item == UI_SCDV_SYSTEM_WIFI_PW_EXIT)
			{
				//don't save and exit
				menuProcParent();
				appScdvMenuDisplay();
				ret = MENU_IGNORE_DEFAULT;
			}
			else if(item == UI_SCDV_SYSTEM_WIFI_PW_OK)
			{
				//save and exit
				appWifiSsidPasswordSet();
				
				menuProcParent();
				appScdvMenuDisplay();			
				ret = MENU_IGNORE_DEFAULT;
			}
			else
			{
				//adjust value
				menuProcChild (menuProcParaGet(layer, item));
				menuProcMoveDown();
				layer = menuProcLayerGet();
				item = menuProcCurrItemGet(layer);
				menuProcParaSet(layer, item);
				menuProcParent();
				layer = menuProcLayerGet();
				item = menuProcCurrItemGet(layer);

				appScdvMenuSystemWifiPassWordDisplay();
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemVersionInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			menuProcChild(0);
			appScdvMenuSystemVersionDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemVersionSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			//appScdvMenuSubMenuEnter(msg, para, TRUE);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_SystemExitSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appScdvMenuParentMenuEnter(msg, para);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			switch (puiPara->ScdvMainMode)
			{
				case UI_SCDV_MAIN_MODE_SPORT:
					menuProcCurrPosSet(MENU_MARK_SPORT_DV);
					break;
				case UI_SCDV_MAIN_MODE_CAR:
					menuProcCurrPosSet(MENU_MARK_CAR_DV);
					break;	
			}
			appScdvMenuDisplay();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_OK:
			appScdvMenuStateMenuExit();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeDelete (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteAllOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT32 FileNumVideo, FileNumJpg;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			printf("will delete all sdv file......\n");
			appCdfsTheAllFileDel(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFileNumGet(&FileNumVideo);

			appCdfsTheAllFileDel(CDFS_FILE_FOLDER_JPG);
			appCdfsFileNumGet(&FileNumJpg);
			
			appGlobal.appCdfsCount = FileNumVideo + FileNumJpg;
			if(appGlobal.appCdfsCount == 0)
			{
				return appMenuEvt_DeleteMenuExit(0, 0, APP_KEY_PRESS_OK, 0);
			}			
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteLastOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT32 FileNumVideo, FileNumJpg;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			printf("will delete last sdv file.......\n");
			__FUNC_TRACK__;
			appCdfsTheNewestFileDel(CDFS_FILE_FOLDER_LAST);
			__FUNC_TRACK__;
			
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);			
			appCdfsFileNumGet(&FileNumVideo);
			
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
			appCdfsFileNumGet(&FileNumJpg);
			__FUNC_TRACK__;

			appGlobal.appCdfsCount = FileNumVideo + FileNumJpg;
			if(appGlobal.appCdfsCount == 0)
			{
				return appMenuEvt_DeleteMenuExit(0, 0, APP_KEY_PRESS_OK, 0);
			}			
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appStateChange(APP_STATE_SPORTDV_PB_DELETE,STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeDeleteLoop (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteLoopAllOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT32 FileNumVideo, FileNumJpg;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			printf("will delete all cdv loop file.......\n");
			appCdfsTheAllFileDel(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFileNumGet(&FileNumVideo);

			appCdfsTheAllFileDel(CDFS_FILE_FOLDER_JPG);
			appCdfsFileNumGet(&FileNumJpg);
			
			appGlobal.appCdfsCount = FileNumVideo + FileNumJpg;
			if(appGlobal.appCdfsCount == 0)
			{
				return appMenuEvt_DeleteLoopMenuExit(0, 0, APP_KEY_PRESS_OK, 0);
			}
			
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteLoopLastOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT32 FileNumVideo, FileNumJpg;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			printf("will delete last cdv loop file.......\n");
			appCdfsTheNewestFileDel(CDFS_FILE_FOLDER_LAST);
			
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);			
			appCdfsFileNumGet(&FileNumVideo);
			
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
			appCdfsFileNumGet(&FileNumJpg);

			appGlobal.appCdfsCount = FileNumVideo + FileNumJpg;
			if(appGlobal.appCdfsCount == 0)
			{
				return appMenuEvt_DeleteLoopMenuExit(0, 0, APP_KEY_PRESS_OK, 0);
			}			
			
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteLoopMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appStateChange(APP_STATE_CARDV_PB_DELETE_LOOPING,STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ModeDeleteEvent (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteEventAllOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT32 FileNumEvent;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			printf("will delete all cdv event file.......\n");
			appCdfsTheAllFileDel(CDFS_FILE_FOLDER_EVENT);
			appCdfsFileNumGet(&FileNumEvent);
			
			appGlobal.appCdfsCount = FileNumEvent;
			if(appGlobal.appCdfsCount == 0)
			{
				return appMenuEvt_DeleteEventMenuExit(0, 0, APP_KEY_PRESS_OK, 0);
			}

			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteEventLastOKSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	UINT32 FileNumEvent;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			printf("will delete last cdv event file.......\n");

			appCdfsTheNewestFileDel(CDFS_FILE_FOLDER_EVENT);
			appCdfsFileNumGet(&FileNumEvent);

			appGlobal.appCdfsCount = FileNumEvent;
			if(appGlobal.appCdfsCount == 0)
			{
				return appMenuEvt_DeleteEventMenuExit(0, 0, APP_KEY_PRESS_OK, 0);
			}			
			
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_DeleteEventMenuExit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			appStateChange(APP_STATE_CARDV_PB_DELETE_EVENT,STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32 appMenuEvt_ScdvPbFileTypeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:
			break;
		case APP_KEY_PRESS_OK:
			if(appDiskStatusGet() == APP_DISK_STATUS_MOUNTED_OK)
			{
				menuProcParaSet(layer, item);

				switch(puiPara->ScdvPbFileType)
				{
					case UI_SCDV_PB_TYPE_VIDEO:
						puiPara->fileType = FILETYPE_VIDEO;
						break;
					case UI_SCDV_PB_TYPE_PHOTO:
						puiPara->fileType = FILETYPE_JPG;
						break;
					case UI_SCDV_PB_TYPE_EVENT:
						puiPara->fileType = FILETYPE_EVENT;
						break;
				}
				
				appStateChange(APP_STATE_HDMI_PB_MAIN,STATE_PARAM_NORMAL_INIT);

			}
			else
			{
				appOsdLib_TextIDDraw(160, 60, SP5K_GFX_ALIGN_CENTER, ID_STR_NO_CARD_);
					
				appBuzzerStop(TRUE);
				appBuzzerPlay(BUZZER_WARNING);
			}
			
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

