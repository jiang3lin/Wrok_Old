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
#include "app_com_api.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "app_pal.h"
#include "app_osd.h"
#include "gpio_custom.h"

#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"

#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_disk_api.h"

#include "app_audio.h"
#include "app_btn_def.h"
#include "app_still.h"
#include "app_jpg_draw.h"
#include "app_error_msg.h"
#include "app_view_osd.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_res_def.h"
#include "app_sound.h"
#include "app_battery.h"
#include "app_video.h"
#include "Sp5k_usb_api.h"
#include "app_error_msg.h"
#include "app_cdfs_api.h"
#include "app_msg.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define APP_DEFAULT_REC_AUD_VOL		31/*24/31*/
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appAudioCB_t gAudioCB;
UINT32 AudioRemainTime = 0;
extern UINT8 fileTypeSelect;
extern UINT8 cdfsFirstCreate;

static BOOL gIsAudRecMute=FALSE;
static UINT32 gAudRecVol=APP_DEFAULT_REC_AUD_VOL;
static UINT32 gAudRecVolSave=APP_DEFAULT_REC_AUD_VOL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

static void _AudioKeyCtrl_BUSY(void)
{
	appBtnDisable(BTN_ALL);
}

static void _AudioKeyCtrl_STANDBY(void)
{
	appBtnDisable(BTN_ALL);
	#ifdef HW_PRJ_6330
	appBtnEnable(BTN_DEL|BTN_S1|BTN_S2|BTN_LEFT|BTN_TV);
	#else
	appBtnEnable(BTN_PB|BTN_S1|BTN_S2|BTN_MODE|BTN_TV);
	#endif
}

static void _AudioKeyCtrl_REC_START(void)
{
	appBtnDisable(BTN_ALL);
	appBtnEnable(BTN_S1|BTN_S2|BTN_UP|BTN_PB|BTN_MODE|BTN_TV);
}

static void _AudioKeyCtrl_EXIT(void)
{
	appBtnEnable(BTN_ALL);
}

UINT32
_AudioCheckDiskReady(void)
{
	gAudioCB.isDiskReady = FALSE;
	if (!IS_DISK_MOUNT_READY)
	{
		return FALSE;
	}
	gAudioCB.isDiskReady = TRUE;

	gAudioCB.recSec = 0;
	if(IS_DISK_MOUNT_FAIL)
	{
		DBG_PRINT("card error\n");
		gAudioCB.err = VIEW_ERR_CARD_ERROR;
		gAudioCB.isDiskError = TRUE;
	}
	else
	{
		gAudioCB.err = VIEW_ERR_NO_ERROR;
		gAudioCB.isDiskError = FALSE;
	}
	return TRUE;
}

/*-------------------------------------------------------------------------
*  File Name : appAudioDiskRemainGet
*------------------------------------------------------------------------*/
UINT32
appAudioDiskRemainGet(
	void
)
{
	UINT64 n, oneSecSize,diskFreeSpace,cluSz;
	appDiskInfo_t* pDisk;

	pDisk = appCurDiskInfoGet();
	cluSz = (UINT64)appDiskClusterSizeGet(); /* in Bytes */
	diskFreeSpace = (UINT64)appDiskFreeClusterGet(pDisk->actDsk)*cluSz;
	oneSecSize = (UINT64)AUDIO_ONE_SECOND_FILE_SIZE;
	n = (/*oneSecSize/2*/0 + diskFreeSpace)/oneSecSize;

	DBG_PRINT("Audio: remaining %d sec \n", (UINT32)n);

	return (UINT32)n;
}

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
	gIsAudRecMute = FALSE;
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
	gIsAudRecMute = TRUE;
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
	
	/*e Modify by Aries 10/03/12*/
	gIsAudRecMute = FALSE;
}

