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
#include "sp5k_cdsp_api.h"

#include "sp5k_rsvblk_api.h"
#include "sp5k_media_api.h"

#include "app_still.h"
#include "app_view_param.h"
#include "app_sound.h"
#include "app_strobe.h"
#include "app_flash_api.h"
#include "app_aaa.h"
#include "app_osd_api.h"
#include "app_view_osd.h"
#include "sp5k_media_api.h"
#include "app_ui_para.h" 
#if SPCA6330
#include "app_ptp.h"
#include "app_wifi_utility.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	AE_FASTAE_OK_FLASH_OFF,
	AE_FASTAE_OK_FLASH_ON,	
	AE_FASTAE_RUNNING=0xff	
};

enum {
	AAA_STAGE_AE,
	AAA_STAGE_AF,	
	AAA_STAGE_READY
};

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 appStillAaa_stage = AAA_STAGE_AE;
UINT32 appStillAaa_force_capture = 0; // enable this will capture without s2 press.
UINT32 appStillAaa_af_on = 0; 
UINT32 appStillAaa_ae_on = 0; 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if CAP_VID_PV
extern UINT32 CapVidPreview;
#endif
extern UINT32 SnapSetflag;  
extern UINT8 IsHDR;
extern UINT32 sp5kAeModeGet(UINT32 *pmode);
SINT32 pv_expidx_to_tvidx(SINT32 expidx);
SINT32 pv_agcidx_to_svidx(SINT32 agcidx);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void 
_doSnapTwoPressAction(
	UINT32 preState
)
{
	UINT32 next_state,next_param;	
	uiPara_t* puiPara = appUiParaGet();

	DBG_PRINT("s2SnapTicket=%d, forceCapture=%d\n", gStillCB.s2SnapTicket, appStillAaa_force_capture);
		
	/* flow control variables */
	gStillCB.s2SnapTicket=0; // We will consume this ticket when we execute snap job.
	gStillCB.remainImg = (puiPara->AEB || IsHDR)? 3:(puiPara->DriverMode == UI_DRIVE_DOUBLE)? 2:1;
	DBG_PRINT("%s, gStillCB.remainImg = %d\n", __FUNCTION__, gStillCB.remainImg);
	//profLogAdd(0, "aaa clr face -s");
	appStillUtil_CaptureWithFaceCheck(); 
	appStillOsd_UpdateFaceOsd(TRUE);
	appStillOsd_UpdateObjectOsd(TRUE);		/* sc.liu */
	appViewOsd_AllClean();   /*junqiong.hu@20121203*/
	//profLogAdd(0, "aaa clr face -e");
	switch (preState) 
	{		
		case APP_STATE_VIDEO_PREVIEW:	
			#if CAP_VID_PV
			if(CapVidPreview)
			{
				appStateChange(APP_STATE_STILL_CAPTURE, STATE_PARAM_NORMAL_INIT);	
			}
			else
		#endif	
			{
				appStateChange(APP_STATE_VIDEO_REC,STATE_PARAM_NORMAL_INIT);
			}
			break;
		default:
			appStillFlow_GetNextState(&next_state, &next_param);
			profLogAdd(0, "aaa goto cap");
			appStateChange(next_state, next_param);
			break;
	}	
}

void 
_doSnapOneReleaseAction(
	UINT32 CurMode
)
{
	if(CurMode == VIDEO_MODE) 
	{
		/* DIST + RSC OFF */ /* add by sc.liu */
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL,MEDIA_REC_RSC_DIS_OFF);
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
	}
	else 
	{
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
	}
}

