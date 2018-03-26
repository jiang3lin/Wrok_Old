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
#if SPCA6330
#include "app_ptp.h"
#endif
#if ICAT_WIFI
#include "app_wifi_connection.h"
#include "app_wifi_utility.h"
#endif
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
extern void apptbCIPA_Log(UINT32 capCnt);
extern void appStrobeChargeTimerReset(void);
extern void appShowCurExpAgc(void);
extern void _stillUtil_ShutterDelaySet(void);
extern void appAeStableSet(void);
extern void _processAfterFastAe(UINT32 ae_result);
extern void appAeStableReset(void);
extern SINT32 appFdResultGet(UINT32 param,UINT32 *pnface,sp5kFaceTrackFaceRes_t *presult);
extern void appFdBlinkDetectSemPut(void);
extern void  appFdBlinkDetectCheckEnd( void );
extern void appFdBlinkDetectSet(UINT32 set);


 #if KIT_SHUT_CTRL_TIMER
extern UINT32 CapExpTime=0;
extern UINT32 CapShutColseTime=0;
#endif

extern UINT32 BlinkYUVCallBackSet;
extern UINT32 BlinkGet;
extern UINT32 BlinkSaveSelect;
#if CAP_VID_PV
extern UINT32 CapVidPreview;
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 * Function Name : appStillCapFlashStatusGet
 *------------------------------------------------------------------------*/
 UINT32 appStillCapFlashStatusGet(void)
{
/*0 :Not have Flash*/
/*1 :Need Flash*/
/*2 :Fake Flash*/

//lester 20120612 s
#ifdef FLASH_ROLLING
	return appAeFlashNeedGet(); /* Eric 110823 determine by AE */
#else
	if(gStillCB.needFlash) 
	{
		return 1;	
	}
	else 
	{
		return 0;	
	}
#endif
//lester 20120612 e

}


/*-------------------------------------------------------------------------
 * Function Name : appStillCapExpTimeGet
 *------------------------------------------------------------------------*/
 UINT32 appStillCapExpTimeGet(void)
{
	#if KIT_SHUT_CTRL_TIMER
		return CapExpTime;	
	#else
		return 0;	
	#endif
}


/*-------------------------------------------------------------------------
 * Function Name : appStillCapShutCloseTimeGet
 *------------------------------------------------------------------------*/
 UINT32 appStillCapShutCloseTimeGet(void)
{
	#if KIT_SHUT_CTRL_TIMER
		return CapShutColseTime;
	#else
		return 0;	
	#endif
}

void
IcatUsbCBCapStateInit(void)
{
	UINT32 snapPulseW;
	appCalibData_t *pcalib= appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();

	appAeCaptureSet(0);


//lester 20120612 s
#ifdef FLASH_ROLLING
	printf("<lester> IcatUsbCBCapStateInit \n");
	appStillCapFlashStatusGet();
	appStrobeChargeTimerReset(); 
#else
	if(gStillCB.needFlash) 
	{
		DBG_PRINT("cap: need flash\n");
		printf("cap: need flash\n");
		appPreflashConfig((puiPara->flash==UI_FLASH_SLOW_SYNC)? SP5K_SNAP_FLASH_MODE_FORCED_ON_REAR_SYNC : SP5K_SNAP_FLASH_MODE_FORCED_ON); 
		appPreflashTrig();

		snapPulseW = appSnapFlashCalculate(appSnapflashTargetGet());
		if (snapPulseW) 
		{
					
			appSnapFlashInstall(/*NULL*/0, snapPulseW, appSnapflashCfgHPosGet());
			// update the ae information after flash.
			appAeCaptureFlashAeOverrideSet();
			appAeGetCurrentExpIsoUiIdx(&gStillCB.ae.shutter, &gStillCB.ae.iso);
			appStrobeChargeTimerReset();
		}
		else 
		{
			appSnapFlashDismiss();
		}
	}
	else 
	{
		DBG_PRINT("cap: don't need flash\n");

		#if DEBUG_PREFH_USE_DATE_STAMP
		{
			appPrefhCb_t* pprefhCb;
			pprefhCb = appPrefhCbGet();
			if (pprefhCb->isStampDbg==TRUE) 
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, NULL);
			}
		}
		#endif
	}
