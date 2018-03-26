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

#include "sp5k_media_api.h"
#include "app_cdfs_api.h"
#include "app_scdv_ui_para_def.h"
#include "app_osd_draw_lcm.h"
#include "app_osd_api_lcm.h"
#include "sp5k_disk_api.h"
#include "app_msg.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TIME_LAPSE_TIMER_UNIT	1000   //ms


/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct stillLapse_s 
{
	UINT32 stillLapseTimerIsr;
	UINT32 stillLapseTime; 
	UINT32 stillLapseTimeCount;	//for trigger capture
	UINT32 stillLapseFrame;
	UINT8 stillLapseFile[CDFS_FILE_NAME_LENGTH];
}stillLapse_t;


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static stillLapse_t gStillLapse;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appShowCurExpAgc(void);
extern void apptbCIPA_Log(UINT32 capCnt);
extern void _processAfterFastAe(UINT32 ae_result);
extern void appSdvStillDateStampset(void);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appSdvStillCapLapseOsdShow(void)
{
	CHAR TimeString[OSD_STRING_SIZE];

	//sprintf(TimeString, "\x01%d:%02d:%02d", gStillLapse.stillLapseTime / 3600, (gStillLapse.stillLapseTime / 60) % 60, gStillLapse.stillLapseTime % 60);
	sprintf(TimeString, "%c%d:%02d:%02d", ID_TEXT_REC_ICON, gStillLapse.stillLapseTime / 3600, (gStillLapse.stillLapseTime / 60) % 60, gStillLapse.stillLapseTime % 60);
	appLcmOsdTipsTextShow(TimeString, FALSE);

	sprintf(TimeString, "%03d", gStillLapse.stillLapseFrame);
	appLcmOsdLargeTextShow(TimeString, FALSE);
	
	appLcmRefresh();
}

void appSdvStillCapLapseTimerIsr(void)
{
	sp5kHostMsgSend(APP_UI_MSG_TIMELAPSE_TIMER, 0);
}

void appSdvStillCapLapseInit(void)
{
	uiPara_t* puiPara = appUiParaGet();

	gStillLapse.stillLapseTimerIsr = appTimerIsrReg(appSdvStillCapLapseTimerIsr, TIME_LAPSE_TIMER_UNIT/appTimerIsrTickIntervalGet());
	/*
	switch(puiPara->SdvPhotoLapseTime)
	{
		case UI_SCDV_PHOTO_LAPSE_TIME_3S:
			gStillLapse.stillLapseTimeCount = 3;
			break;
		case UI_SCDV_PHOTO_LAPSE_TIME_5S:
			gStillLapse.stillLapseTimeCount = 5;
			break;
		case UI_SCDV_PHOTO_LAPSE_TIME_10S:
			gStillLapse.stillLapseTimeCount = 10;
			break;
		case UI_SCDV_PHOTO_LAPSE_TIME_30S:
			gStillLapse.stillLapseTimeCount = 30;
			break;
		case UI_SCDV_PHOTO_LAPSE_TIME_60S:
			gStillLapse.stillLapseTimeCount = 60;
			break;
	}*/
	gStillLapse.stillLapseTime = 0;
	gStillLapse.stillLapseFrame = 0;

	appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
	appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
	appCdfsNextFileNameGet(SP5K_MEDIA_AVI, gStillLapse.stillLapseFile);	
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
	appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
}

void appSdvStillCapLapseCapture(void)
{
	gStillLapse.stillLapseTime ++;
	if((gStillLapse.stillLapseTime % gStillLapse.stillLapseTimeCount) == 0)
	{
		UINT32 param;

		profLogAdd(0, "cap sp5kModeSet()");
		
		appSdvStillDateStampset();

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
		
		sp5kModeSet(SP5K_MODE_STILL_SNAP);	
		///appMsgFlush(APP_MSG_UI_QUE);
		gStillLapse.stillLapseFrame ++;
	}			
}