void 
_processAfterAf(
	UINT32 af_result
)
{
		switch(af_result) 
		{
		default:	
		case AF_BUSY:
			// the af is running.
		break;	
		case AF_SUCCESS:
		case AF_FAIL:	
			// OSD and Sound
			if(af_result == AF_SUCCESS) 
			{
				gStillCB.focusStatus = FOCUS_STATUS_OK;
			}
			else 
			{
				gStillCB.focusStatus = FOCUS_STATUS_NG;
				LED_FLICKER_AF_ERROR
			}
			
			#if ICAT_WIFI && SPCA6330
			#if 0
			if(appPtpStatGet()){
				if(gStillCB.focusStatus == FOCUS_STATUS_OK)
					appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_AF_OK, 0/*X*/,0/*Y*/);/* Need to input AF (X,Y) here !!*/
				else
					appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_AF_NG, 0, 0);
			}
			#endif
			#endif
			
			if(!appStillAaa_force_capture) 
			{

				#if 0
				appViewOsdShow(0,0, OSD_VIEW_S_ONE_HOLD);
				if(gStillCB.focusStatus == FOCUS_STATUS_OK) 
				{
					appPreloadSoundPlay(SOUND_ID_FOCUSOK);
				}			
				#endif
			}
			AfStatusSet(AF_FINI);
			_processAfterAf(AF_FINI);
			break;
		case AF_FINI :	
			appAfValueReset();
			profLogAdd(0, "aaa af finish");
			// the af finish
			// TODO: the af should return ok or ng here.
			// In 10-sec self case, we will don't re-draw the 
			// osd and directly capture after af.
			if(appStillAaa_force_capture) 
			{
				_doSnapTwoPressAction(appPreviousStateGet());				
			}
			else
			{				
				if(gStillCB.focusStatus == FOCUS_STATUS_VIEW) 
				{
					// We miss the AF_SUCCESS/AF_FAIL af_result.
					_processAfterAf(AF_SUCCESS);
				}
				// S1 & S2 job
				if(gStillCB.s2SnapTicket) 
				{
					_doSnapTwoPressAction(appPreviousStateGet());
				}
			}			
			appStillAaa_stage = AAA_STAGE_READY;
			if(SnapSetflag == 1)
			{
				sp5kHostMsgSend(APP_KEY_PRESS_S2 , 0);  /*add for it can contiue to record when recording more than 4G*/
				SnapSetflag = 0;
			}
			break;
		}	
}

void
_processAfterFastAe(
	UINT32 ae_result
)
{	
	switch(ae_result) {
	case AE_FASTAE_RUNNING:
		break;
	case AE_FASTAE_OK_FLASH_ON:
	case AE_FASTAE_OK_FLASH_OFF:
		{
			appAeSet_t* pAe;
			pAe = appAeSetGet();
			printf("appAeSetGet():\n==> pasm=%d, scene=%d, meter=%d, ev=%d, iris=%d, shutter=%d, iso=%d\n",
				pAe->pasm, pAe->scene,pAe->meter,pAe->evbias,pAe->iris,pAe->shutter,pAe->iso);
		}		
		if(appStillAaa_ae_on) {			
			gStillCB.needFlash = (gStillCB.flashMode == UI_FLASH_ON)? 1 : 
				(gStillCB.flashMode == UI_FLASH_OFF)? 0: ae_result;
		}
		else {
			// preflash will use ae message. When it is off, we should stop flash.
			gStillCB.needFlash = 0;
		}

		if(gStillCB.needFlash) 
		{
			LED_FLICKER_STROBE_CHARGE
		}				
		printf("appAeCapParaGet():\n==>");	

		appAeCapParaGet(&gStillCB.ae.iris, &gStillCB.ae.shutter, &gStillCB.ae.iso);
		
		// check the value
		if(gStillCB.ae.iris <= APERTURE_AUTO || gStillCB.ae.iris >= APERTURE_MAX) {
			gStillCB.ae.iris = APERTURE_2_8;
		}
		if(gStillCB.ae.shutter <= SHUTTER_AUTO || gStillCB.ae.shutter >= SHUTTER_MAX) {
			gStillCB.ae.shutter = SHUTTER_1_15S;
		}
		gStillCB.ae.iso = (gStillCB.ae.iso <= UI_ISO_50)?  UI_ISO_50:
			(gStillCB.ae.iso >= UI_ISO_MAX)?  UI_ISO_MAX-1: gStillCB.ae.iso;
		
		if(gStillCB.ae.shutter <= SHUTTER_1S || appIqDarkFrameEnableGet()) 
		{
			#if KIT_WITH_MSHUT
			DBG_PRINT("%s: do dark sub\n", __FUNCTION__);
			gStillCB.needDarkFrameSub = 1;
			sp5kStillCapAdvCfgSet( SP5K_CAPTURE_DARK_FRAME, 1 );
			sp5kImgDarkFrameSubCfgSet(0xffffffff);
			appStillRawCbSet(1);
			#else

			
			#endif
		} 
		else 
		{
			DBG_PRINT("%s: no dark sub\n", __FUNCTION__);
			gStillCB.needDarkFrameSub = 0;
			sp5kStillCapAdvCfgSet( SP5K_CAPTURE_DARK_FRAME, 0);
			appStillRawCbSet(0);
		}			
		appStillAaa_stage = AAA_STAGE_AF;	
		profLogAdd(0, "aaa ae finish");
	default:
		break;
	}	
}