#endif
//lester 20120612 e

	appShowCurExpAgc();

	appStill_SetSp5kStillSize(puiPara->ImageSize);
	appStill_SetFlash(FLASH_CHARGE_STOP); 
	profLogAdd(0, "cap destroy ft -s");
	appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	profLogAdd(0, "cap destroy ft -e");
	profLogAdd(0, "cap iqset -s");
	appStillSnapIqSet();
//	appIqSaturationFactorSet(120); /*Saturation Normal*/ /*110 ->1.1x */ 
	profLogAdd(0, "cap iqset -e");
	#if (FIELD_QV_ENABLE	== 1)
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 1);
	#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	#endif
	#if CAP_AUTO_ENABLE
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, 1);
	#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, -1);
	#endif
	gStillCB.isReady2View = 0;

	if (pcalib->CIPASign ==1)
	{
		pcalib->CIPACapCnt++;
		apptbCIPA_Log(pcalib->CIPACapCnt);
	}

	appShowCurExpAgc();
	profLogAdd(0, "cap sp5kModeSet()");
	_stillUtil_ShutterDelaySet();

}



void Icatsnap3Acb(void)
{
	APP_SLEEP_MS(200);
	/*sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_VLC_BUF_NUM, 1);*/
	/*appAeStableReset();*/
	appPreflashInit();  /*add for mantis bug 0047558*/
	appAeStableSet();
       _processAfterFastAe(appAeFastAdjust(0));			
	IcatUsbCBCapStateInit();
	appAeStableReset();
	appAePreviewRestore();
	APP_SLEEP_MS(200);

}



void
appStillCapStateInit(void)
{
	UINT32 snapPulseW;
	appCalibData_t *pcalib= appCalibDataGet();
	uiPara_t* puiPara = appUiParaGet();

	appShowCurExpAgc();
	appAeCaptureSet(0);
	appShowCurExpAgc();

//lester 20120612 s
#ifdef FLASH_ROLLING
	printf("<lester> appStillCapStateInit\n");
	appStillCapFlashStatusGet();
	appStrobeChargeTimerReset(); 
#else
	if(gStillCB.needFlash) 
	{
		DBG_PRINT("cap: need flash\n");		
		appPreflashConfig((puiPara->flash==UI_FLASH_SLOW_SYNC)? SP5K_SNAP_FLASH_MODE_FORCED_ON_REAR_SYNC : SP5K_SNAP_FLASH_MODE_FORCED_ON); // supposedly, we dont need to config all the time
		appPreflashTrig();//blocking till SP5K_MSG_PREFLASH_READY

		snapPulseW = appSnapFlashCalculate(appSnapflashTargetGet());
		if (snapPulseW)
		{
					
			appSnapFlashInstall(/*NULL*/0, snapPulseW, appSnapflashCfgHPosGet());
			// update the ae information after flash.
			appAeCaptureFlashAeOverrideSet();
			appAeGetCurrentExpIsoUiIdx(&gStillCB.ae.shutter, &gStillCB.ae.iso);
			appStrobeChargeTimerReset();
		} 
		else
		{
			appSnapFlashDismiss();
		}
	}
	else 
	{
		DBG_PRINT("cap: don't need flash\n");

		#if DEBUG_PREFH_USE_DATE_STAMP
		{
			appPrefhCb_t* pprefhCb;
			pprefhCb = appPrefhCbGet();
			if (pprefhCb->isStampDbg==TRUE) 
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_STAMP, NULL);
			}
		}
		#endif
	}
