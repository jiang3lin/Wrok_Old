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
 /*

 The suggestion video fllow.
 1. set video size.
 2. sp5k mode set to "video_privew." (need to allocate buffer)
 3. sp5k mode set to "video_record" as recording.
 4. set media abort to stop. (to stop and add file name to DCF)
 5. set sp5k mode set to "video_preview."

 */
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_osd.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_rsvblk_api.h"

/* for display osd */
#include "sp5k_gfx_api.h"
#include "app_pal.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"
#include "app_stringid.h"	//gx
#include "app_face_detect_ex.h"
#include "app_ui_para.h"
#include "app_zoom_api.h"
#include "app_still.h"
#include "app_video.h"
#include "app_user_setting.h"
#include "app_sound.h"
#include "app_aaa.h"
#include "app_view_param.h"
#include "app_error_msg.h"
#include "app_view_osd.h"
#include "gpio_custom.h"
#include "app_audio.h"
/*s Add by Aries 09/08/18*/
#include "app_osd_api.h"
/*e Add by Aries 09/08/18*/
#include "app_cdfs_api.h"
#include "Sp5k_usb_api.h"
#include "app_init.h"
#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif
#include "app_menu_tab.h"
#if GPS_SUPPORT
#include "app_gps.h"
#endif
#if ICAT_WIFI
#include "app_wifi_connection.h"
#include "app_ptp.h"
#include "app_wifi_utility.h"
#endif
#include "app_awbalg_api.h"
#include "app_still_timelapse.h"
#if ICAT_FCWS || ICAT_LDWS
#include "app_cvrws_api.h"
#if ICAT_LDWS
#include "sp5k_cvr_warning_system_api.h"
#endif
#endif

#if VIEW_YUV_CAP
#define ROUND_DOWN_TO_DIVISIBLE(num,div) \
	( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div) \
	ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
static UINT32 seamlessReqMemSize;
extern UINT8 cdfsFirstCreate;
extern UINT8 fileTypeSelect;
extern UINT8 countDownRecFlag;
extern UINT8 carchargerDetectFlag;
extern UINT8 gsensorDelayRecTimes;
extern UINT8 gsensorTriggerByIntFlag;
UINT8 GPSIconFlag;
#if CAM_TYPE_CVR
extern UINT32 RTCTimer;
#endif
#if CAP_VID_PV
UINT32 CapVidPreview = 0;
#endif

UINT32 SnapSetflag = 0;
#if SP5K_MOTION_DETECT
UINT32 mdTime=TIMER_NULL;
#endif
extern UINT32 trigFlg;
extern UINT8 timelapseStartTimer;
extern UINT8 timelapse_act;
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
void	appVideoStampInit(void	);
extern void appCdfsInit(void);
#if CAP_VID_PV
extern void appCapVidPreviewInit(void);
#endif
#if VIEW_YUV_CAP
extern void VideoCapYuvCb(UINT32 idx,UINT32 pbuf,UINT32 width,UINT32 hight);
extern void *sp5kYuvBufferAlloc(UINT32 bufW, UINT32 bufH);
extern void sp5kYuvBufferFree(void *ptr);
#endif
extern UINT32 appVideoCapRemainGet(void);
extern void sioCtrl(UINT32);
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appVideoCB_t gVideoCB=
{
	.CapTimer = TIMER_NULL,
	.CapSec = 0,
	.osd = OSD_VIDEO_DEFAULT,
	.fCapture = 0,
	.fPause = 0,
	.isDiskReady=0,
	.isAutoPowerOffOn=1,
	.err=0
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appPowerOnPreviewFreeze(BOOL flag);
extern void appErrMsgTimeISRWithSound(appErrMsgCaseOption_e errMsg,appErrMsgSoundOption_e errSound,UINT32 showTimeInMs);// for clean warning
extern UINT32 appAWBALGLib_WBParamSet( UINT32 sel, UINT32 val );
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#if CAM_TYPE_CVR
#define APP_DEFAULT_REC_AUD_VOL		31

static UINT32 gAudRecVol=APP_DEFAULT_REC_AUD_VOL;
static UINT32 gAudRecVolSave=APP_DEFAULT_REC_AUD_VOL;

/*-------------------------------------------------------------------------
*  File Name : appAudioRecVolumeRestore
*------------------------------------------------------------------------*/
void
appAudioRecVolumeRestore(
	void
)
{
	//printf("\n******************** \n AUD Vol RESTORE !! \n\n");
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, FALSE);
	gAudRecVol = gAudRecVolSave;
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, gAudRecVol);
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST | SP5K_AUD_REC_CH_ALL, 0x80000001);
}

/*-------------------------------------------------------------------------
*  File Name : appAudioRecVolumeMute
*------------------------------------------------------------------------*/
void
appAudioRecVolumeMute(
	void
)
{
	//printf("\n>>>>>>>>>>>>>>>>>>\n AUD Vol MUTE !! \n\n");
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, 1);
	gAudRecVolSave = gAudRecVol;
	gAudRecVol = 0;
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, gAudRecVol);
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST | SP5K_AUD_REC_CH_ALL, 15/* MUTE */);
}

/*-------------------------------------------------------------------------
*  File Name : appAudioRecVolumeConfig
*------------------------------------------------------------------------*/
void
appAudioRecVolumeConfig(
	void
)
{
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, APP_DEFAULT_REC_AUD_VOL);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MUTE, 0);
	/*sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_ON);*/
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
	/* Spca6330 ALC Function */
