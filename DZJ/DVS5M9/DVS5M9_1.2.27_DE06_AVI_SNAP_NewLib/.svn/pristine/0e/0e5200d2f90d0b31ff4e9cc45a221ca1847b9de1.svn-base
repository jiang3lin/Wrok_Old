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

#include "sp5k_capture_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_view_param.h"
#include "app_aaa.h"
#include "app_still.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_pal.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_icon_def.h"
#include "app_menu_tab.h"
#include "app_osd.h"
#include "app_view_osd.h"
#include "app_util.h"
#include "app_ui_para.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define CWB_STORE_FILE_NAME "B:\\UDF\\cwb.bin"

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/ 
sp5kWbGain_t cwb = {0};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32
appStillCwbGet(
	sp5kWbGain_t* pWb
)
{
	if(cwb.rgain ==0 && cwb.grgain ==0 && cwb.bgain ==0 && cwb.gbgain ==0) {		
		if(ReadFile(CWB_STORE_FILE_NAME, (UINT8*)&cwb, sizeof(sp5kWbGain_t))!=SUCCESS) {
			return FAIL;
		}	
		else
		{
			pWb->rgain = cwb.rgain;
			pWb->grgain = cwb.grgain;
			pWb->bgain = cwb.bgain;
			pWb->gbgain = cwb.gbgain;
			return SUCCESS;
		}
	}	
	else {			
		//To Fix: compatibility
		//5110's base is 0x400 and 5100's is 0x40
		//In cwb state, we use 0x400 as base, but in real setting
		//we use 0x40. So we do translation here.
		pWb->rgain = cwb.rgain;
		pWb->grgain = cwb.grgain;
		pWb->bgain = cwb.bgain;
		pWb->gbgain = cwb.gbgain;
		return SUCCESS;
	}	
}

void
DoCwbCapture(void)
{
	//UINT32 cfg;
	//appAeSet_t * pAeInfo;	
	//sp5kWbGain_t cwb;
	uiPara_t* puiPara = appUiParaGet();
	
	appStill_SetFaceTracking(UI_FACETRACK_OFF);
	appStill_SetPhotoFrame(UI_PHOTOFRAME_OFF);
	
	#if APP_CWB_CAPTURE
	/* We will do the customer white balance in still mode. */
	//if (gStillCB.cap.CurViewMode == VIDEO_MODE) {		
	//	sp5kModeSet(APP_STATE_STILL_PREVIEW);
	//}	
	/* setup the parameters */	
	//cfg = 0;
	//sp5kStillCapCfgSet(SP5K_CAPTURE_QV_LENGTH, &cfg);
	//sp5kStillCapAdvCfgSet(SP5K_CAPTURE_FIELD_QV, 0);
	//sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, NULL);
	//sp5kStillCapAdvCfgSet(SP5K_CAPTURE_VIEW_DISP_CTRL, SP5K_CAPTURE_VIEW_DISP_AUTO);
	//sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CONFIRM_STORE, 1);	
	//appStill_SetQuality(puiPara->ImageQuality);
	//appStill_SetWB(puiPara->WBMode);
	//appStill_SetDateStamp(0,0);
	//appStill_SetDZoom();
	//appStill_SetMetering(UI_METER_SPOT);	
	//appStill_SetExpsure(UI_EXP_BIAS_ZERO);
	//appStill_SetISO(UI_ISO_AUTO);
	//appStill_SetDriveMode(UI_DRIVE_OFF);
	//appStill_SetAEB(UI_AEB_OFF);
	//appStill_SetSharpness(UI_SHARP_NORMAL);
	//appStill_SetHighlight(UI_HIGHLIGHT_OFF);
	//appIqEffectSet(UI_EFFECT_NORMAL);
	//appStill_SetStabilizer(UI_STABILIZER_OFF);	
	//appStill_SetFlash(UI_FLASH_OFF);	
	//if(gStillCB.cap.CurViewMode == VIDEO_MODE) {		
	//	appStill_SetResolution(pViewParam->vidsize==UI_VID_SIZE_3? 
	//		UI_PIC_SIZE_16_9 : UI_PIC_SIZE_FULL);
	//}	

	/* set ae */
	//appStill_SetShutterSpeed(SHUTTER_AUTO);
 	//appStill_SetApertureValue(APERTURE_AUTO);
	//pAeInfo = appAeSetGet();
	//pAeInfo->pasm = PASM_PROGRAM;	
	
	/* set wb call back */
	//appStillRawCbSet(RAW_CB_SHUTTER_NORAML);
	//appStillYuvCbSet(FALSE);
	
	/* start the capture */
	//appStillUtil_CaptureSet();	
	#else
	/* black the screen */
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);	
	/* do the cwb */
	appViewWbCb(&cwb);
	#endif
	
	if(WriteFile(CWB_STORE_FILE_NAME, (UINT8*)&cwb, sizeof(sp5kWbGain_t)) != SUCCESS) {
		cwb.rgain =0x400; 
		cwb.grgain =0x400;
		cwb.bgain =0x400;
		cwb.gbgain =0x400;
	}
	
	/* back to the still/video view */			
	puiPara->WBMode = UI_WB_CUSTOM;		
	if(IS_VIDEO_MODE) {
		appStateChange(APP_STATE_VIDEO_PREVIEW, APP_STATE_MSG_INIT);
	}	
	else {
		appStateChange(APP_STATE_STILL_PREVIEW, APP_STATE_MSG_INIT);
	}	
}

