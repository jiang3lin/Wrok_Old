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
#include "sp5k_af_api.h"
#include "app_video.h"
#include "app_calib_api.h"
#include "app_errorosd.h"
#include "app_3aalg.h"
#include "app_ui_para.h"
#include "Sp5k_usb_api.h"
#include "app_cdfs_api.h"
#include "app_key_def.h"
#if SPCA6330
#include "app_wifi_utility.h"
#include "app_ptp.h"
#include "app_wifi_connection.h"
#endif
#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif

#include "app_pal.h"
#include "sp5k_media_api.h"
#include "app_still_timelapse.h"
#include "app_awbalg_api.h"
#include "app_view_param_def.h"

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
appStillCB_t gStillCB = {
	.viewParam = NULL,
	.cap = {
		.CurViewMode = PASM_DEFAULT,
		.CurScene = SCENE_DEFAULT,
		.qvSec = 3
	},
	.ae = {
		.iris = APERTURE_2_8,
		.shutter = SHUTTER_1_2000S,
		.iso = ISO_100,
		.ev = 0.0,
		.av = APERTURE_2_8,
		.tv = SHUTTER_1_2000S
	},
	.diskClusterSize = 16,
	.estFileSize=3000,
	.estJobFilesSize=3000,
	.remainImg=0,
	.s1press=0,
	.s2press=0,
	.s1AaaTicket=0,
	.s2SnapTicket=0,
	.tvAvSel=0,
	.flashCharging=0,
	.lensZoomSeg=0,
	.amodeApertureIdx=0,
	.smodeShutterIdx=SHUTTER_1_2000S,
	.needFlash=0,
	.focusStatus=FOCUS_STATUS_VIEW,
	.needDarkFrameSub=0,
	.isDiskReady=0,
	.ftStatus= VIEW_FT_STATUS_DESTROY,
	.isLensReady=FALSE,
	.curPhotoFrameIdx = UI_PHOTOFRAME_OFF,
	.flashChargeMonitorTimer = TIMER_NULL,
	.flashMode =  UI_FLASH_AUTO,
	.awbSpeed = SP5K_AWB_SPEED_NORMAL,
	.err=VIEW_ERR_NULL,
	.osd = OSD_VIEW_DEFAULT
};
UINT8 IsHDR = 0;
UINT8 IsHDRNightScene = 0;
extern UINT8 TimLapVidFile[64];
extern UINT32 timelapseflag;
extern UINT8 timelapse_act;
extern UINT32 flash_idx;
extern UINT8 cdfsFirstCreate;
extern UINT8 fileTypeSelect;
extern UINT32 isBatteryCheckEnable ;
extern UINT8 gsensorTriggerRecordingStateFlag;
UINT8 StillCapModeBak[2]={PASM_DEFAULT,SCENE_DEFAULT};
/* Add by sc.liu */
static UINT32 objID[1];
static BOOL g_OBStart = FALSE;
BOOL ObjTrackTargetStop = FALSE;

#define OBJ_RIGHT_EDGE			940
#define OBJ_DOWN_EDGE			920
#define OBJ_LEFT_EDGE			100
#define OBJ_UP_EDGE 			100
#define OBJ_CENTER_X			512
#define OBJ_CENTER_Y			512
#define OBJ_WIDTH				140
#if SP5K_MOTION_DETECT
extern UINT32 SnapSetflag;
#endif

/* Add by sc.liu */
#if PRECOMPOSITION
#define COORDINATE_SHIFT_X		20
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appPowerOnPreviewFreeze(BOOL flag);
extern void appPreflashInit(void);
extern void appTbCIPA_CapNumShow(UINT32 Cnt);
extern void appAeFaceCfg(void);
extern void Icatsnap3Acb(void);
extern UINT32 appStillSpeedyBurstJobDone(void); // for clean warning
extern void applightFrequencySetting(LIGHT_FREQ_e lightfreq);// for clean warning
extern UINT32 appScriptCIPARun(UINT32 drive);// for clean warning
extern void dbgOsd_Print(UINT32 row, const char *format, ...);// for clean warning
extern void appErrMsgTimeISRWithSound(appErrMsgCaseOption_e errMsg,appErrMsgSoundOption_e errSound,UINT32 showTimeInMs);// for clean warning
extern SINT32 appObdResultGet(UINT32 param, UINT32 *pnobj, sp5kObjTrackResult_t *presult);// for clean warning
extern void util_start_connection(UINT8 wifiParm);
extern UINT32 appAWBALGLib_WBParamSet( UINT32 sel, UINT32 val );

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appStillPvStateOnKey(UINT32 msg,UINT32 param);
void appBtnEnableDelay(void);
void appAAAOff(void);
UINT32 _StillCheckDiskReady(void);