#if SPCA6330
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MODE, SP5K_MEDIA_REC_ALC_DRC_MODE);
	sp5kMediaAlcParamFileLoad("A:\\RO_RES\\WAV\\ALC.TXT");
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_ON);
#endif
	/* Spca6330 ALC Function */
	gAudRecVol = gAudRecVolSave = APP_DEFAULT_REC_AUD_VOL;
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, gAudRecVol);
	/*s Modify by Aries 10/03/12*/

#if SPCA5310
#if SPCA6330
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST | SP5K_AUD_REC_CH_ALL,
			0x40000001);
#else
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST, 0x40000000);
#endif
#else
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST, 0x80000016);
#endif

#if 0/*SPCA6330 */
		WRITE8(0xb0006404 ,3);
		WRITE8(0xb0006405 ,3);
		WRITE8(0xb0006408 ,1);

#endif
}
#endif /* CAM_TYPE_CVR */


void appVideo_SetFaceTracking(UINT8 command)
{
	appStill_SetFaceTracking(command);
}

UINT32
_VideoCheckDiskReady(void)
{
	uiPara_t* puiPara = appUiParaGet();

	if (!IS_DISK_MOUNT_READY)
	{
		return FALSE;
	}

	gVideoCB.isDiskReady = TRUE;
	if(IS_DISK_MOUNT_FAIL)
	{
		gVideoCB.err = VIEW_ERR_CARD_ERROR;
	}
	else
	{
		gVideoCB.err = VIEW_ERR_NO_ERROR;

		gVideoCB.framePerSec = (IS_CARD_EXIST)? 30: 15;
		appVideoStreamPerByteSet(puiPara->VideoSize,puiPara->VideoQuality, IS_CARD_EXIST);
		DBG_PRINT("%s disk is exist\n", (IS_CARD_EXIST)? "SD" : "NAND");
	}
	return TRUE;
}

UINT32
appVideoCheckReady(UINT32 question)
{
	UINT32 mode, time=0; // for clean warning
	uiPara_t* puiPara = appUiParaGet();

	if(gVideoCB.err == VIEW_ERR_CARD_ERROR)
	{
		return VIDEO_STATE_NO;
	}

	switch (question)
	{
	default:
	case CAN_I_START_RECORD:
		if(!IS_CARD_EXIST){
			return VIDEO_STATE_NO;
		}

		if(puiPara->Seamless)
		{
			switch(puiPara->Seamless)
			{
				case UI_VIDEO_SEAMLESS_1MIN:
					time = 60*2;
					break;
				case UI_VIDEO_SEAMLESS_3MIN:
					time = 180*2;
					break;
				case UI_VIDEO_SEAMLESS_5MIN:
					time = 300*2;
					break;
				case UI_VIDEO_SEAMLESS_OFF:
				default:
					break;
			}
			if(appVideoRemainSecGet() < time){
				return VIDEO_STATE_NO;
			}
			else{
				return VIDEO_STATE_YES;
			}
		}
		if(!gVideoCB.isDiskReady)
		{
			return VIDEO_STATE_NO;
		}
		if(IS_DISK_SD_MOUNT && IS_CARD_LOCK)
		{
			gVideoCB.err = VIEW_ERR_CARD_LOCKED;
			return VIDEO_STATE_NO;
		}
		if(appVideoRemainSecGet()<1)
		{
			return VIDEO_STATE_NO;
		}
		#if SP5K_CDFS_OPT
			if(appCdfsFreeSpaceCheck(CDFS_FILE_FOLDER_VIDEO, TRUE) == FAIL)
			{
				if(appPreviousStateGet()==APP_STATE_POWER_ON)
				{
					APP_OSD_REFRESH_ON;
				}
				gVideoCB.err = VIEW_ERR_MEMORY_FULL;
				return VIDEO_STATE_NO;
			}
		#else
			if(puiPara->Seamless < UI_VIDEO_SEAMLESS_MAX)
			{
				if(appVideoRemainSecGet()<seamlessReqMemSize)
				{
					/* manits 44856 */
					if(appPreviousStateGet()==APP_STATE_POWER_ON)
					{
						APP_OSD_REFRESH_ON;
					}
					gVideoCB.err = VIEW_ERR_MEMORY_FULL;
					//appErrMsgWithSound(ERR_MSG_MEMORY_FULL, ERR_MSG_WITH_WARNING_SOUND, ERR_MSG_SHOW_FOREVER);
					return VIDEO_STATE_NO;
				}
			}
		#endif
		if(appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			printf("lens is moving, can not do aaa\n");
			return VIDEO_STATE_NO;
		}
		break;
	case CAN_I_LEAVE_VIDEO:
		sp5kModeGet(&mode);
		#ifdef NO_VIDEO_PREVIEW_MODE
		if (mode!= SP5K_MODE_STILL_PREVIEW)
		{
		#else
		if (mode!= SP5K_MODE_VIDEO_PREVIEW)
		{
		#endif
			return VIDEO_STATE_NO;
		}
		break;
	}
	return VIDEO_STATE_YES;
}

void appVideo_SetSeamLess(UINT8 idx)
{
	UINT32 time=0;

	switch(idx)
	{
		case UI_VIDEO_SEAMLESS_1MIN:
			time = 60;
			break;
		case UI_VIDEO_SEAMLESS_3MIN:
			time = 180;
			break;
		case UI_VIDEO_SEAMLESS_5MIN:
			time = 300;
			break;
		case UI_VIDEO_SEAMLESS_OFF:
		default:
			break;
	}

	seamlessReqMemSize = 2*time;
	//if(time)
	{
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT,time); /* Second */
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SEAMLESS_CALLBACK, (UINT32)AppVideoSeamlessCallback);
//		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SEAMLESS_CALLBACK, (UINT32)AppVideoSeamlessSetNextDCFKey);
//		AppVideoSeamlessSetNextDCFKey();
	}
}



