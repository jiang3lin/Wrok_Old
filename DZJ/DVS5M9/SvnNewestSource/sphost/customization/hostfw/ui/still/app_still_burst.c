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
#include "sp5k_disk_api.h"

#include "app_still.h"
#include "app_view_param.h"
#include "app_view_osd.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_error_msg.h"
#include "sp5k_cdsp_api.h"
#include "app_ui_para.h" 
#include "app_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define BURST_STATUS_NORMAL 0
#define BURST_STATUS_ABOURT 1
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct burst_s 
{
	UINT32 aeb;
	UINT32 status;
	UINT32 imgId; 		/* message IMAGE_ID accumulate number */
}burst_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
burst_t gBtCB; 	/* burst control block */
//extern UINT8 IsHDR;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appStillAFBktCbSet(BOOL isEnable);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void 
appStillBurstStateInit(void)
{	
	UINT32 cfg;
	uiPara_t* puiPara = appUiParaGet();
	
	appStill_SetSp5kStillSize(puiPara->ImageSize);
	appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	appStillSnapIqSet();
	appAeCaptureSet(0);
	appAeAebSet(puiPara->AEB);
	/* we miss the s2 release message */
	if(gStillCB.s2press == 0) 
	{
		sp5kHostMsgSend(APP_KEY_RELEASE_S2, 0);
	}
	gStillCB.isReady2View = 0;
	gBtCB.status = BURST_STATUS_NORMAL;
	gBtCB.imgId = 0;
	gBtCB.aeb = puiPara->AEB;
	#if (FIELD_QV_ENABLE	== 1)
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 1);	
	#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);	
	#endif
	#if CAP_AUTO_ENABLE
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, -1);
	#else
	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_COMPRESSION_DELAY, -1);
	#endif
	if(puiPara->AEB){
		cfg=3;
	}
	else 
	{
#if CAM_TYPE_CVR
		switch(puiPara->PhotoBurst){
			case 1:
				cfg=10;
			break;
			case 2:
				cfg=20;
			break;
			case 3:
				cfg=30;
			break;

		}
#else		
		/*
		sp5kStillCapAdvCfgSet( SP5K_CAPTURE_REENCODE_BYPASS , 1);
		 burst with 10 */ 
	   	cfg=10;
#endif
	}	
	sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_LENGTH, &cfg); 
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
	if(gBtCB.aeb == 1){
		sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_CFG_BURST_FAST_SENSOR_TRIG,0); 
	}
	else{
		sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG,SNAP_SYS_CFG_BURST_FAST_SENSOR_TRIG,1); 
	}
	cfg = SP5K_CAPTURE_BURST_VLC; 
	sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, &cfg); 
	#if 0
	sp5kIqBlockEnable(SP5K_IQ_BLOCK_LDC,0);
	sp5kIqBlockEnable(SP5K_IQ_BLOCK_AHD,0);
	#endif
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, SNAP_SYS_CFG_FAST_BK_PREVIEW, 0); 
	sp5kStillCapYUVCallbackSet(NULL); 
	sp5kStillCapRawCallbackSet(NULL); 
	profLogAdd(0, "burst sp5kModeSet()");
	appStillUtil_CaptureSet();
	DBG_PRINT("start %s snap\n", gBtCB.aeb? "AEB":"Burst");
}

void 
appStillBurstStateClose(void)
{
	printf("\033[32mappStillBurstStateClose!!\033[0m\n");
	sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, 1,SNAP_MAX_COMPRESSION_RATIO);/**/
	appStillViewIqSet();
	appAePreviewSet();
	appAePreviewRestore();
	sp5kStillCapAbort();		
	appStateCloseDone();
}


