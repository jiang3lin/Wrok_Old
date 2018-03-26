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
#include "app_dcf.h"
#include "app_rtc.h"
#include "app_still.h"
#include "app_playback_def.h"
#include "app_battery.h"
#include "app_fast_boot.h"
#include "app_com_api.h"
#include "app_ui_para.h"
#include "app_view_param_def.h"
#include "app_playback_uiparam.h"
#include "app_usb_dpslib.h"
#include "app_user_setting.h"
#include "app_setup_lib.h"

#include "app_scdv_ui_para_def.h"

#if CAM_TYPE_CVR
#if GPS_SUPPORT
extern UINT8 gps_update_RtcTime_flag;
#endif
#endif
#if 0
#define USER_SETTING_FILENAME  "B:\\USER_RES\\USER_SET.BIN"
#define MODE_SAVE_WHEN_POWER_OFF 1


void
_userSettingDefaultSet(
	uiParamSetting_t *pbuf
)
{
	pbuf->capSetting.CurViewMode = PASM_DEFAULT;
	pbuf->capSetting.CurScene = SCENE_DEFAULT;
	pbuf->sound.volume = SOUND_VOLUME_DEFAULT;
	pbuf->sound.beep = TRUE;
	pbuf->sound.powerOn = POWERON_SOUND_DEFAULT;
	pbuf->sound.shutter = SHUTTER_SOUND_DEFAULT;
	pbuf->qvTime = QV_TIME_DEFAULT;
	pbuf->powerSave = POWER_SAVE_DEFAULT;
	pbuf->language = LANG_DEFAULT;
	pbuf->fileNumber = FILENUM_DEFAULT;
	pbuf->dateTime = DATE_TIME_DEFAULT;
	pbuf->TV = TV_DEFAULT;
	pbuf->usbMode= USB_DEFAULT;
	pbuf->powerOnLogo = POWERON_LOGO_DEFAULT;
	pbuf->batteryType = BATTERY_TYPE_DEFAULT;
	pbuf->lightFrequency= LIGHT_FREQ_DEFAULT;	 /*Leo @20100413*/
	pbuf->videoFormat = VIDEO_FORMAT_TYPE_DEFAULT;
	pbuf->mediaFormat = MEDIA_FORMAT_TYPE_DEFAULT;
	pbuf->periodicalCap= PERIODICAL_CAPTURE_DEFAULT;
	pbuf->dcfDirKey = 0;
	pbuf->dcfFileKey = 0;

}


UINT32
appUserSettingLoad(
	uiParamSetting_t *pbuf
)
{
	uiPara_t *psPara = appUiParaGet();
	if(pbuf == NULL)
	{
		printf("[ERR] load user setting file error!!! : %s(%d)\n",__FUNCTION__,__LINE__);
		return FAIL;
	}
	if(ReadFile(USER_SETTING_FILENAME, (UINT8*)(pbuf), sizeof(uiParamSetting_t))!= SUCCESS)
	{
		DBG_PRINT("[ERR] read file[%s] fail\n", USER_SETTING_FILENAME);
		/* set default */
		_userSettingDefaultSet(pbuf);
		psPara->USBMode = pbuf->usbMode;
		return FAIL;
	}
	psPara->USBMode = pbuf->usbMode;
	gStillCB.cap.CurScene = psPara->CurScene;
	gStillCB.lastcap = gStillCB.cap;

	return SUCCESS;
}