void
appVideoSettingInit(
	UINT32 flg
)
{
	extern void AppSetDCFKey();
	uiPara_t* puiPara = appUiParaGet();

	if (flg & VIDEO_SET_CAPTURE_MODE)
	{
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MUTE_PERIOD, 100);
		if(puiPara->VideoFormat == VIDEO_FORMAT_H264)
		{
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
		}
		else
		{
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_MJPG);
			if(puiPara->VideoSize == UI_VID_SIZE_FHD)
			{
				puiPara->VideoSize = UI_VID_SIZE_HD_30FPS;
			}
		}
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE, SP5K_MEDIA_CBR);

		#if 0 /*VIDEO_WITH_1080P */
		switch(puiPara->VideoSize)
		{
		#if 1
		case 0:
			gVideoCB.framePerSec =30;
		    break;
		case 1:
			gVideoCB.framePerSec =30;
		    break;
		case 2:
			gVideoCB.framePerSec =60;
		    break;
		case 3:
			gVideoCB.framePerSec =120;
		    break;
		case 4:
			gVideoCB.framePerSec =240;
		    break;
		default:

		    break;
		#else
		default:
			gVideoCB.framePerSec =30;
		    break;
		#endif
		}
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, gVideoCB.framePerSec);
		#else
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, gVideoCB.framePerSec);
		#endif

		#if ICAT_WIFI
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_MOV);
		#else
		if(puiPara->MediaFormat == MEDIA_FORMAT_AVI){
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);
		}
		else
		{
			#if SPCA6330
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_MOV);
			#else
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);
			#endif
		}
		#endif

		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, VIDEO_AUDIO_SAMPLE_RATE);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, VIDEO_AUDIO_SAMPLE_BIT);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, VIDEO_AUDIO_CHANNEL);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, 31 );
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MUTE, 0);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_HB, 500);
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_LB, 100);
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, 31);
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST, 0x80000016);

	}
	if (flg & VIDEO_SET_CAPTURE_SIZE)
	{
		#ifndef NO_VIDEO_PREVIEW_MODE
		appStill_SetResolution(UI_PIC_SIZE_FULL);//clean the still's black strip if need.
		#endif

		appVidSizeSet(puiPara->VideoSize);
	}

	//  recover the breoken files
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_RECOVERY_EN,1);
	appStill_SetMetering(puiPara->AEMetering);
	appStill_SetWB(puiPara->WBMode);
	appStill_SetISO(puiPara->Iso);
	appStill_SetExposure(puiPara->AEMode);
	//appStill_SetAF(puiPara->af);
	appStill_SetShutterSpeed(puiPara->shutter);
	appStill_SetApertureValue(pViewParam->aperture);
	/*appStill_SetPhotoFrame(puiPara->PhotoFrame);*/  /*mask for mantis bug 0047368  0047512*/
	appVideo_SetSeamLess(puiPara->Seamless);
	#if CAM_TYPE_CVR
	appVideo_SetParkingMode(puiPara->ParkingMode);
	#endif

}

void
appVideoPvStateInit(
	UINT32 preState
)
{
	UINT32 mode;
	uiPara_t* puiPara = appUiParaGet();

	LED_NORMAL_ON;
//	if(IS_HDMI_IN){
		appBtnEnable(BTN_ALL);
//	}
	gVideoCB.fCapture = 0;
	gVideoCB.CapSec = 0;

#if 0  /*mark by fan.xiao  , date stamp on/off does not controled by puiPara->Seamless*/
	if(puiPara->Seamless != UI_VIDEO_SEAMLESS_OFF) {
		puiPara->VideoStampMode = UI_STAMP_DATE_TIME;
	}
	else{
		puiPara->VideoStampMode = UI_STAMP_OFF;
	}
#endif

	#ifdef VIDEO_REC_STAMP
	if(puiPara->VideoStampMode)
	{
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_OPTION, SP5K_MEDIA_STAMP_RGB565);
	}
	else
	{
		GPrintf("stamp off");
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_OPTION, 0);
	}
    #endif
	#if SP5K_CDFS_OPT
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
	appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
	#endif
	if(SnapSetflag ==1)
	{
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , 0); /*add for it can contiue to record when recording more than 4G*/
	}

	switch(preState)
	{
	case APP_STATE_VIDEO_AAA:
		gStillCB.s1press=0;
		break;
	case APP_STATE_VIDEO_REC:
			#if ICAT_CVR_WARN_SYS_DBG
		// nothing to do
		#else
		#if ICAT_FCWS || ICAT_LDWS
		sioCtrl(0);
		#endif
		#endif

    #if SP5K_ALGORITHM_AWB
		if(puiPara->WBMode == UI_WB_CUSTOM)/*if awb mode is custom,AWB must be disabled*/
		{
			sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
		}
		else
		{
			sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
		}
   #else
		sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
   #endif
		appPowerSavingEnableSet(TRUE);
//		appVideo_SetDistRsc();
		gVideoCB.isAutoPowerOffOn=1;
	case APP_STATE_STILL_AAA:
		//appAePreviewRestore();
//		appVideo_SetDistRsc();
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			appViewOsd_AllClean();
		}
		#else
		appViewOsd_AllClean();
		#endif
	case APP_STATE_STILL_BUSY:
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			appVideoOsdShow(0,0,gVideoCB.osd);
		}
		#else
		appVideoOsdShow(0,0,gVideoCB.osd);
		#endif
		/*if restart rec,parameter need set again,so we shouldn't break here*/
		if(!appVideoRecAlwaysRestartGet())
		{
			printf("appVideoRecAlwaysRestart no");
			break;
		}
	default:
		/*appSensorPowerDownSet(FALSE);*/
		sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
	#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			sp5kDispDevAttrSet(SP5K_DISP_OSD_WINDOW,0,0,1920,1080);
		}
	#endif
		appAePreviewSet();

		gStillCB.s1press=0;
		gStillCB.s2press=0;
		gStillCB.s1AaaTicket=0;
		gStillCB.s2SnapTicket=0;
		gStillCB.err = VIEW_ERR_NO_ERROR;
		if(IS_VIEW_MODE_CHANGE)
		{
			gVideoCB.osd = OSD_VIEW_SIMPLE;
		}
		gStillCB.cap.CurViewMode = VIDEO_MODE;
		appStill_SetPasmSceneParam(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appStillViewIqSet();

		gVideoCB.isDiskReady = FALSE;
		gVideoCB.err = VIEW_ERR_NO_ERROR;
		gVideoCB.CapTimer = TIMER_NULL;
		gVideoCB.framePerSec = VIDEO_FRAME_RATE;
		gVideoCB.isAutoPowerOffOn=1;
		sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, 0xffffffff);
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);

