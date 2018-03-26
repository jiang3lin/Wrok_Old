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

#include "app_osd_api_lcm.h"
#include "app_osd_draw_lcm.h"
#include "app_scdv_ui_para_def.h"
#include "app_rf_def.h"
#include "app_ui_msg.h"
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
extern sp5kCaptureStamp_t gStampInfo;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appPreflashInit(void);
extern UINT32 appAWBALGLib_WBParamSet( UINT32 sel, UINT32 val );
extern void applightFrequencySetting(LIGHT_FREQ_e lightfreq);


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void appSdvStillPvTipsOsdShow(void)
{
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->SdvPhotoMode == UI_SCDV_PHOTO_MODE_NORMAL)
	{
		if(puiPara->SdvPhotoNormalSize == UI_SCDV_PHOTO_NORMAL_SIZE_2M_16X9)
		{
			appLcmOsdTipsTextShow("16X9", FALSE);
		}
	}
	/*
	else if(puiPara->SdvPhotoMode == UI_SCDV_PHOTO_MODE_LAPSE)
	{
		
		switch(puiPara->SdvPhotoLapseTime)
		{
			case UI_SCDV_PHOTO_LAPSE_TIME_3S:
				appLcmOsdTipsTextShow("3SEC", FALSE);
				break;
			case UI_SCDV_PHOTO_LAPSE_TIME_5S:
				appLcmOsdTipsTextShow("5SEC", FALSE);
				break;
			case UI_SCDV_PHOTO_LAPSE_TIME_10S:
				appLcmOsdTipsTextShow("10SEC", FALSE);
				break;
			case UI_SCDV_PHOTO_LAPSE_TIME_30S:
				appLcmOsdTipsTextShow("30SEC", FALSE);
				break;
			case UI_SCDV_PHOTO_LAPSE_TIME_60S:
				appLcmOsdTipsTextShow("60SEC", FALSE);
				break;
		}
	}*/
	else if(puiPara->SdvPhotoMode == UI_SCDV_PHOTO_MODE_BURST)
	{
		switch(puiPara->SdvPhotoBurstNumber)
		{
			case UI_SCDV_PHOTO_BURST_3:
				appLcmOsdTipsTextShow("3PIC", FALSE);
				break;
			case UI_SCDV_PHOTO_BURST_5:
				appLcmOsdTipsTextShow("5PIC", FALSE);
				break;
			case UI_SCDV_PHOTO_BURST_10:
				appLcmOsdTipsTextShow("10PIC", FALSE);
				break;
		}
	}
	else if(puiPara->SdvPhotoMode == UI_SCDV_PHOTO_MODE_SELF_TIMER)	
	{
		switch(puiPara->SdvPhotoSelfTimer)
		{
			case UI_SCDV_PHOTO_SELF_TIMER_2S:
				appLcmOsdTipsTextShow("2SEC", FALSE);
				break;
			case UI_SCDV_PHOTO_SELF_TIMER_5S:
				appLcmOsdTipsTextShow("5SEC", FALSE);
				break;
			case UI_SCDV_PHOTO_SELF_TIMER_10S:
				appLcmOsdTipsTextShow("10SEC", FALSE);
				break;
		}
	}
}