void 
appStillCwbStateInit(void)
{
	appOsdLib_OsdLayerClearDraw();

	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 139, 90, 82, 60, 0, 0);
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 141, 92, 78, 56, 0, 0);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,78, 220, 88, 33,0,0);

	appOsd_FontIconFontSet(ID_FONT_ICON);
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
	appOsdLib_OsdLayIconDraw(40, 205, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);
	appOsdLib_OsdLayIconDraw(200, 205, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_MENU);
	
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(78, 206,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_SET);		
	appOsdLib_TextIDDraw(238, 206,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_BACK);
}

void 
appStillCwbStateClose(void)
{
	appStillSettingInit(VIEW_SET_STATE_START);	
}

void 
appStillCwbStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case SP5K_MSG_STILL_CONFIRM_STORE:
		DBG_PRINT("__CONFIRM_STORE");
		sp5kStillCapStoreConfirm(0);
		break;	
	case SP5K_MSG_MODE_READY:
		DBG_PRINT("__MODE_READY");
		if(param==SP5K_MODE_STILL_PREVIEW) {
			appStillCwbStateClose();
			if(gStillCB.cap.CurViewMode == VIDEO_MODE) {
				appModeSet(SP5K_MODE_VIDEO_PREVIEW);
				appStateChange(APP_STATE_VIDEO_PREVIEW, STATE_INIT);
			}	
			else {				
				appStateChange(APP_STATE_STILL_PREVIEW, STATE_INIT);
			}	
		}
		break;
	default:
		break;
	}
}

void 
appStillCwbStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {		
	case APP_KEY_PRESS_MENU:
		DBG_PRINT("__KEY_PRESS_MENU");
		/* back to submenu */		
		appStateChange(appPreviousStateGet(), STATE_INIT);		
		break;		
	case APP_KEY_PRESS_MODE:
		DBG_PRINT("__KEY_PRESS_MODE");
		/* % 2008/01/10 : spectial control for VC7340 platform% */
		//sp5kHostMsgSend(APP_KEY_MODE4, 0);
		/* todo: transfer to playback */
		break;	
	case APP_KEY_PRESS_SET:	
		DBG_PRINT("__KEY_PRESS_SET");
		//appStateChange(appPreviousStateGet(), STATE_INIT);		
		DoCwbCapture();
		break;
	case APP_KEY_PRESS_S1:
		if(gStillCB.cap.CurViewMode == VIDEO_MODE){
			appStateChange(APP_STATE_VIDEO_PREVIEW, APP_STATE_MSG_INIT);
		}else{
			appStateChange(APP_STATE_STILL_PREVIEW, APP_STATE_MSG_INIT);
		}	
		break;	
	default:
		break;
	}
}

void 
appStillCwbState(
	UINT32 msg,
	UINT32 param
)
{
	//if ((msg&0xffffff00) != (SP5K_MSG_AE_READY&0xffffff00)) 
	//	DBG_PRINT("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appStillCwbState\n");
		appStillCwbStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appStillCwbStateClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appStillCwbStateOnKey(msg,param);
		}
		else{
			appStillCwbStateOnMsg(msg,param);
		}
		break;
	}
}