/*#if SP5K_ALGORITHM_AWB
		sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else*/
		sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);

        #ifdef ALGORITHM_G4_AWB
		appAWBALGLib_WBParamSet ( AWB_PARA_PREVIEW_IMMEDIATELY, 0 );
		appAWBALGLib_WBParamSet ( AWB_PARA_UPDTREFE_FRAME_NUM, 99 );
		appAWBALGLib_WBParamSet ( AWB_PARA_HIGH_SPEED, 1 );
		appAWBALGLib_WBParamSet ( AWB_PARA_MEDIUM_SPEED, 2 );
		appAWBALGLib_WBParamSet ( AWB_PARA_LOW_SPEED, 4 );
		appAWBALGLib_WBParamSet ( AWB_PARA_SW_WHITE_CLIP_EN, 1 );
        #endif

		#if SP5K_DISK_SPI
 	    if(IS_CARD_EXIST)
 	    #endif
 	    {
			if(_VideoCheckDiskReady()==FALSE)
			{
				sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			}
			else
			{
				UINT32 sTime;
				//workaround for remain space is not enough to seamless record
				sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT,&sTime);
				if(appVideoRemainSecGet() < sTime*2){
					gVideoCB.err=VIEW_ERR_MEMORY_FULL;
				}
				profLogAdd(0, "video pv disk ready");
				appVideoRecAlwaysDo();  /*add for mantis bug 0048963*/
			}
 	    }

		#if ICAT_WIFI
		if(!appRtpStreamStateGet())
		#endif
		{/* streaming re-start, no need to reset the dzoom ratio */
			appDZoomReset(APP_DZOOM_UI_MODE_VIDEO_VIEW);
			appStill_SetDZoom(puiPara->DigitalZoom);
		}

		#ifdef VIDEO_REC_STAMP
		if(puiPara->VideoStampMode)
		{
			sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
			appVideoRecStampInit();
			sp5kGfxAttrSet(SP5K_GFX_FONT_ID, SP5K_RES_OSD_FONT, 0, 0, 0);
		//appOsd_FontIconFontSet("UI\\FONT\\DATSTAMP.SFN");
		}
		#endif

		#if ICAT_WIFI && SPCA6330 //Ethan@20121214
		if(!appRtpStreamStateGet())
		#endif
		appVideoSettingInit(VIDEO_SET_ALL);

		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
		appOsdLib_PIPLayerClearDraw();

		if(preState == APP_STATE_POWER_ON)
		{
			#if ICAT_WIFI
			if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
				appVideoOsdShow(0,0,gVideoCB.osd);
			}
			#else
			appVideoOsdShow(0,0,gVideoCB.osd);
			#endif
		}else
		{
			#if ICAT_WIFI
			if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
				appVideoOsdShow(1,1,gVideoCB.osd);
			}
			#else
				appVideoOsdShow(1,1,gVideoCB.osd);
			#endif
		}

		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
		appOsdLib_PIPLayerClearDraw();

		#if (KIT_WITH_HDMI) /* add for mantis 47440 */
		if(IS_HDMI_IN && preState == APP_STATE_POWER_ON)
		{
			sp5kModeGet(&mode);
			#ifdef NO_VIDEO_PREVIEW_MODE
			if (mode!= SP5K_MODE_STILL_PREVIEW)
			{
			#else
			if (mode!= SP5K_MODE_VIDEO_PREVIEW)
			{
			#endif
				appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			}
		}
		#endif

		if(!IS_VIDEO_GROUP(preState) && preState != APP_STATE_POWER_ON)
		{
			sp5kModeGet(&mode);
			#ifdef NO_VIDEO_PREVIEW_MODE
			if (mode!= SP5K_MODE_STILL_PREVIEW)
			{
			#else
			if (mode!= SP5K_MODE_VIDEO_PREVIEW)
			{
			#endif
				// vieo preview mode change take 30ms
				appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			}
			else
			{
				printf("we are in video preview\n");
			}
		}
		#if ICAT_WIFI
		else if(appPreviousStateGet() == APP_STATE_WIFI_CONNECTION){
			sp5kModeGet(&mode);
			if (mode!= SP5K_MODE_VIDEO_PREVIEW){
				appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			}
		}
		#endif
		sp5kModeGet(&mode);
		#ifdef NO_VIDEO_PREVIEW_MODE
		while(mode!= SP5K_MODE_STILL_PREVIEW)
		{
		#else
		while(mode!= SP5K_MODE_VIDEO_PREVIEW)
		{
		#endif
			APP_SLEEP_MS(20);
			sp5kModeGet(&mode);
		}
		APP_OSD_REFRESH_ON;

		#if KIT_WITH_LENS /* %REVIEW%, alex */
		{
			UINT32 qryResult;
			qryResult = appLensQuery(LENS_QUERY_BOOT_DONE);

			if (!qryResult)
			{
				/*-- Lens never booted */
				appLensInit();
				appLensBoot();
			}
			else
			{
				/*-- Lens has booted ever */
				if (appLensQuery(LENS_QUERY_LENS_CLOSED))
				{
					appLensOpen();
				}
			}
			appLensStopWait();
		}
		#endif
		appAudioRecVolumeConfig();
		#if CAM_TYPE_CVR
		if(puiPara->VoiceRec == VOICEREC_OFF){
			appAudioRecVolumeMute();
		}else{
			appAudioRecVolumeRestore();
		}
		#endif
		appPowerOnPreviewFreeze(FALSE);  /* unfreeze preview if lens&3A ready(only for poweron flow)*/
		sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, gStillCB.awbSpeed);
//		appGsensorInit();
//		appTouchInit(320, 240);
		break;
	}
	#if SP5K_DISK_SPI
	if (!IS_CARD_EXIST)
	{
		if(IS_USB_IN)
			appErrMsgTimeISRWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);
		else
			appErrMsgTimeISRWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);
		appBtnDisable(BTN_ALL);
	}
	#endif

	#if ICAT_FCWS
	if(puiPara->DAS_mode){
		puiPara->VideoFaceTracking=UI_FACETRACKING_ON;
	}
	#endif
	if(puiPara->VideoFaceTracking)
	{
		appVideo_SetFaceTracking(VIEW_FT_STATUS_RUN);
	}
	else
	{
		appVideo_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	}
	#if 0
	if(appDiskSpaceGet() < 1){			/* ADD BY SC.LIU */
		appErrMsgTimeISRWithSound(ERR_MSG_CARD_REMAIN_SIZE, ERR_MSG_ONLY, 2000);
	}
	#endif

	#if ICAT_LDWS
	if(puiPara->DAS_mode){
		appLaneDepartureWarnSysParamSet();
		#if ICAT_CVR_WARN_SYS_DBG
		extern UINT8 gVideoSizeTag;
		if (appVidFrameRateGet(gVideoSizeTag) == 30) {
			sp5kPreviewCallbackSet(1, appCvrWarningSystemFrameProc, 640, 360);
		}
		else {
			sp5kPreviewCallbackSet(2, appCvrWarningSystemFrameProc, 640, 360);
		}
		#endif
	}
	else
	{
		appVideo_SetFaceTracking(VIEW_FT_STATUS_PAUSE);
	}
	#endif

	#if SP5K_CDFS_OPT
	if(cdfsFirstCreate == 0)
	{
		if(appDiskMountReady()==TRUE)
		{
			appCdfsInit();
		}
	}
	cdfsFirstCreate=1;
	#endif

	#if ICAT_WIFI
	if(appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)
	#endif
	{
		if(carchargerDetectFlag)
		{
			if(IS_CARD_EXIST)
			{
				sp5kHostMsgSend(APP_KEY_PRESS_S1, 0);
			}
			else
			{
				countDownRecFlag = 0;
				carchargerDetectFlag = 0;
				gsensorDelayRecTimes = 0;
				gsensorTriggerByIntFlag = 0;
			}
		}
		else if(gsensorTriggerByIntFlag == 3)
		{
			gsensorTriggerByIntFlag = 0;
			if(IS_CARD_EXIST)
			{
				sp5kHostMsgSend(APP_KEY_PRESS_S1, 0);
			}
		}
		else if(gsensorTriggerByIntFlag == 50)
		{
			gsensorTriggerByIntFlag = 0;
			if(IS_CARD_EXIST)
			{
				sp5kHostMsgSend(APP_KEY_PRESS_S1, 0);
			}
		}
	}

}