/*-------------------------------------------------------------------------
*  File Name : appAudioRecDenoiseConfig
*------------------------------------------------------------------------*/
void
appAudioRecDenoiseConfig(
	void
)
{
	#if SPCA5330
	sp5kAudFilterNotchCfg_t NotchCfg;
	
	NotchCfg.chnlId=0;
	NotchCfg.Fsample =44100;
	NotchCfg.firstNotch.en=1;
	NotchCfg.firstNotch.Fcentre=8210;
	NotchCfg.firstNotch.Qfactor=10;
	NotchCfg.secondNotch.en=0;
	NotchCfg.secondNotch.Fcentre=8000;
	NotchCfg.secondNotch.Qfactor=10;
	NotchCfg.thirdNotch.en=0;
	NotchCfg.thirdNotch.Fcentre=8000;
	NotchCfg.thirdNotch.Qfactor=10;
	NotchCfg.fourthNotch.en=0;
	NotchCfg.fourthNotch.Fcentre=8000;
	NotchCfg.fourthNotch.Qfactor=10;
	NotchCfg.fifthNotch.en=0;
	NotchCfg.fifthNotch.Fcentre=8000;
	NotchCfg.fifthNotch.Qfactor=10;
		
	sp5kAudFilterNotchCfg(&NotchCfg);
	#endif
}


/*-------------------------------------------------------------------------
*  File Name : appAudioCheckReady
*------------------------------------------------------------------------*/
UINT32
appAudioCheckReady(UINT32 question)
{
	//UINT32 mode;
	appDiskInfo_t* pDskInf;

	if(gAudioCB.err == VIEW_ERR_CARD_ERROR)
	{
		return AUDIO_STATE_NO;
	}

	switch (question) {
	default:
	case CAN_I_START_AUDIO_RECORD:
		pDskInf = appCurDiskInfoGet();
		if(pDskInf->actDsk == SP5K_DRIVE_RSVA) {
			return AUDIO_STATE_NO;
		}
		if(IS_DISK_SD_MOUNT && IS_CARD_LOCK) {
			gAudioCB.err = VIEW_ERR_CARD_LOCKED;
			return AUDIO_STATE_NO;
		}
		printf("pDskInf->freeSz =%d %d\n",((pDskInf->freeSz)<<10),AUDIO_ONE_SECOND_FILE_SIZE);
		if(((pDskInf->freeSz)<<10) < AUDIO_ONE_SECOND_FILE_SIZE) {
			gAudioCB.err = VIEW_ERR_CARD_LOCKED;
			return AUDIO_STATE_NO;
		}
		break;
	case CAN_I_LEAVE_AUDIO:
		break;
	}
	return AUDIO_STATE_YES;
}

void
appAudioRecStart(void)
{
	LED_FLICKER_AUDIO_RECORDING
	DBG_PRINT("__%s()__\n",__FUNCTION__);
	AudioRemainTime = appAudioDiskRemainGet();
	gAudioCB.recStatus = AUDIO_REC_STATUS_RECORD_STARTING;
	gAudioCB.recTimer = appTimerSet(200, "Audio");
	gAudioCB.recSec = 0;
	appModeSet(SP5K_MODE_AUDIO_RECORD);
}

extern UINT16 LastProgBarLength;
void
appAudioRecStop(void)
{
	_AudioKeyCtrl_BUSY();
	LED_NORMAL_ON;
	DBG_PRINT("__%s()__\n",__FUNCTION__);
	/*
	 * modify by kevin 20110728 for fix 0043079 issue.
	 * warning: here MUST switch to IDLE mode to stop audio record,
	 * otherwise the current file will not be updated into DCF pool.
	 */
	#if 0
	sp5kMediaRecControl(SP5K_MEDIA_REC_ABORT);
	#else
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP,1);  /* 0 :not duplicate  .1 :duplicate */ 
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_FORCE, 1);/*1: enter standby mode even already in, 0: enter standby mode if not in*/ 
	sp5kModeSet(SP5K_MODE_STANDBY);//perhap clean img here
	sp5kModeWait(SP5K_MODE_STANDBY);
	#endif
	gAudioCB.recStatus = AUDIO_REC_STATUS_STOP;
	gAudioCB.recSec = 0;
	if (gAudioCB.deferredStopTimer != TIMER_NULL) 
	{
		appTimerClear(&gAudioCB.deferredStopTimer);
	}
	LastProgBarLength = 0;
	/*-- Bad programming habit !!,
	 * But I dont want to change too much at this moment !!
	 */
	appTimerClear(&gAudioCB.recTimer);
	_AudioKeyCtrl_STANDBY();
}

#ifdef APP_AUDIO_REC_PAUSE_RESUME_SUPPORT
void
appAudioRecPause(void)
{
	LED_NORMAL_ON;
	DBG_PRINT("__%s()__\n",__FUNCTION__);
	sp5kMediaRecControl(SP5K_MEDIA_REC_PAUSE);
	gAudioCB.recStatus = AUDIO_REC_STATUS_PAUSE;
	appTimerClear(&gAudioCB.recTimer);
	appAudioOsdShow(1);
}