#if SPCA6330
void appStillCaptureTrigger(sp5kUsbTrigCaptureType_e cap_type)
{
	UINT32 sp5kMode;
	/*UINT32 ulActualEvents;*/
	uiPara_t* puiPara = appUiParaGet();

	printf("appStillCaptureTrigger: %d\n", cap_type);

	if (cap_type == SP5K_USB_PIMA_CAPTURE_TRIG_STILL) {

		printf("PTP triggers to capture still image...\n");
		/* todo : switch to still preview mode */
					
		static BOOL bFirst = TRUE;		
		//if(appSdvStillDiskCheckMemeoryFull()==TRUE)
			//return;				
		appStateChange(APP_STATE_SPORTDV_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);		
		printf("%s : change to capture state!\n",__FUNCTION__);		
        	//appStateChange(APP_STATE_STILL_AAA, APP_STATE_MSG_INIT);		
		
		if(bFirst == TRUE)
		{
			bFirst = FALSE;
	        	appOsDelayMs(100);	//wait ae convert
		}
		else
        	appOsDelayMs(100);
		//sp5kHostMsgSend(APP_KEY_PRESS_OK, PTP_KEY_MSG_PARAM);
		//appStateChange(APP_STATE_SPORTDV_STILL_CAPTURE,STATE_PARAM_NORMAL_INIT);
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
	else if (cap_type == SP5K_USB_PIMA_CAPTURE_TRIG_VIDEO) {
		printf("PTP triggers to capture video...\n");
		/* todo : switch to video mode */
		sp5kModeGet(&sp5kMode);
		if(sp5kMode!=SP5K_MODE_VIDEO_PREVIEW)
		{
			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
		}

		printf("%s : change to video pv/rec state!\n",__FUNCTION__);
		//appStateChange(APP_STATE_VIDEO_PREVIEW,  STATE_PARAM_NORMAL_INIT);
   		//appStateChange(APP_STATE_VIDEO_REC,  APP_STATE_MSG_INIT);
		sp5kHostMsgSend(APP_KEY_PRESS_OK, PTP_KEY_MSG_PARAM);
		//appStateChange(APP_STATE_WIFI_CONNECTION,  STATE_PARAM_NORMAL_INIT);
	}
}

void appStillCaptureAbort(sp5kUsbTrigCaptureType_e cap_type)
{
	UINT32 sp5kMode;
	sp5kModeGet(&sp5kMode);
	if (cap_type == SP5K_USB_PIMA_CAPTURE_TRIG_STILL) {
		printf("PTP stop capturing...\n");
		/* todo : switch to still preview mode */
		if(sp5kMode!=SP5K_MODE_STILL_PREVIEW)
		{
			sp5kModeSet(SP5K_MODE_STILL_PREVIEW);
			sp5kModeWait(SP5K_MODE_STILL_PREVIEW);
		}

	}
	else if (cap_type == SP5K_USB_PIMA_CAPTURE_TRIG_VIDEO) {
		printf("PTP stop recording...\n");
		/* todo : switch to video mode */
		if(sp5kMode!=SP5K_MODE_VIDEO_PREVIEW)
		{
			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
		}
	}
	//sp5kHostMsgSend(APP_KEY_PRESS_S1,0);
	//sp5kHostMsgSend(APP_KEY_PRESS_S2,0);
	sp5kHostMsgSend(APP_KEY_PRESS_OK, PTP_KEY_MSG_PARAM);
}
#endif /* SPCA6330 */

void appObjTrackTargetStop(
	void
)
{
	sp5kObjTrackTargetStop(objID[0]);
	objID[0] = 0xFF;
	g_OBStart = FALSE;
	ObjTrackTargetStop = TRUE;
}


void appObjTrackTargetStart(
	void
)
{
	UINT32 err;

	err = sp5kObjTrackTargetStart(&objID[0],OBJ_CENTER_X, OBJ_CENTER_Y, OBJ_WIDTH);
	ObjTrackTargetStop = FALSE;
}


UINT32
_StillCheckDiskReady(void)
{
	uiPara_t* puiPara = appUiParaGet();

	if (!IS_DISK_MOUNT_READY)
	{
		return FALSE;
	}

	gStillCB.isDiskReady = TRUE;
	if(IS_DISK_MOUNT_FAIL)
	{
		gStillCB.err = VIEW_ERR_CARD_ERROR;
	}
	else
	{
		gStillCB.err = VIEW_ERR_NO_ERROR;
		appStillSettingInit(VIEW_SET_DISK);
	}
	#if OLD_TLS_OPT
	if((puiPara->TimeLapse != UI_TIMELAPSE_OFF) && timelapse_act)
	{
		sp5kHostMsgSend(APP_UI_MSG_TIMELAPSE, 0);
	}
	#endif
	return TRUE;
}

UINT32
appStillWaitJobFinish(void)
{
	UINT32 waitJob, availabe;
    BOOL speedySaveNotReady = FALSE;

    while(appStillSpeedyBurstJobDone()) {
		appTimeDelay(100,TRUE);
		speedySaveNotReady = TRUE; //fix VSB400z:SPT-1962
		printf("View_WaitForBGStore speedy saving\n");
	}
	if(speedySaveNotReady) {
		/* show busy message*/
	}

	sp5kStillCapJobInfo(&waitJob, &availabe);

	while(waitJob)
	{
		APP_SLEEP_MS(20);
		sp5kStillCapJobInfo(&waitJob, &availabe);
	}
	return 0;
}

UINT32
appStillCheckReady(UINT32 question)
{
	UINT32 dskfree, ret, remain_number;
	uiPara_t* puiPara = appUiParaGet();

	if(gStillCB.err == VIEW_ERR_CARD_ERROR)
	{
		return STILL_STATE_NO;
	}

	switch (question)
	{
	default:
	case CAN_I_START_SNAP:

		if(!gStillCB.isDiskReady)
		{
			printf("%s: Fail (disk not ready)\n", __FUNCTION__);
			return STILL_STATE_NO;
		}
		if(IS_CARD_EXIST && IS_CARD_LOCK)
		{
			printf("%s: Fail (card lock)\n", __FUNCTION__);
			gStillCB.err = VIEW_ERR_CARD_LOCKED;
			return STILL_STATE_NO;
		}

		#if 1
		ret = appStillBackgroundGetFree(&dskfree);
		remain_number = dskfree/gStillCB.estJobFilesSize;

		#if CAP_FILE_ERROR  /*just for file error debug*/
		printf("\033[32m^^dskfree=%d, gStillCB.estJobFilesSize=%d, remain_number=%d^^\033[0m\n", dskfree, gStillCB.estJobFilesSize, remain_number);
		#endif
		/*add for mantis bug 0047510*/
		if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_SPEEDY_CAPTURE)
		{
			if((remain_number>0 && remain_number<25) && ret==SUCCESS)
			{
				printf("%s: Fail (speed capture_disk space not enough)\n", __FUNCTION__);
				gStillCB.err = VIEW_ERR_MEMORY_NOT_ENOUGH;
				return STILL_STATE_MEMORY_FULL;
			}
		}
		/*else if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_16IN1)
		{
			if((remain_number>0 && remain_number<8) && ret==SUCCESS)
			{
				printf("%s: Fail (16 in 1 capture_disk space not enough)\n", __FUNCTION__);
				gStillCB.err = VIEW_ERR_MEMORY_NOT_ENOUGH;
				return STILL_STATE_MEMORY_FULL;
			}
		}*/
		if(remain_number==0 && ret==SUCCESS)
		{
			printf("%s: Fail (disk space not enough)\n", __FUNCTION__);
			printf("%s: disk-free = %d kbyte, need size = %d kbyte \n", __FUNCTION__, dskfree, gStillCB.estJobFilesSize);
			gStillCB.err = VIEW_ERR_MEMORY_FULL;
			return STILL_STATE_MEMORY_FULL;
		}
		else if(remain_number<=2 && ret==FAIL)
		{
			printf("%s: Fail (disk space close to empty)\n", __FUNCTION__);
			printf("%s: disk-free = %d kbyte, need size = %d kbyte \n", __FUNCTION__, dskfree, gStillCB.estJobFilesSize);
			gStillCB.err = VIEW_ERR_NO_ERROR;
			return STILL_STATE_NO;
		}
		else
		{

            #if CAP_FILE_ERROR	/*just for file error debug*/
			printf("\033[32m^^^%s: OK (disk space enough)^^^\033[0m\n", __FUNCTION__);
			#endif
			gStillCB.err = VIEW_ERR_NO_ERROR;
		}
		#else
		appStillBackgroundGetFree(&dskfree);
		if(dskfree < gStillCB.estJobFilesSize){
			gStillCB.err = VIEW_ERR_MEMORY_FULL;
			return STILL_STATE_MEMORY_FULL;
		}
		else {
			gStillCB.err = VIEW_ERR_NO_ERROR;
		}
		#endif

		if(!appStrobeQueryReady() && gStillCB.flashMode!=UI_FLASH_OFF)
		{
			printf("%s: Fail (flash is charging)\n", __FUNCTION__);
			printf("%s: needflash=%d, flashready=%d\n", __FUNCTION__, puiPara->flash, appStrobeQueryReady());
			return STILL_STATE_FLASH_CHARGIN;
		}
		if(appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			printf("lens is moving, can not do aaa\n");
			return STILL_STATE_NO;
		}
		break;
	case CAN_I_LEAVE_STILL:
		break;
	}
	return STILL_STATE_YES;
}

