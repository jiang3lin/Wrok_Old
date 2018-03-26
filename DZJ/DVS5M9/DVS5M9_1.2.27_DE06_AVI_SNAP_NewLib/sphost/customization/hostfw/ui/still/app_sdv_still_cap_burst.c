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
#include "sp5k_gfx_api.h"

#include "app_still.h"
#include "app_view_param.h"
#include "app_strobe.h"
#include "app_osd_api.h"
#include "app_error_msg.h"
#include "app_view_osd.h"
#include "app_osd.h"
#include "app_flash_api.h"
#include "app_calib_api.h"
#include "sp5k_ftrack_api.h"
#include "app_ui_para.h" 
#include "app_ptp.h"
#include "app_wifi_connection.h"
#include "app_wifi_utility.h"

#include "app_init.h"
#include "app_scdv_ui_para_def.h"
#include "app_osd_draw_lcm.h"
#include "app_osd_api_lcm.h"
#include "app_cdfs_api.h"
#include "sp5k_disk_api.h"
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
typedef struct stillBurst_s 
{
	UINT32 stillBurstFileCnt;
}stillBurst_t;


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
stillBurst_t gStillBurst;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appShowCurExpAgc(void);
extern void apptbCIPA_Log(UINT32 capCnt);
extern void _processAfterFastAe(UINT32 ae_result);
extern void appSdvStillCapStateWifiMsg(UINT32 msg,UINT32 param);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appSdvStillCapBurstOsdShow(UINT32 param)
{
	CHAR String[OSD_STRING_SIZE];
	sprintf(String, "%03d", gStillBurst.stillBurstFileCnt + param);
	appLcmOsdLargeTextShow(String, FALSE);

	appLcmRefresh();
}

void appSdvStillCapBurstStateInit(void)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT8 AEB = puiPara->AEB;
	UINT32 cfg;
	UINT32 param;

	//Global Setting
	appAutoPowerOffEnable(FALSE);
	appLcmPowerSaveEnable(FALSE);
	
	appRfCopyBufferCallbackRelease();

	appCdfsFileNumGet(&gStillBurst.stillBurstFileCnt);

	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);	
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, 0);

	appHostMsgWait(SP5K_MSG_AE_READY, &param, 1000);
	_processAfterFastAe(appAeFastAdjust(param));

	appShowCurExpAgc();
	appAeCaptureSet(0);
	appShowCurExpAgc();

	profLogAdd(0, "cap iqset -s");
	appStillSnapIqSet();	
	profLogAdd(0, "cap iqset -e");

	appShowCurExpAgc();
	
	appAeAebSet(AEB);	
	AEB = 0;
	if(AEB)
	{
		//AEB
		cfg=3;
	}
	else 
	{
		//Burst
		switch(puiPara->SdvPhotoBurstNumber)
		{
			case UI_SCDV_PHOTO_BURST_3:
				cfg=3;
				break;
			case UI_SCDV_PHOTO_BURST_5:
				cfg=5;
				break;
			case UI_SCDV_PHOTO_BURST_10:
				cfg=10;
				break;
		}
	}	
	sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_LENGTH, &cfg); 
	//cfg = 0;
	//sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_INTERVAL, &cfg); 
	
	/* no date stamp */ 
	sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, NULL); 

	/* no Qv display  
	cfg=10;
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, &cfg);  */
	
	/* no delay */ 
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY,0); 
	
	/* no yuv callback */ 
	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_YUV, NULL); 
	
	/* compression ratio is 1/8 */ 
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_MAX_COMPRESSION_RATIO,8); 
	
	/* fast burst mode */ 
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_CFG_BURST_FAST_SENSOR_TRIG, (AEB) ? (0) : (1));
	
	cfg = SP5K_CAPTURE_BURST_RGB;
	sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, &cfg); 

	
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_FAST_BK_PREVIEW, 0); 
	
	sp5kStillCapYUVCallbackSet(NULL); 
	sp5kStillCapRawCallbackSet(NULL); 
	
	DBG_PRINT("start %s snap\n", AEB ? "AEB":"Burst");
	
	profLogAdd(0, "burst sp5kModeSet()");
	RFRecCaptureWaitSetting();
	sp5kModeSet(SP5K_MODE_STILL_SNAP);

}