void

appAudioRecResume(void)
{
	LED_FLICKER_AUDIO_RECORDING;
	DBG_PRINT("__%s()__\n",__FUNCTION__);
	sp5kMediaRecControl(SP5K_MEDIA_REC_RESUME);
	gAudioCB.recStatus = AUDIO_REC_STATUS_RECORDING;
	gAudioCB.recTimer = appTimerSet(1000, "Audio");
	appAudioOsdShow(1);
}
#endif

UINT32
appAudioRecStateInit(void)
{
	
	gStillCB.cap.CurViewMode = AUDIO_MODE;

	gAudioCB.recStatus = AUDIO_REC_STATUS_NULL;

	#if (KIT_WITH_HDMI)
	if(appPreviousStateGet() == APP_STATE_VIDEO_PREVIEW && IS_HDMI_IN){
		//sp5kDispDevAttrSet(SP5K_DISP_OSD_WINDOW,240,0,1440,1080);
		sp5kDispDevAttrSet(SP5K_DISP_OSD_WINDOW,0, 0, 1920, 1080);
	}
	#endif
	appModeSet(SP5K_MODE_STANDBY);

	/*-- Background JPG */
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		appOsdIconModeSet(APP_ICON_MODE_AUDIO_HDMI);
	}else{
		appOsdIconModeSet(APP_ICON_MODE_AUDIO);
	}
	#else
	appOsdIconModeSet(APP_ICON_MODE_AUDIO);
	#endif

	appJpgDraw_ImgLayer(RES_JPG_AUDIO_BG, 0,0,1920,1080);
	appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
	appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
	/*-- Check if DISK Error !! */
	#if SP5K_DISK_SPI
 	if(IS_CARD_EXIST)
 	#endif
 	{
		if (gAudioCB.isDiskError) 
		{
			APP_OSD_REFRESH_OFF;
			CLEAR_OSD_SRC;
			/*-- Set ICON Font file */
			appOsd_FontIconFontSet(ID_FONT_ICON);
			/*-- Mode ICON */
			appIconShow(ID_ICON_MODE_VOICE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
			/*-- Battery ICON */
			appViewOsd_BatteryDraw(TRUE, appBatteryLevelGet());
			/*-- Disk remaining time */
			appAudioOsdDiskShow();
			/*-- Warning Msg */
			appErrMsgWithSound(ERR_MSG_CARD_ERROR,
							ERR_MSG_WITH_WARNING_SOUND,
							ERR_MSG_SHOW_FOREVER);
			APP_OSD_REFRESH_ON;
			return FAIL;
		}
 	}
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_UNKNOWN);
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_WAV);
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, AUDIO_REC_SAMPLING_RATE);
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, AUDIO_REC_SAMPLE_BITS);
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CHANNELS, VIDEO_AUDIO_CHANNEL); 
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DISK_THR_KB, 0);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT,0); /* Second */ 

	appAudioRecVolumeConfig(); /*-- Control Volume */

	gAudioCB.recStatus = AUDIO_REC_STATUS_STOP;
	gAudioCB.recSec = 0;
	gAudioCB.deferredStopTimer = TIMER_NULL;
	gAudioCB.osd = 0;
	gAudioCB.state = AUDIO_REC_STATE_INIT;
	gAudioCB.s1press = 0;
	gAudioCB.err = VIEW_ERR_NULL;
	#if !(SP5K_DISK_SPI)
		AudioRemainTime = appAudioDiskRemainGet();
    #endif 
	CLEAR_OSD_SRC;
	sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0, PAL_TRANSPARENCY);
	appAudioOsdStatusDraw(gAudioCB.recStatus);
	APP_OSD_REFRESH_OFF; //appOsd_GfxOsdRefreshSet(FALSE);
	
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
	APP_OSD_REFRESH_ON; //appOsd_GfxOsdRefreshSet(TRUE);

	_AudioKeyCtrl_STANDBY();

	return SUCCESS;
}