void appSdvStillPvOsdShow(void)
{
	uiPara_t* puiPara = appUiParaGet();
	
	appLcmOsdClean();

	//Mode Icon
	switch(puiPara->SdvPhotoNormalSize)
	{
		//case UI_SCDV_PHOTO_NORMAL_SIZE_16M:
			//appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SDV_PHOTO_SIZE_16M, FALSE);
			//break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_5M:
			appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SDV_PHOTO_SIZE_05M, FALSE);
			break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_3M:
			appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SDV_PHOTO_SIZE_03M, FALSE);
			break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_2M_16X9:
			appLcmOsdLargeIconShow(LcmOsdIconIndex0, ID_ICON_SDV_PHOTO_SIZE_02M, FALSE);
			break;
	}

	//File Cnt
	switch(appDiskStatusGet())
	{
		case APP_DISK_STATUS_NO_CARD:
		case APP_DISK_STATUS_MOUNTING:
		case APP_DISK_STATUS_MOUNTED_FAIL:
			appLcmOsdLargeTextShow("___", FALSE);
			break;
		case APP_DISK_STATUS_CARD_LOCKED:
		case APP_DISK_STATUS_MEMORY_FULL:
		case APP_DISK_STATUS_MOUNTED_OK:
			{
				UINT32 FileNume;
				appCdfsFileNumGet(&FileNume);
				CHAR String[OSD_STRING_SIZE];
				sprintf(String, "%03d", FileNume);
				appLcmOsdLargeTextShow(String, FALSE);
			}			
			break;
	}
	
	//Mode Tips
	switch(puiPara->SdvPhotoMode)
	{
		case UI_SCDV_PHOTO_MODE_NORMAL:
			appLcmOsdModeStringShow(ID_STR_PHOTO, FALSE);
			break;
		/*
		case UI_SCDV_PHOTO_MODE_LAPSE:
			appLcmOsdModeStringShow(ID_STR_LAPSE, FALSE);
			break;
		*/	
		case UI_SCDV_PHOTO_MODE_BURST:
			appLcmOsdModeStringShow(ID_STR_BURST, FALSE);
			break;
		case UI_SCDV_PHOTO_MODE_SELF_TIMER:
			appLcmOsdModeStringShow(ID_STR_TIMER, FALSE);
			break;			
	}

	//Tips
	CHAR tipsString[OSD_STRING_SIZE];
	switch(appDiskStatusGet())
	{
		case APP_DISK_STATUS_NO_CARD:
			sprintf(tipsString, " %cNOCARD", ID_TEXT_CARD_ICON);			
			appLcmOsdTipsTextShow(tipsString, FALSE);	
			//appLcmOsdTipsTextShow(ID_TEXT_CARD_NULL, FALSE);
			break;
		case APP_DISK_STATUS_MOUNTING:
			sprintf(tipsString, " %cMOUNT", ID_TEXT_CARD_ICON);
			appLcmOsdTipsTextShow(tipsString, FALSE);
			//appLcmOsdTipsTextShow(ID_TEXT_CARD_MOUNTING, FALSE);
			break;
		case APP_DISK_STATUS_MOUNTED_FAIL:
			sprintf(tipsString, " %cERROR", ID_TEXT_CARD_ICON);
			appLcmOsdTipsTextShow(tipsString, FALSE);
			//appLcmOsdTipsTextShow(ID_TEXT_CARD_ERROR, FALSE);
			break;
		case APP_DISK_STATUS_CARD_LOCKED:
			sprintf(tipsString, " %cLOCKED", ID_TEXT_CARD_ICON);
			appLcmOsdTipsTextShow(tipsString, FALSE);
			//appLcmOsdTipsTextShow(ID_TEXT_CARD_LOCKED, FALSE);
			break;
		case APP_DISK_STATUS_MEMORY_FULL:
			sprintf(tipsString, " %cFULL", ID_TEXT_CARD_ICON);
			appLcmOsdTipsTextShow(tipsString, FALSE);
			//appLcmOsdTipsTextShow(ID_TEXT_CARD_FULL, FALSE);
			break;
		case APP_DISK_STATUS_MOUNTED_OK:
		default:
			appSdvStillPvTipsOsdShow();			
			break;
	}
	
	//Battery
	appLcmOsdDrawBattery(TRUE, -1);
	
	appLcmRefresh();
}

void appSdvStillSizeSet(void)
{
	uiPara_t* puiPara = appUiParaGet();
	sp5kImgSize_t capSize;
	
	//see ImgSize
	switch(puiPara->SdvPhotoNormalSize)
	{
		//case UI_SCDV_PHOTO_NORMAL_SIZE_16M:
			//sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, VIEW_SENSOR_MODE_4_3);
			//sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, CAP_SENSOR_MODE_4_3);
			//capSize.width = 4640;
			//capSize.height = 3480;
			//break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_5M:
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, VIEW_SENSOR_MODE_4_3);
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, CAP_SENSOR_MODE_4_3);
			#if 1
				capSize.width = 2592;
				capSize.height = 1944;
			#else
				capSize.width = 2560;
				capSize.height = 1920;
			#endif
			break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_3M:
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, VIEW_SENSOR_MODE_4_3);
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, CAP_SENSOR_MODE_4_3);
			capSize.width = 2048;
			capSize.height = 1536;
			break;
		case UI_SCDV_PHOTO_NORMAL_SIZE_2M_16X9:
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_PREVIEW, VIEW_SENSOR_MODE_16_9);
			sp5kSensorModeCfgSet(SP5K_MODE_STILL_SNAP, CAP_SENSOR_MODE_16_9);
			capSize.width = 1920;
			capSize.height = 1080;
			break;
	}
	capSize.cropWidth = capSize.width;
	capSize.cropHeight = capSize.height;	
	
	sp5kStillCapCfgSet(SP5K_CAPTURE_MAIN_IMAGESIZE, &capSize);	
}