void appSdvStillCapBurstStateClose(void)
{
	printf("\033[32mappSdvStillCapBurstStateClose!!\033[0m\n");
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_CFG_BURST_FAST_SENSOR_TRIG,0); 
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_MAX_COMPRESSION_RATIO,SNAP_MAX_COMPRESSION_RATIO);/**/
	appStillViewIqSet();
	appAePreviewSet();
	appAePreviewRestore();
	sp5kStillCapAbort();		

	appStillWaitJobFinish();
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillCapStateOnMsg
 *  Description :   sk5p still capture mode will disable all display layers
 					than turn on the image layer in qv and other layer after
 					qv. If you want to show or keep something in non-image
 					layers, you can active disp_osd after into sill capture
 					mode.
 *------------------------------------------------------------------------*/
void appSdvStillCapBurstStateOnMsg(UINT32 msg,UINT32 param)
{
	UINT32 next_state,next_param;
	BOOL is_yuv_cb_on=FALSE;
	#ifdef HW_DIST
	UINT8 dist=0;
	#endif
	uiPara_t* puiPara = appUiParaGet();

	appSdvStillCapStateWifiMsg(msg, param);

	switch (msg) 
	{
		//message flow begain
		case SP5K_MSG_MODE_READY:	//SP5K_MODE_STILL_PREVIEW
			DBG_PRINT("SP5K_MSG_MODE_READY: remain=%d param=%x\n",gStillCB.remainImg ,param);
			break;
		case SP5K_MSG_STILL_READOUT_START:

			break;
		case SP5K_MSG_MEDIA_SOUND_FINISH:
			appBuzzerPlay(BUZZER_CAPTURE);
			break;
		case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
			appSdvStillCapBurstOsdShow(param);
			break;			
		case SP5K_MSG_STILL_QV_TIMEOUT:
			DBG_PRINT("Cap msg: QV_TIMEOUT\n");
			break;
		case SP5K_MSG_STILL_VIEW_SETTING_NOTIFY:
			DBG_PRINT("Cap msg: VIEW_SETTING_NOTIFY\n");
		     	appAePreviewSet();//expidx ,agcidx setting
			appAePreviewRestore();
			//appStill_SetSp5kStillSize(UI_PIC_SIZE_FULL);//size set
			appStillViewIqSet();//IQ resource load

			#ifdef HW_DIST
			appAeDistStatusGet(&dist);
			if (dist ==TRUE)
			{
				appStill_SetStabilizer(puiPara->Stabilizer);	
			}
			#endif
			
			break;
		case SP5K_MSG_STILL_SAVE_PROGRESSIVE:
			LED_RULES_CAPTURE;
			break;
		//case SP5K_MSG_MODE_READY:	//SP5K_MODE_STILL_SNAP

			break;
		case SP5K_MSG_STILL_STORE_COMPLETE:
			LED_RULES_NORMAL;
			break;			
		case SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE:
			appSdvStillCapBurstStateClose();
			RFRecCaptureWaitRelease();
		 	if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		 	{
				appStateChange(APP_STATE_WIFI_CONNECTION, STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				appStateChange(APP_STATE_SPORTDV_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
			}
			break;
		//message flow end
		
		case SP5K_MSG_STILL_STORAGE_FULL:
			sp5kStillCapAbort();
			break;
		case SP5K_MSG_STILL_CONFIRM_STORE:

			break;
		default:
			break;
	}
}

void appSdvStillCapBurstStateOnKey(UINT32 msg, UINT32 param)
{
	UINT32 temp=0;
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:

			break;
		case APP_KEY_PRESS_OK:

			break;
		default:
			break;
	}
}

void appSdvStillCapBurstState(UINT32 msg, UINT32 param)
{
	UINT32 preState=0;

	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("StillCapState\n");
			profLogAdd(3,"Still Capture State");
			appSdvStillCapBurstStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appSdvStillCapBurstStateClose();
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appSdvStillCapBurstStateOnKey(msg,param);
			}
			else
			{
				
				appSdvStillCapBurstStateOnMsg(msg,param);
			}
			break;
	}
}