void
appAudioRecStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 elsTime, recRemainingSec;
	switch ( msg) {
	/* storage check */
	case APP_UI_MSG_VIEW_WAIT_DISK:
		DBG_PRINT("__OmMsg: WAIT_DISK \n");
		#if SP5K_DISK_SPI
 		if(IS_CARD_EXIST)
 		#endif
 			{
		if(_AudioCheckDiskReady()==FALSE)
		{
			APP_SLEEP_MS(20);
			sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			break;
		} 
		else
		{	
			
			sp5kHostMsgSend(APP_UI_MSG_AUDIO_REC_INIT, 0);
		}
 			}
		break;

	/* state initialization */
	case APP_UI_MSG_AUDIO_REC_INIT:
		DBG_PRINT("__OmMsg: REC_INIT \n");
		appAudioRecStateInit();
		break;
	case SP5K_MSG_MEDIA_REC_START:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{		
			appPowerSavingEnableSet(FALSE);
			DBG_PRINT("__OmMsg: REC_START(OKAY)\n");
			gAudioCB.recStatus = AUDIO_REC_STATUS_RECORDING;
			_AudioKeyCtrl_REC_START();
			appAudioOsdShow(1);
		} 
		else
		{
			DBG_PRINT("__OmMsg: REC_START(FAIL) !!\n");
			printf("Audio: !!FATAL!! REC Start Failed !! \n");
			_AudioKeyCtrl_STANDBY();
		}
		break;
	case SP5K_MSG_MEDIA_REC_STOP:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{		
			appPowerSavingEnableSet(TRUE);
			DBG_PRINT("__OmMsg: REC_STOP(OKAY)\n");
			_AudioKeyCtrl_STANDBY();
			if(appNextStateGet() != APP_STATE_NULL)
			{
				appTimerClear(&gAudioCB.deferredStopTimer);
				_AudioKeyCtrl_EXIT();
				appStateCloseDone();
			}
			else
			{
				appAudioOsdShow(1);
			}
		}
		break;
	case SP5K_MSG_TIMER:
		if ((param == gAudioCB.recTimer)
			 && (gAudioCB.recStatus == AUDIO_REC_STATUS_RECORDING))
		{
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
			elsTime = (elsTime+500)/1000;
			if(gAudioCB.recSec != elsTime)
			{
				gAudioCB.recSec = elsTime;
				if (AudioRemainTime==elsTime)
				{
					sp5kHostMsgSend(SP5K_MSG_MEDIA_REC_DISK_FULL, 0);
				}
				recRemainingSec = AudioRemainTime-elsTime;
				appAudioOsdProgressBarDraw(TRUE, gAudioCB.recSec, AudioRemainTime);
				appAudioOsdElsTimeShow();
				appAudioOsdRecRemainingShow(recRemainingSec);
			}
		} 
		else if (param == gAudioCB.deferredStopTimer) 
		{
			appTimerClear(&gAudioCB.deferredStopTimer);
			_AudioKeyCtrl_EXIT();
			appStateCloseDone();
		}
		break;

	/* media rec */
	case SP5K_MSG_MEDIA_ELAPSED_TIME_UPDATE:
		DBG_PRINT("__OmMsg: ELAPSED_TIME_UPDATE\n");
		break;
	case SP5K_MSG_MEDIA_REC_PAUSE_FINISH:
		DBG_PRINT("__OmMsg: REC_PAUSE_FINISH\n");
		break;
	case SP5K_MEDIA_REC_RESUME:
		break;
	case SP5K_MSG_DISK_ERROR:
		DBG_PRINT("__OmMsg: REC DISK_ERROR !!\n");
		break;
	case SP5K_MSG_MEDIA_REC_DISK_FULL:	
		appPowerSavingEnableSet(TRUE);
		DBG_PRINT("__OmMsg: REC_DISK_FULL\n");
		appAudioRecStop();
		gAudioCB.err = VIEW_ERR_MEMORY_FULL;
		break;
	case APP_STATE_MSG_ABORT:	
		appPowerSavingEnableSet(TRUE);
		DBG_PRINT("__OmMsg: APP_STATE_MSG_ABORT\n");
		appAudioRecStop();
		gAudioCB.err = VIEW_ERR_TO_POWER_OFF;
		break;

	default:
		break;
	}
}