void appSdvStillWdrSet(void)
{
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->SystemWdr)
	{
		case UI_SCDV_SYSTEM_WDR_ON:
			appStill_SetPasmSceneParam(PASM_DEFAULT,SCENE_WDR);
			appExif_SetExpProgram(PASM_DEFAULT,SCENE_WDR);
			appExif_SetSceneCaptureType(PASM_DEFAULT, SCENE_WDR);
			break;
		case UI_SCDV_SYSTEM_WDR_OFF:
			appStill_SetPasmSceneParam(PASM_DEFAULT,SCENE_DEFAULT);
			appExif_SetExpProgram(PASM_DEFAULT,SCENE_DEFAULT);
			appExif_SetSceneCaptureType(PASM_DEFAULT, SCENE_DEFAULT);
			break;
	}
}

void appSdvStillDistSet(void)	//USE HW_DIST
{
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->SystemFrequnecy)
	{
		case UI_SCDV_SYSTEM_DIS_ON:
			appStill_SetStabilizer(UI_STABILIZER_ON);	//puiPara->Stabilizer
			break;
		case UI_SCDV_SYSTEM_DIS_OFF:
			appStill_SetStabilizer(UI_STABILIZER_OFF);	//puiPara->Stabilizer
			break;
	}
}

void appSdvStillFrequencySet(void)
{
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->SystemFrequnecy)
	{
		case UI_SCDV_SYSTEM_FREQUENCY_50HZ:
			appStill_SetExpFlickerMode(0);
			break;
		case UI_SCDV_SYSTEM_FREQUENCY_60HZ:
			appStill_SetExpFlickerMode(1);
			break;
		case UI_SCDV_SYSTEM_FREQUENCY_AUTO:
			sp5kModeCfgSet(SP5K_MODE_CFG_BAND_DETECT_PEAK, 200 , 310 ,400);
			sp5kModeCfgSet(SP5K_MODE_CFG_BAND_DETECT_ENABLE, 1);
			return;
		default:
			break;
	}
	appAePvReset();
}

void appSdvStillDateStampset(void)
{
	uiPara_t* puiPara = appUiParaGet();
	sp5kImgSize_t capSize;

	switch(puiPara->SystemDateStamp)
	{
		case UI_SCDV_SYSTEM_DATE_STAMP_ON:
			//see ImgSize
			switch(puiPara->SdvPhotoNormalSize)
			{
				//case UI_SCDV_PHOTO_NORMAL_SIZE_16M:
					//capSize.width = 4640;
					//capSize.height = 3480;
					//break;
				case UI_SCDV_PHOTO_NORMAL_SIZE_5M:
					capSize.width = 2592;
					capSize.height = 1944;
					break;
				case UI_SCDV_PHOTO_NORMAL_SIZE_3M:
					capSize.width = 2048;
					capSize.height = 1536;
					break;
				case UI_SCDV_PHOTO_NORMAL_SIZE_2M_16X9:
					capSize.width = 1920;
					capSize.height = 1080;
					break;
			}
			
			gStampInfo.dateFmtId = SP5K_CAPTURE_DATE_DDMMYYYY | SP5K_CAPTURE_DATE_TIME;
			gStampInfo.scaleRatio = 100 * 2 * capSize.height / 1080;	//base 1080 is 2X
			gStampInfo.x = (capSize.width * 60 / 100) & 0xFFFFFFFE;
			gStampInfo.y = (capSize.height * 90 / 100) & 0xFFFFFFFE;				
			sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, &gStampInfo);
			break;
		case UI_SCDV_SYSTEM_DATE_STAMP_OFF:
			sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, NULL);
			break;
	}
}