void
appVideoPvStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 sTime;
	UINT32 tmpU32;
	uiPara_t* puiPara = appUiParaGet();

	switch ( msg) {
	case SP5K_MSG_MODE_READY:
		if(param == 0x21)
		{
			appBtnEnable(BTN_ALL);
			#if ICAT_WIFI
			if((appWiFiConnection_UtilityStateGet() >= WIFI_UTILITY_OPEN)){
				appStateChange(APP_STATE_WIFI_CONNECTION, APP_STATE_MSG_INIT);
			}
			else if(appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE){
			//	appModeSet(SP5K_MODE_STANDBY);
			}
			#if 0
			if(app_PTP_Get_DscOpMode(MODE_ACTIVE)!=PTP_DSCOP_MODE_VIDEO_OFF){
				app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_VIDEO_OFF); /* sync with UI video preview state */
			}
			#endif
			#endif
		}
		break;
	case APP_UI_MSG_VIEW_WAIT_DISK:
		if(_VideoCheckDiskReady()==FALSE)
		{
			APP_SLEEP_MS(20);
			sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			break;
		}
		else
		{
			appVideoRecAlwaysDo();  /*add for mantis bug 0048963*/
		}
		#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			appBtnDisable(BTN_ALL);
		}else{
			appBtnEnable(BTN_ALL);
		}
		#else
		appBtnEnable(BTN_ALL);
		#endif

		//workaround for remain space is not enough to seamless record
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT,&sTime);

		if(appVideoRemainSecGet() < sTime*2){
			gVideoCB.err=VIEW_ERR_MEMORY_FULL;
		}

		appVideoOsdShow(0,0,gVideoCB.osd);
		break;
	/* view handling */
	case SP5K_MSG_AE_READY:
	#if SP5K_ALGORITHM_AE
	#else
		appAePreviewAdjust(param);
	#endif
    #if SP5K_MOTION_DETECT
	if(puiPara->MotionDetect == 1)
	{
        appMotionDetectHdr();
	}
    #endif
		break;
	case SP5K_MSG_AWB_READY:
	/*	appAwbPreviewAdjust(param);	*/
		break;

	/* DZOOM Position bar update */
	case APP_UI_MSG_DZOOM_POS_UPDATE:
		tmpU32 = 0xFFUL & param;
		appViewOsd_ZoomFrameDraw(TRUE);
		appViewOsd_DigZoomPosDraw((UINT8)tmpU32);
		break;
	/* TELE/WIDE Self Repeat */
	case APP_UI_MSG_LENS_DZOOM_TELE:
		appVideoDZoomTele();
		break;
	case APP_UI_MSG_LENS_DZOOM_WIDE:
		appVideoDZoomWide();
		break;
	case APP_UI_MSG_VIEW_MEMORY_FULL:
		/*appStateChange(APP_STATE_STILL_BUSY, APP_STATE_MSG_INIT); */
		break;
	#ifdef HW_FACETRACK
	case SP5K_MSG_FACE_TRACK_READY:
	#if ICAT_FCWS
	case SP5K_MSG_VEHICLE_TRACK_READY:
		/*FCWS*/
		if (puiPara->DAS_mode == 0) {
			return;
		}
		#if ICAT_CVR_WARN_SYS_DBG
		APP_OSD_REFRESH_OFF;
		appForwardCollisionWarningInfo();
		APP_OSD_REFRESH_ON;
		#endif
		{
			vehicleDetectGetInfo_t vdInfor;
			static UINT8 fcwsWarnMsgCnt = 0;
			sp5kVehicleDetectResultGet(&vdInfor);
			if (vdInfor.vehicleWarn == 1) {
				fcwsWarnMsgCnt++;
			}
			if (fcwsWarnMsgCnt >= FCWS_WARN_CNT_DBG) {
				fcwsWarnMsgCnt = 0;
				appBeep(FALSE, FALSE);
			}
		}
	#endif
	case SP5K_MSG_FACE_TRACK_TIMEOUT:
		{
			UINT32 nface=0;
			SINT32 err=0;
			sp5kFaceTrackFaceRes_t facelist[12];
			err = appFdResultGet(param, &nface, facelist);
			if (err!=-1)
			{
				appFdDataUpdate(nface,facelist);
				appFd_FaceRectUpdateMsgSend();
			}

		}
		break;
	case SP5K_MSG_FACE_TRACK_STOP:
		break;
	case SP5K_MSG_FACE_TRACK_INVALID:
		break;
	case APP_UI_MSG_FACE_DETECT_OSD_UPDATE:
		if(!appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			if(puiPara->VideoFaceTracking)
			{
				appStillOsd_UpdateFaceOsd(FALSE);
				#if FACE_WITH_AE
				appAeFaceCfg();
				#endif
			}
		}
		break;