void
appAudioRecStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	if ((gAudioCB.recStatus==AUDIO_REC_STATUS_NULL) && (gAudioCB.isDiskError == FALSE)) 
	{
		DBG_PRINT("__OmKey: Ignored, STATUS_NULL !! \n");
		return;
	}

	if(gAudioCB.isDiskError == TRUE){
		gAudioCB.recStatus=AUDIO_REC_STATUS_STOP;
	}
	
	switch (msg) 
	{
	/* star or stop */
	//case APP_KEY_PRESS_S1:
	case APP_KEY_PRESS_S1:
		if(gAudioCB.recStatus == AUDIO_REC_STATUS_STOP)
		{
			if (appAudioCheckReady(CAN_I_START_AUDIO_RECORD) == AUDIO_STATE_YES )
			{
				appAudioRecStart();
				_AudioKeyCtrl_BUSY();
				gAudioCB.s1press = 0;
			}
		}
		else 
		{
			if (gAudioCB.recSec>1) 
			{
				appAudioRecStop();
				appAudioOsdProgressBarDraw(FALSE,gAudioCB.recSec,0);
				AudioRemainTime =0;
			}
		}
		break;
	#ifdef HW_PRJ_6330	
	case APP_KEY_PRESS_LEFT:	
	#endif
	case APP_KEY_PRESS_MODE: 
		if((gAudioCB.recStatus == AUDIO_REC_STATUS_STOP) || (gAudioCB.isDiskError))
		{
			appBeep(TRUE, FALSE);
			appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);			

		}
		break;		
	case APP_KEY_PRESS_PB:
		if(gAudioCB.recStatus == AUDIO_REC_STATUS_STOP)
		{
			appBeep(TRUE, FALSE);
			#if SP5K_CDFS_OPT
			fileTypeSelect=1;
			appBtnEnable(BTN_UP|BTN_PB|BTN_DOWN);
			appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
			#else
			appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
			#endif
		}
		break;
	#ifdef APP_AUDIO_REC_PAUSE_RESUME_SUPPORT
	/*-- DO NOT SUPPORT PAUSE/RESUME at this moment */
	/* pause or resume */
	case APP_KEY_PRESS_UP:
		if( gAudioCB.recStatus == AUDIO_REC_STATUS_PAUSE) 
		{
			appAudioRecResume();
		}
		else if (gAudioCB.recStatus == AUDIO_REC_STATUS_RECORDING) 
		{
			if (gAudioCB.recSec>1) 
			{
				/*-- SP5K Constraint : Do not PAUSE @ very first sec !!*/
				appAudioRecPause();
			}
		}
		break;
	#endif
	default:
		break;
	}
}

void
appAudioRecState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) 
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appAudioRecState");
		appPowerSavingEnableSet(TRUE);
		gAudioCB.isDiskReady = 0;
		#if SP5K_DISK_SPI
 		if(IS_CARD_EXIST)
 		#endif
		{
			if (_AudioCheckDiskReady()==FALSE) 
			{
				gAudioCB.recStatus = AUDIO_REC_STATUS_NULL;
				sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			} 
			else 
			{
				sp5kHostMsgSend(APP_UI_MSG_AUDIO_REC_INIT, 0);
			}
		}
		#if SP5K_DISK_SPI
		else
			sp5kHostMsgSend(APP_UI_MSG_AUDIO_REC_INIT, 0);
		#endif
		
		#if SP5K_CDFS_OPT && !CAM_TYPE_CVR
		appCdfsFileFolderSet(CDFS_FILE_FOLDER_AUDIO);
		appCdfsFolderInit(CDFS_FILE_FOLDER_AUDIO);
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
		
		break;
	case APP_STATE_MSG_CLOSE:
		LED_NORMAL_ON;
		CLEAR_OSD_SRC;
		sp5kGfxPageClear(SP5K_GFX_PAGE_PF_0, PAL_TRANSPARENCY);
		if(gAudioCB.recStatus != AUDIO_REC_STATUS_STOP) 
		{
			gAudioCB.state = AUDIO_REC_STATE_CLOSE;
			if (gAudioCB.recSec<2) 
			{
				/*-- install a timer to stop record job for MIN REC TIME */
				gAudioCB.deferredStopTimer = appTimerSet(2500-(gAudioCB.recSec*1000), "timed REC_STOP");
			} 
			else 
			{			
				appPowerSavingEnableSet(TRUE);
				appAudioRecStop();
			}
		}
		else 
		{
			appTimerClear(&gAudioCB.deferredStopTimer);
			_AudioKeyCtrl_EXIT();
			appStateCloseDone();
		}
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appAudioRecStateOnKey(msg,param);
		}
		else
		{
			appAudioRecStateOnMsg(msg,param);
		}
		break;
	}
}