void appSdvStillSettingInit(void)
{
	appDiskInfo_t* pDsk;
	UINT32 cfg, ret;
	UINT32 qvtime[QV_TIME_MAX] = {0, 1 ,3 ,5};
	
	uiPara_t* puiPara = appUiParaGet();
	appCalibData_t *pcalib= appCalibDataGet();

	appStillRawCbSet(RAW_CB_SHUTTER_NORAML);
	appStillYuvCbSet(FALSE);
	sp5kStillCapExifHdrCallbackSet(appExifEncodeCb);
	//If we want to use fieldQV we should disable yuv callback.
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_SAVE_RAW, !!pcalib->StillRawSign);

	appStillViewIqSet();		
	sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, 0xffffffff);
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
	appPreflashInit();
	appAePreviewSet();
	#ifdef ALGORITHM_G4_AWB
	appAWBALGLib_WBParamSet ( 22/*AWB_PARA_PREVIEW_IMMEDIATELY*/, 0 );
	appAWBALGLib_WBParamSet ( 8/*AWB_PARA_UPDTREFE_FRAME_NUM*/, 64 );
	#endif
	sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, gStillCB.awbSpeed);

	cfg = 0xffff;
	ret = sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_LENGTH, &cfg);	
	cfg = 4; /* QVGA put on APP4 */
	ret = sp5kStillCapAdvCfgSet(SP5K_CAPTURE_QVGA_IMG, cfg);	
	cfg = 0; 
	ret = sp5kStillCapAdvCfgSet(SP5K_CAPTURE_SAVE_PROGRESSIVE_SIZE, cfg);	

	appStill_SetResolution(puiPara->ImageSize);
	appStill_SetSp5kStillSize(puiPara->ImageSize);	
	
	#if 1
	//if(puiPara->SdvPhotoNormalSize==UI_SCDV_PHOTO_NORMAL_SIZE_16M)
	{				
		//appStill_SetQuality(UI_QUALITY_FINE);
	}	
	//else
	{		
		appStill_SetQuality(puiPara->ImageQuality);		
	}	
	#endif
	///printf("--puiPara->ImageQuality=%d\n",puiPara->ImageQuality);
	appStill_SetMetering(puiPara->AEMetering);
	appStill_SetWB(puiPara->WBMode);
	appStill_SetISO(puiPara->Iso);
	appStill_SetExposure(puiPara->AEMode);
	appStill_SetDriveMode(puiPara->DriverMode);
	appStill_SetAEB(puiPara->AEB);
	appStill_SetPasmSceneParam(gStillCB.cap.CurViewMode,gStillCB.cap.CurScene);
	appStill_SetHDR(0);//IsHDR);
	appStill_SetSharpness(puiPara->Sharpness);
	appStill_SetTimeLapse(puiPara->TimeLapse);
	///appStill_SetStabilizer(puiPara->Stabilizer);
	appStill_SetPhotoFrame(puiPara->PhotoFrame);
	if(appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)
	appStill_SetDZoom(puiPara->DigitalZoom);
	appStill_SetFlash(puiPara->flash);
	appStill_SetShutterSpeed(puiPara->shutter);
	appStill_SetApertureValue(pViewParam->aperture);
	appAeFrameRateSet(30);
	applightFrequencySetting(puiPara->LightFreq);
	appStill_SetStabilizer(puiPara->Stabilizer);
	
	appStill_SetPasmSceneParam(PASM_DEFAULT,SCENE_DEFAULT);
	appExif_SetExpProgram(PASM_DEFAULT,SCENE_DEFAULT);
	appExif_SetSceneCaptureType(PASM_DEFAULT, SCENE_DEFAULT);

	gStillCB.estFileSize = appStillEstFileSize(puiPara->ImageSize, puiPara->ImageQuality, gStillCB.diskClusterSize);				
	cfg = (puiPara->AEB == 1)? 3: IS_DRIVE_DOUBLE? 2: 1;
	gStillCB.estJobFilesSize = gStillCB.estFileSize*cfg;
	appStillBackgroundSet(gStillCB.estFileSize);

	pDsk = appCurDiskInfoGet();
	if(pDsk->actDsk)
	{
		if(pDsk->cluSz > 1024)//large sector
		{
			gStillCB.diskClusterSize = pDsk->cluSz >>10;//unit in Kbyte		
		}
		else//small sector
		{
			gStillCB.diskClusterSize = 1;//unit in Kbyte
		}
		appStillBackgroundInit(pDsk->cluSz, pDsk->freeSz, gStillCB.estFileSize);
	}

	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,1);
	appStill_SetDateStamp(puiPara->StampMode, puiPara->DateStyle);

	// This may be set to zero in caputure state to interrupt the QV.
	gStillCB.cap.qvSec = qvtime[puiPara->AutoReviewTime];
	cfg = gStillCB.cap.qvSec*100;
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, &cfg);

	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, 0);// raw 0

	//for DVS5M2
	appSdvStillSizeSet();
	appSdvStillWdrSet();
	appSdvStillDistSet();
	appSdvStillFrequencySet();
	appSdvStillDateStampset();
	appSensorFlipSet();	
	//for DVS5M2
}

