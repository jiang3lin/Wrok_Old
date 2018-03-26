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



#include "app_com_def.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "sp5k_gfx_api.h"
#include "app_pal.h"
#include "app_osdfont.h"
#include "app_iconfont1.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "app_stringid.h"
#include "app_errorosd.h"
#include "app_menu_frameosd.h"
#include "app_menu_tab.h"
#include "gpio_custom.h"
#include "api/sp5k_disp_api.h"

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


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appMenu_TKTypeWarningMsgFrameDraw
 *------------------------------------------------------------------------*/
void 
appOsd_TKWarningMsgFrameDraw(
	unsigned char bShow
)
{
	UINT32 warningX,warningY,warningW,warningH;
	UINT32 warningIconX,warningIconY;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		warningX = 205;
		warningY = 120;
		warningW = TK_WARNING_W;
		warningH = TK_WARNING_H;
		warningIconX = 280;
		warningIconY = 128;
	}else{
	#else
	{
	#endif
		warningX = TK_WARNING_X;
		warningY = TK_WARNING_Y;
		warningW = TK_WARNING_W;
		warningH = TK_WARNING_H;
		warningIconX = TK_WARNING_ICON_X;
		warningIconY = TK_WARNING_ICON_Y;
	}
	
	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,warningX, warningY,warningW,warningH,0,0);
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT,warningX+2,warningY+2,warningW-4,warningH-4,0,0);	
		return ;
	}


	appOsd_ColorBlendingSet(PAL_GRAY_1,PAL_BLEND_50);
	appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1,PAL_GRAY_1,PAL_BLEND_50);
	appOsdLib_TransparentLayerShapeDraw(APP_OSD_SHAPE_RECT,warningX+2,warningY+2,warningW-4,warningH-4,0,0);	

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, warningX, warningY,warningW, warningH, 0, 0);

	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,warningX,warningY,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNFRAME,0x0000);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,warningIconX,warningIconY,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNICON,0x0000);


}

/*-------------------------------------------------------------------------
 *  File Name : appMenu_TKTypeWarningMsgBigFrame2Draw
 *------------------------------------------------------------------------*/
void 
appOsd_TKWarningMsgBigFrame2Draw(
	unsigned char bShow
)
{

	if (!bShow)
	{	
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,180, 120,TK_WARNING_FRAME2_W,TK_WARNING_FRAME2_H,0,0);
		}else
		#endif
		{
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,TK_WARNING_FRAME2_X, TK_WARNING_FRAME2_Y,TK_WARNING_FRAME2_W,TK_WARNING_FRAME2_H,0,0);
		}
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT,TK_WARNING_FRAME2_X+2,TK_WARNING_FRAME2_Y+2,TK_WARNING_FRAME2_W-4,TK_WARNING_FRAME2_H-4,0,0);	
		return ;
	}


	appOsd_ColorBlendingSet(PAL_GRAY_1,PAL_BLEND_50);
	appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1,PAL_GRAY_1,PAL_BLEND_50);
	appOsdLib_TransparentLayerShapeDraw(APP_OSD_SHAPE_RECT,TK_WARNING_FRAME2_X+2,TK_WARNING_FRAME2_Y+2,TK_WARNING_FRAME2_W-4,TK_WARNING_FRAME2_H-4,0,0);	

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TK_WARNING_FRAME2_X, TK_WARNING_FRAME2_Y,TK_WARNING_FRAME2_W, TK_WARNING_FRAME2_H, 0, 0);

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,180,120,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNFRAME2,0x0000);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,280,140,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNICON,0x0000);
	}else
	#endif
	{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,TK_WARNING_FRAME2_X,TK_WARNING_FRAME2_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNFRAME2,0x0000);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,TK_WARNING_FRAME2_ICON_X,TK_WARNING_FRAME2_ICON_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_WARNICON,0x0000);
	}
}


void  appOsdMessageBoxShow(BOOL bShow, unsigned short usStrId)
{

#if 1
	UINT32 Warn_X,Warn_Y,FRAME_X,FRAME_W;//
	
 	if(appOsdLib_FontGetStringWidthbyId(usStrId)<190)
	{
    	Warn_X = TK_WARNING_X+(TK_WARNING_W/2);
     	Warn_Y = TK_WARNING_Y+(TK_WARNING_H-50);
		FRAME_X = TK_WARNING_X;
		FRAME_W = TK_WARNING_TEXT_CLEAR_W;
		appOsd_TKWarningMsgFrameDraw(bShow);////
	}
	else
	{
		Warn_X = TK_WARNING_FRAME2_X+(TK_WARNING_FRAME2_W/2);
		Warn_Y = TK_WARNING_FRAME2_Y+(TK_WARNING_FRAME2_H-50);
		FRAME_X = TK_WARNING_FRAME2_X;
		FRAME_W = TK_WARNING_FRAME2_TEXT_CLEAR_W;
		appOsd_TKWarningMsgBigFrame2Draw(bShow);////
	
	} 

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		Warn_X = 2*Warn_X - 20;
		Warn_Y = Warn_Y*3/2 - 5;
		FRAME_X = 210;
		FRAME_W = 180;
	}
	#endif
	if(usStrId == ID_STR_PRINTING___)
	{
		appOsdLib_OsdLayIconDraw(/*80*/105,206,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_BUTTON_MENU);
		appOsdLib_TextIDDraw(/*120*/145,/*230*/240,SP5K_GFX_ALIGN_BOTTOM_LEFT,ID_STR_CANCEL);
	}

	if(bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, FRAME_X+2, Warn_Y,FRAME_W, TK_WARNING_TEXT_CLEAR_H , 0, 0);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextIDDraw(Warn_X,Warn_Y,SP5K_GFX_ALIGN_TOP,usStrId);		
	}

#endif

}


/*-------------------------------------------------------------------------
*	File Name : appErrorOsd_VoiceMemoWarnScreenDraw
*------------------------------------------------------------------------*/
void appErrorOsd_VoiceMemoWarnScreenDraw(BOOL bShow, unsigned short usStrId)
{
	#if 0
	unsigned int Warn_X,Warn_Y;
	
	Warn_X = TK_WARNING_X+(TK_WARNING_W/2);
	Warn_Y = TKTYPE_WARNING_VOICEMEMO_Y;


	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,32, 0, 0);
		return ;
	}

	if(usStrId==ID_STR_READY_TO_PLAY||usStrId==ID_STR_PLAYING_AUDIO___||usStrId==ID_STR_PAUSE)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X-6, Warn_Y,TKTYPE_WARNING_W+12,32, 0, 0);
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,32, 0, 0);
	}
	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(Warn_X, Warn_Y, SP5K_GFX_ALIGN_CENTER, usStrId);
	#endif
}

/*-------------------------------------------------------------------------
*	File Name : appErrorOsd_BlackWarnScreen
*------------------------------------------------------------------------*/
void  appErrorOsd_BlackWarnScreen(BOOL bShow, unsigned short usStrId)
{
#if 0
	unsigned int Warn_X,Warn_Y;

	Warn_X = TK_WARNING_X+(TK_WARNING_W/2);
	Warn_Y = TK_WARNING_Y+(TK_WARNING_H-50);
	
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,TKTYPE_WARNING_H-50, 0, 0);

	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,TKTYPE_WARNING_H-50, 0, 0);
		return ;
	}
	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(Warn_X, Warn_Y, SP5K_GFX_ALIGN_CENTER, usStrId);
#endif
}