#endif
	case SP5K_MSG_TIMER:
		#if CAM_TYPE_CVR
		if(param == RTCTimer){
			#if GPS_SUPPORT
			if(appGpsInfoValidCheck() == TRUE){
				if(puiPara->GPS)
				{
					if(GPSIconFlag == 0)
					{
						GPSIconFlag = 1;
						appViewOsd_GPSModeDraw(TRUE, puiPara->GPS);
					}
					else
					{
						GPSIconFlag = 0;
						appViewOsd_GPSModeDraw(FALSE, 0);
					}
				}
			}
			#endif
		}
		else
		#endif
		if(param == timelapseStartTimer){
			if(puiPara->TimeLapse){
				trigFlg = 1;
			}
		}
		break;
	#if SP5K_MOTION_DETECT
	case APP_UI_MSG_MD_START:
		if(mdTime != TIMER_NULL)
		{
			appTimerClear(&mdTime);
		}
		SnapSetflag =1;
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , 0);
		break;
	#endif
  #if VIEW_YUV_CAP
   case APP_UI_MSG_VID_CAP_STOP:
		sp5kVideoUrgentCallbackSet(0, NULL);
	   	break;
   #endif
   	#if ICAT_LDWS && ICAT_CVR_WARN_SYS_DBG
	case SP5K_MSG_LANE_SHIFT_WARNING:
		if (puiPara->DAS_mode == 0) {
			return;
		}
		appLaneDepartureWarningUpdate(param);
		break;
	#endif
	default:
		break;
	}
}