#if 0
UINT8 appSdvStillDiskCheckMemeoryFull(void)
{
	UINT32 dskfree, ret, remain_number;
	uiPara_t* puiPara = appUiParaGet();
	
	appDiskInfo_t *pDiskInfo = appCurDiskInfoGet();

	profLogAdd(0, "appSdvVideoDiskCheckMemeoryFull");

	ret = appStillBackgroundGetFree(&dskfree);
	remain_number = dskfree/gStillCB.estJobFilesSize;

	if(remain_number==0 && ret==SUCCESS) 
	{
		printf("%s: Fail (disk space not enough)\n", __FUNCTION__);
		printf("%s: disk-free = %d kbyte, need size = %d kbyte \n", __FUNCTION__, dskfree, gStillCB.estJobFilesSize);
		return TRUE;
	}
	else if(remain_number<=2 && ret==FAIL)
	{
		printf("%s: Fail (disk space close to empty)\n", __FUNCTION__);
		printf("%s: disk-free = %d kbyte, need size = %d kbyte \n", __FUNCTION__, dskfree, gStillCB.estJobFilesSize);
		return TRUE;
	}
	else 
	{
		printf("\033[32m^^^%s: OK (disk space enough)^^^\033[0m\n", __FUNCTION__);
		return FALSE;
	}

	return FALSE;
}
#else

UINT8 appSdvStillDiskCheckMemeoryFull(void)
{
	uiPara_t* puiPara = appUiParaGet();	
	appDiskInfo_t* pDisk = NULL;
	int ret=0;
	pDisk = appCurDiskInfoGet(); 
	if(puiPara->SdvPhotoMode==UI_SCDV_PHOTO_MODE_BURST)
	{
		if(pDisk->freeSz < 70*1024)
			ret=TRUE;
		else
			ret=FALSE;
	}
	else
	{
		if(pDisk->freeSz < 20*1024)
			ret=TRUE;
		else
			ret=FALSE;
	}
			
	if(appWiFiConnection_UtilityStateGet() > WIFI_UTILITY_CLOSE)
		ret=FALSE;
		
	return ret;	
}

#endif