#endif

	appShowCurExpAgc();

	appStill_SetSp5kStillSize(puiPara->ImageSize);
	appStill_SetFlash(FLASH_CHARGE_STOP); // This will turn off the flash but keep its setting.
	profLogAdd(0, "cap destroy ft -s");
	appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	profLogAdd(0, "cap destroy ft -e");
	profLogAdd(0, "cap iqset -s");
	appStillSnapIqSet();
	profLogAdd(0, "cap iqset -e");
	#if (FIELD_QV_ENABLE	== 1)
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 1);
	#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	#endif
	#if CAP_AUTO_ENABLE
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, 1);
	#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, -1);
	#endif
	gStillCB.isReady2View = 0;

	if (pcalib->CIPASign ==1)
	{
		pcalib->CIPACapCnt++;
		apptbCIPA_Log(pcalib->CIPACapCnt);
	}


	appShowCurExpAgc();
	#if ICAT_WIFI
	#if 0
	if (appPtpStatGet())
		appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_HOST_CAPTURING, 0, 0);
	#endif
	#endif
	profLogAdd(0, "cap sp5kModeSet()");
	appStillUtil_CaptureSet();
}
/*-------------------------------------------------------------------------
 *  Function Name : appStillCapStateOnMsg
 *  Description :   sk5p still capture mode will disable all display layers
 					than turn on the image layer in qv and other layer after
 					qv. If you want to show or keep something in non-image
 					layers, you can active disp_osd after into sill capture
 					mode.
 *------------------------------------------------------------------------*/

void
appStillCapStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 next_state,next_param;
	BOOL is_yuv_cb_on=FALSE;
	#ifdef HW_DIST
	UINT8 dist=0;
	#endif
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
	case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
		gStillCB.isReady2View = 1;
		gStillCB.remainImg--;
		break;
	case APP_UI_MSG_LENS_MSHUT_OPEN:
		DBG_PRINT("Cap msg: LENS_MSHUT_OPEN\n");		
		appSnapFlashDismiss();
		appStill_SetFlash(gStillCB.flashMode);
		break;
	case SP5K_MSG_STILL_QV_TIMEOUT:
		DBG_PRINT("Cap msg: QV_TIMEOUT\n");
		break;
	case SP5K_MSG_STILL_VIEW_SETTING_NOTIFY:
		DBG_PRINT("Cap msg: VIEW_SETTING_NOTIFY\n");
     	appAePreviewSet();//expidx ,agcidx setting
		appAePreviewRestore();
		appStill_SetSp5kStillSize(UI_PIC_SIZE_FULL);//size set
		appStillViewIqSet();//IQ resource load

		#ifdef HW_DIST
		appAeDistStatusGet(&dist);
		if (dist ==TRUE)
		{
			appStill_SetStabilizer(puiPara->Stabilizer);	
		}
		#endif
	
		break;
	case SP5K_MSG_MODE_READY:
		DBG_PRINT("SP5K_MSG_MODE_READY: remain=%d param=%x\n",gStillCB.remainImg ,param);
		if (param==SP5K_MODE_STILL_PREVIEW) 
		{
		#if CAP_VID_PV
			if(CapVidPreview)
			{		
				appStateChange(APP_STATE_VIDEO_PREVIEW, STATE_PARAM_NORMAL_INIT);	 
			}
			else
		#endif
			{
				appStillFlow_GetNextState(&next_state, &next_param);
				appStateChange(next_state, next_param);
			}
		}
		if (param==SP5K_MODE_STILL_SNAP) 
		{
			appViewOsd_AllClean();
			if(puiPara->PhotoFrame != UI_PHOTOFRAME_OFF) 
			{
				appViewDrawPhotoFrame(UI_PHOTOFRAME_OFF);//off photoframe when capture
				is_yuv_cb_on = TRUE;				
			}
			if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) 
			{
				is_yuv_cb_on = TRUE;				
				sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
				appErrMsgWithSound(ERR_MSG_BUSY_WAIT, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);
			}
			if (((IS_SCENE_MODE)&&(IS_BLINK_DETECTION_MODE))||puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
			{
				is_yuv_cb_on = TRUE;
				appStill_CfgCaptureConfirmStore(TRUE);	/*junqiong.hu@20121126*/
			}
			appStillYuvCbSet(is_yuv_cb_on);			
			sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);			
		}
		break;