UINT32
appUserSettingSave(
	uiParamSetting_t *pbuf
)
{
	uiPara_t *psPara = appUiParaGet();

	DBG_PRINT("[26324] - (B)");
	if(pbuf == NULL)
	{
		printf("[ERR] save user setting file error!!! : %s(%d)\n",__FUNCTION__,__LINE__);
		return FAIL;
	}
	DBG_PRINT("[26324] - (C)");
	#if MODE_SAVE_WHEN_POWER_OFF
	psPara->CurViewMode =gStillCB.cap.CurViewMode;
	psPara->CurScene = gStillCB.cap.CurScene;
	#else
	psPara->CurViewMode =PASM_DEFAULT;
	psPara->CurScene = SCENE_DEFAULT;
	#endif
	DBG_PRINT("[26324] - (D)");
	appDcfKeySettingUpdate(&psPara->dcfDirKey, &psPara->dcfFileKey);

	DBG_PRINT("[26324] - (E)");
	if(WriteFile(USER_SETTING_FILENAME, (UINT8*)(pbuf), sizeof(uiParamSetting_t))!=SUCCESS)
	{
		DBG_PRINT("[ERR] write file[%s] fail\n", USER_SETTING_FILENAME);
		return FAIL;
	}
	DBG_PRINT("[26324] - (F)");
	appFastBootLensModeSet(psPara->CurViewMode);

	DBG_PRINT("[26324] - (G)");
	appFastBootLensBattSet(psPara->batteryType,appLensBootBatteryLevelGet());
	DBG_PRINT("[26324] - (H)");
	return SUCCESS;

}
#endif
void
appDcfUserSettingGet(
	fileNum_e *fnumRule,
	UINT16 *dirKey,
	UINT16 *fileKey
)
{
	uiPara_t *psPara = appUiParaGet();

	*fnumRule = psPara->fileNumber;
	*dirKey = psPara->dcfDirKey;
	*fileKey = psPara->dcfFileKey;
}
#if CAM_TYPE_CVR
void
appWIFIPassword(void)
{

	uiPara_t* puiPara = appUiParaGet();
	UINT32 fd;
	UINT8 temp[8];
	UINT8 i;

	fd = sp5kFsFileOpen("B:/UDF/SSID_PW.CFG",SP5K_FS_OPEN_CREATE|SP5K_FS_OPEN_RDWR);
	temp[0]=puiPara->WIFIPassword_7+0x30;
	temp[1]=puiPara->WIFIPassword_6+0x30;
	temp[2]=puiPara->WIFIPassword_5+0x30;
	temp[3]=puiPara->WIFIPassword_4+0x30;
	temp[4]=puiPara->WIFIPassword_3+0x30;
	temp[5]=puiPara->WIFIPassword_2+0x30;
	temp[6]=puiPara->WIFIPassword_1+0x30;
	temp[7]=puiPara->WIFIPassword_0+0x30;
	sp5kFsFileWrite(fd,"rtwap_evb\r\n",strlen("rtwap_evb\r\n"));
		if(fd){
			for(i=0;i<8;i++){
				sp5kFsFileWrite(fd,&temp[i],sizeof(UINT8));
			}
		}
		sp5kFsFileWrite(fd,"\r\n",strlen("\r\n"));
		sp5kFsFileClose(fd);

}
#endif