void appSdvStillPvStateInit(UINT32 preState)
{
	UINT32 mode;

	uiPara_t* puiPara = appUiParaGet();	
	appCalibData_t *pcalib= appCalibDataGet();
	
	profLogAdd(0,"appSdvStillPvStateInit s");
	
	//Global Setting
	appAutoPowerOffEnable(TRUE);
	appLcmPowerSaveEnable(TRUE);
	
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
	appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	
	//Led setting
	LED_RULES_NORMAL;
	RFRecCaptureWaitRelease();
	appRfCopyBufferCallbackRelease();

	//Still Setting
	//sp5kModeCfgSet(SP5K_MODE_CFG_PV_FREEZE, 1);
	appSdvStillSettingInit();

	//IQ Setting

	//disk
	if(appScdvWaitForDiskReady() == TRUE)
	{
		//appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
		//appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	}

	//Osd
	appSdvStillPvOsdShow();

	/* enter preview mode */
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
	sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
	//sp5kModeCfgSet(SP5K_MODE_CFG_PV_FREEZE, 2);
	appRfCopyBufferCallbackSet();

	//button setting
	appBtnEnable(BTN_ALL);

	profLogAdd(0,"appSdvStillPvStateInit e");
}

void appCdvStillPvStateInit(UINT32 preState)
{
	UINT32 mode;

	uiPara_t* puiPara = appUiParaGet();	
	appCalibData_t *pcalib= appCalibDataGet();
	
	profLogAdd(0,"appSdvStillPvStateInit ss");
	
	//Global Setting
	appAutoPowerOffEnable(TRUE);
	appLcmPowerSaveEnable(TRUE);

	//Led setting
	LED_RULES_NORMAL;

	//appRfCopyBufferCallbackRelease();

	//Still Setting
	//sp5kModeCfgSet(SP5K_MODE_CFG_PV_FREEZE, 1);
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
	appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	appSdvStillSettingInit();

	//IQ Setting

	//disk
	//if(appScdvWaitForDiskReady() == TRUE)
	{
		//appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
		//appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	}

	//Osd
	//appSdvStillPvOsdShow();

	/* enter preview mode */
	sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
	//sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
	//sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
	//sp5kModeCfgSet(SP5K_MODE_CFG_PV_FREEZE, 2);
	//appRfCopyBufferCallbackSet();

	//button setting
	appBtnEnable(BTN_ALL);

	profLogAdd(0,"appSdvStillPvStateInit e");
}

void appSdvStillPvStateOnMsg(UINT32 msg, UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();

	switch(msg)
	{
		case APP_UI_MSG_RF_RCWD01_SNAP_OK:	
			if(appDiskStatusGet() == APP_DISK_STATUS_MOUNTED_OK)
			{
				switch(puiPara->SdvPhotoMode)
				{
					case UI_SCDV_PHOTO_MODE_NORMAL:
						appStateChange(APP_STATE_SPORTDV_STILL_CAPTURE, STATE_PARAM_NORMAL_INIT);
						break;
					/*	
					case UI_SCDV_PHOTO_MODE_LAPSE:
						appStateChange(APP_STATE_SPORTDV_STILL_CAP_LAPSE, STATE_PARAM_NORMAL_INIT);
						break;
					*/	
					case UI_SCDV_PHOTO_MODE_BURST:
						appStateChange(APP_STATE_SPORTDV_STILL_CAP_BURST, STATE_PARAM_NORMAL_INIT);
						break;
					case UI_SCDV_PHOTO_MODE_SELF_TIMER:
						appStateChange(APP_STATE_SPORTDV_STILL_CAP_SELF_TIMER, STATE_PARAM_NORMAL_INIT);
						break;
				}
			}
			else
			{
				appBuzzerStop(TRUE);
				appBuzzerPlay(BUZZER_WARNING);
			}
			break;

		case SP5K_MSG_MODE_READY:
			if(param == SP5K_MODE_VIDEO_PREVIEW)
			{

			}
			break;
		case APP_UI_MSG_VIEW_WAIT_DISK:
			if(appScdvWaitForDiskReady() == TRUE)
			{
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
				appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
			}
			break;
		case SP5K_MSG_DISK_MOUNT_COMPLETE:
		case SP5K_MSG_DISK_MOUNT_FAIL:
			appScdvWaitForDiskReady();
			appSdvStillPvOsdShow();
			break;
		case SP5K_MSG_AE_READY:
			//appAePreviewAdjust(param);
	
			break;
		case SP5K_MSG_AWB_READY:

			break;
		case SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE:

			break;
		case SP5K_MSG_TIMER:
			
			break;
		case APP_UI_MSG_GSENSOR_FLIP:
			if(puiPara->SCdvSensorFlip == UI_SCDV_SENSOR_FLIP_AUTO)
			{
				appSensorFlipSet();
			}
			break;
		default:
			DBG_PRINT(".");
			break;
	}
}

void appSdvStillPvStateOnKey(UINT32 msg, UINT32 param)
{
	uiPara_t* puiPara = appUiParaGet();

	switch(msg)
	{
		case APP_KEY_PRESS_POWER:
			appModeChangeFlow();
			break;
		case APP_KEY_PRESS_OK:
		//case APP_UI_MSG_RF_RCWD01_SNAP_OK:	
			if(appDiskStatusGet() == APP_DISK_STATUS_MOUNTED_OK)
			{
				switch(puiPara->SdvPhotoMode)
				{
					case UI_SCDV_PHOTO_MODE_NORMAL:
						appStateChange(APP_STATE_SPORTDV_STILL_CAPTURE, STATE_PARAM_NORMAL_INIT);
						break;
					/*	
					case UI_SCDV_PHOTO_MODE_LAPSE:
						appStateChange(APP_STATE_SPORTDV_STILL_CAP_LAPSE, STATE_PARAM_NORMAL_INIT);
						break;
					*/	
					case UI_SCDV_PHOTO_MODE_BURST:
						appStateChange(APP_STATE_SPORTDV_STILL_CAP_BURST, STATE_PARAM_NORMAL_INIT);
						break;
					case UI_SCDV_PHOTO_MODE_SELF_TIMER:
						appStateChange(APP_STATE_SPORTDV_STILL_CAP_SELF_TIMER, STATE_PARAM_NORMAL_INIT);
						break;
				}
			}
			else
			{
				appBuzzerStop(TRUE);
				appBuzzerPlay(BUZZER_WARNING);
			}
			break;
	}
}

void appSdvStillPvState(UINT32 msg, UINT32 param)
{
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("appSdvStillPvState\n");
			appSdvStillPvStateInit(appPreviousStateGet());
			RFSnapMovieMutex =0;
			break;
		case APP_STATE_MSG_CLOSE:
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg))
			{
				appSdvStillPvStateOnKey(msg,param);
			}
			else
			{
				appSdvStillPvStateOnMsg(msg,param);
			}
			break;
	}
}