void 
appStillAaaInit(
	UINT32 param
)
{
	/*UINT32 aemode;*/
	uiPara_t* puiPara = appUiParaGet();
	
	profLogAdd(0, "aaa ini");
	appBtnDisable(BTN_PB|BTN_MODE);
	appPowerSavingEnableSet(FALSE);
	gStillCB.s1AaaTicket=0; // We will consume this ticket when we execute Aaa job.
	appStillAaa_force_capture = param;
	appStillAaa_stage = AAA_STAGE_AE;
	
	#if KIT_WITH_LENS && KIT_WITH_FOCUS
	appStillAaa_af_on = (puiPara->af == UI_FOCUS_INFINITY)? 0:1;
	#else
	puiPara = puiPara; /*clean warning , fan.xiao@121108*/
	appStillAaa_af_on = 0;
	#endif
	sp5kAeModeGet(&appStillAaa_ae_on);	 
	if(appStillAaa_ae_on == SP5K_AE_MODE_OFF) 
	{
		sp5kHostMsgSend(SP5K_MSG_AE_READY, 0); // aeoff, self sending
	}	

    #if PTP_CAPTURE
    gStillCB.s2SnapTicket=1;
    #endif

	profLogAdd(0, "aaa af ini -s");
	if(appStillAaa_af_on) 
	{
		appAfInit();//this function would return SUCCESS.
	}	
	profLogAdd(0, "aaa af ini -e");
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillAaaStateOnMsg
 *  Description :   Here we will apply AE then AF. So in this state, we have
 *			        3 stage, AE, AF and Ready. Capture only allowed in Ready
 *					stage. If we don't support lens, the af will be bypssed.
 *------------------------------------------------------------------------*/
void 
appStillAaaStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{		
	switch (msg) 
	{
	case SP5K_MSG_AE_READY:	
		/* 1) do fast AE, next is AF */
		if(appStillAaa_stage != AAA_STAGE_AE) 
		{
			break;
		}			

		if(appStillAaa_ae_on) 
		{	
		    UINT32 currMode;
			_processAfterFastAe(appAeFastAdjust(param));
			sp5kModeGet(&currMode);
			if(currMode == SP5K_MODE_VIDEO_PREVIEW)
				appStillAaa_stage = AAA_STAGE_AF ;
		}	
		else 
		{   
			_processAfterFastAe(AE_FASTAE_OK_FLASH_OFF);
		}	

		/* after ae, check if we need to do af */
		if(appStillAaa_stage == AAA_STAGE_AF) 
		{
			if(appStillAaa_af_on == 1) 
			{
				aaa_AutoFocus();
			}
			else 
			{
				_processAfterAf(AF_SUCCESS);
				_processAfterAf(AF_FINI);
			}
		}	
		break;		
	case SP5K_MSG_AWB_READY:	
		/*appAwbPreviewAdjust(param);*/
		break;		
	case SP5K_MSG_AF_READY:
		/* 2) do AF, next is Ready */
		if(appStillAaa_stage != AAA_STAGE_AF) 
		{
			break;
		}		
		appFocusScan(param);
		_processAfterAf(AfStatusGet());			
		break;
	case APP_STATE_MSG_ABORT:
		gStillCB.s1AaaTicket=0;
		gStillCB.s2SnapTicket=0;
		_doSnapOneReleaseAction(gStillCB.cap.CurViewMode);
		break;
	default:
		//DBG_PRINT("__OnMsg: Ignored Message\n");
		break;
	}
}

void 
appStillAaaStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) 
	{	
		case APP_KEY_RELEASE_S1:
			/*if(appStillAaa_stage == AAA_STAGE_READY) *//*mark for mantis #43638 */
			if(!gStillCB.s2SnapTicket)  /*junqiong.hu@20121205  add for capture would failed when S1 released at AAA state*/
			{
				appAePreviewRestore();
				_doSnapOneReleaseAction(gStillCB.cap.CurViewMode);	
			}			
			break;
		case APP_KEY_PRESS_S2:
			profLogAdd(0, "--> s2 press");
			printf("s2 press\n");
			gStillCB.s2SnapTicket=1;
			if(appStillAaa_stage == AAA_STAGE_READY) 
			{
				_doSnapTwoPressAction(appPreviousStateGet());
			}		
			break;
		default:		
			break;
	}
}

void 
appStillAaaState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			profLogAdd(3,"Still AAA State");
			printf("appstillAAAState S\n");
			appStillAaaInit(FLOW_EXTRCT_ST_PAR(param));
			break;
		case APP_STATE_MSG_CLOSE:		
			appStateCloseDone();
			break;	
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appStillAaaStateOnKey(msg,param);
			}
			else 
			{
				appStillAaaStateOnMsg(msg,param);
			}
			break;
	}
}
