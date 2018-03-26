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

#include "app_pal.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "app_zoom_api.h"
#include "app_lens_api.h"
#include "app_menu.h"
#include "app_still.h"
#include "app_sound.h"
#include "app_exif.h"
#include "app_view_param.h"
#include "app_strobe.h"
#include "app_led.h"
#include "app_view_osd.h"
#include "app_error_msg.h"
#include "app_osd.h"
#include "app_res_def.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_face_detect_ex.h"
#include "app_video.h"
#include "app_calib_api.h"
#include "app_errorosd.h"
#include "app_3aalg.h"
#include "app_ui_para.h" 
#include "app_cdfs_api.h"
#include "app_key_def.h"
#include "app_wifi_utility.h"
#include "app_ptp.h"
#include "app_wifi_connection.h"
#include "app_md.h"
#include "app_pal.h"
#include "app_still_timelapse.h"
#include "app_awbalg_api.h"  
#include "app_view_param_def.h"

#include "app_buzzer.h"
#include "app_osd_api_lcm.h"
#include "app_osd_draw_lcm.h"
#include "app_rf_def.h"
#include "app_scdv_ui_para_def.h"
#include "app_msg.h"
 UINT8  CdvLapseWifiPreStatus=UI_SCDV_VIDEO_MODE_NORMAL;
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

void appCdvWifiModeOsdShow(void)
{
	appLcmOsdClean();
	
	//Mode Icon
	appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SCDV_WIFI_MODE, FALSE);

	//WIFI	
	//appLcmOsdLargeStringShow(ID_STR_WIFI, FALSE);
	appLcmOsdLargeTextShow("OFF", FALSE);

	//Mode Tips
	appLcmOsdModeStringShow(ID_STR_WIFI, FALSE);
	
	//Tips
	switch(appDiskStatusGet())
	{
		case APP_DISK_STATUS_NO_CARD:
			appLcmOsdTipsTextShow(ID_TEXT_CARD_NULL, FALSE);
			break;
		case APP_DISK_STATUS_MOUNTING:
			appLcmOsdTipsTextShow(ID_TEXT_CARD_MOUNTING, FALSE);
			break;
		case APP_DISK_STATUS_MOUNTED_FAIL:
			appLcmOsdTipsTextShow(ID_TEXT_CARD_ERROR, FALSE);
			break;
		case APP_DISK_STATUS_CARD_LOCKED:
			appLcmOsdTipsTextShow(ID_TEXT_CARD_LOCKED, FALSE);
			break;
		case APP_DISK_STATUS_MEMORY_FULL:
			appLcmOsdTipsTextShow(ID_TEXT_CARD_FULL, FALSE);
			break;
	}

	//Battery
	appLcmOsdDrawBattery(TRUE, -1);
	
	appLcmRefresh();
}

void appCdvWifiModeStateInit(UINT32 preState)
{
	UINT32 mode;

	uiPara_t* puiPara = appUiParaGet();	
	appCalibData_t *pcalib= appCalibDataGet();
	
	profLogAdd(0,"appCdvWifiModeStateInit s");
	
	//Global Setting	
	appAutoPowerOffEnable(TRUE);
	appLcmPowerSaveEnable(TRUE);

	//Led setting
	LED_RULES_NORMAL;
	
	//WIFI Setting
	/* Do one time if you come from other state group or view re-entry */	
	switch (preState) 
	{
		case APP_STATE_SPORTDV_STILL_CAPTURE:

			break;
		case APP_STATE_SPORTDV_STILL_CAP_LAPSE:

			break;
		case APP_STATE_SPORTDV_STILL_CAP_BURST:
			sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG ,SNAP_SYS_CFG_BURST_FAST_SENSOR_TRIG,0); 
			break;
		case APP_STATE_SPORTDV_STILL_CAP_SELF_TIMER:

			break;
		case APP_STATE_MENU:

			break;		
		case APP_STATE_POWER_ON:

			break;
		default:		

		break;
	}			

	//disk
	if(appScdvWaitForDiskReady() == TRUE)
	{
		appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
	}
	
	//Osd
	appCdvWifiModeOsdShow();
	
	///sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_OFF);
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
	#ifndef RF_PREVIEW_KEEP 
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
	#else
		if(puiPara->ScdvMainMode == UI_SCDV_MAIN_MODE_SPORT)
		{
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		}
		else
		{
			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
		}
	#endif
	appRfCopyBufferCallbackSet();

	//button setting
	appBtnEnable(BTN_ALL);

	profLogAdd(0,"appCdvWifiModeStateInit e");
}

void appCdvWifiModeStateOnMsg(UINT32 msg, UINT32 param)
{
	switch(msg)
	{
		case APP_UI_MSG_VIEW_WAIT_DISK:
			if(appScdvWaitForDiskReady() == TRUE)
			{
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
				appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);

			}
			break;
		case SP5K_MSG_DISK_MOUNT_COMPLETE:
			appScdvWaitForDiskReady();
			appCdvWifiModeOsdShow();
			break;
		default:

			break;
	}
}

void appCdvWifiModeStateOnKey(UINT32 msg, UINT32 param)
{

	uiPara_t* puiPara=appUiParaGet();
	switch(msg)
	{
		case APP_KEY_PRESS_POWER:
			appModeChangeFlow();
			break;
		case APP_KEY_PRESS_OK:
			if(appDiskStatusGet() == APP_DISK_STATUS_MOUNTED_OK)
			{
				//appWifiSsidPasswordSet();
				nWifiRFMutex=1;
				nCdvLapseWifiRFMutex=1;
				nWifiRecKeySkip=FALSE;
				//view
				appBtnDisable(BTN_OK);
				appMsgFlush(APP_MSG_KEY_QUE);
				CdvLapseWifiPreStatus=puiPara->CdvVideoMode;
				puiPara->CdvVideoMode=UI_SCDV_VIDEO_MODE_NORMAL;
				appStateChange(APP_STATE_WIFI_CONNECTION, STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				appBuzzerStop(TRUE);
				appBuzzerPlay(BUZZER_WARNING);
			}
			break;
	}
}

void appCdvWifiModeState(UINT32 msg, UINT32 param)
{
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("appSdvWifiModeState\n");
			appCdvWifiModeStateInit(appPreviousStateGet());
			break;
		case APP_STATE_MSG_CLOSE:
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg))
			{
				appCdvWifiModeStateOnKey(msg,param);
			}
			else
			{
				appCdvWifiModeStateOnMsg(msg,param);
			}
			break;
	}
}