#if !CAM_TYPE_CVR
	case SP5K_MSG_FACE_TRACK_READY:
		{	
			if ((((IS_SCENE_MODE)&&(IS_BLINK_DETECTION_MODE))|| puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
				&&(BlinkYUVCallBackSet==TRUE))
			{
				sp5kFaceTrackFaceRes_t facelist[12];
				UINT32 nface,i,qvw=320, qvh=240;
				if (appFdResultGet(param, &nface, facelist)== SUCCESS)
				{
					if (nface > 0)
					{
						for (i = 0; i < nface; i++) 
						{
							UINT32 startx, starty, size, attr;
							startx = facelist[i].x * qvw / 1024;
							starty = facelist[i].y * qvh / 1024;
							size = facelist[i].size * qvw / 1024;
							attr = (facelist[i].attr >> 24) & 0xff;
							/*Note:0x50 -> FACETRACK_TYPE_MAINFACE|FACETRACK_TYPE_BLINK*/
							if ((attr ==0x50)||(attr ==0x40))
							{
								BlinkGet =TRUE;
								
							}
							#if 0
     							printf("attr = %x,idx:%d\n", attr,i);
							printf("nStartX = %x,j:%d\n", startx,i);
		 					printf("nStartY = %x,j:%d\n", starty,i);
		 					printf("nSize = %x,j:%d\n", size,i);
							#endif
						}
					}
				}
				else
				{


				}
				appFdBlinkDetectCheckEnd();
				appFdBlinkDetectSet(FALSE);
				sp5kFaceTrackYuvBufEnd(0);
				appFdBlinkDetectSemPut();
			}
		}
		break;
	case APP_UI_MSG_VIEW_BLINKDETECT_CONFIRM:
		if (((IS_SCENE_MODE)&&(IS_BLINK_DETECTION_MODE))||puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)
		{
			UINT32 storeflag=0;
			storeflag = BlinkSaveSelect& 0xf;
			appStill_SetQV(0);
			appStill_CfgCaptureConfirmStore(0);
			appStill_SetCaptureConfirmStore(storeflag);
		}
		break;
#endif
	case APP_UI_MSG_VIEW_YUV_CALLBACK:
		if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) 
		{
			gStillCB.needDarkFrameSub = FALSE;
			gStillCB.isXDenoiseOn = FALSE;
			//appStillYuvCbSet(FALSE);
			appErrMsgWithSound(ERR_MSG_CLEAR, 0, 0);
			#if (FIELD_QV_ENABLE	== 1)
				if(puiPara->StampMode == UI_STAMP_OFF 
					&& puiPara->PhotoFrame == UI_PHOTOFRAME_OFF) 
				{
					sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 1);
				}
			#endif
		}
		break;
	default:
		break;
	}
}

void
appStillCapStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 temp=0;
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) 
	{
	case APP_KEY_PRESS_S1:
		if(!gStillCB.needDarkFrameSub && !gStillCB.isXDenoiseOn) 
		{
			gStillCB.s1AaaTicket = 1;
			temp=0;
			if(puiPara->AutoReviewTime == QV_TIME_OFF)  /*add for mantis bug 0047309*/
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, &temp);
			}
		}
		break;
	case APP_KEY_PRESS_S2:
		gStillCB.s2SnapTicket = gStillCB.s1AaaTicket? 1 : 0;
		break;
/*junqiong.hu@20121205 add for mantis 0046454 */
	case APP_KEY_RELEASE_S1:
		gStillCB.s1AaaTicket = gStillCB.s2SnapTicket? 1 : 0;
		break;
	default:
		break;
	}
}

void
appStillCapState(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 preState=0;

	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("StillCapState\n");
			profLogAdd(3,"Still Capture State");
			preState =appPreviousStateGet();
			if (preState ==APP_STATE_STILL_BLINK_DETECT)
			{

			}
			else
			{
				appStillCapStateInit();
			}
			break;
		case APP_STATE_MSG_CLOSE:
			appStillWaitJobFinish();
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appStillCapStateOnKey(msg,param);
			}
			else
			{
				
				appStillCapStateOnMsg(msg,param);
			}
			break;
	}
}


