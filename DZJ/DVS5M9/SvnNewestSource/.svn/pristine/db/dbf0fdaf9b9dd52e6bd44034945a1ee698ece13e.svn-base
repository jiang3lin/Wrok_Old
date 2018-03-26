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

/* for display osd */
#include "sp5k_gfx_api.h"
#include "app_pal.h"
#include "app_osd.h"
#include "app_icon_def.h"

#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"

#include "app_audio.h"
#include "app_still.h"
#include "app_view_osd.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_stringid.h"
#include "app_jpg_draw.h"
#include "app_battery.h"
#include "app_menu_tab.h"
#include "gpio_custom.h"

//#include "app_user_setting.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define VOICE_MEMO_BAR_X					VOICE_MEMO_PROG_FRAME_CENT_X - (VOICE_MEMO_PROG_FRAME_SIZE_X / 2) + 37
#define VOICE_MEMO_BAR_Y					VOICE_MEMO_PROG_FRAME_CENT_Y - (VOICE_MEMO_PROG_FRAME_SIZE_Y / 2) + 107
#define VOICE_MEMO_BAR_SIZE_X				165
#define VOICE_MEMO_BAR_SIZE_Y				19

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT16 LastProgBarLength;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern appAudioCB_t gAudioCB;
extern UINT16 AudioRemainTime;


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
appAudioOsdElsTimeShow(
	void
)
{
	unsigned char 	szTime[16];
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	UINT32 strX,strY;
	UINT32 strSizeX = FontEngNumSize_X,strSizeY = FontEngNumSize_Y;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		strSizeX = 24;
		strSizeY = 42;
	}
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}

	strX = pPageCap->frmW/2 - 8;
	strY = pPageCap->frmH/3 + 30;
	
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, strX-strSizeX*6/2, strY,strSizeX*6, strSizeY, 0, 0);

	memset(szTime, 0, 16);
	sprintf(szTime, "%02u:%02u:%02u", gAudioCB.recSec/3600, (gAudioCB.recSec%3600)/60, (gAudioCB.recSec%3600)%60);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	if(gAudioCB.recSec/3600>99)
	{
		appOsdLib_TextDraw(strX-40-strSizeX, strY, SP5K_GFX_ALIGN_TOP_LEFT, szTime);
	}
	else
	{
		appOsdLib_TextDraw(strX-40, strY, SP5K_GFX_ALIGN_TOP_LEFT, szTime);
	}

	sp5kFree(pPageCap);
}

void
appAudioOsdRecRemainingShow(
	UINT32 remainingSec
)
{
	appViewOsd_AudioTotalFreeTimeDraw(1,0,remainingSec);
}

void
appAudioOsdDiskShow(
	void
)
{
	UINT32 n;

	if (gAudioCB.isDiskError)
	{
		n = 0;
	} 
	else
	{
		n = appAudioDiskRemainGet();
	} 

	appAudioOsdRecRemainingShow(n);
}

/*-------------------------------------------------------------------------
 *  Function Name : appAudioOsd_BatteryDraw
 *  Description :
 *------------------------------------------------------------------------*/