void
appVideoPvStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	/*UINT32 temp=0; */ // for clean warning
	uiPara_t* puiPara = appUiParaGet();
	UINT32 Remainsize;

	switch (msg)
	{
	case APP_KEY_PRESS_OK:
	case APP_KEY_PRESS_S2:
		if(puiPara->TimeLapse == UI_TIMELAPSE_OFF)
		{
			gStillCB.s2SnapTicket=1;
		}
		else
		{
			/*break;*/	/* mask for timelapse open under video view mode*/
		}
	case APP_KEY_PRESS_S1:
     #if 0//CAM_TYPE_CVR
        #if  (KIT_WITH_HDMI)
		if(IS_HDMI_IN)
		{
			return ;
		}
        #endif
     #endif
		if(appVideoCheckReady(CAN_I_START_RECORD)==VIDEO_STATE_YES)
		{
			if(puiPara->TimeLapse != UI_TIMELAPSE_OFF)
			{
				if(!timelapse_act)
				{
					appTimelapseStart();
				}
				else
				{
					appTimelapseStop();
				}
				break;
			}
			appLensStopWait();
			appVideoRecManulStopSet(0);/*when manual start record,we should restart record auto*/
			/*-- shall wait Lens completes before going focus */
			appVideo_SetDistRsc();

			#if ICAT_LDWS && ICAT_CVR_WARN_SYS_DBG
			if(puiPara->DAS_mode){
				sp5kPreviewCallbackSet(60, NULL, 0, 0);
			}
			#endif
			appStateChange(APP_STATE_VIDEO_AAA, STATE_PARAM_FREE_CARRY);
		}
		else
		{
			SnapSetflag = 0;
			//appViewOsd_WarmingMsgDraw(TRUE, gVideoCB.err, TRUE);
		}
		break;
	case APP_KEY_PRESS_MENU:
		appStillOsd_UpdateFaceOsd(TRUE);
        #if VIDEO_REC_ALWAYS
		appVideoRecAlwaysRestart(1); /*add for mantis bug 0048984*/
		appVideoRecAlwaysMenuEnter();
		#else
		#if ICAT_LDWS && ICAT_CVR_WARN_SYS_DBG
		if(puiPara->DAS_mode){
			sp5kPreviewCallbackSet(60, NULL, 0, 0);
		}
		#endif
	    appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
		#endif
		break;
	case APP_KEY_PRESS_SET:
		gVideoCB.osd++;
		if (gVideoCB.osd >= OSD_VIEW_CLEAN_ALL)
		{
			gVideoCB.osd = OSD_VIEW_SIMPLE;
		}
		appVideoOsdShow(0,0,gVideoCB.osd);
		break;
	case APP_KEY_PRESS_PB:
	  #if !CAM_TYPE_CVR
		#if SP5K_CDFS_OPT
		fileTypeSelect=1;
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
		#else
		appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
		#endif
      #endif
		break;
	case APP_KEY_PRESS_MODE:
		#if CAM_TYPE_CVR
		if(!IS_CARD_EXIST)
		{
			break;
		}
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_STAMP_OPTION, 0);
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		#else
		appStateChange(APP_STATE_AUDIO_REC,STATE_PARAM_NORMAL_INIT);
		#endif
		break;
	case APP_KEY_PRESS_TELE:
		if(puiPara->VideoStampMode == UI_STAMP_OFF){
			appVideoZoomTele();
		}
		break;
	case APP_KEY_PRESS_WIDE:
		appVideoZoomWide();
		break;
	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
		appZoomStop();
		break;
	case APP_KEY_PRESS_DEL:
		break;
	case APP_KEY_PRESS_UP:
		#if ICAT_FCWS || ICAT_LDWS
		if(puiPara->DAS_mode)
		{
			printf("DAS off!\n");
			puiPara->DAS_mode=0;
		}
		else
		{
			printf("DAS on!\n");
			puiPara->DAS_mode=1;
		}

		appCvrwsEnableSet(puiPara->DAS_mode);
		#endif
		break;
#if !CAM_TYPE_CVR
	case APP_KEY_PRESS_DOWN:
		#if SP5K_CDFS_OPT
			#if HW_DISK_SPI
			if(!IS_CARD_EXIST)
			{
				break;
			}
			#endif
			if(appCdfsFileNameValidCheck(SP5K_DCF_FILETYPE_JPG) == SUCCESS)
			{
				appCdfsEventFlagSet(JPG_EVENT_IN_VIDEO);
				appStillSpeedyCapture(1);
			}
		#endif
		break;
#endif
	case APP_KEY_PRESS_LEFT:
		#ifdef HW_PRJ_6330
		CLEAR_OSD_SRC;
		#if !CAM_TYPE_CVR
		appStateChange(APP_STATE_AUDIO_REC,STATE_PARAM_NORMAL_INIT);
		#else
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		#endif
		#else
 		#endif
		break;
	//case APP_KEY_PRESS_OK:
	case APP_KEY_PRESS_RIGHT:
        #if CAP_VID_PV   /*add by JQ*/
			Remainsize = appVideoCapRemainGet();
			if(Remainsize >= 10)
			{
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
				appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
				CapVidPreview=1;
				appModeSet(SP5K_MODE_STILL_PREVIEW);
				appCapVidPreviewInit();
				SnapSetflag = 1;
				appStateChange(APP_STATE_STILL_AAA, STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				gVideoCB.err = VIEW_ERR_MEMORY_FULL;
				appViewOsd_WarmingMsgDraw(TRUE, gVideoCB.err, TRUE);
			}
        #elif VIEW_YUV_CAP
			Remainsize = appVideoCapRemainGet();
			if(Remainsize >= 10)
			{
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
				appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
				sp5kVideoUrgentCallbackSet(2 /*interval*/, VideoCapYuvCb/*pfunc*/);
			}
			else
			{
				gVideoCB.err = VIEW_ERR_MEMORY_FULL;
				appViewOsd_WarmingMsgDraw(TRUE, gVideoCB.err, TRUE);
			}
		#endif
		#if 0
		puiPara->VideoSize=appView_SizeItemIncrease(UI_VID_SIZE_MAX,puiPara->VideoSize);
		/*appVideoSettingInit(VIDEO_SET_ALL); */
		/* ymkao_20111220 fix 0044014,
		 * the appVideoSettingInit already set to video preview mode.
		 */
		appVidSizeSet(puiPara->VideoSize);
		appViewOsd_PicSizeVideoDraw(TRUE,puiPara->VideoSize);
		appVideoStreamPerByteSet(puiPara->VideoSize, uiPara->VideoQuality, IS_CARD_EXIST);
		appVideoOsdDiskShow(TRUE);
		#endif
		#if ICAT_WIFI==1
		printf("\n@@-----WiFi connection, entering WiFi state-----\n");
		appStateChange(APP_STATE_WIFI_CONNECTION, APP_STATE_MSG_INIT);
		#endif
		break;


	default:
		break;
	}

	switch (msg)
	{
		case APP_KEY_PRESS_DEL:
		case APP_KEY_PRESS_MODE:
		case APP_KEY_PRESS_PB:
		case APP_KEY_PRESS_MENU:
		case APP_KEY_PRESS_SET:
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			appBeep(TRUE, FALSE);
			break;
	}
}