void
appFactorySettingApply(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();


	///////////////////////// FOR DVS5M2 /////////////////////////////
	puiPara->SdvVideoMode = UI_SCDV_VIDEO_MODE_NORMAL;
	puiPara->SdvVideoNormalSize = UI_SCDV_VIDEO_SIZE_1080P30;
	puiPara->SdvVideoSlowTime = UI_SCDV_VIDEO_SLOW_1_2;
	//puiPara->SdvVideoLapseTime = UI_SSPDV_VIDEO_LAPSE_TIME_1S;
	puiPara->SdvPhotoMode = UI_SCDV_PHOTO_MODE_NORMAL;
	puiPara->SdvPhotoNormalSize = UI_SCDV_PHOTO_NORMAL_SIZE_5M;
	//puiPara->SdvPhotoLapseTime = UI_SCDV_PHOTO_LAPSE_TIME_3S;
	puiPara->SdvPhotoBurstNumber = UI_SCDV_PHOTO_BURST_3;
	puiPara->SdvPhotoSelfTimer = UI_SCDV_PHOTO_SELF_TIMER_5S;
	puiPara->SCdvSensorFlip = UI_SCDV_SENSOR_FLIP_OFF;

	puiPara->CdvVideoMode = UI_SCDV_VIDEO_MODE_NORMAL;
	puiPara->CdvVideoNormalSize = UI_SCDV_VIDEO_SIZE_1080P30;
	puiPara->CdvVideoLapseTime = UI_SCDV_VIDEO_LAPSE_TIME_1S;
	puiPara->CdvLoopingTime = UI_SCDV_VIDEO_LOOP_TIME_2MIN;
	puiPara->CdvGsensorTrigger = UI_SCDV_GSENSOR_TRIGGER_OFF;
	puiPara->CdvGsensorSensitivity = UI_SCDV_GSENSOR_SENSITIVITY_MIDDLE;
	puiPara->CdvMicPhone = UI_SCDV_MICPHONE_ON;

	puiPara->SystemDis = UI_SCDV_SYSTEM_DIS_OFF;
	puiPara->SystemWdr = UI_SCDV_SYSTEM_WDR_OFF;
	puiPara->SystemLedIndicator = UI_SCDV_SYSTEM_LED_INDICATOR_ON;
	puiPara->SystemOsdFlip = UI_SCDV_SYSTEM_OSD_FLIP_OFF;
	puiPara->SystemOsdBrightness = UI_SCDV_SYSTEM_OSD_BRIGHTNESS_60;
	puiPara->SystemBeepVolume = UI_SCDV_SYSTEM_BEEP_VOLUME_60;
	puiPara->SystemDateStamp = UI_SCDV_SYSTEM_DATE_STAMP_ON;
	puiPara->SystemAutoPowerOff = UI_SCDV_SYSTEM_AUTO_POWER_OFF_3MIN;
	puiPara->SystemRemoter = UI_SCDV_SYSTEM_REMOTER_OFF;
	puiPara->SystemFrequnecy = UI_SCDV_SYSTEM_FREQUENCY_50HZ;
	puiPara->SystemWifiPassword00 = 1;
	puiPara->SystemWifiPassword01 = 2;
	puiPara->SystemWifiPassword02 = 3;
	puiPara->SystemWifiPassword03 = 4;
	puiPara->SystemWifiPassword04 = 5;
	puiPara->SystemWifiPassword05 = 6;
	puiPara->SystemWifiPassword06 = 7;
	puiPara->SystemWifiPassword07 = 8;
	puiPara->SystemWifiPassword08 = 9;
	puiPara->SystemWifiPassword09 = 0;

	puiPara->ScdvPbFileType = UI_SCDV_PB_TYPE_VIDEO;
	puiPara->ScdvMainMode = UI_SCDV_MAIN_MODE_SPORT;
	puiPara->SystemRfCuid[0] = 0x6;
	puiPara->SystemRfCuid[1] = 0x7;
	puiPara->SystemRfCuid[2] = 0x8;
	puiPara->SystemRfCuid[3] = 0x9;
	puiPara->SystemRfCuid[4] = 0xa;	
	puiPara->SystemRfType = UI_SCDV_SYSTEM_RF_TYPE_RCW5K8;	

	puiPara->batteryType = BATTERY_TYPE_DEFAULT;
	///////////////////////// FOR DVS5M2 /////////////////////////////
	//appWifiSsidPasswordSet();

	puiPara->ImageSize = UI_PIC_SIZE_FULL;
	puiPara->ImageQuality = QUALITY_DEFAULT;
	puiPara->AEMetering = METERING_DEFAULT;
	puiPara->WBMode = WHITE_BALANCE_DEFAULT;
	puiPara->Iso = ISO_DEFAULT;
	puiPara->AEMode = EXPOSURE_DEFAULT;
	puiPara->DriverMode = DRIVE_MODE_DEFAULT;
	puiPara->AEB = AEB_DEFAULT;
	puiPara->Sharpness = SHARPNESS_DEFAULT;
	puiPara->ImageEffect = IMAGE_FFECT_DEFAULT;
	puiPara->FeatureEffect = FEATURE_EFFECT_DEFAULT;
	puiPara->StampMode = DATE_STAMP_DEFAULT;
	puiPara->Stabilizer = STABILIZER_DEFAULT;
	puiPara->FaceTracking = FACETRACKING_DEFAULT;
	puiPara->PhotoFrame = PHOTOFRAME_DEFAULT;
	puiPara->DigitalZoom = DIGITAL_ZOOM_DEFAULT;
	puiPara->WDRMode = WDR_DEFAULT;
	puiPara->MotionDetect = MD_DEFAULT;
	puiPara->TimeLapse = TIMELAPSE_DEFAULT;
	puiPara->VideoSize = VIDEO_SIZE_720P_30FPS;
	puiPara->VideoQuality = QUALITY_DEFAULT;
	puiPara->RollingShutter = ROLLING_SHUTTER_DEFAULT;
	puiPara->VideoFaceTracking = FACETRACKING_DEFAULT;
	puiPara->DisMode = DIS_DEFAULT;
	puiPara->AdvanceDenoise = ADVANCE_DENOISE_DEFAULT;
	puiPara->selImgProtect = PB_PROTECT_DEFAULT;
	puiPara->selImgDelete = PB_DELETE_DEFAULT;
	puiPara->selSlideInteval = PB_SLSHOW_INTERVAL_DEFAULT;
	puiPara->selSlideMusic = PB_SLSHOW_MUSIC_DEFAULT;
	puiPara->selSlideTrans = PB_SLSHOW_TRANSITION_DEFAULT;
	puiPara->selSlideRepeat = PB_SLSHOW_REPEAT_DEFAULT;
	puiPara->selCopy2Card = PB_COPY2CARD_DEFAULT;
	puiPara->selImgEffect = PB_EFFECT_DEFAULT;
	puiPara->selImgRotate = PB_ROTATE_SELECT_DEFAULT;
	puiPara->selImgResize = PB_RESIZE_DEFAULT;
	puiPara->selVoiceMemo = PB_VOICE_MEMO_DEFAULT;
	puiPara->selStartImage = PB_START_IMAGE_DEFAULT;
	puiPara->selPhotoframe = PB_FRAME_DEFAULT;
	#if !CAM_TYPE_CVR
	/*puiPara->DPSCopies    = 0;*/
	puiPara->DPSDate      =DPS_DATE_DEFAULT;
	puiPara->DPSDateType  =DPS_DATETYPE_DEFAULT;
	puiPara->DPSFileName  =DPS_FILE_NAME_DEFAULT;
	puiPara->DPSImageMode =DPS_IMAGE_MODE_DEFAULT;
	puiPara->DPSLayout    =DPS_LAYOUT_DEFAULT;
	puiPara->DPSPageSize  =DPS_PAPER_SIZE_DEFAULT;
	puiPara->DPSPageType  = DPS_PAPER_TYPE_DEFAULT;
	puiPara->DPSQuality   =DPS_PAPER_QUALITY_DEFAULT;
	#endif
	puiPara->CurViewMode = PASM_DEFAULT;
	puiPara->CurScene = SCENE_DEFAULT;
	puiPara->Volume= SOUND_VOLUME_DEFAULT;
	puiPara->BeepSound = TRUE;
	puiPara->StartupSound = POWERON_SOUND_DEFAULT;
	puiPara->ShutterSound = SHUTTER_SOUND_DEFAULT;
	puiPara->AutoReviewTime = QV_TIME_DEFAULT;
	puiPara->SleepTime = POWER_SAVE_DEFAULT;
	puiPara->Language= LANG_DEFAULT;
	puiPara->fileNumber = FILENUM_DEFAULT;
	puiPara->DateStyle= DATE_TIME_DEFAULT;
	puiPara->TVMode= TV_DEFAULT;
	puiPara->USBMode = USB_DEFAULT;
	puiPara->OpenPic = POWERON_LOGO_DEFAULT;
	puiPara->LightFreq =LIGHT_FREQ_DEFAULT;
	puiPara->PeriodicalCap= PERIODICAL_CAPTURE_DEFAULT;
	puiPara->dcfDirKey = 0;
	puiPara->dcfFileKey = 0;
	puiPara->soundcontrol= SOUND_CONTROL_DEFAULT;
	puiPara->VideoFormat = VIDEO_FORMAT_TYPE_DEFAULT;
	puiPara->MediaFormat = MEDIA_FORMAT_TYPE_DEFAULT;
	puiPara->flash = UI_FLASH_OFF;
	puiPara->fileType = FILETYPE_DEFAULT;
#if CAM_TYPE_CVR
	puiPara->Seamless = UI_VIDEO_SEAMLESS_3MIN;
	puiPara->VideoStampMode = UI_STAMP_DATE_TIME;
	puiPara->G_Sensor = UI_GSENSOR_STATUS_OFF;
	puiPara->WaterMark = 0;
	puiPara->LOGOMark = 0;
	puiPara->CollisionDetection = UI_COLLISION_DETECT_OFF;
	puiPara->TimeZones = UI_TimeZones_East8;
	puiPara->ParkingMode = UI_PARKING_MODE_OFF;
	#if GPS_SUPPORT
	puiPara->GPS = UI_GPS_STATUS_OFF;
	gps_update_RtcTime_flag=0;//是否要GPS更新时间
	#endif
#else
	puiPara->Seamless = VIDEO_SEAMLESS_DEFAULT;
	puiPara->VideoStampMode = DATE_STAMP_DEFAULT;
#endif
#if ICAT_FCWS || ICAT_LDWS
	puiPara->DAS_mode=1;
	puiPara->cvrwsRightCnt = 0;
	puiPara->cvrwsLeftCnt = 0;
#endif
	printf("---appStill_ResetAllParam\n");
	appStill_ResetAllParam();
	//reset slide show param
	appPbBackdoorEventCBF(PB_APP_EVENT_SETUP_RESET);
	appRtcDefaultSet();
	//reset setup param
	appSetupMenu_SetCurLanguage(puiPara->Language);/*for set language*/
	appCdfsInit();
	appUiParaPowerOnSet();
	
}

