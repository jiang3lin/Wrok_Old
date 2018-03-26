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

void appCdvSystemOsdShow(void)
{
	appLcmOsdClean();
	
	appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SCDV_SETUP_MODE, FALSE);
	appLcmOsdLargeTextShow("SET", FALSE);
	appLcmOsdModeStringShow(ID_STR_SETUP, FALSE);
	//appLcmOsdTipsTextShow("WIFI OFF", FALSE);
	appLcmOsdDrawBattery(TRUE, -1);

	appLcmRefresh();
}

void appCdvSystemStateInit(UINT32 preState)
{
	UINT32 mode;

	uiPara_t* puiPara = appUiParaGet();	
	appCalibData_t *pcalib= appCalibDataGet();
	
	profLogAdd(0,"appCdvSystemStateInit s");
	
	appPowerSavingEnableSet(TRUE);

	appCdvSystemOsdShow();

	/* enter standby mode */
	////sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_OFF); //de06
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
	#ifndef RF_PREVIEW_KEEP 
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
	#else
		#if 1
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
	#endif
	appRfCopyBufferCallbackSet();

	//button setting
	appBtnEnable(BTN_ALL);
	
	profLogAdd(0,"appCdvSystemStateInit e");
}

void appCdvSystemStateOnMsg(UINT32 msg, UINT32 param)
{
	switch(msg)
	{
		default:

			break;
	}
}

void appCdvSystemStateOnKey(UINT32 msg, UINT32 param)
{	
	switch(msg)
	{
		case APP_KEY_PRESS_POWER:				
			appModeChangeFlow();
			break;
		case APP_KEY_PRESS_OK:
			appStateChange(APP_STATE_SCDV_MENU, STATE_PARAM_NORMAL_INIT);
			break;
	}
}

void appCdvSystemState(UINT32 msg, UINT32 param)
{
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("appCdvSystemState\n");
			appCdvSystemStateInit(appPreviousStateGet());
			break;
		case APP_STATE_MSG_CLOSE:
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg))
			{
				appCdvSystemStateOnKey(msg,param);
			}
			else
			{
				appCdvSystemStateOnMsg(msg,param);
			}
			break;
	}
}