void
appStillSettingInit(
	UINT32 flg
)
{
	appDiskInfo_t* pDsk;
	UINT32 cfg, ret;
	UINT32 qvtime[QV_TIME_MAX] = {0, 1 ,3 ,5};
	uiPara_t* puiPara = appUiParaGet();

	if(flg & VIEW_SET_STATE_START)
	{
		cfg = 0xffff;
		ret = sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_LENGTH, &cfg);
		cfg = 4; /* QVGA put on APP4 */
		ret = sp5kStillCapAdvCfgSet(SP5K_CAPTURE_QVGA_IMG, cfg);
		cfg = 0;
		ret = sp5kStillCapAdvCfgSet(SP5K_CAPTURE_SAVE_PROGRESSIVE_SIZE, cfg);
		/*appViewOsd_PicSizeDraw(TRUE, puiPara->ImageSize);*/  /*mask for mantis bug 0047296*/
		appStill_SetQuality(puiPara->ImageQuality);
		appStill_SetMetering(puiPara->AEMetering);
		appStill_SetWB(puiPara->WBMode);
		appStill_SetISO(puiPara->Iso);
		appStill_SetExposure(puiPara->AEMode);
		appStill_SetDriveMode(puiPara->DriverMode);
		appStill_SetAEB(puiPara->AEB);
		appStill_SetPasmSceneParam(gStillCB.cap.CurViewMode,gStillCB.cap.CurScene);
		appStill_SetHDR(IsHDR);
		appStill_SetSharpness(puiPara->Sharpness);
		#if OLD_TLS_OPT
		appStill_SetTimeLapse(puiPara->TimeLapse);
		#endif
		//appIqEffectSet(puiPara->ImageEffect);
		if(puiPara->FaceTracking)
		{
			puiPara->Stabilizer= 0; /* off */
		}
		appStill_SetStabilizer(puiPara->Stabilizer);
		appStill_SetPhotoFrame(puiPara->PhotoFrame);
		appStill_SetDZoom(puiPara->DigitalZoom);
		appStill_SetFlash(puiPara->flash);
		appStill_SetShutterSpeed(puiPara->shutter);
		appStill_SetApertureValue(pViewParam->aperture);
		appAeFrameRateSet(30);
		applightFrequencySetting(puiPara->LightFreq);
	}

	if(flg == VIEW_SET_ALL)
	{
		StillCapModeBak[0] = gStillCB.cap.CurViewMode = PASM_DEFAULT;
		StillCapModeBak[1] = gStillCB.cap.CurScene = SCENE_DEFAULT;

		appStill_SetPasmSceneParam(PASM_DEFAULT,SCENE_DEFAULT);
		appExif_SetExpProgram(PASM_DEFAULT,SCENE_DEFAULT);
		appExif_SetSceneCaptureType(PASM_DEFAULT, SCENE_DEFAULT);
	}

	gStillCB.estFileSize = appStillEstFileSize(puiPara->ImageSize, puiPara->ImageQuality, gStillCB.diskClusterSize);
	cfg = (puiPara->AEB == 1)? 3: IS_DRIVE_DOUBLE? 2: 1;
	gStillCB.estJobFilesSize = gStillCB.estFileSize*cfg;
	appStillBackgroundSet(gStillCB.estFileSize);

	if(flg & VIEW_SET_DISK)
	{
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
			appStillCheckReady(CAN_I_START_SNAP);
		}
	}
	// This may be set to zero in caputure state to interrupt the QV.
	gStillCB.cap.qvSec = qvtime[puiPara->AutoReviewTime];
	cfg = gStillCB.cap.qvSec*100;

	if(puiPara->ImageEffect==UI_EFFECT_SKETCH){
	   	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,0);/*kellogs modify 2011.12.1*/
	}
	else{
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,1);/*kellogs modify 2011.12.1*/
	}
	appStill_SetDateStamp(puiPara->StampMode, puiPara->DateStyle);
	sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, &cfg);
	// This may be changed in setup menu state. (this cost ~30msec, so we move behid osd)
	gStillCB.focusStatus = FOCUS_STATUS_VIEW;
	flash_idx =puiPara->flash;
}

void
appStillPvPowerOnInit(void)
{
	appCalibData_t *pcalib;

	DBG_PRINT("__appStillPowerOnInit\n");
	//sp5kStillCapBurstCallbackSet(appStillBurstCB);
	appStillRawCbSet(RAW_CB_SHUTTER_NORAML);
	appStillYuvCbSet(FALSE);
	sp5kStillCapExifHdrCallbackSet(appExifEncodeCb);
	//UiParamViewInit();
	//If we want to use fieldQV we should disable yuv callback.
	#if (FIELD_QV_ENABLE	== 1)
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 1);
	#else
		sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	#endif

	/*-- to register callback functions for MShutter */
	appLensMShutCallbackRegister();

	pcalib = appCalibDataGet();
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_SAVE_RAW, !!pcalib->StillRawSign);
}


void appStillCIPACB(
	void
)
{
	profLogAdd(0, "appStillCIPACB");

	if (FAIL==appScriptCIPARun(IS_CARD_EXIST ? SP5K_DRIVE_SD : SP5K_DRIVE_NAND)) {
		printf("\n*** Script file not found !! **********\n\n");
		dbgOsd_Print(4, "No Script File");
		APP_SLEEP_MS(2000);
	} else {
		dbgOsd_Print(4, "CIPA Start ...");
		APP_SLEEP_MS(2000);
	}

}
/*save current view and scene*/
void appStillCapStatusSave(void)
{
	StillCapModeBak[0]=gStillCB.cap.CurViewMode;
	StillCapModeBak[1]=gStillCB.cap.CurScene;
}

void appStillCapStatusRecover(void)
{
	gStillCB.cap.CurViewMode=StillCapModeBak[0];
	gStillCB.cap.CurScene=StillCapModeBak[1];
}