void appSdvStillCapLapseMerge(void)
{
	UINT8 jpgFile[CDFS_FILE_NAME_LENGTH], dirName[CDFS_FILE_NAME_LENGTH], videoFile[CDFS_FILE_NAME_LENGTH];

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);
	/* Lapse should use MJPG */
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_MJPG);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, 30);/* need sensor check */

	strcpy(jpgFile, appCdfsLastFileNameGet());
	strcpy(videoFile, gStillLapse.stillLapseFile);
	if ( SUCCESS == sp5kMediaPlayControl(SP5K_MEDIA_PLAY_VIDEO_FRM_APPEND, (UINT32)videoFile, (UINT32)jpgFile))
	{
		appCdfsFileDel(jpgFile);
		strcpy(dirName,sp5kFsGetCurPath());
		strcat(dirName, "/");
		strcat(dirName, jpgFile);
		sp5kFsFileDelete(dirName);/*file name with dir path*/
	}
	else
	{
		printf("ERR [%s %d] Append Image File\n",__func__,__LINE__);
	}	
}

void appSdvStillCapLapseRelease(void)
{
	if(gStillLapse.stillLapseTimerIsr != 0)
	{
		appTimerIsrUnreg(&gStillLapse.stillLapseTimerIsr);
		gStillLapse.stillLapseTimerIsr = 0;
	}
	
	///appSdvStillCapLapseMerge(); 
}

void appSdvStillCapLapseStateInit(void)
{
	UINT32 snapPulseW;
	appCalibData_t *pcalib= appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();
	UINT32 param;

	//Global Setting
	appAutoPowerOffEnable(FALSE);
	appLcmPowerSaveEnable(FALSE);

	appSdvStillCapLapseInit();
	appSdvStillCapLapseOsdShow();
	
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
}

void appSdvStillCapLapseStateClose(void)
{
	appSdvStillCapLapseRelease();
	sp5kStillCapAbort();
	appStillWaitJobFinish();

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
	
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillCapStateOnMsg
 *  Description :   sk5p still capture mode will disable all display layers
 					than turn on the image layer in qv and other layer after
 					qv. If you want to show or keep something in non-image
 					layers, you can active disp_osd after into sill capture
 					mode.
 *------------------------------------------------------------------------*/
void appSdvStillCapLapseStateOnMsg(UINT32 msg,UINT32 param)
{
	UINT32 next_state,next_param;
	uiPara_t* puiPara = appUiParaGet();
	#ifdef HW_DIST
	UINT8 dist=0;
	#endif

	switch (msg) 
	{
		case APP_UI_MSG_TIMELAPSE_TIMER:
			appSdvStillCapLapseCapture();
			appSdvStillCapLapseOsdShow();
			break;

		//message flow begain
		case SP5K_MSG_MODE_READY:	//SP5K_MODE_STILL_SNAP
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
		//case SP5K_MSG_MODE_READY:	//SP5K_MODE_STILL_PREVIEW

			break;
		case SP5K_MSG_STILL_STORE_COMPLETE:
			appSdvStillCapLapseMerge();
			LED_RULES_NORMAL;
			break;
		case SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE:
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

void appSdvStillCapLapseStateOnKey(UINT32 msg, UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) 
	{
		case APP_KEY_PRESS_POWER:

			break;
		case APP_KEY_PRESS_OK:
			appSdvStillCapLapseStateClose();
			//appCdfsFileListSecondCreate();			
		 	if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		 	{
				appStateChange(APP_STATE_WIFI_CONNECTION, STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				appStateChange(APP_STATE_SPORTDV_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
			}
			break;
		default:
			break;
	}
}

void appSdvStillCapLapseState(UINT32 msg, UINT32 param)
{
	UINT32 preState=0;

	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("appSdvStillCapLapseState\n");
			profLogAdd(3,"Still Capture State");
			appSdvStillCapLapseStateInit();
			break;
		case APP_STATE_MSG_CLOSE:
			appSdvStillCapLapseStateClose();
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appSdvStillCapLapseStateOnKey(msg,param);
			}
			else
			{
				appSdvStillCapLapseStateOnMsg(msg,param);
			}
			break;
	}
}