void
appAudioOsd_BatteryDraw(
	UINT8 bShow,
	UINT8 idx
)
{
	if(!bShow){
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_BATTERY_X , APPVIEW_BATTERY_Y, APPVIEW_ICON_SIZE_W, APPVIEW_ICON_SIZE_H, 0, 0);
		return;
	}
	if (bShow) {
		appOsd_FontIconFontSet(ID_FONT_ICON);
		switch(idx)	{
		case APP_BATT_LEVEL_0:
			//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_LOW);
			appIconShow(ID_ICON_BATTERY_LOW,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
			break;
		case APP_BATT_LEVEL_1:
			//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_HALF);
			appIconShow(ID_ICON_BATTERY_HALF,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
			break;
		case APP_BATT_LEVEL_2:
			//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_FULL);
			appIconShow(ID_ICON_BATTERY_FULL,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
			break;
		case APP_BATT_LEVEL_3:
			//appOsdLib_OsdLayIconDraw(APPVIEW_BATTERY_X,APPVIEW_BATTERY_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BATTERY_FULL);
			printf("[WARN] APP_BATT_LEVEL_3 is not used. %s (%d)\n",__FUNCTION__,__LINE__);
			break;
		default:
			printf("[WARN] Illegal BATT Level (%d) used. %s (%d)\n",idx, __FUNCTION__,__LINE__);
			break;
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appAudioOsdProgressBarDraw
*------------------------------------------------------------------------*/
void
appAudioOsdProgressBarDraw(
		UINT8 bShow,
		UINT32 CurrentTime_msec,
		UINT32 TotalTime_msec
)
{
	UINT32 barX=VOICE_MEMO_BAR_X,barY=VOICE_MEMO_BAR_Y;
	UINT32 barW =VOICE_MEMO_BAR_SIZE_X, barH=VOICE_MEMO_BAR_SIZE_Y;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		barX = 193;
		barY = 224;
		barW = barW*3/2;
		barH = barH*6/5 + 1;
	}
	#endif

	if(!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, barX , barY, barW, barH, 0, 0);
		return;
	}

		printf("__________LastProgBarLength = %d___________\n",LastProgBarLength);
		if (CurrentTime_msec > TotalTime_msec)
			CurrentTime_msec = TotalTime_msec;

		if (CurrentTime_msec ==TotalTime_msec)
			CurrentTime_msec = TotalTime_msec;
		UINT32 CurrentProgBarLength = ((float)CurrentTime_msec / (float)TotalTime_msec) * (float)barW;

		appOsd_ColorDrawShapeColorSet(PAL_YELLOW, PAL_BLEND_100, PAL_YELLOW, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, barX + LastProgBarLength, barY, CurrentProgBarLength - LastProgBarLength, barH, 0, 0);
		LastProgBarLength = CurrentProgBarLength;

}

/*-------------------------------------------------------------------------
*  File Name : appAudioOsdStatusDraw
*------------------------------------------------------------------------*/
void appAudioOsdStatusDraw(UINT8 Audiostatus)
{
	UINT32 strX = APPVIEW_AUDIO_STRING_X,strY = APPVIEW_AUDIO_STRING_Y;
	UINT32 iconPosX = VOICE_MEMO_PROG_FRAME_CENT_X,iconPosY = VOICE_MEMO_PROG_FRAME_CENT_Y;
	UINT32 strW=APPVIEW_AUDIO_STRING_W,strH=FONT_H;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		strX = strX*2;
		strY = strY*3/2;
		iconPosX = 257; 
		iconPosY = 170;
		strW = 260;
		strH = 42;
	}
	#endif

	/*Els time update*/
	appOsd_FontIconFontSet(ID_FONT_ICON);
	appAudioOsdDiskShow();
	appAudioOsdElsTimeShow();

	appAudioOsd_BatteryDraw(TRUE, appBatteryLevelGet());
	//appOsdLib_OsdLayIconDraw(APPVIEW_MODE_X, APPVIEW_MODE_Y,
	//	SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_VOICE);
	appIconShow(ID_ICON_MODE_VOICE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	/*-- Center Background */
	appOsd_FontIconFontSet(ID_ICON_WARNMEDIASTANDBY);

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		sp5kGfxAttrSet(SP5K_GFX_IDX8_PIC_SCALE_FACTOR,1500,1200,0,0);
	}
	#endif

	appOsdLib_OsdLayIconDraw(iconPosX,
							iconPosY,
							SP5K_GFX_ALIGN_CENTER,0x0000);
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		sp5kGfxAttrSet(SP5K_GFX_IDX8_PIC_SCALE_FACTOR,1000,1000,0,0);
	}
	#endif
	
	if(0 == appAudioDiskRemainGet()){
		gAudioCB.err = VIEW_ERR_MEMORY_FULL;
		appViewOsd_WarmingMsgDraw(1, gAudioCB.err, TRUE);
		appBtnDisable(BTN_S1 | BTN_S2);
	}
	else{
		/*RECORD TIME*/
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, strX-(strW/2), strY, strW, strH, 0, 0);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		if(Audiostatus == AUDIO_REC_STATUS_STOP)
		{
			appOsdLib_TextIDDraw(strX,strY,SP5K_GFX_ALIGN_TOP,ID_STR_READY_TO_RECORD);
		}
		else if(Audiostatus== AUDIO_REC_STATUS_RECORDING)
		{
			appOsdLib_TextIDDraw(strX,strY,SP5K_GFX_ALIGN_TOP,ID_STR_RECORDING_MEMO___);
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appAudioOsdShow
*------------------------------------------------------------------------*/
void
appAudioOsdShow(
	UINT32 clr
)
{
	if (clr) 
	{
		appAudioOsdStatusDraw(gAudioCB.recStatus);
	}
	
	if (gAudioCB.recStatus == AUDIO_REC_STATUS_STOP)
	{
		appAudioOsdDiskShow();
	//	appViewOsd_WarmingMsgDraw(1, gAudioCB.err, TRUE);
	} 
	else
	{
		appAudioOsdStatusDraw(gAudioCB.recStatus);
		appAudioOsdProgressBarDraw(TRUE,gAudioCB.recSec,AudioRemainTime);
	}
}


void
appAudioOsdInit(
	void
)
{
	/*-- Background JPG */
	appJpgDraw_ImgLayer(RES_JPG_AUDIO_BG, 0,0,320,240);
}