void
appStillPvStateInit(
	UINT32 preState
)
{
	static UINT32 doFirstTime = 1;
	UINT32 mode;
	uiPara_t* puiPara = appUiParaGet();

	appCalibData_t *pcalib= appCalibDataGet();
        profLogAdd(0,"appStillPvStateInit s");
//	if(IS_HDMI_IN){
		appBtnEnable(BTN_ALL);
//	}
	appPowerSavingEnableSet(TRUE);

	/* Do one time after power on. */
	if(doFirstTime)
	{
		VIEW_PERF_LOG(0, "pv init poweron -s");
		doFirstTime = 0;
		appStillPvPowerOnInit();
		VIEW_PERF_LOG(0, "pv init poweron -e");
	}
	#if SP5K_CDFS_OPT
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
	appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
	#endif

	#if CAM_TYPE_CVR
	if(puiPara->ImageSize == UI_PIC_SIZE_2M_169 )
	{
		puiPara->PhotoFrame = UI_PHOTOFRAME_OFF;
	}
	#else
	if(puiPara->ImageSize == UI_PIC_SIZE_2M_169 || puiPara->ImageSize == UI_PIC_SIZE_7M_169)
	{
		puiPara->PhotoFrame = UI_PHOTOFRAME_OFF;
	}
	#endif

	appStillOsd_UpdateFaceOsd(TRUE);
	appStillOsd_UpdateObjectOsd(TRUE);
	/* Do one time if you come from other state group or view re-entry */
	switch (preState)
	{
	case APP_STATE_BURST_CAPTURE:
		sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_CFG_BURST_FAST_SENSOR_TRIG,0);
	/*	sp5kStillCapAdvCfgSet( SP5K_CAPTURE_REENCODE_BYPASS , 0); */
	case APP_STATE_STILL_CAPTURE:
		#if PRECOMPOSITION
		appViewDrawPreComposition(TRUE, COORDINATE_SHIFT_X);
		#endif
		if(puiPara ->PeriodicalCap)
		{
			//sp5kHostMsgSend(SP5K_MSG_POWER_OFF,0);
			sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
			RPrintf("Enter periodical capture--->to power off\n");
			break;
		}
//	case APP_STATE_STILL_AAA:
	case APP_STATE_STILL_SELF_TIMER:
	case APP_STATE_STILL_SMILE_DETECT:
	#if PANORAMA_MODE
	case APP_STATE_STILL_PAN0RAMA:
	#endif
		appStill_SetFlash(gStillCB.flashMode);// we put here for none m-shutter case.
		appAePreviewRestore();
		if(!appStrobeQueryReady() && gStillCB.flashMode!=UI_FLASH_OFF)
		{
			LED_FLICKER_STROBE_CHARGE
		}
		else
		{
			LED_NORMAL_ON
		}
 	case APP_STATE_STILL_BUSY:
	//case APP_STATE_STILL_CWB:
	//case APP_STATE_STILLSENCEMENU:
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			appViewOsdShow(0,0,gStillCB.osd);
		}
		#else
		appViewOsdShow(0,0,gStillCB.osd);
		#endif
		if (pcalib->CIPASign ==1)
		{
			appTbCIPA_CapNumShow(pcalib->CIPACapCnt);
		}
		break;
#if !HDMI_MENU_BY_JPG
	case APP_STATE_MENU:
 		appViewOsd_AllClean();
		appStillSettingInit(VIEW_SET_ALL);
		appViewOsdShow(0,0,gStillCB.osd);
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
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
		break;
#endif
	case APP_STATE_STILL_CWB:
		appModeSet(SP5K_MODE_STILL_PREVIEW);
	case APP_STATE_POWER_ON:
		if (pcalib->CIPASign ==1)
		{
			profLogAdd(0, "CIPASign on");
			isBatteryCheckEnable =FALSE;
			sp5kTimerIsrOneShotReg( 0, 5000, appStillCIPACB);
		}
		StillCapModeBak[0]=puiPara->CurViewMode;
		StillCapModeBak[1]=puiPara->CurScene;

	default:

		/*appSensorPowerDownSet(FALSE);*/
		sensorDevPowerCustomCb(SENSOR_DEV_POWER_ID_SYS_ON);
		/* flow&osd control */
		gStillCB.s1press = 0;
		gStillCB.s2press = 0;
		gStillCB.s1AaaTicket=0;
		gStillCB.s2SnapTicket=0;
		gStillCB.tvAvSel = PASM_MMODE_SEL_TV;
		gStillCB.isLensReady = FALSE;
		gStillCB.isXDenoiseOn = 0;
		if(IS_VIEW_MODE_CHANGE)
		{
			gStillCB.osd = OSD_VIEW_SIMPLE;
		}

		/* pasm scene parameters */
		VIEW_PERF_LOG(0, "pv init pasm -s");

		if((preState!= APP_STATE_MENU) && (preState != APP_STATE_STILL_AAA))
		{
			appStillCapStatusRecover();
		}

		if(!IS_PASM_OR_SCENE_MODE)
		{
			gStillCB.cap.CurViewMode = PASM_PROGRAM;
		}

		appStill_SetPasmSceneParam(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appExif_SetExpProgram(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appExif_SetSceneCaptureType(gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
		appStillViewIqSet();
		VIEW_PERF_LOG(0, "pv init pasm -e");
		/* The size has to be set early than sp5k mode set */
		/*
		#if SP5K_SENSOR_OV2715
		puiPara->ImageSize = UI_PIC_SIZE_2M;
		appStill_SetResolution(puiPara->ImageSize);
		#else
		*/
		appStill_SetResolution(puiPara->ImageSize);
		//#endif

		/* make sure we are in still-view */
		sp5kModeGet(&mode);
		if(mode!=SP5K_MODE_STILL_PREVIEW || (!IS_VIEW_GROUP(preState)))
		{
			appModeSet(SP5K_MODE_STILL_PREVIEW);
		}

		appStillViewIqSet();
		sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, 0xffffffff);
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
#if SP5K_ALGORITHM_AWB
		sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
		sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif

		appPreflashInit();
		appAePreviewSet();
		/* appStillSettingInit(VIEW_SET_STATE_START);
		check disk */
		gStillCB.isDiskReady = 0;
		gStillCB.err = VIEW_ERR_NO_ERROR;
		#if SP5K_DISK_SPI
 	    if(IS_CARD_EXIST)
 	    #endif
 	    {
			if(_StillCheckDiskReady()==FALSE)
			{
				sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			}
			else
			{
				if(puiPara->PeriodicalCap)
				{
					if(IS_KEY_OK_PRESSED)
					{
						puiPara->PeriodicalCap = PERIODICAL_CAPTURE_OFF;
					}
					else
					{
						sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
					}
				}
				profLogAdd(0, "still pv disk ready");
			}
 	    }
		if(gStillCB.err == VIEW_ERR_MEMORY_FULL)
		{
			sp5kHostMsgSend(APP_UI_MSG_VIEW_MEMORY_FULL, 0);
		}
		appStillCheckReady(CAN_I_START_SNAP);
		if(gStillCB.err > VIEW_ERR_NO_ERROR && gStillCB.osd == OSD_VIEW_VERBOSE)
		{
			gStillCB.osd = OSD_VIEW_SIMPLE;
		}

		if(preState == APP_STATE_POWER_ON){	/* Mantis 44982 */
			appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
		}
		appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
#ifdef ALGORITHM_G4_AWB
		appAWBALGLib_WBParamSet ( 22/*AWB_PARA_PREVIEW_IMMEDIATELY*/, 0 );
		appAWBALGLib_WBParamSet ( 8/*AWB_PARA_UPDTREFE_FRAME_NUM*/, 64 );
#endif

		profLogAdd(0, "pv osd -s");
		if(preState == APP_STATE_POWER_ON)
		{
			appViewOsdShow(0,0,gStillCB.osd);
		}
		else
		{
			#if ICAT_WIFI
			if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
				appViewOsdShow(1,1,gStillCB.osd);
			}
			#else
				appViewOsdShow(1,1,gStillCB.osd);
			#endif
		}
		profLogAdd(0, "pv osd -e");

		/* lens control */
		VIEW_PERF_LOG(0, "pv init lens -s");
		#if KIT_WITH_LENS /* %REVIEW%, alex */
		{
			UINT32 qryResult;
			qryResult = appLensQuery(LENS_QUERY_BOOT_DONE);
			if (!qryResult)
			{
				/*-- Lens Never booted */
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
			//appLensStopWait();
			if(!appLensQuery(LENS_QUERY_MOTION_STATE))
			{
				profLogAdd(0, "still pv lens ready");
				gStillCB.isLensReady = TRUE;
				gStillCB.lensZoomSeg =  appLensQuery(LENS_QUERY_ZOOM_SEG_POS);
			}
		}
		VIEW_PERF_LOG(0, "pv init lens -e");
		#endif

		appPowerOnPreviewFreeze(FALSE); /* unfreeze preview if lens&3A ready(only for poweron flow)*/

		sp5kModeGet(&mode);
		while(mode!= SP5K_MODE_STILL_PREVIEW)
		{
			APP_SLEEP_MS(20);
			sp5kModeGet(&mode);
		}
		appStillSettingInit(VIEW_SET_STATE_START);

		APP_OSD_REFRESH_ON;
		sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, gStillCB.awbSpeed);
		#if SP5K_MOTION_DETECT
		if(puiPara->MotionDetect == UI_MD_ON) /*add for mantis bug 0048401*/
		{
			appMotionDetectSet(APP_MD_EN, 1);
		}
		#endif
		break;
	}

	/* To reset the first state of flow */
	appStillFlow_SetDriveFlow(puiPara->DriverMode,puiPara->AEB, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);
