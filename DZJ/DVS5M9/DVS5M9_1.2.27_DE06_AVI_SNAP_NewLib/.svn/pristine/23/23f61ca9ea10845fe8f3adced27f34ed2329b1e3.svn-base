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


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appShowCurExpAgc(void);
extern void apptbCIPA_Log(UINT32 capCnt);
extern void _processAfterFastAe(UINT32 ae_result);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appSdvStillCapOsdShow(void)
{

}

void appSdvStillCapStateInit(void)
{
	UINT32 snapPulseW;
	appCalibData_t *pcalib= appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();
	UINT32 param = 0;

	//Global Setting
	appAutoPowerOffEnable(FALSE);
	appLcmPowerSaveEnable(FALSE);

	appRfCopyBufferCallbackRelease();
	
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, 0);

	if (pcalib->CIPASign ==1)
	{
		pcalib->CIPACapCnt++;
		apptbCIPA_Log(pcalib->CIPACapCnt);
	}

	appHostMsgWait(SP5K_MSG_AE_READY, &param, 1000);
	_processAfterFastAe(appAeFastAdjust(param));

	appShowCurExpAgc();
	appAeCaptureSet(0);
	appShowCurExpAgc();

	profLogAdd(0, "cap iqset -s");
	appStillSnapIqSet();	
	profLogAdd(0, "cap iqset -e");

	appShowCurExpAgc();

	profLogAdd(0, "cap sp5kModeSet()");
	RFRecCaptureWaitSetting();
	sp5kModeSet(SP5K_MODE_STILL_SNAP);	
}

void appSdvStillCapStateClose(void)
{

}

void appSdvStillCapStateWifiMsg(UINT32 msg,UINT32 param)
{
	switch(msg)
	{
		case SP5K_MSG_STILL_STORE_COMPLETE:
			/* Notify the pima capture is done */
			if(appPtpStatGet())
			{
				UINT32 dskFree;
				sp5kUsbPimaCapDoneNotify(0);
				if (appStillBackgroundGetFree(&dskFree) == SUCCESS)
				{
					if(IS_CARD_EXIST)
						appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_SD, 0, 0);
					else
						appStill_PIMA_Send_Event(PTP_EC_STORAGE_INFO_CHANGED, SP5K_DRIVE_NAND, 0, 0);

					printf("[PIMA Storage Info Changed] dsk free: %d bytes = %d image capacity\n", dskFree, dskFree/gStillCB.estFileSize);
				}
			}
			break;
	}	
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillCapStateOnMsg
 *  Description :   sk5p still capture mode will disable all display layers
 					than turn on the image layer in qv and other layer after
 					qv. If you want to show or keep something in non-image
 					layers, you can active disp_osd after into sill capture
 					mode.
 *------------------------------------------------------------------------*/
 void appSdvStillCapStateOnMsg(UINT32 msg,UINT32 param)
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
		/* backgroud store */	
		case SP5K_MSG_STILL_SAVE_PROGRESSIVE:
			LED_RULES_CAPTURE;
			break;
		//case SP5K_MSG_MODE_READY:	//SP5K_MODE_STILL_SNAP

			break;
		case SP5K_MSG_STILL_STORE_COMPLETE:
			LED_RULES_NORMAL;
			break;
		case SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE:
			appSdvStillCapStateClose();
			RFRecCaptureWaitRelease();
			
 		 	if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		 	{
				appStateChange(APP_STATE_WIFI_CONNECTION, STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				appStateChange(APP_STATE_SPORTDV_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
				
			}
			///sp5kHostMsgSend(APP_UI_MSG_RF_RCWD01_REC_START, KEY_MSG_PARAM_DIRECT_RUN);
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

void appSdvStillCapStateOnKey(UINT32 msg, UINT32 param)
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

void appSdvStillCapState(UINT32 msg, UINT32 param)
{
	UINT32 preState=0;

	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("appSdvStillCapState\n");
			profLogAdd(3,"appSdvStillCapState");
			appSdvStillCapStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appStillWaitJobFinish();
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appSdvStillCapStateOnKey(msg,param);
			}
			else
			{
				
				appSdvStillCapStateOnMsg(msg,param);
			}
			break;
	}
}