void
appVideoPvState(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appVideoPvState");
		#if CAP_VID_PV
		CapVidPreview = 0;
		#endif
		appVideoPvStateInit(appPreviousStateGet());
		#if CAM_TYPE_CVR
		if(RTCTimer == TIMER_NULL){
			appTimerClear(&RTCTimer);
			RTCTimer = appTimerSet(1000,"RTCTimer");
		}
		#endif
		#if ICAT_FCWS || ICAT_LDWS
		if(puiPara->DAS_mode){
			appCvrwsCentralPointCal();
			appCvrwsOsdDraw();
			appCvrwsCentralPointSet();
		}
		#endif
		//appOsdLib_TextIDDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_TOP_LEFT,STRTK_SCENE_PROGRAM);	//gx
		break;
	case APP_STATE_MSG_CLOSE:
		#if ICAT_LDWS && ICAT_CVR_WARN_SYS_DBG
		sp5kPreviewCallbackSet(60, NULL, 0, 0);
		#endif
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appVideoPvStateOnKey(msg,param);
		}
		else
		{
			appVideoPvStateOnMsg(msg,param);
		}
		break;
	}
}



UINT32 appVideoSeamLessSecGet(void)
{
	return seamlessReqMemSize;
}

#if ICAT_WIFI
UINT32 appVideoSizeChange(UINT8 vidsize)
{
	extern void appVideoSetQuality(UINT8 idx);
	uiPara_t* puiPara = appUiParaGet();

	/* recalculation stream bytes per sec according to the video size, but currently RTP bitrate setting not sync with UI */
	appVideoSetQuality(puiPara->VideoQuality);

	appMediaAttrUpdate(vidsize);
	return TRUE;
}

#endif

#if VIEW_YUV_CAP
void
VideoCapYuvCb(    /*add by JQ*/
	UINT32 idx,
	UINT32 pbuf,
	UINT32 width,
	UINT32 hight
)
{
	UINT8 fileName[30];
	UINT32 w,h;

	appCdfsNextFileNameGet(SP5K_DCF_FILETYPE_JPG ,fileName);

#if 0   /*3M /5M*/
	UINT32 scaleW=2560/*2048*/,scaleH=1920/*1536*/;

	UINT8 *pDstYUVAddr;
	sp5kGfxObj_t Dstvidcapjpg;
	sp5kGfxObj_t Srcvidcapjpg;

	scaleW = (scaleW+15)>>4<<4;
	scaleH = (scaleH+15)>>4<<4;

	pDstYUVAddr = (UINT8 *)sp5kYuvBufferAlloc(scaleW,scaleH);

	Dstvidcapjpg.pbuf = pDstYUVAddr;
	Dstvidcapjpg.bufW = scaleW;
	Dstvidcapjpg.bufH = scaleH;
	Dstvidcapjpg.fmt = SP5K_GFX_FMT_YUV422;
	Dstvidcapjpg.roiX = Dstvidcapjpg.roiY = 0;
	Dstvidcapjpg.roiW = scaleW;
	Dstvidcapjpg.roiH = scaleH;
	Dstvidcapjpg.paplaneBuf = NULL;

	w = ROUND_UP_TO_DIVISIBLE(width-15,16);
    h = ROUND_UP_TO_DIVISIBLE(hight-15,16);

	Srcvidcapjpg.fmt = SP5K_GFX_FMT_YUV422;
	Srcvidcapjpg.pbuf = (UINT8 *)pbuf;
	Srcvidcapjpg.bufW = w ;
	Srcvidcapjpg.bufH = h;
	Srcvidcapjpg.roiX = Srcvidcapjpg.roiY = 0;
	Srcvidcapjpg.roiW = w;
	Srcvidcapjpg.roiH = h;

	sp5kGfxObjectScale(&Srcvidcapjpg,&Dstvidcapjpg);

	sp5kResourceJpgSave(Dstvidcapjpg.pbuf, Dstvidcapjpg.bufW , Dstvidcapjpg.bufH, Dstvidcapjpg.bufW , Dstvidcapjpg.bufH, fileName);

	sp5kYuvBufferFree((void *)pDstYUVAddr);
#else  /*2M*/
	w = ROUND_UP_TO_DIVISIBLE(width-15,16);
    h = ROUND_UP_TO_DIVISIBLE(hight-15,16);
	sp5kResourceJpgSave((UINT8 *)pbuf, w , h, w , h, fileName);

#endif

	sp5kHostMsgSend(APP_UI_MSG_VID_CAP_STOP,0);
}
#endif

UINT32 appVideoCapRemainGet(void)
{
	appDiskInfo_t* pDisk;
	UINT32 dskfree;
	UINT32 clustersize = 1024;

	pDisk = appCurDiskInfoGet();
	dskfree = (pDisk->freeSz / clustersize);
	return dskfree;
}