/* add by sc.liu */
	if(puiPara->FaceTracking == UI_FACETRACK_OBJECT_DETECT){
		UINT32 focus_x,focus_y;

		focus_x = APPVIEW_FOCUS_X;
		focus_y = APPVIEW_FOCUS_Y;

		#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			focus_x = APPVIEW_FOCUS_X*2 + 36;
			focus_y = APPVIEW_FOCUS_Y*3/2;
		}
		#endif

		appOsd_FontIconFontSet(ID_ICON_FOCUS);
		appOsdLib_OsdLayIconDraw(focus_x, focus_y, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
	}
	appObjTrackTargetStop();
/* add by sc.liu */
	if(gStillCB.err <= VIEW_ERR_NO_ERROR)
	{
		if(puiPara->FaceTracking== UI_FACETRACK_ON ||
			puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT ||
			puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
		{
			appStill_SetFaceTracking(VIEW_FT_STATUS_RUN);
		}
		else
		{
			appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
		}
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
	if(gStillCB.s1AaaTicket)
	{
		appStillPvStateOnKey(APP_KEY_PRESS_S1, 0);
	}

	if(appStill_GetMiniatureStatus())
	{
		sp5kPreviewUrgentCallbackSet(1, appMiniatureEffectPvCb);
	}
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
	#if 0//ICAT_WIFI==1 /* WiFi driver init, dhcp server, host ap, ptp-ip */
	static BOOL gbnetON = TRUE;
	if(gbnetON)
	{
		delay_ms(3000); /* delay to prevent conflict with sd mount */
		puiPara->VideoSize=UI_VID_SIZE_HD_30FPS;
		util_start_connection(0x1F);
		gbnetON = FALSE;
	}

	#endif

    profLogAdd(0,"appStillPvStateInit e");
}

void
appStillPvStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;
	UINT8 FileName[40];
	/*sp5kAeResult_t ae;*/ 	// for clean warning
	/*float Tv, Sv, tmp1,tmp2, Lv;*/  // for clean warning
	uiPara_t* puiPara = appUiParaGet();

	switch ( msg)
	{
	/* storage check */
	case SP5K_MSG_MODE_READY:
	if(param == 0x11)
	{
		#if ICAT_WIFI
		if(appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE){
			//appModeSet(SP5K_MODE_STANDBY);
		}
		app_PTP_Set_DscOpMode(PTP_DSCOP_MODE_CAMERA);
		#endif
	}
	break;
	case APP_UI_MSG_VIEW_WAIT_DISK:
		DBG_PRINT("APP_UI_MSG_VIEW_WAIT_DISK\n");

		if(_StillCheckDiskReady()==FALSE)
		{
			APP_SLEEP_MS(20);
			sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
			break;
		}
		else if(puiPara->PeriodicalCap)
		{
			if(IS_KEY_OK_PRESSED)
			{
				puiPara->PeriodicalCap = PERIODICAL_CAPTURE_OFF;
			}
			else
			{
				sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
			}

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
		profLogAdd(0, "still pv disk ready");
		appStillCheckReady(CAN_I_START_SNAP);
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			appViewOsdShow(0,0,gStillCB.osd);
		}
		#else
		appViewOsdShow(0,0,gStillCB.osd);
		#endif
		break;
	case APP_UI_MSG_VIEW_DISKSIZE_READY:
		appStillCheckReady(CAN_I_START_SNAP);
		#if ICAT_WIFI
		if((appWiFiConnection_UtilityStateGet() <= WIFI_UTILITY_CLOSE)){
			appViewOsdShow(0,0,gStillCB.osd);
		}
		#else
		appViewOsdShow(0,0,gStillCB.osd);
		#endif
		break;
	/* view handling */
	case SP5K_MSG_AE_READY:
        #if  SP5K_MOTION_DETECT
		if(puiPara->MotionDetect == 1)
	    {
			appMotionDetectHdr();
		}
        #endif
		#if SP5K_ALGORITHM_AE

		#else
		appAePreviewAdjust(param);
		#if KIT_WITH_LENS
		if(!gStillCB.isLensReady)
		{
			if(!appLensQuery(LENS_QUERY_MOTION_STATE))
			{
				profLogAdd(0, "still pv lens ready a");
				gStillCB.isLensReady = TRUE;
				gStillCB.lensZoomSeg =  appLensQuery(LENS_QUERY_ZOOM_SEG_POS);
			}
		}
		#endif
		#endif
		break;
	case SP5K_MSG_AWB_READY:
		break;
	case SP5K_MSG_AF_READY:
		DBG_PRINT("__OnMsg: AF_READY\n");
		break;
/* Modify sc.liu */
#if !CAM_TYPE_CVR
	case SP5K_MSG_OBJ_TRACK_START:
		g_OBStart = TRUE;
		break;
	case SP5K_MSG_OBJ_TRACK_READY:
		{
			UINT32 obj=0;
			SINT32 objerr=0;
			sp5kObjTrackResult_t objlist[5];
			if(!g_OBStart)
				return;

			objerr = appObdResultGet(param, &obj, objlist);
			printf("X = %d, Y = %d\n", objlist[0].pos.elem.y, objlist[0].pos.elem.y);
			if(objlist[0].pos.elem.x >= OBJ_RIGHT_EDGE || objlist[0].pos.elem.x <= OBJ_LEFT_EDGE
				|| objlist[0].pos.elem.y >= OBJ_DOWN_EDGE || objlist[0].pos.elem.y <= OBJ_UP_EDGE)
			{
				UINT32 focus_x,focus_y;

				focus_x = APPVIEW_FOCUS_X;
				focus_y = APPVIEW_FOCUS_Y;

				#if KIT_WITH_HDMI
				if(IS_HDMI_IN){
					focus_x = APPVIEW_FOCUS_X*2 + 36;
					focus_y = APPVIEW_FOCUS_Y*3/2;
				}
				#endif

				appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
				sp5kObjTrackTargetStop(objID[0]);
				appOsd_FontIconFontSet(ID_ICON_FOCUS);
				appOsdLib_OsdLayIconDraw(focus_x, focus_y, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
				objID[0] = 0xFF;
				g_OBStart = FALSE;
				ObjTrackTargetStop = TRUE;
			}
			if (objerr!=-1)
			{
				appOdDataUpdate(obj,objlist);
				appOd_ObjectRectUpdateMsgSend();
			}
		}
		break;
/* Modify sc.liu */
	case SP5K_MSG_FACE_TRACK_READY:
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
#endif
	case SP5K_MSG_FACE_TRACK_STOP:
		break;
	case SP5K_MSG_FACE_TRACK_INVALID:
		break;

	/* DZOOM Position bar update */
	case APP_UI_MSG_DZOOM_POS_UPDATE:
		tmpU32 = 0xFFUL & param;
		appViewOsd_ZoomFrameDraw(TRUE);
		appViewOsd_DigZoomPosDraw((UINT8)tmpU32);
		break;
	/* TELE/WIDE Self Repeat */
	case APP_UI_MSG_LENS_DZOOM_TELE:
		appStillDZoomTele();
		break;
	case APP_UI_MSG_LENS_DZOOM_WIDE:
		appStillDZoomWide();
		break;
	/* face trac */
	case APP_UI_MSG_FACE_DETECT_OSD_UPDATE:
		if(puiPara->FaceTracking== UI_FACETRACK_OFF || puiPara->FaceTracking == UI_FACETRACK_OBJECT_DETECT ){
			break;
		}
		if(!appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			if(puiPara->FaceTracking)
			{
				appStillOsd_UpdateFaceOsd(FALSE);
				#if FACE_WITH_AE
				appAeFaceCfg();
				#endif
			}
		}
		break;
/* Modify sc.liu */
	case APP_UI_MSG_OBJECT_DETECT_OSD_UPDATE:
		if(!appLensQuery(LENS_QUERY_MOTION_STATE))
		{
			appStillOsd_UpdateObjectOsd(FALSE);
			#if FACE_WITH_AE
				appAeFaceCfg();
			#endif
		}
		break;
/* Modify sc.liu */
	case APP_UI_MSG_VIEW_MEMORY_FULL:
		appStateChange(APP_STATE_STILL_BUSY, APP_STATE_MSG_INIT);
		break;
	case APP_UI_MSG_VIEW_STROB_TEST_START:
		appStateChange(APP_STATE_STILL_STROB_TEST, APP_STATE_MSG_INIT);
		break;
	case SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE:
		gStillCB.osd = OSD_VIEW_SIMPLE;
		appViewOsdShow(1,0,gStillCB.osd);
		break;
	#if ICAT_WIFI /* for demo, show WiFi icon */
	case SP5K_MSG_TIMER :
		break;
	#endif
    #if SP5K_MOTION_DETECT
	case APP_UI_MSG_MD_START:
		SnapSetflag = 1;
		sp5kHostMsgSend(APP_KEY_PRESS_S1 , 0);
		break;
	case APP_UI_MSG_MD_STOP:
		break;
    #endif
	#if OLD_TLS_OPT
	case APP_UI_MSG_TIMELAPSE:
		if(puiPara->TimeLapse >= TIMELAPSE_POWER_OFF && timelapse_act)
		{
	     #if CAM_TYPE_CVR
			strcpy(FileName, appCdfsLastAviNameGet());
			sprintf(TimLapVidFile,"D:\\VIDEO\\%s",&FileName[strlen(FileName)-19]);
	     #endif
			appStillTimelapseTaskInit(1);
		}
		break;
	#endif
	default:
		DBG_PRINT(".");
		break;
	}
}

void
appStillPvStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 temp=0,next_state,next_param;
	uiPara_t* puiPara = appUiParaGet();
	UINT32 focus_x,focus_y;
	#if OLD_TLS_OPT
	UINT16 curDirIdx, curFileIdx;
	UINT8 FileName[40];
	#endif

	focus_x = APPVIEW_FOCUS_X;
	focus_y = APPVIEW_FOCUS_Y;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		focus_x = APPVIEW_FOCUS_X*2 + 36; /*36 = sp5kGfxIconSizeGet(ID_ICON_FOCUS,...)/2*/
		focus_y = APPVIEW_FOCUS_Y*3/2;
	}
	#endif

	switch (msg)
	{
	case APP_KEY_PRESS_S2:
		#if OLD_TLS_OPT
		if(puiPara->TimeLapse == UI_TIMELAPSE_OFF)
		{
		#endif
			#if PTP_CAPTURE&&ICAT_WIFI
			if(appPtpStatGet())
			{
				appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SHUTTER_S2_ON, 0, 0);
			}
			else
			#endif
			{
				gStillCB.s2SnapTicket=1;
				if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_SPEEDY_CAPTURE || puiPara->FeatureEffect == UI_FEATURE_EFFECT_16IN1)
				{
					gsensorTriggerRecordingStateFlag = 0;
					break;
				}
			}
		#if OLD_TLS_OPT
		}
		else
		{
			break;
		}
		#endif
	case APP_KEY_PRESS_S1:
		profLogAdd(3,"Snap Key");
		#if OLD_TLS_OPT
		if(puiPara->TimeLapse != UI_TIMELAPSE_OFF)
		{
			if(param != 0xaf)
			{
			#if CAM_TYPE_CVR
                #if SP5K_CDFS_OPT
				appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
				appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
				#endif
				appCdfsNextFileNameGet(SP5K_DCF_FILETYPE_MOV,FileName);
				sprintf(TimLapVidFile,"D:\\VIDEO\\%s",&FileName[strlen(FileName)-19]);
			#else
				sp5kDcfFsNextKeyGet(&curDirIdx, &curFileIdx);
				if(puiPara->MediaFormat == MEDIA_FORMAT_MOV)
				{
					sprintf(TimLapVidFile, "D:\\DCIM\\%03dMEDIA\\SUNP%04d.MOV", curDirIdx,curFileIdx);
				}
				else
				{
					sprintf(TimLapVidFile, "D:\\DCIM\\%03dMEDIA\\SUNP%04d.AVI", curDirIdx,curFileIdx);
				}
				curFileIdx ++;
				sp5kDcfFsNextKeySet(curDirIdx,curFileIdx);
			#endif
				appStillTimelapseTaskInit(1);
				break;
			}
		}
		#endif
		#if PTP_CAPTURE&&ICAT_WIFI
         if(appPtpStatGet())
         {
	         appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SHUTTER_S1_ON, 0, 0);
         }
         else
		 #endif
		 {
		 printf("PV S1 Start\n");
		if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_SPEEDY_CAPTURE || puiPara->FeatureEffect == UI_FEATURE_EFFECT_16IN1)
		{
			if(appStillCheckReady(CAN_I_START_SNAP) != STILL_STATE_MEMORY_FULL )/*add for mantis bug 0047510*/
			{
				appBtnDisable(BTN_ALL);
				appOsdMessageBoxShow(TRUE, ID_STR_IMAGE_SAVED);
				if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_SPEEDY_CAPTURE)
				{
					appStillSpeedyCapture(60);
				}
				else
					app16in1Capture(16);
			}
			break;
		}
		if(appStillCheckReady(CAN_I_START_SNAP)==STILL_STATE_YES)
		{
			appLensStopWait();
			if((IS_SCENE_MODE && IS_SMILE_DETECTION_MODE)||puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT)
			{
			if(appStill_GetMainFaceData(NULL,NULL,NULL,NULL) == SUCCESS)
			{
				appStillFlow_GetNextState(&next_state, &next_param);
				appStateChange(next_state,next_param);
			}
			}
			else if ((IS_SCENE_MODE && IS_BLINK_DETECTION_MODE) ||puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
			{
				appStillFlow_GetNextState(&next_state, &next_param);
				appStateChange(next_state,next_param);
			}
			else
			{
	            #if PRECOMPOSITION
				sp5kGfxAttrSet(SP5K_GFX_LAYER_ALPHA, SP5K_GFX_PAGE_PF_0, 0, 0, 0);
	            #endif
				appStill_SetFaceTracking(VIEW_FT_STATUS_PAUSE);
				appStillFlow_GetNextState(&next_state, &next_param);
				appStateChange(next_state,next_param);
			}
		}
		else
		{
			printf("PV S1 Error\n");
			#if SP5K_MOTION_DETECT
			SnapSetflag = 0;
			#endif
			gStillCB.s2SnapTicket = 0;
			if(gStillCB.err != VIEW_ERR_NO_ERROR)
			{
				appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
				//appViewOsd_WarmingMsgDraw(1, gStillCB.err, TRUE);
			}
		}
		gsensorTriggerRecordingStateFlag = 0;
		printf("PV S1 End\n");
        }
		break;
	case APP_KEY_PRESS_MENU:
		GREEN_LED_ON;
		appBtnDisable(BTN_MODE);
		appStillOsd_UpdateFaceOsd(TRUE);
		appStillOsd_UpdateObjectOsd(TRUE);	/* add by sc.liu */
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_DEL:
		appStillOsd_UpdateFaceOsd(TRUE);
		appStillOsd_UpdateObjectOsd(TRUE);	/* add by sc.liu */
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_SET:
		if(IS_PASM_MMODE)
		{
			gStillCB.tvAvSel = (IS_PASM_MMODE_TV)? PASM_MMODE_SEL_AV: PASM_MMODE_SEL_TV;
		}
		else
		{
			gStillCB.osd++;
			if (gStillCB.osd >= OSD_VIEW_CLEAN_ALL)
			{
				gStillCB.osd = OSD_VIEW_SIMPLE;
			}
		}
		appViewOsdShow(0,0,gStillCB.osd);
		break;
	case APP_KEY_PRESS_MODE:
		appStillCapStatusSave();
		#if CAM_TYPE_CVR
		fileTypeSelect=1;
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
		#else
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		#endif
		break;
	case APP_KEY_PRESS_PB:
	  #if !CAM_TYPE_CVR
		appStillCapStatusSave();
		#if SP5K_CDFS_OPT
		fileTypeSelect=1;
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
		#else
		appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
		#endif
     #endif
		break;
	case APP_KEY_PRESS_UP:
		/*s Mask by Aries 12/02/09*/
		#if 0
		#if PANORAMA_MODE
		if((IS_SCENE_MODE)&&(IS_PANORAMA_MODE))
		{
			pViewParam->panorama=appView_PANOItemDecrease(UI_PV_PANORAMA_MAX,pViewParam->panorama);
			appViewOsd_PanoramaDraw(1,pViewParam->panorama);
			break;
		}
		#endif
		#endif
		/*e Mask by Aries 12/02/09*/
		if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_SPEEDY_CAPTURE \
			|| puiPara->FeatureEffect == UI_FEATURE_EFFECT_16IN1 /*\
			||puiPara->TimeLapse != UI_TIMELAPSE_OFF*/)
		{
			break;
		}

		puiPara->ImageSize++;
		if(puiPara->ImageSize >= UI_PIC_SIZE_MAX)
		{
			puiPara->ImageSize = UI_PIC_SIZE_FULL;
		}

		appStillSettingInit(VIEW_SET_DISK);
		appViewOsdShow(1,0,gStillCB.osd);
		appStill_SetResolution(puiPara->ImageSize); /*add for mantis bug 0047554*/
		#if CAM_TYPE_CVR

		if(puiPara->ImageSize==UI_PIC_SIZE_2M_169){
			 puiPara->PhotoFrame= UI_PHOTOFRAME_OFF;
			 appStill_SetPhotoFrame(puiPara->PhotoFrame);
		}
		#else
		if(puiPara->ImageSize==UI_PIC_SIZE_7M_169||puiPara->ImageSize==UI_PIC_SIZE_2M_169){
			 puiPara->PhotoFrame= UI_PHOTOFRAME_OFF;
			 appStill_SetPhotoFrame(puiPara->PhotoFrame);
		}
		#endif

		break;
	case APP_KEY_PRESS_DOWN:
		/* add by sc.liu */
		if(objID[0] != 0xFF)
		{
			appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);
			appObjTrackTargetStop();
			appOsd_FontIconFontSet(ID_ICON_FOCUS);
			appOsdLib_OsdLayIconDraw(focus_x, focus_y, SP5K_GFX_ALIGN_TOP_LEFT, 0x0000);
			break;
		}
		if(puiPara->FaceTracking == UI_FACETRACK_OBJECT_DETECT){
			appObjTrackTargetStart();
		}
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, focus_x, focus_y, APPVIEW_FOCUS_W, APPVIEW_FOCUS_H,0,0);
		/* add by sc.liu */
		break;
	case APP_KEY_PRESS_LEFT:
		#ifdef HW_EVB_6330
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN) /*add for mantis bug 0047470*/
		{
			break;
		}
		#endif
		#endif
		/*s Modify by Aries 12/02/09*/
		#if 0/* PANORAMA_MODE*/
		if((IS_SCENE_MODE)&&(IS_PANORAMA_MODE))
		{
			pViewParam->panorama=appView_PANOItemIncrease(UI_PV_PANORAMA_MAX,pViewParam->panorama);
			appViewOsd_PanoramaDraw(TRUE,pViewParam->panorama);
			break;
		}
		#endif
	#ifdef HW_PRJ_6330
		CLEAR_OSD_SRC;
		appStillCapStatusSave();
        #if CAM_TYPE_CVR
		fileTypeSelect=1;
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
        #else
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
	    #endif
	#else
        #if !CAM_TYPE_CVR
		appMenu_PosPreset(MENU_MARK_SCENEMODEITEM);
		appStateChange(APP_STATE_MENU,STATE_PARAM_NORMAL_INIT);
        #endif
	#endif
		/*e Modify by Aries 12/02/09*/
		break;
	case APP_KEY_PRESS_RIGHT:
		#if ICAT_WIFI==1
		printf("\n@@-----WiFi connection, entering WiFi state-----\n");
		appStateChange(APP_STATE_VIDEO_PREVIEW, APP_STATE_MSG_INIT);
		appStateChange(APP_STATE_WIFI_CONNECTION, APP_STATE_MSG_INIT);
		#endif
		/*s Modify by Aries 12/02/09*/
		#if 0/* PANORAMA_MODE*/
		if((IS_SCENE_MODE)&&(IS_PANORAMA_MODE))
		{
			pViewParam->panorama=appView_PANOItemDecrease(UI_PV_PANORAMA_MAX,pViewParam->panorama);
			appViewOsd_PanoramaDraw(TRUE,pViewParam->panorama);
			break;
		}
		#endif
		/*s Modify by Aries 12/02/09*/
		break;
	case APP_KEY_PRESS_TELE:
		if(appZoomNextModeQuery(APP_ZOOM_TELE) == APP_ZOOM_NEXT_LENS)
		{
			appStill_SetFaceTracking(VIEW_FT_STATUS_RESETDATA_PAUSE);
			appStillOsd_UpdateFaceOsd(TRUE);
		}
		if(appStrobeQueryChargeEnable() && !appStrobeQueryReady())
		{
			break;
		}
		appStillZoomTele();
		#if ICAT_WIFI && SPCA6330
		appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_ZOOM_VECTOR,(UINT8)(appDZoomGetRation()/1000),0);
		#endif
		break;
	case APP_KEY_PRESS_WIDE:
		if(appZoomNextModeQuery(APP_ZOOM_WIDE) == APP_ZOOM_NEXT_LENS) {
			appStill_SetFaceTracking(VIEW_FT_STATUS_RESETDATA_PAUSE);
			appStillOsd_UpdateFaceOsd(TRUE);
		}
		if(appStrobeQueryChargeEnable() && !appStrobeQueryReady())
		{
			break;
		}
		appStillZoomWide();
		#if ICAT_WIFI && SPCA6330
		appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_ZOOM_VECTOR,(UINT8)appDZoomGetRation()/1000,0);
		#endif
		break;
	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
		appZoomStop();
		if(IS_PASM_AMODE||IS_PASM_MMODE)
		{
			appViewOsdShow(0,0,gStillCB.osd);
		}
		break;

	case APP_KEY_PRESS_FLASH:
		if(puiPara->DriverMode == UI_DRIVE_BURST)
		{
			break;
		}
		temp = puiPara->flash;
 		appStill_ChangeFlash();
		if(gStillCB.osd == OSD_VIEW_NONE ||gStillCB.osd == OSD_VIEW_GRID)
		{
			gStillCB.osd = OSD_VIEW_SIMPLE;
			appViewOsdShow(0,0,gStillCB.osd);
		}
		else
		{
			appViewOsd_FlashModeDraw(TRUE, gStillCB.flashMode);
		}
		if(temp == puiPara->flash)
		{
			return;
		}
		appStill_SetFlash(gStillCB.flashMode);
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
		case APP_KEY_PRESS_UP:
		appBeep(TRUE, FALSE);
		break;
		default:
		break;
	}
}