void 
appStillBurstStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{	
	UINT32 next_state,next_param;	
	BOOL is_yuv_cb_on=FALSE;
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) {
	case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
	
		#if 1
		if(puiPara->DriverMode == UI_DRIVE_BURST)
		{
			break;
		}
		#endif
		
		profLogAdd(0, "burst IMAGE_ID");
		/*printf("\033[31mIMAGE ID(%d)\033[0m\n",gBtCB.imgId);*/
		gBtCB.imgId++;
		if (gBtCB.status == BURST_STATUS_ABOURT) {
			gStillCB.isReady2View = 1;
			/*sp5kStillCapAbort();*/
			DBG_PRINT("Cap Burst msg: abort\n");
		}
		if(gBtCB.aeb == 1 && gBtCB.imgId == 3) {
			gStillCB.isReady2View = 1;
			/*sp5kStillCapAbort();*/
			gBtCB.status = BURST_STATUS_ABOURT;
			printf("Cap Burst msg: AEB mode finish\n");
		}
		if(IsBracketMode() && gBtCB.imgId == (UINT32)(appBracketSnapNumGet() + 1)) {
			gStillCB.isReady2View = 1;
			/*sp5kStillCapAbort();*/
			gBtCB.status = BURST_STATUS_ABOURT;
			DBG_PRINT("Cap Burst msg: Bracket mode finish\n");
		}
		// for AEB, ae relative data
		appAeCaptureSet(gBtCB.imgId);
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
		if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) {			
			appErrMsgWithSound(ERR_MSG_BUSY_WAIT, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);	
		}	
		
	case SP5K_MSG_STILL_QV_TIMEOUT:
		DBG_PRINT("Cap Burst msg: QV_TIMEOUT\n");
		break;
	case SP5K_MSG_STILL_VIEW_SETTING_NOTIFY:	
		DBG_PRINT("Cap Burst msg: VIEW_SETTING_NOTIFY\n");
		if(gBtCB.status == BURST_STATUS_ABOURT) {
			printf("\033[32mSP5K_MSG_STILL_VIEW_SETTING_NOTIFY:ABOURT\033[0m\n");
			/*appStill_SetSp5kStillSize(UI_PIC_SIZE_FULL);*/	
			sp5kSystemCfgSet(SP5K_SNAP_SYS_CFG, 1,SNAP_MAX_COMPRESSION_RATIO);/**/
			appStillViewIqSet();
			appAePreviewSet();
			appAePreviewRestore();
		}	
		break;
	case SP5K_MSG_MODE_READY:
		printf("~~~~~~Cap Burst msg: MODE_READY\n");
		if (param == SP5K_MODE_STILL_PREVIEW) {
			if( IS_BLINK_DETECTION_MODE || puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT){
				is_yuv_cb_on = TRUE;
			}
			printf("Cap Burst msg: MODE_READY-STILL_PREVIEW = %d \n",is_yuv_cb_on);
			appStillYuvCbSet(is_yuv_cb_on);
			appStillFlow_GetNextState(&next_state, &next_param);
			appStateChange(next_state, next_param);
		}
		if (param == SP5K_MODE_STILL_SNAP) {
			appViewOsd_AllClean();				
			if(puiPara->PhotoFrame!= UI_PHOTOFRAME_OFF) {
				is_yuv_cb_on = TRUE;
				appViewDrawPhotoFrame(UI_PHOTOFRAME_OFF);
			}
			if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) {
				is_yuv_cb_on = TRUE;
				sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);				
			}
			if(IsBracketMode()) {
				appStillAFBktCbSet(TRUE);
			}
			if( IS_BLINK_DETECTION_MODE){ /*  HDR is not processed by YUV callback NOW fan.xiao*/
				is_yuv_cb_on = TRUE;
			}
			printf("Cap Burst msg: MODE_READY-STILL_SNAP = %d \n",is_yuv_cb_on);
			appStillYuvCbSet(is_yuv_cb_on);
			sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);
		}
		break;	
	case APP_UI_MSG_VIEW_YUV_CALLBACK:
		if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) {
			appErrMsgWithSound(ERR_MSG_CLEAR, 0, 0);
			#if (FIELD_QV_ENABLE	== 1)
			if(puiPara->StampMode== UI_STAMP_OFF && puiPara->PhotoFrame == UI_PHOTOFRAME_OFF) {
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
appStillBurstStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) 
	{
		case APP_KEY_RELEASE_S2:
			if( gBtCB.aeb == 0 && (!IsBracketMode())) 
			{
				gBtCB.status = BURST_STATUS_ABOURT;
			}
			break; 
		default:
			break;
	}
}

void 
appStillBurstState(
	UINT32 msg,
	UINT32 param
)
{	
	switch (msg) 
	{
		case APP_STATE_MSG_INIT:	
			RPrintf("appStillBurstState\n");
			appStillBurstStateInit();		
			break;
		case APP_STATE_MSG_CLOSE:
			/*sp5kStillCapAbort();		
			appStateCloseDone();*/
			appStillBurstStateClose();
			break;	
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appStillBurstStateOnKey(msg,param);
			}
			else 
			{
				appStillBurstStateOnMsg(msg,param);
			}
			break;
	}	
}