/*
The following variable and function are used for AF testing and tuning.

Brian.
*/
static UINT32 CurrAFRDYParam = 0;
UINT32 CurrAFRDYParamGet(void)
{
	return CurrAFRDYParam;
}


void
appStillPvState(
	UINT32 msg,
	UINT32 param
)
{
	//For AF testing and tuning.
	if(msg == SP5K_MSG_AF_READY)
	{
		CurrAFRDYParam = param;
	}


	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appStillPvState\n");
		appStillPvStateInit(appPreviousStateGet());
		profLogAdd(3,"Still view State");
		break;
	case APP_STATE_MSG_CLOSE:
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appStillPvStateOnKey(msg,param);
		}
		else
		{
			appStillPvStateOnMsg(msg,param);
		}
		break;
	}
}

/*-------------------------------------------------------------------------
 *  Function Name : appViewInit
 *  Description : This function can make sure you are in sitll/vieo preview
 				  status.
 *------------------------------------------------------------------------*/

void appViewInit(void)
{
	UINT32 mode;
	uiPara_t* puiPara = appUiParaGet();

   	sp5kModeGet(&mode);
	if(IS_PASM_OR_SCENE_MODE && mode!=SP5K_MODE_STILL_PREVIEW)
	{
		appStill_SetResolution(puiPara->ImageSize);
		appModeSet(SP5K_MODE_STILL_PREVIEW);
		appStillSettingInit(VIEW_SET_STATE_START);
	}
	#ifdef NO_VIDEO_PREVIEW_MODE
	if(IS_VIDEO_MODE && mode!=SP5K_MODE_STILL_PREVIEW)
	#else
	if(IS_VIDEO_MODE && mode!=SP5K_MODE_VIDEO_PREVIEW)
	#endif
	{
		appVidSizeSet(puiPara->VideoSize);
		//sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	}
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	appStillViewIqSet();
	appAePreviewSet();
}


void
appAAAOff(
	void
)
{
	/* Mantis 46692, 46771, 46755 */
	sp5kAeModeSet(SP5K_AE_MODE_OFF);
	sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
}


void
appBtnEnableDelay(
	void
)
{
	/* Mantis 45321 */
	BPrintf("Button Enable");
	appBtnControl(BTN_ALL, ENABLE);
}

#if CAP_VID_PV
void appCapVidPreviewInit(void) /*add by JQ*/
{
	UINT32 mode;
	uiPara_t* puiPara = appUiParaGet();

		/* flow&osd control */
		gStillCB.s1press = 0;
		gStillCB.s2press = 0;
		gStillCB.s1AaaTicket=0;
		gStillCB.s2SnapTicket=0;

		appStill_SetResolution(puiPara->ImageSize);

		appStillViewIqSet();
		sp5kAwbParamSet(SP5K_AWB_CONVERGE_SPEED, 0xffffffff);
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
#if SP5K_ALGORITHM_AWB
		sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#else
		sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
#endif

		appPreflashInit();
		appAePreviewSet();
		gStillCB.isDiskReady = 0;
		gStillCB.err = VIEW_ERR_NO_ERROR;

#ifdef ALGORITHM_G4_AWB
		appAWBALGLib_WBParamSet ( 22/*AWB_PARA_PREVIEW_IMMEDIATELY*/, 0 );
		appAWBALGLib_WBParamSet ( 8/*AWB_PARA_UPDTREFE_FRAME_NUM*/, 64 );
#endif

		appStillSettingInit(VIEW_SET_STATE_START);
}
#endif

