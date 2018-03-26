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
#include "sp5k_dcf_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "app_stringid.h"
#include "app_jpg_draw.h"
#include "app_osdfont.h"
#include "app_playback_osd.h"
#include "app_exif.h"
#include "app_view_param_def.h"
#include "app_error_msg.h"
#include "app_menu_frameosd.h"
#include "app_ui_para.h" 
#include "app_menu_tab.h"
#include "app_menu_disp.h"

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
static pbDlgBoxInfo_t gpbDlgCb;

static UINT16 PBLastProgBarLength;	//For progress bar drawing in P/B mode.

UINT16 gbTKPlayMenuStartIndex; 

extern UINT8 trimCount;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_HighlightDraw
 *------------------------------------------------------------------------*/
#define PB_AUXNOTE_ICON_HEIGHT	32
#define PB_AUXNOTE_ICON_WIDTH	36
void
appPbOsd_AuxNote_HighlightDraw(
	SINT32 iconX, SINT32 iconY,  UINT32 iconID,
	SINT32 textX, SINT32 textY,  UINT32 textID,
	COLOR_DEF textColor
)
{
	/*appOsd_FontIconFontSet(ID_FONT_TK_FONT1);*/
	appOsd_FontIconFontSet(ID_FONT_ICON); /*- Heigt: 32 */

	appOsdLib_OsdLayIconDraw(iconX, iconY,  SP5K_GFX_ALIGN_TOP_LEFT, iconID);

	appOsd_ColorDrawTextColorSet(textColor, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(textX, textY,SP5K_GFX_ALIGN_TOP_LEFT, textID);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNoteDraw
 *------------------------------------------------------------------------*/
#if 1  /*nan.yang 20121108 for clear warning*/
static
void
appPbOsd_AuxNoteDraw(
	SINT32 iconX, SINT32 iconY,  UINT32 iconID,
	SINT32 textX, SINT32 textY,  UINT32 textID
)
{
	appPbOsd_AuxNote_HighlightDraw(
		iconX, iconY, iconID,
		textX, textY, textID, PAL_WHITE);
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_LeftDraw
 *------------------------------------------------------------------------*/
static
void
appPbOsd_AuxNote_LeftDraw(
	BOOL bShow,
	UINT32 iconId,
	UINT32 textId
)
{
#if 0
	UINT32 textW;
	if(bShow)
	{
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X,
			TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,
			iconId,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2,
			TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10,
			textId);

	}
	else
	{
		textW = appOsdLib_FontGetStringWidthbyId(textId);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
									TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,
									PB_AUXNOTE_ICON_WIDTH+textW, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
	}
#endif
	
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_RightDraw
 *------------------------------------------------------------------------*/
static
void
appPbOsd_AuxNote_RightDraw(
	BOOL bShow,
	UINT32 iconId,
	UINT32 textId
)
{
#if 1
	//UINT32 textW;
	SINT32 iconX, iconY;
      #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		iconX = 640/2-PB_AUXNOTE_ICON_WIDTH+100;
		iconY = 200+90/*39*/;
	}else{
		iconX = 320/2-PB_AUXNOTE_ICON_WIDTH+50;
		iconY = 200;
	}
	#endif

	if (!bShow) {
		//textW = appOsdLib_FontGetStringWidthbyId(textId);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
									iconX, iconY,
									PB_AUXNOTE_ICON_WIDTH+80, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
		return;
	}

	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN)
	{
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			iconX,
			iconY,
			iconId,
			/*-- Text */
			iconX+52,
			iconY,
			textId);
	}
	else
	{
		appPbOsd_AuxNoteDraw(
		/*-- Icon */
		iconX,
		iconY,
		iconId,
		/*-- Text */
		iconX+36,
		iconY,
		textId);
	}
	#endif
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_CenterDraw
 *------------------------------------------------------------------------*/
static
void
appPbOsd_AuxNote_CenterDraw(
	BOOL bShow,
	UINT32 iconId,
	UINT32 textId
)
{
#if 1
	//UINT32 textW;
	SINT32 iconX, iconY;

	//textW = appOsdLib_FontGetStringWidthbyId(textId);
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN)
	{
		iconX = 640/2-PB_AUXNOTE_ICON_WIDTH-150;
		iconY = 200+90/*39*/;
		
		if (!bShow)
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
										iconX, iconY,
										PB_AUXNOTE_ICON_WIDTH+120/*80*/, PB_AUXNOTE_ICON_HEIGHT+8/*4*/, 0, 0);
			return;
		}
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			iconX,
			iconY,
			iconId,
			/*-- Text */
			iconX+52,
			iconY,
			textId);		
	}
	else
	{
		iconX = 320/2-PB_AUXNOTE_ICON_WIDTH-75;
		iconY = 200;

		if (!bShow) 
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
										iconX, iconY,
										PB_AUXNOTE_ICON_WIDTH+80, PB_AUXNOTE_ICON_HEIGHT+4, 0, 0);
			return;
		}
			appPbOsd_AuxNoteDraw(
				/*-- Icon */
				iconX,
				iconY,
				iconId,
				/*-- Text */
				iconX+36,
				iconY,
				textId);
	}
	#endif
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKPause
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKPause(
	BOOL bShow
)
{
	appPbOsd_AuxNote_CenterDraw(bShow, ID_ICON_BUTTON_OK, ID_STR_PAUSE);
}
/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKPlay
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKPlay(
	BOOL bShow
)
{
	appPbOsd_AuxNote_CenterDraw(bShow, ID_ICON_BUTTON_OK, ID_STR_PLAY);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKStop
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKStop(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, ID_STR_STOP);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKRec
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKRec(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, ID_STR_BACK);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKSet
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKSet(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, ID_STR_SET);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKSelect
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKSelect(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, ID_STR_SELECT);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_MenuCancel
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_MenuCancel(
	BOOL bShow
)
{
	appPbOsd_AuxNote_RightDraw(bShow, ID_ICON_BUTTON_MENU, ID_STR_CANCEL);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_MenuBack
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_MenuBack(
	BOOL bShow
)
{
	appPbOsd_AuxNote_RightDraw(bShow, ID_ICON_BUTTON_MENU, ID_STR_BACK);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKDispPhoto
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKDispPhoto(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, ID_STR_DISPLAY_FILE);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_TrashCanDelet
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_TrashCanDelet(
	BOOL bShow,
	BOOL bActive
)
{
#if 0
	UINT32 textW;

	if (!bShow)
	{
		textW = appOsdLib_FontGetStringWidthbyId(ID_STR_DELETE);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, PB_AUXNOTE_ICON_WIDTH+textW, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
		return;
	}

	if (bActive) 
	{
		/*-- Set Green Color for TEXT */
		appPbOsd_AuxNote_HighlightDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_PLAYBACK_THUMBNAIL_GREENBLACKDELETE,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X+TKTYPE_MENUICON_OFFSET_X+10, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, ID_STR_DELETE,
			/*-- Highligh Color */
			PAL_YELLOWGREEN);
	}
	else
	{
		/*-- Set Normal (White) for TEXT */
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_PLAYBACK_THUMBNAIL_DELETE,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X+TKTYPE_MENUICON_OFFSET_X+10, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, ID_STR_DELETE);
	}
#endif
}


/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_LockProtect
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_LockProtect(
	BOOL bShow,
	BOOL bActive
)
{
#if 0
	UINT32 textW;

	if (!bShow) {
		textW = appOsdLib_FontGetStringWidthbyId(ID_STR_PROTECT_FILE_);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, PB_AUXNOTE_ICON_WIDTH+textW, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
		return;
	}

	if (bActive) {
		/*-- Set Green Color for TEXT */
		appPbOsd_AuxNote_HighlightDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_PROTECT,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, ID_STR_PROTECT_FILE_,
			/*-- Highligh Color */
			PAL_YELLOWGREEN);
	} else {
		/*-- Set Normal (White) for TEXT */
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_PROTECT,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, ID_STR_PROTECT_FILE_);
	}
#endif

}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxCBGet
 *------------------------------------------------------------------------*/
pbDlgBoxInfo_t
*appPlayOsd_DlgBoxCBGet(
	void
)
{
	return &gpbDlgCb;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxSelectSet
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxSelectSet(
	UINT32 selIndex
)
{
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex = selIndex;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxSelectGet
 *------------------------------------------------------------------------*/
UINT8
appPlayOsd_DlgBoxSelectGet(
	void
)
{
	return gpbDlgCb.actIndex;
}

UINT32 appPlayOsd_DlgBoxStatusGet(void)
{
	return gpbDlgCb.dlgStatus;
}

void appPlayOsd_DlgBoxStatusSet(UINT32 status)
{
	 gpbDlgCb.dlgStatus = status;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxClear
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxClear(
	void
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, gpbDlgCb.boxX, gpbDlgCb.boxY, gpbDlgCb.boxW, gpbDlgCb.boxH, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxTitleSet
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxTitleSet(
	UINT32 titleStrId
)
{
	gpbDlgCb.titleStrId = titleStrId;
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxTitleUpdate
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxTitleUpdate(
	UINT32 titleStrId
)
{
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	appPlayOsd_DlgBoxTitleSet(titleStrId);
	appPlayOsd_DlgBoxShow();
	#if 0
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, gpbDlgCb.boxX, gpbDlgCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_TXT_TITLE_ICON, 0x0000);
	appOsdLib_TextIDDraw(gpbDlgCb.titleX, gpbDlgCb.titleY, SP5K_GFX_ALIGN_BOTTOM, gpbDlgCb.titleStrId);
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxActiveBarChange
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxActiveBarChange(
	void
)
{
	//UINT8 index;

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_ColorIconColorSet(PAL_GRAY_1, PAL_GRAY_1);
	appPlayOsd_DlgBoxShow();
	#if 0
	/*-- Inactivate last option */
	index = gpbDlgCb.lastActIndex;
	//PB_VERBOSE("\n==> lastActIndex (%d) ..\n\n", index);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\MU2CLBAR.SFN", 0x0000);
	appOsdLib_TextIDDraw(gpbDlgCb.optX[index], gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, gpbDlgCb.optionStrID[index]);

	/*-- Activate current option */
	index = gpbDlgCb.actIndex;
	//PB_VERBOSE("\n==> actIndex (%d) ..\n\n", index);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\MUDLGBAR.SFN", 0x0000);
	appOsdLib_TextIDDraw(gpbDlgCb.optX[index], gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, gpbDlgCb.optionStrID[index]);
	#endif
	appOsd_ColorIconColorSet(PAL_GRAY_1, PAL_NOCOLOR);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxScroll
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxScroll(
	BOOL isUp
)
{
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex;

	if (isUp) 
	{
		PB_INDEX_DEC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	} 
	else
	{
		PB_INDEX_INC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	}

	appPlayOsd_DlgBoxActiveBarChange();
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxShow
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxShow(
void
)
{
	UINT8 *fileName;
    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		switch(gpbDlgCb.numOptions)
		{
			case 2:
				fileName=ID_DLGBOX_TXT2_TITLE_ICON_HDMI;
				break;
			case 3:
				fileName=ID_DLGBOX_TXT3_TITLE_ICON_HDMI;
				break;
			case 4:
				fileName=ID_DLGBOX_TXT4_TITLE_ICON_HDMI;
				break;
			default:
				fileName=ID_DLGBOX_TXT2_TITLE_ICON_HDMI;
				break;
		}
	}
	else{
		switch(gpbDlgCb.numOptions)
		{
			case 2:
				fileName=ID_DLGBOX_TXT2_TITLE_ICON;
				break;
			case 3:
				fileName=ID_DLGBOX_TXT3_TITLE_ICON;
				break;
			case 4:
				fileName=ID_DLGBOX_TXT4_TITLE_ICON;
				break;
			default:
				fileName=ID_DLGBOX_TXT2_TITLE_ICON;
				break;
		}
	}
	#endif
	sp5kGfxAttrSet(SP5K_GFX_BLEND_FACTOR, 127, 0, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);
	appOsd_ColorBlendingSet(PAL_GRAY_1, PAL_BLEND_75);
	APP_OSD_REFRESH_OFF;
	/*-- Title ICON & TEXT */
	__FUNC_TRACK__;

	#if HDMI_MENU_BY_JPG
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		menuDlgShow(gpbDlgCb.numOptions);
	}else{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, gpbDlgCb.boxX, gpbDlgCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, fileName, 0x0000);
	}	
	#endif
	#else
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, gpbDlgCb.boxX, gpbDlgCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, fileName, 0x0000);
	#endif
	if (gpbDlgCb.titleStrId)
	{
		appOsdLib_TextIDDraw(gpbDlgCb.titleX, gpbDlgCb.titleY, SP5K_GFX_ALIGN_BOTTOM, gpbDlgCb.titleStrId);
	}
	#if 0		//marked by gx
	else 
	{
		__FUNC_TRACK__;
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, gpbDlgCb.boxX, gpbDlgCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_NULL_TITLE_ICON, 0x0000);
	}
	#endif

	/*-- Options TEXT & ICON, except the last */
	UINT32 i;
	for (i=0; i<gpbDlgCb.numOptions; i++)
	{
		#if 0	//marked by gx
		/*-- TEXT BG ICONT */
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, gpbDlgCb.boxX, gpbDlgCb.titleY+(i*DLGBOX_DATA_CONT_H), SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_OPTION_TXT_ICON, 0x0000);
		#endif
		/*-- Show High-Light ICON --*/
		if (i==gpbDlgCb.actIndex)
		{
		    #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
			if(IS_HDMI_IN){
				#if HDMI_MENU_BY_JPG
				menuDlgBarShow(gpbDlgCb.numOptions,gpbDlgCb.actIndex);
				#else
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (640-DLGBOX_HL_BAR_W_HDMI)>>1, gpbDlgCb.optY[i], SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\MUSUSBAR_HDMI.SFN", 0x0000);
				#endif
			}
			else{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[i], SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\MUDLGBAR.SFN", 0x0000);
			}
		    #endif
		}
		if (gpbDlgCb.optionStrID[i])
		{
			appOsdLib_TextIDDraw(gpbDlgCb.optX[i], gpbDlgCb.optY[i], SP5K_GFX_ALIGN_TOP, gpbDlgCb.optionStrID[i]);
		}
	}
	#if 0	//marked by gx
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, gpbDlgCb.boxX, gpbDlgCb.titleY+(i*DLGBOX_DATA_CONT_H), SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_OPTION_BOTTOM_ICON, 0x0000);
	if (gpbDlgCb.actIndex==i)
	{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[i], SP5K_GFX_ALIGN_TOP_LEFT, RES_ICON_MU2NDBAR, 0x0000);
	}
	appOsdLib_TextIDDraw(gpbDlgCb.optX[i], gpbDlgCb.optY[i], SP5K_GFX_ALIGN_TOP_LEFT, gpbDlgCb.optionStrID[i]);
	#endif
	APP_OSD_REFRESH_ON;
	appOsd_ColorIconColorSet(PAL_GRAY_1, PAL_NOCOLOR);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxActiveBarChange
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxActiveBarChangeSet(
	UINT32 DlgStartX,
	UINT32 DlgStartY,	
	UINT32 DlgFirstItemX,	
	UINT32 DlgFirstItemY,
	UINT32 DlgItemGap
)
{
	UINT8 index;

	
	/*-- Inactivate last option */
	index = gpbDlgCb.lastActIndex;
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);																																																		   
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,DlgStartX,DlgStartY+(index+1)*DlgItemGap,291,31,0,0);	

	appOsd_ColorDrawTextColorSet(PAL_GRAY, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+index*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[index]);

	/*-- Activate current option */
	index = gpbDlgCb.actIndex;	
 /*s Added for highlight mark dot, Leo 20090827*/
 appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
appOsdLib_OsdLayIconDraw(DlgFirstItemX-24,DlgFirstItemY+index*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SELECT);	
/*e Added for highlight mark dot, Leo 20090827*/
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+index*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[index]);
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxScrollSet
 *------------------------------------------------------------------------*/

void
appPlayOsd_DlgBoxScrollSet(
	BOOL isUp,
	UINT32 DlgStartX,
	UINT32 DlgStartY,	
	UINT32 DlgFirstItemX,	
	UINT32 DlgFirstItemY,
	UINT32 DlgItemGap
)
{
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex;

	if (isUp) {
		PB_INDEX_DEC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	} else {
		PB_INDEX_INC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	}

	appPlayOsd_DlgBoxActiveBarChangeSet(DlgStartX,DlgStartY,DlgFirstItemX,DlgFirstItemY,DlgItemGap);
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxSetandShow()
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxSetandShow(
	UINT32 DlgStartX,
	UINT32 DlgStartY,	
	UINT32 DlgFirstItemX,	
	UINT32 DlgFirstItemY,
	UINT32 DlgItemGap
)
{
	UINT8 i;
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DlgStartX ,DlgStartY,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.titleStrId);
	for(i=0;i<gpbDlgCb.numOptions;i++)
	{
		if(gpbDlgCb.actIndex==i)
		{
		
 			appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
			appOsdLib_OsdLayIconDraw(DlgFirstItemX-24,DlgFirstItemY+i*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_SELECT);	 //Ethan
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+i*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[i]);
		}
		else
		{
			appOsd_ColorDrawTextColorSet(PAL_GRAY, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+i*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[i]);
		}
	}

}

/*e added by Leo Li @20091126*/

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxConfig
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxConfig(
	UINT32 titleStrId,
	UINT32 numOptions,
	UINT32 *poptionStrID,
	UINT32 actIndex,
	SINT32 yOffset
)
{
	UINT32 i;

	memset(&gpbDlgCb, 0, sizeof(pbDlgBoxInfo_t));

	gpbDlgCb.titleStrId = titleStrId;
	gpbDlgCb.numOptions = (numOptions>DLGBOX_OPTION_NUM_MAX) ? DLGBOX_OPTION_NUM_MAX : numOptions;
	gpbDlgCb.actIndex = (actIndex>=DLGBOX_OPTION_NUM_MAX) ? DLGBOX_OPTION_NUM_MAX-1 : actIndex;
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex;
	gpbDlgCb.dlgStatus = DLGSTATUS_SHOW;
	/*-- Construct coordinates & dimension */
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		gpbDlgCb.boxW = DLGBOX_WIDTH_HDMI;
		switch(gpbDlgCb.numOptions){
			case 2:
				gpbDlgCb.boxH = 149;
				break;
			case 3:
				gpbDlgCb.boxH = 198;
				break;
			case 4:
				gpbDlgCb.boxH = 198;
				break;
			default:
				gpbDlgCb.boxH = 149;
				break;
		}

		gpbDlgCb.boxX = (640-gpbDlgCb.boxW)>>1;
		gpbDlgCb.boxY = ((360-gpbDlgCb.boxH)>>1);
		gpbDlgCb.boxY += yOffset*3>>1;

		gpbDlgCb.titleX = 320;	/*-- title text is BOTTOM-CENTER aligned */
		gpbDlgCb.titleY = gpbDlgCb.boxY+DLGBOX_NULL_TITLE_H+10;

		UINT32 optX = 320;
		UINT32 optY = gpbDlgCb.titleY;
		for (i=0; i<numOptions; i++)
		{
			gpbDlgCb.optionStrID[i] = *(poptionStrID+i);
			gpbDlgCb.optX[i] = optX;
			gpbDlgCb.optY[i] = optY+(i*(DLGBOX_DATA_CONT_H+10));
		}
	}
	else{
		gpbDlgCb.boxW = DLGBOX_WIDTH;
		switch(gpbDlgCb.numOptions){
			case 2:
				gpbDlgCb.boxH = 120;
				break;
			case 3:
				gpbDlgCb.boxH = 159;
				break;
			case 4:
				gpbDlgCb.boxH = 138;
				break;
			default:
				gpbDlgCb.boxH = 120;
				break;
		}

		gpbDlgCb.boxX = (320-gpbDlgCb.boxW)>>1;
		gpbDlgCb.boxY = ((240-gpbDlgCb.boxH)>>1);
		gpbDlgCb.boxY += yOffset;

		gpbDlgCb.titleX = 160;	/*-- title text is BOTTOM-CENTER aligned */
		gpbDlgCb.titleY = gpbDlgCb.boxY+DLGBOX_NULL_TITLE_H;

		UINT32 optX = 160;
		UINT32 optY = gpbDlgCb.titleY;
		for (i=0; i<numOptions; i++)
		{
			gpbDlgCb.optionStrID[i] = *(poptionStrID+i);
			gpbDlgCb.optX[i] = optX;
			gpbDlgCb.optY[i] = optY+(i*DLGBOX_DATA_CONT_H);
		}
	}

	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgConfirmYesNoConfig
 *------------------------------------------------------------------------*/
static UINT32 strID_DlgConfirmYesNo[2] = {ID_STR_YES, ID_STR_NO};
void
appPlayOsd_DlgConfirmYesNoConfig(
	UINT32 strId,
	UINT32 actIdx
)
{
	appPlayOsd_DlgBoxConfig(strId, 2, strID_DlgConfirmYesNo, actIdx, 40);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_IconDraw		Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_IconDraw
(
	UINT8 bShow,
	SINT32 X,
	SINT32 Y,
	UINT32 IconID
)
{
	appOsd_FontIconFontSet(ID_FONT_ICON);
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, X, Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	if (bShow)
	{
		appOsdLib_OsdLayIconDraw(X, Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProtectDraw		Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProtectDraw(
	UINT8 bShow
)
{
	//appPlayOsd_IconDraw(bShow, PROTTECT_ICON_X, PROTTECT_ICON_Y, ID_ICON_PROTECT);
	appIconShow(ID_ICON_PROTECT,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DPOFDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_DPOFDraw(
	UINT8 bShow
)
{
	#if 0
	appPlayOsd_IconDraw(bShow, DPOF_ICON_X, DPOF_ICON_Y, ID_ICON_PLAYBACK_THUMBNAIL_DPOF);
	#endif
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoDraw		Brian added
 *------------------------------------------------------------------------*/
void appPlayOsd_VoiceMemoDraw(void)
{
	if(gpbImgCb.fileType==PB_FT_JPG_MEMO)
	{
		//appPlayOsd_IconDraw(TRUE, VOICE_MEMO_ICON_X, VOICE_MEMO_ICON_Y, ID_ICON_MODE_VOICE);
		appIconShow(ID_ICON_MODE_VOICE,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}
	else
	{
		//appPlayOsd_IconDraw(FALSE, VOICE_MEMO_ICON_X, VOICE_MEMO_ICON_Y, ID_ICON_MODE_VOICE);
		appIconShow(ID_ICON_MODE_VOICE,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
	}
}
/*-------------------------------------------------------------------------
*  File Name : appPlayOsd_VideoModeDraw				Brian added
*------------------------------------------------------------------------*/
 void
 appPlayOsd_VideoModeDraw(
	 UINT8 bShow
 )
 {
	 //appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_MODE_VIDEO);
	 appIconShow(ID_ICON_MODE_VIDEO,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
 }


/*-------------------------------------------------------------------------
 *  File Name : appPlayOSD_JPGImageIconDraw		Brian added
*------------------------------------------------------------------------*/

 void
appPlayOSD_JPGImageIconDraw(
	UINT8 bShow,
	pbImgExifInfo_t *psPBCurImgExif
 )
 {
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	if (bShow)
	{
		switch(psPBCurImgExif->sceneMode)
		{
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_STANDARD:
				if (psPBCurImgExif->expProgMode== SP5K_EXIF_TAGDATA_EXPPROG_MANUAL)
				{
					//appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_MANUAL);
					appIconShow(ID_ICON_PASM_MANUAL,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				}
				else if (psPBCurImgExif->expProgMode == SP5K_EXIF_TAGDATA_EXPPROG_APERTURE)
				{
					//appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_APERTURE);
					appIconShow(ID_ICON_PASM_APERTURE,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				}
				else if (psPBCurImgExif->expProgMode == SP5K_EXIF_TAGDATA_EXPPROG_SHUTTER)
				{
					//appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_TV);
					appIconShow(ID_ICON_PASM_TV,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				}
				else
				{
					//appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_PROGRAM);
					appIconShow(ID_ICON_PASM_PROGRAM,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				}
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_PORTRAIT:
				//appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_SCENE_HISO_PORTRAIT);
				appIconShow(ID_ICON_SCENE_HISO_PORTRAIT,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_LANDSCANPE:
				//appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_SCENE_LANDSCAPE);
				appIconShow(ID_ICON_SCENE_LANDSCAPE,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_NIGHTSCENE:
				//appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_SCENE_NIGHTSCENERY);
				appIconShow(ID_ICON_SCENE_NIGHTSCENERY,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_OTHER:
			default:
				break;
		}
	}
}
 /*-------------------------------------------------------------------------
  *  File Name : appPlayOsd_ResolutionDraw	 Brian added
  *------------------------------------------------------------------------*/
void
appPlayOsd_ResolutionDraw(
	UINT8 bShow,
	UINT16 Resolution
)
{

	UINT16 ResolutionModeIcon[] = 
	{
		[PB_IMG_SIZE_GRADE_0	] = ID_ICON_RESOLUTION_VGA,
		[PB_IMG_SIZE_GRADE_1	] = ID_ICON_RESOLUTION_1M,
		[PB_IMG_SIZE_GRADE_2	] = ID_ICON_RESOLUTION_2M,
		[PB_IMG_SIZE_GRADE_3	] = ID_ICON_RESOLUTION_3M,
		[PB_IMG_SIZE_GRADE_4   	] = ID_ICON_RESOLUTION_4M,
		[PB_IMG_SIZE_GRADE_5	] = ID_ICON_RESOLUTION_5M,
		[PB_IMG_SIZE_GRADE_6	] = ID_ICON_RESOLUTION_6M,
		[PB_IMG_SIZE_GRADE_7  	] = ID_ICON_RESOLUTION_7M,
		[PB_IMG_SIZE_GRADE_8   	] = ID_ICON_RESOLUTION_8M,
		[PB_IMG_SIZE_GRADE_9	] = ID_ICON_RESOLUTION_9M,
		[PB_IMG_SIZE_GRADE_10	] = ID_ICON_RESOLUTION_10M,
		[PB_IMG_SIZE_GRADE_TOTAL] = 0,
	};

	if (Resolution > PB_IMG_SIZE_GRADE_TOTAL)
	{
		bShow = FALSE;
	}
	//appPlayOsd_IconDraw(bShow, RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ResolutionModeIcon[Resolution]);
	appIconShow(ResolutionModeIcon[Resolution],bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}
void appPlayOsd_VideoSizeDraw(UINT8 bShow,UINT32 width)
{
	switch(width)
	{
		case 320:
			//appPlayOsd_IconDraw(bShow,RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ID_ICON_MOVIE_SIZE_320);
			appIconShow(ID_ICON_MOVIE_SIZE_320,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
			break;
		case 640:
			//appPlayOsd_IconDraw(bShow,RESOLUTION_ICON_X, RESOLUTION_ICON_Y,ID_ICON_MOVIE_SIZE_640);
			appIconShow(ID_ICON_MOVIE_SIZE_640,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
			break;
		case 720:
			//appPlayOsd_IconDraw(bShow, RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ID_ICON_MOVIE_SIZE_720P);
			appIconShow(ID_ICON_MOVIE_SIZE_D1,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
			break;			
		case 1280:
			//appPlayOsd_IconDraw(bShow, RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ID_ICON_MOVIE_SIZE_720P);
			appIconShow(ID_ICON_MOVIE_SIZE_720P,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
			break;
		case 1920:
			appIconShow(ID_ICON_MOVIE_SIZE_1080P,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
			break;
		default:
			break;
	}

}
/*-------------------------------------------------------------------------
 *	File Name : appPlayOsd_WhiteBalanceDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_WhiteBalanceDraw(
	UINT8 bShow,
	UINT16 WhiteBalanceMode,
	unsigned int LightSource
)
{
	UINT16 WhiteBalanceModeIcon = 0;
	switch(WhiteBalanceMode)
	{
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_UNKNOWN:
			bShow =FALSE;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT:
			WhiteBalanceModeIcon = ID_ICON_WB_DAYLIGHT;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_FLUORESCENT:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT_FLUORESCENT:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_WHITE_FLUORESCENT:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_A:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_B:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_C:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D55:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D65:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D75:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D50:
			WhiteBalanceModeIcon = ID_ICON_WB_FLUORESCENT;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_CLOUDY_WEATHER:
			WhiteBalanceModeIcon = ID_ICON_WB_CLOUDY;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_ISO_STUDIO_TUNGSTEN:
			WhiteBalanceModeIcon = ID_ICON_WB_TUNGSTEN;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAY_WHITE_FLUORESCENT:
			WhiteBalanceModeIcon = ID_ICON_WB_FLUORESCENT_H;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_COOL_WHITE_FLUORESCENT:
			WhiteBalanceModeIcon = ID_ICON_WB_FLUORESCENT_L;
			break;
		default:
			WhiteBalanceModeIcon = ID_ICON_WB_AUTO;
			break;
	}
	//appPlayOsd_IconDraw(bShow, WHITE_BALANCE_ICON_X, WHITE_BALANCE_ICON_Y, WhiteBalanceModeIcon);
	appIconShow(WhiteBalanceModeIcon,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}


/*-------------------------------------------------------------------------
 *	File Name : appPlayOsd_ExposureDraw 		Brian added
 *------------------------------------------------------------------------*/
 void
appPlayOsd_ExposureDraw(
	UINT8 bShow,
	SINT32 ExpBias
)
{

	UINT16 ExplosureModeIcon[] = 
	{
		[0 ] = ID_ICON_EV_N20,
		[1 ] = ID_ICON_EV_N17,
		[2 ] = ID_ICON_EV_N13,
		[3 ] = ID_ICON_EV_N10,
		[4 ] = ID_ICON_EV_N07,
		[5 ] = ID_ICON_EV_N03,
		[6 ] = ID_ICON_EV_00,
		[7 ] = ID_ICON_EV_P03,
		[8 ] = ID_ICON_EV_P07,
		[9] = ID_ICON_EV_P10,
		[10] = ID_ICON_EV_P13,
		[11] = ID_ICON_EV_P17,
		[12] = ID_ICON_EV_P20,

	};

	int ExpIconIdex = (int)(ExpBias / 33) + 6;

	if (ExpIconIdex > 12)
	{
		bShow = FALSE;
		ExpIconIdex = 12;
	}
	//appPlayOsd_IconDraw(bShow, EXPLOSURE_ICON_X, EXPLOSURE_ICON_Y, ExplosureModeIcon[ExpIconIdex]);
	appIconShow(ExplosureModeIcon[ExpIconIdex],bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ISODraw                      Brian added
 *------------------------------------------------------------------------ */
void
appPlayOsd_ISODraw(
	UINT8 bShow,
	UINT16 ISOMode
	)
{

	UINT16 ISOType = 0;
	switch (ISOMode)
		{
		case 50:
			ISOType = ID_ICON_ISO_50;
			break;
		case 100:
			ISOType = ID_ICON_ISO_100;
			break;
		case 200:
			ISOType = ID_ICON_ISO_200;
			break;
		case 400:
			ISOType = ID_ICON_ISO_400;
			break;
		case 800:
			ISOType = ID_ICON_ISO_800;
			break;
		case 1600:
			ISOType = ID_ICON_ISO_1600;
			break;
		default:
			bShow = FALSE;
			break;
		}
	//appPlayOsd_IconDraw(bShow, ISO_ICON_X, ISO_ICON_Y, ISOType);
	appIconShow(ISOType,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_StrobeModeDraw                              Brian added
 *------------------------------------------------------------------------*/

void
appPlayOsd_StrobeModeDraw(
	UINT8 bShow,
	UINT16 StrobeMode
	)
{

	UINT16 FlashType = 0;
	switch (StrobeMode)
	{
		case EXIF_VALUE_FLASH_RED_EYE:
			FlashType = ID_ICON_FLASH_REDEYE_ON;
			break;
		case EXIF_VALUE_FLASH_SLOW_SYNC:
			FlashType = ID_ICON_FLASH_SLOW_SYNC;
			break;
		case EXIF_VALUE_FLASH_AUTO_FIRED:
			FlashType = ID_ICON_FLASH_AUTO;
			break;
		case EXIF_VALUE_FLASH_FORCE_ON:
			FlashType = ID_ICON_FLASH_ON;
			break;
		case EXIF_VALUE_FLASH_FORCE_OFF:
		case EXIF_VALUE_FLASH_AUTO_NO_FIRED:
		case EXIF_VALUE_FLASH_OFF:
			FlashType = ID_ICON_FLASH_OFF;
			break;
		default:
			bShow = FALSE;
			break;
	}
	//appPlayOsd_IconDraw(bShow,STROBE_MODE_ICON_X, STROBE_MODE_ICON_Y,FlashType);
	appIconShow(FlashType,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_MeteringModeDraw
 *------------------------------------------------------------------------*/

void
appPlayOsd_MeteringModeDraw(
	UINT8 bShow,
	UINT16 MeteringMode
	)
{

	UINT16 MeteringModeIcon = 0;
	switch (MeteringMode)
	{
		case SP5K_EXIF_TAGDATA_METERMODE_CENTERWEIGHTED:
			MeteringModeIcon = ID_ICON_METERING_CENTER;
			break;
		case SP5K_EXIF_TAGDATA_METERMODE_SPOT:
			MeteringModeIcon = ID_ICON_METERING_SPOT;
			break;
		case SP5K_EXIF_TAGDATA_METERMODE_MULTISPOT:
			MeteringModeIcon = ID_ICON_METERING_MULTI;
			break;
		default:
			bShow = FALSE;
			break;
	}

	//appPlayOsd_IconDraw(bShow, METERING_MODE_ICON_X, METERING_MODE_ICON_Y, MeteringModeIcon);
	appIconShow(MeteringModeIcon,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DateTimeDraw                      Brian added
 *-----------------------------------------------------------------------*/
void
appPlayOsd_DateTimeDraw (
	UINT8 bShow,
	UINT32 DateTimeFormat,
	sp5kDcfTime_t *FileTime
)
{
	UINT16 iconX,iconY;
	UINT32 posX,posY;
	UINT32 iconSizeX,iconSizeY,fontSizeX;
	static UINT32 dateStrW = 0,dateStrH = 0;
	static UINT32 timeStrW = 0,timeStrH = 0;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		iconSizeX = 48;
		iconSizeY = 42;
		fontSizeX = 20;
	}else{
	#else
	{
	#endif
		iconSizeX = APPPB_ICON_SIZE_X;
		iconSizeY = APPPB_ICON_SIZE_Y;
		fontSizeX = OSD_FONT_CONST_WIDTH;
	}
	
	appOsdIconPosGet(ID_ICON_MODE_PLAYBACK,&iconX,&iconY);
	#if CAM_TYPE_CVR
	if(IS_HDMI_IN){
		posX = iconX+iconSizeX-20;
		posY = iconY+250+14;
	}else{
		posX = iconX+iconSizeX-20;
		posY = iconY+173;
	}
	#else
	posX = iconX+iconSizeX+10;
	posY = iconY+6;
	#endif
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, posX, posY, dateStrW + timeStrW + fontSizeX, dateStrH, 0, 0);
	if (bShow)
	{
		UINT8 DateString[12], TimeString[10];
		memset(DateString, 0, 12);
		memset(TimeString, 0, 10);
		if (DateTimeFormat == DATE_TIME_YYMMDD)
		{
			sprintf (DateString, "%04d.%02d.%02d", FileTime->year ,FileTime->mon, FileTime->day);
		}
		else if (DateTimeFormat == DATE_TIME_DDMMYY) 
		{
			sprintf (DateString, "%02d.%02d.%04d", FileTime->day, FileTime->mon, FileTime->year);
		}
		else 
		{
			sprintf (DateString, "%02d.%02d.%04d", FileTime->mon, FileTime->day, FileTime->year);
		}
		sprintf (TimeString, "%02d:%02d", FileTime->hour, FileTime->min);
		
		sp5kGfxStringSizeGet(STR_ENCODE_ASCII, DateString, &dateStrW, &dateStrH);
		sp5kGfxStringSizeGet(STR_ENCODE_ASCII, TimeString, &timeStrW, &timeStrH);
		
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(posX, posY, SP5K_GFX_ALIGN_TOP_LEFT, DateString);
		appOsdLib_TextDraw(posX+dateStrW+fontSizeX, posY, SP5K_GFX_ALIGN_TOP_LEFT, TimeString);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_FileNumDraw			Brian Modified
 *------------------------------------------------------------------------*/
void
appPlayOsd_FileNumDraw(
	UINT8 bShow,
	UINT32	CurrntIndex,
	UINT32	Total
)
{
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	static UINT32 strW=0,strH=0;
	UINT32 strPosX;

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	strPosX = pPageCap->frmW;
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		strPosX -= 20;
	}
	#endif
	
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, strPosX- strW, 0, strW, strH, 0, 0);
	if (bShow)
	{
		UINT8 FileNumString[10];
		memset(FileNumString, 0, 10);
		sprintf(FileNumString, "%d/%d ", CurrntIndex, Total);
		sp5kGfxStringSizeGet(STR_ENCODE_ASCII, FileNumString, &strW, &strH);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		if(IS_HDMI_IN)
		{
			appOsdLib_TextDraw(strPosX, PLAYBACK_ICON_Y+5, SP5K_GFX_ALIGN_TOP_RIGHT, FileNumString);
		}
		else
		{
		 	appOsdLib_TextDraw(strPosX, PLAYBACK_ICON_Y, SP5K_GFX_ALIGN_TOP_RIGHT, FileNumString);
		}
	}

	sp5kFree(pPageCap);
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_CurrntFileNameDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_CurrntFileNameDraw(
	BOOL bShow,
	UINT8* CurrntFileName
)
{
	UINT32 posX,posY;
	UINT32 fontSizeY = 28;
	UINT32 offsetX=0,offsetY = 2*(APPPB_ICON_SIZE_Y+fontSizeY);
	static UINT32 strW = 0,strH = 0;;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX=80;
		fontSizeY = 42;
		offsetY = 2*(42 + fontSizeY)+6;
	}	
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	posX = pPageCap->frmW/2+offsetX;
	posY = pPageCap->frmH - offsetY;

	sp5kGfxStringSizeGet(STR_ENCODE_ASCII, CurrntFileName, &strW, &strH);
	
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, posX, posY, strW, strH, 0, 0);

	if (bShow)
	{
	 	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(posX, posY, SP5K_GFX_ALIGN_TOP_LEFT, CurrntFileName);
	}

	sp5kFree(pPageCap);
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_CurrntFileSizeDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_CurrntFileSizeDraw(
	BOOL bShow,
	UINT32 FileSize
)
{
	UINT32 posX,posY;
	UINT32 fontSizeY = 5;
	UINT32 offsetX=55,offsetY = 2*APPPB_ICON_SIZE_Y+fontSizeY;
	static UINT32 strW = 0,strH = 0;;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX=52;
		fontSizeY = 38;
		offsetY = 60 + fontSizeY;
	}	
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);
    #if !CAM_TYPE_CVR
	posX = pPageCap->frmW/2+65;
	posY = pPageCap->frmH - 60;
    #else
 	posX = pPageCap->frmW/2+offsetX;
	posY = pPageCap->frmH - offsetY;
    #endif
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, posX, posY, strW, strH, 0, 0);

	if (bShow)
	{
		unsigned char  FileSizeString[6];
		FileSize = FileSize / 10000;
		sprintf(FileSizeString, "%d.%02d MB", FileSize / 100, FileSize % 100);
		sp5kGfxStringSizeGet(STR_ENCODE_ASCII, FileSizeString, &strW, &strH);
	 	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(posX, posY, SP5K_GFX_ALIGN_TOP_LEFT, FileSizeString);
	}

	sp5kFree(pPageCap);
}

void
ExposureStringGet(
	UINT32 Exp0,
	UINT32 Exp1,
	char *outstr)
{
	printf("\n\n\n %d,  %d", Exp0, Exp1);

	if (Exp0 > 999 || Exp1 > 9999 || Exp0 == 0 || Exp1 == 0) {
		strcpy(outstr, "0/0");
	}
	else if ((Exp0 == 1) && (Exp1 > 1)) {
		sprintf(outstr, "%d/%ds", Exp0, Exp1);
	}
	else if ((Exp0 >= 1) && (Exp1 == 1)) {
		sprintf(outstr, "%ds", Exp0);
	}
	else if ((Exp0 >= 1) && (Exp1 == 10)) {
		sprintf(outstr, "%d.%ds", Exp0/10, Exp0%10);
	} else {
		sprintf(outstr, "%d/%d", Exp0, Exp1);

	}


}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_CurrntExpFNumDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_CurrntExpFNumDraw(
	UINT8 bShow,
	pbImgExifInfo_t *psPBCurImgExif
)
{
	UINT32 posX,posY;
	UINT32 offsetX=0,offsetY = 2*APPPB_ICON_SIZE_Y;
	UINT32 fontSizeX = 14;
	static UINT32 strW = 0,strH = 0;;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX=80;
		offsetY = 2*42+6;
		fontSizeX = 20;
	}	
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	posX = pPageCap->frmW/2+offsetX;
	posY = pPageCap->frmH - offsetY;

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, posX, posY, strW, strH, 0, 0);
	if (bShow)
	{
		unsigned char ExpString[9];
		unsigned char FNumString[2];
		unsigned int szFNum[2];
		memset(ExpString, 0, 9);
		memset(FNumString, 0, 2);
		if(psPBCurImgExif->fNum[1])
		{
			szFNum[0] = (psPBCurImgExif->fNum[0] / psPBCurImgExif->fNum[1]);
			szFNum[1] = (psPBCurImgExif->fNum[0] % psPBCurImgExif->fNum[1]) * 10 / psPBCurImgExif->fNum[1];
		}
		sprintf(FNumString, "F%d.%d", szFNum[0], szFNum[1]);
		sp5kGfxStringSizeGet(STR_ENCODE_ASCII, FNumString, &strW, &strH);
	 	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(posX, posY, SP5K_GFX_ALIGN_TOP_LEFT, FNumString);
		ExposureStringGet(psPBCurImgExif->expTime[0], psPBCurImgExif->expTime[1], ExpString);
		appOsdLib_TextDraw(posX + (6 * fontSizeX) , posY, SP5K_GFX_ALIGN_TOP_LEFT, ExpString);
	}

	sp5kFree(pPageCap);
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_HistogramDraw			Brian added
 *------------------------------------------------------------------------*/
 
#define PB_RECT_leftupper_x	    175
#define PB_RECT_leftupper_y	    56
#define PB_HISTOGRAM_SHAPE_HEIGHT	118
void
appPlayOsd_HistogramDraw(
	UINT8 bShow,
	UINT32 *pLuma
)
{
	UINT32 i;
	UINT32 bar_width;
	UINT32 bar_Hight;
	UINT32 posX,posY;
	UINT32 fontSizeY = 28;
	UINT32 histogramSizeX = PB_HISTOGRAM_SIZE_X,histogramSizeY = PB_HISTOGRAM_SIZE_Y;
	UINT32 histogramNum = PB_HISTOGRAM_BAR_NUM;
	UINT32 offsetX=0,offsetY = 2*APPPB_ICON_SIZE_Y+4*fontSizeY+10;
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		offsetX = 40;
		fontSizeY = 42;
		offsetY = 2*42 + 4*fontSizeY+30;
		histogramSizeX *=2;
		histogramSizeY =histogramSizeY*3/2;
		histogramNum *= 2;
	}	
	#endif

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	posX = pPageCap->frmW/2+offsetX;
	posY = pPageCap->frmH - offsetY;

	if (bShow==FALSE) 
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, posX, posY, histogramSizeX, histogramSizeY, 0, 0);
		sp5kFree(pPageCap);
		return;
	}

	 //bar_width = PB_HISTOGRAM_RECT_WIDTH / bBinNum;
	bar_width = (histogramSizeX ) / histogramNum;
	UINT32 Luma[histogramSizeX];
	if (pLuma==NULL) 
	{
		pLuma = Luma;
		if (appPbHistogramGet(pLuma,histogramSizeX,histogramSizeY)!=SUCCESS)
		{
			sp5kFree(pPageCap);
			return;
		}
	}

	bar_width = (histogramSizeX ) / histogramNum;
	appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_75, PAL_GRAY_2, PAL_BLEND_75);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, posX, posY, histogramSizeX, histogramSizeY, 0, 0);

	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);

	for(i = 0; i < histogramNum; i++)
	{
		if(*pLuma > posY)
		{
			*pLuma = posY;
		}
		bar_Hight = posY-(histogramSizeY - *pLuma);
		if (bar_Hight !=0)
		{
			//OSDLib_DrawShape(OSDLIB_SHAPE_RECT, x + i*bar_width,
			//	y + PB_HISTOGRAM_SHAPE_HEIGHT - bar_Hight,
			//	bar_width, bar_Hight, 0, 0);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, posX  + (i * bar_width), posY + histogramSizeY  - *pLuma, bar_width, *pLuma, 0, 0);
		}
		pLuma++;
	}
	sp5kFree(pPageCap);

}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoCntrlDraw			Brian added
 *------------------------------------------------------------------------*/

void
appPlayOsd_MenuIconDraw(
	UINT8 bShow
)
{
	UINT16 x,y;
	UINT32 iconSizeX=APPPB_ICON_SIZE_X;
	UINT32 iconSizeY=APPPB_ICON_SIZE_Y;
	UINT32 fontWidth=OSD_FONT_CONST_WIDTH;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		iconSizeX=48;
		iconSizeY=42;
		fontWidth=20;
	}
	#endif

	appOsdIconPosGet(ID_ICON_BUTTON_MENU,&x,&y);
	
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, MENU_ICON_X, MENU_ICON_Y - APPPB_ICON_SIZE_Y + 3, APPPB_ICON_SIZE_X + (OSD_FONT_CONST_WIDTH * 3), APPPB_ICON_SIZE_Y, 0, 0);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y, iconSizeX + (fontWidth * 3), iconSizeY, 0, 0);
	if (bShow)
	{
		//appOsdLib_OsdLayIconDraw(MENU_ICON_X, MENU_ICON_Y, SP5K_GFX_ALIGN_BOTTOM_LEFT, ID_ICON_BUTTON_MENU);
		//appOsdLib_TextIDDraw(MENU_ICON_X + APPPB_ICON_SIZE_X + OSD_FONT_CONST_WIDTH/3, MENU_ICON_Y + 4, SP5K_GFX_ALIGN_BOTTOM_LEFT, ID_STR_BACK);
		appIconShow(ID_ICON_BUTTON_MENU,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
		appOsdLib_TextIDDraw(x + iconSizeX + fontWidth/3, y, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoCntrlDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_VoiceMemoCntrlDraw(
	UINT8 bShow,
	UINT8 isPlay
)
{
	//appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X, INDICATE_ARROW_UP_ICON_Y, ID_ICON_BUTTON_UP);
	#ifdef HW_DVS5M2
	///appIconShow(ID_ICON_BUTTON_OK,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
	#else
	appIconShow(ID_ICON_BUTTON_UP,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
	#endif
	if (isPlay)
	{
		//appPlayOsd_IconDraw(bShow, INDICATE_PLAY_ICON_X - 8, INDICATE_PLAY_ICON_Y, ID_ICON_VIDEOPLAY_START);
		appIconShow(ID_ICON_VIDEOPLAY_PAUSE,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
		appIconShow(ID_ICON_VIDEOPLAY_START,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
	}
	else
	{
		//appPlayOsd_IconDraw(bShow, INDICATE_PLAY_ICON_X - 8, INDICATE_PLAY_ICON_Y, ID_ICON_VIDEOPLAY_PAUSE);
		appIconShow(ID_ICON_VIDEOPLAY_START,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);	
		appIconShow(ID_ICON_VIDEOPLAY_PAUSE,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
	}

}
void
appPlayOsd_VideoTrimCntrlDraw(
	UINT8 bShow,
	UINT8 item
)
{
	//appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X-20, INDICATE_ARROW_UP_ICON_Y, ID_ICON_BUTTON_UP);
	appIconShow(ID_ICON_BUTTON_UP,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
	//appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X+60, INDICATE_ARROW_UP_ICON_Y, ID_ICON_BUTTON_DOWN);
	appIconShow(ID_ICON_BUTTON_DOWN,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
/*	appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X+80, INDICATE_ARROW_UP_ICON_Y+8, ID_ICON_PLAYBACK_VIDEO_TRIM_NORMAL);
	appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X+120, INDICATE_ARROW_UP_ICON_Y+8, ID_ICON_PLAYBACK_VIDEO_TRIM_SECOND);
*/	if (item==0)
	{
		//appPlayOsd_IconDraw(bShow, INDICATE_PLAY_ICON_X-20, INDICATE_PLAY_ICON_Y, ID_ICON_BUTTON_RIGHT);
		appIconShow(ID_ICON_BUTTON_RIGHT,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
	}
/*	else if (item==1)
	{
		appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X+80, INDICATE_PLAY_ICON_Y+8, ID_ICON_PLAYBACK_VIDEO_TRIM_SEL_NOR);
	}
	else if (item==2)
	{
		appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X+120, INDICATE_PLAY_ICON_Y+8, ID_ICON_PLAYBACK_VIDEO_TRIM_SEL_SEC);
	}
*/
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoCntrlDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_VideoCntrlDraw(
	UINT8 bShow,
	UINT8 isPlay,
	UINT8 bShowRWD,
	UINT8 bShowFWD
)
{
	#ifdef HW_DVS5M2
	
	UINT32 x=40,y=200,w=280,h=40;

	if(IS_HDMI_IN)
	{
		x = 70;
		y = 308;
		w = 400;
		h = 50;
	}

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y, w, h, 0, 0);
	appPlayOsd_ShowTimeInCenter(FALSE,gpbImgCb.mediaTotalTime);

	appPlayOsd_VoiceMemoCntrlDraw(bShow, isPlay);

	///appIconShow(ID_ICON_BUTTON_MODE,bShowFWD,SP5K_GFX_ALIGN_TOP_LEFT);
	///appIconShow(ID_ICON_VIDEOPLAY_STOP,bShowFWD,SP5K_GFX_ALIGN_TOP_LEFT);

	#else
	
	UINT32 x=40,y=200,w=280,h=40;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN)
	{
		x = 70;
		y = 308;
		w = 400;
		h = 50;
	}
	#endif

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y, w, h, 0, 0);
	appPlayOsd_ShowTimeInCenter(FALSE,gpbImgCb.mediaTotalTime);
#if 0
	//appPlayOsd_IconDraw(bShowRWD, INDICATE_ARROW_UP_ICON_X - APPPB_ICON_SIZE_X, INDICATE_ARROW_UP_ICON_Y, ID_ICON_VIDEOPLAY_REW);
	//appPlayOsd_IconDraw(bShowRWD, INDICATE_ARROW_UP_ICON_X - APPPB_ICON_SIZE_X - 28, INDICATE_ARROW_UP_ICON_Y, ID_ICON_BUTTON_LEFT);
	appIconShow(ID_ICON_VIDEOPLAY_REW,bShowRWD,SP5K_GFX_ALIGN_TOP_LEFT);
	appIconShow(ID_ICON_BUTTON_LEFT,bShowRWD,SP5K_GFX_ALIGN_TOP_LEFT);

	appPlayOsd_VoiceMemoCntrlDraw(bShow, isPlay);

	//appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X-4, INDICATE_ARROW_UP_ICON_Y, ID_ICON_BUTTON_RIGHT);
	//appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X + 24, INDICATE_ARROW_UP_ICON_Y, ID_ICON_VIDEOPLAY_FF);
	//appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*3, INDICATE_ARROW_UP_ICON_Y - 3, ID_ICON_BUTTON_MENU);
	//appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*4, INDICATE_PLAY_ICON_Y, ID_ICON_VIDEOPLAY_STOP);

	appIconShow(ID_ICON_BUTTON_RIGHT,bShowFWD,SP5K_GFX_ALIGN_TOP_LEFT);
	appIconShow(ID_ICON_VIDEOPLAY_FF,bShowFWD,SP5K_GFX_ALIGN_TOP_LEFT);
	appIconShow(ID_ICON_BUTTON_MENU,bShowFWD,SP5K_GFX_ALIGN_TOP_LEFT);
	appIconShow(ID_ICON_VIDEOPLAY_STOP,bShowFWD,SP5K_GFX_ALIGN_TOP_LEFT);
#endif
	
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_MediaPlayStatusDraw			Brian added
 *------------------------------------------------------------------------*/

void
appPlayOsd_MediaPlayStatusDraw(
	UINT8 bShow,
	UINT16 MediaPlayStatus
)
{
	UINT16 x,y;
	UINT32 iconSizeX = PB_PLAY_MEDIA_STATUS_ICON_SIZE_X;
	UINT32 iconSizeY = PB_PLAY_MEDIA_STATUS_ICON_SIZE_Y;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		iconSizeX = 48;
		iconSizeY = 42;
	}
	#endif
	
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, PB_PLAY_MEDIA_STATUS_ICON_SIZE_X+16, PB_PLAY_MEDIA_STATUS_ICON_SIZE_Y, 0, 0);
	appOsdIconPosGet(ID_ICON_PLAY_STS_PAUSE,&x,&y);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y, iconSizeX+38/*16*/, iconSizeY+8, 0, 0); /*modify for mantis bug 0047359*/
	if (bShow)
	{
		switch (MediaPlayStatus)
		{
			case PLAYING:
				//appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_PLAY);
				appIconShow(ID_ICON_PLAY_STS_PLAY,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case PAUSING:
				//appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_PAUSE);
				appIconShow(ID_ICON_PLAY_STS_PAUSE,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case FWDING:
				//appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_FF);
				appIconShow(ID_ICON_PLAY_STS_FF,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case RWDING:
				//appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_REW);
				appIconShow(ID_ICON_PLAY_STS_REW,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
				break;
			case SLOW :
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextDraw( x , y+16 , SP5K_GFX_ALIGN_TOP_LEFT, "SLOW");
				break;
		}
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressBarFrame			Brian added
 *------------------------------------------------------------------------*/

void
appPlayOsd_ProgressBarFrameDraw(
	UINT8 bShow
)
{
	UINT32 iconPosX = PB_VOICE_MEMO_PROG_FRAME_CENT_X,iconPosY = PB_VOICE_MEMO_PROG_FRAME_CENT_Y;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		iconPosX = 257; 
		iconPosY = 170;
	}
	#endif
	
	PBLastProgBarLength = 0;

	if(bShow)
	{
		appOsd_FontIconFontSet(ID_ICON_WARNMEDIASTANDBY);

		//Ethan need to check SP5K_GFX_PAGE_OSD_0+1 --NG
		#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			sp5kGfxAttrSet(SP5K_GFX_IDX8_PIC_SCALE_FACTOR,1500,1200,0,0);
		}
		#endif
		sp5kGfxIconDraw(SP5K_GFX_PAGE_OSD_0, iconPosX, iconPosY,SP5K_GFX_ALIGN_CENTER, 0x0000);
		#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			sp5kGfxAttrSet(SP5K_GFX_IDX8_PIC_SCALE_FACTOR,1000,1000,0,0);
		}
		#endif
	}
	else
	{
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT, iconPosX, iconPosY, PB_VOICE_MEMO_PROG_FRAME_SIZE_X, PB_VOICE_MEMO_PROG_FRAME_SIZE_Y, 0, 0);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressBarLastTimeReset
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProgressBarLastTimeReset(
		void
)
{
	PBLastProgBarLength =0;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressBarDraw			Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProgressBarDraw(
		UINT32 CurrentTime_msec,
		UINT32 TotalTime_msec
)
{
	UINT32 barX=PB_VOICE_MEMO_BAR_X,barY=PB_VOICE_MEMO_BAR_Y;
	UINT32 barW=PB_VOICE_MEMO_BAR_SIZE_X,barH=PB_VOICE_MEMO_BAR_SIZE_Y;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		barX = 193;
		barY = 224;
		barW = barW*3/2;
		barH = barH*6/5 + 1;
	}
	#endif

	if (CurrentTime_msec > TotalTime_msec)
	{
		CurrentTime_msec = TotalTime_msec;
	}

	UINT32 CurrentProgBarLength = ((float)CurrentTime_msec / (float)TotalTime_msec) * (float)barW;
	if(PBLastProgBarLength <= CurrentProgBarLength)
	{
		appOsd_ColorDrawShapeColorSet(PAL_YELLOW, PAL_BLEND_100, PAL_YELLOW, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, barX + PBLastProgBarLength, barY, CurrentProgBarLength - PBLastProgBarLength, barH, 0, 0);
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_2, PAL_BLEND_100, PAL_GRAY_2, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, barX + CurrentProgBarLength , barY, PBLastProgBarLength - CurrentProgBarLength, barH, 0, 0);
	}
	PBLastProgBarLength = CurrentProgBarLength;
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressTimeDraw			Brian added
 *------------------------------------------------------------------------*/

void
appPlayOsd_ProgressTimeDraw(
	UINT8 bShow,
	UINT32 ProgressTime_msec,
	UINT32 X,
	UINT32 Y
)
{
	UINT32 fontSizeX = ALPHABET_FONT_SIZE_X,fontSizeY = ALPHABET_FONT_SIZE_Y;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		fontSizeX = 20;
		fontSizeY = 32;
	}
	#endif
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	if(IS_HDMI_IN)
       {

                   appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, X/*-30*/, Y-20, fontSizeX * 10, fontSizeY + 4, 0, 0);
        }
       else
       {
                   appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, X/*-30*/, Y, fontSizeX * 10, fontSizeY, 0, 0);
       }
	if (bShow)
	{

		UINT32 ProgressTime = (ProgressTime_msec / 1000);

		/*if(!ProgressTime && ProgressTime_msec != 0)
			ProgressTime = 1;*/

		UINT32  SS =  (ProgressTime % 3600) % 60;
		UINT32  MM =  (ProgressTime / 60) % 60;
		UINT32  HH =  ProgressTime / 3600;
		unsigned char  TimeString[9];
		memset(TimeString, 0, 9);
		sprintf(TimeString, "%02d:%02d:%02d", HH, MM, SS);

		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		if(IS_HDMI_IN)
		{
			appOsdLib_TextDraw( X , Y-20 , SP5K_GFX_ALIGN_TOP_LEFT, TimeString);
		}
		else
		{
			appOsdLib_TextDraw( X , Y , SP5K_GFX_ALIGN_TOP_LEFT, TimeString);
		}
	}
}


void appPlayOsd_ShowTimeInCenter(UINT8 bShow,UINT32 CurrentTime_msec)
{
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	#if CAM_TYPE_CVR
	UINT32 x=15,y=150;
	#else
	UINT32 x=PB_VOICE_MEMO_TIME_X,y=PB_VOICE_MEMO_TIME_Y;
	#endif
	UINT32 fontSizeX = ALPHABET_FONT_SIZE_X,fontSizeY = ALPHABET_FONT_SIZE_Y;

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		fontSizeX = 75;
		fontSizeY = 2;
		x = (pPageCap->frmW/2 - (4*fontSizeX)+40);
		y = (pPageCap->frmH/2 + pPageCap->frmH/4-30);;
	}
	#endif
	
	appPlayOsd_ProgressTimeDraw(bShow, CurrentTime_msec, x, y);

	sp5kFree(pPageCap);
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_seamlessPlayIndexShow			juanjuan added
 *  descripe: show seamless play file index
 *------------------------------------------------------------------------*/
void
appPlayOsd_seamlessPlayIndexShow(
	BOOL bShow
)
{
#if CAM_TYPE_CVR
	UINT8 FileNumString[64];
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	UINT32 x=PB_VIDEO_SEAMPLAY_IDX_X,y=PB_VIDEO_SEAMPLAY_IDX_Y,fontSizeX = ALPHABET_FONT_SIZE_X;
	
	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		fontSizeX = 20;
		x = 2*fontSizeX;
		y = 10;
	}
	#endif
	/*show curr play indx and total indx*/
	APP_OSD_REFRESH_OFF;
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x, y, 10*fontSizeX, ALPHABET_FONT_SIZE_Y, 0, 0);
	if(bShow)
	{
		memset(FileNumString, 0, 10);
		sprintf(FileNumString, "%d/%d ", gpbImgCb.dcfIdx,gpbMainCb.totalImg);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(x, y, SP5K_GFX_ALIGN_TOP_LEFT, FileNumString);
	}
	APP_OSD_REFRESH_ON;
#endif
	/*show curr play indx and total indx*/
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VideoTimeDraw			Brian added
 *------------------------------------------------------------------------*/

void
appPlayOsd_VideoTimeDraw(
		UINT8 bShow,
		UINT32 CurrentTime_msec
)
{
	sp5kGfxPageCapabDesc_t *pPageCap = NULL;
	UINT32 x=PB_VIDEO_TIME_X,y=PB_VIDEO_TIME_Y,fontSizeX = ALPHABET_FONT_SIZE_X;

	if(!pPageCap){
		pPageCap = sp5kMalloc(sizeof(sp5kGfxPageCapabDesc_t));
	}
	if(!pPageCap){
		return;
	}
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,pPageCap);

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		fontSizeX = 20;
		x = (pPageCap->frmW - (8*fontSizeX));
		y = 10;

		//for DVS5M2
		x -= 50;
		y += 18;
	}
	#endif
	
	appPlayOsd_ProgressTimeDraw(bShow, CurrentTime_msec, x, y);
	sp5kFree(pPageCap);
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ThumbnailIconDraw			Brian
 *------------------------------------------------------------------------*/

void
appPlayOsd_ThumbnailIconDraw(
	UINT8 bShow,
	UINT32 type,
	UINT32 X,
	UINT32 Y
)
{
	UINT16 MultiPlaybackIcon[] = {
		[MPLAYBACK_PROTECTED		] = ID_ICON_PROTECT,
		[MPLAYBACK_UNPROTECTED		] = ID_ICON_PROTECT,
		[MPLAYBACK_NEWPROTECTED 	] = ID_ICON_PROTECT,
		[MPLAYBACK_DELETE			] = ID_ICON_DELETE,
		[MPLAYBACK_VOICE_MEMO		] = ID_ICON_MODE_VOICE,
		[MPLAYBACK_VIDEO			] = ID_ICON_MODE_VIDEO
	};

	UINT32 x1, y1,xHDMI,yHDMI;
	UINT32 iconSizeX,iconSizeY;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		iconSizeX = 48;
		iconSizeY = 42;
		xHDMI = 48-32;
		yHDMI = 32-28;
	}else{
	#else
		{
	#endif
		iconSizeX = APPPB_ICON_SIZE_X;
		iconSizeY = APPPB_ICON_SIZE_Y;
		xHDMI = 0;
		yHDMI = 0;
	}
	
	switch(type)
	{
		case MPLAYBACK_PROTECTED:
		case MPLAYBACK_UNPROTECTED:
		case MPLAYBACK_NEWPROTECTED:
			x1 = X + MPB_PROTECT_ICON_X + xHDMI;
			y1 = Y + MPB_PROTECT_ICON_Y + yHDMI;
			break;
		case MPLAYBACK_DELETE:
			x1 = X + MPB_DELETE_ICON_X;
			y1 = Y + MPB_DELETE_ICON_Y;
			break;
		case MPLAYBACK_VOICE_MEMO:
		case MPLAYBACK_VIDEO:
			x1 = X + MPB_VOICE_MEMO_ICON_X;
			y1 = Y + MPB_VOICE_MEMO_ICON_Y + yHDMI;
			break;
		default:
			x1 = y1 = 0;
			break;
	}

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x1, y1, iconSizeX, iconSizeY, 0, 0);

	if (bShow)
	{
		appOsdLib_OsdLayIconDraw(x1, y1, SP5K_GFX_ALIGN_TOP_LEFT, MultiPlaybackIcon[type]);
	}

}
/*-------------------------------------------------------------------------
 *	File Name : appPlayOsd_DigitalZoomWinDraw					Brian added
 *------------------------------------------------------------------------*/
BOOL doFirstTime=TRUE;		/* Modify by sc.liu */

void appPlayOsd_DigitalZoomWinDraw
(
	SINT32 CropImageCentX,
	SINT32 CropImageCentY,
	UINT32 ImageWidth,
	UINT32 ImageHeight,
	UINT32 ZoomFactor,
	UINT8 ImageAspRatio
)
{
	UINT32 DZoom_Pan_W;
	UINT32 DZoom_Pan_H;
	unsigned char  AMPString[5];
/* Modify by sc.liu */
	static UINT32 DZoomCenX;
	static UINT32 DZoomCenY;
	static UINT32 DZoom_Crop_X;
	static UINT32 DZoom_Crop_Y;
/* Modify by sc.liu */
	
	switch(ImageAspRatio)
	{
		case PB_IMG_ORIENT_16X9:
			DZoom_Pan_W = 67;
			DZoom_Pan_H	= 39;
			break;
		case PB_IMG_ORIENT_3X2:
			DZoom_Pan_W = 67;
			DZoom_Pan_H	= 45;
			break;
		case PB_IMG_ORIENT_4X3:
			DZoom_Pan_W = 67;
			DZoom_Pan_H	= 51;
			break;
		case PB_IMG_ORIENT_9X16:
			DZoom_Pan_W = 39;
			DZoom_Pan_H	= 67;
			break;
		case PB_IMG_ORIENT_2X3:
			DZoom_Pan_W = 45;
			DZoom_Pan_H	= 67;
			break;
		case PB_IMG_ORIENT_3X4:
			DZoom_Pan_W = 51;
			DZoom_Pan_H	= 67;
			break;
		case PB_IMG_ORIENT_UNKNOWN:
		default:
			if((ImageWidth / MAX_ZOOM_PAN_W_H) >= (ImageHeight / MAX_ZOOM_PAN_W_H))
			{
				DZoom_Pan_W = ImageWidth  / (ImageWidth /MAX_ZOOM_PAN_W_H);
				DZoom_Pan_H = ImageHeight / (ImageWidth /MAX_ZOOM_PAN_W_H);
			}
			else
			{
				DZoom_Pan_W = ImageWidth  / (ImageHeight /MAX_ZOOM_PAN_W_H);
				DZoom_Pan_H = ImageHeight / (ImageHeight /MAX_ZOOM_PAN_W_H);
			}

			if(DZoom_Pan_W % 2 == 0)
			{
				DZoom_Pan_W = DZoom_Pan_W + 1;
			}
			
			if(DZoom_Pan_H % 2 == 0)
			{
				DZoom_Pan_H = DZoom_Pan_H + 1;
			}

			break;
	}


	int Ratio = (ImageWidth /DZoom_Pan_W);
	
	UINT32 DZoom_Pan_X = CENT_DZOOM_PAN_X - (DZoom_Pan_W)/2.0;
	UINT32 DZoom_Pan_Y = CENT_DZOOM_PAN_Y - (DZoom_Pan_H)/2.0;

	UINT32 AMP_X = DZoom_Pan_X-10;
	UINT32 AMP_Y = DZoom_Pan_Y - 33;

	UINT32 DZoom_Crop_W = (float)(DZoom_Pan_W * 1000) / ZoomFactor;
	UINT32 DZoom_Crop_H = (float)(DZoom_Pan_H * 1000) / ZoomFactor;

/* Modify by sc.liu */
	if(DZoom_Crop_W % 2 == 0)
	{
		DZoom_Crop_W = DZoom_Crop_W - 1;
	}
	
	if(DZoom_Crop_H % 2 == 0)
	{
		DZoom_Crop_H = DZoom_Crop_H - 1;
	}

	if(doFirstTime)
	{
		DZoom_Crop_X = ((float)((float)CropImageCentX * ZoomFactor - (float)(ImageWidth/2.0*1000)))/Ratio/(float)ZoomFactor  ;
		DZoom_Crop_X = DZoom_Crop_X + DZoom_Pan_X + ((DZoom_Crop_X%2)? 0:1);
		DZoomCenX = DZoom_Crop_X + DZoom_Crop_W/2;
		
		DZoom_Crop_Y = ((float)((float)CropImageCentY * ZoomFactor - (float)(ImageHeight/2.0*1000)))/Ratio/(float)ZoomFactor;
		DZoom_Crop_Y = DZoom_Crop_Y + DZoom_Pan_Y + ((DZoom_Crop_Y%2)? 0:1);
		DZoomCenY = DZoom_Crop_Y + DZoom_Crop_H/2;
		doFirstTime = FALSE;
	}
	//Detecting whether reach the border or not after zooming

	DZoom_Crop_X = DZoomCenX - DZoom_Crop_W/2;
	DZoom_Crop_Y = DZoomCenY - DZoom_Crop_H/2;
/* Modify by sc.liu */
	if(DZoom_Crop_Y < DZoom_Pan_Y)
	{
		DZoom_Crop_Y = DZoom_Pan_Y;
	}
	
	if(DZoom_Crop_Y + DZoom_Crop_H > DZoom_Pan_Y + DZoom_Pan_H )
	{
		DZoom_Crop_Y = DZoom_Pan_Y + DZoom_Pan_H - DZoom_Crop_H;
	}
	
	if(DZoom_Crop_X < DZoom_Pan_X)
	{
		DZoom_Crop_X = DZoom_Pan_X;
	}
	
	if(DZoom_Crop_X + DZoom_Crop_W > DZoom_Pan_X + DZoom_Pan_W)
	{
		DZoom_Crop_X = DZoom_Pan_X + DZoom_Pan_W - DZoom_Crop_W;
	}

	//Detecting whether reach the border or not after moving

	UINT32 UpBound = ((UINT32)(ImageHeight/(((float)ZoomFactor)/1000)))>>1;
	UINT32 LowBound = ImageHeight - ((UINT32)(ImageHeight/(((float)ZoomFactor)/1000))>>1) - 1;
	UINT32 LeftBound = ((UINT32)(ImageWidth/(((float)ZoomFactor)/1000)))>>1;
	UINT32 RightBound = ImageWidth - ((UINT32)(ImageWidth/(((float)ZoomFactor)/1000))>>1) - 1;

	if(CropImageCentY > (SINT32)UpBound)
	{
		//appOsdLib_OsdLayIconDraw(LCD_SIZE_X/2 - 10, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MEDIA_UP_ARROW);
		appIconShow(ID_ICON_MEDIA_UP_ARROW,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}
	else
	{
		//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, LCD_SIZE_X/2 - 10, 0, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
		appIconShow(ID_ICON_MEDIA_UP_ARROW,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
	}

	if(CropImageCentY <(SINT32)LowBound)
	{
		//appOsdLib_OsdLayIconDraw(LCD_SIZE_X/2 - 10, LCD_SIZE_Y, SP5K_GFX_ALIGN_BOTTOM_LEFT, ID_ICON_MEDIA_DOWN_ARROW);
		appIconShow(ID_ICON_MEDIA_DOWN_ARROW,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}
	else
	{
		//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, LCD_SIZE_X/2 - 10, LCD_SIZE_Y - APPPB_ICON_SIZE_Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
		appIconShow(ID_ICON_MEDIA_DOWN_ARROW,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
	}

	if(CropImageCentX > (SINT32)LeftBound)
	{
		//appOsdLib_OsdLayIconDraw(0, LCD_SIZE_Y/2 - 10, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MEDIA_LEFT_ARROW);
		appIconShow(ID_ICON_MEDIA_LEFT_ARROW,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}
	else
	{
		//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, LCD_SIZE_Y/2 - 10, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
		appIconShow(ID_ICON_MEDIA_LEFT_ARROW,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
	}

	if(CropImageCentX < (SINT32)RightBound)
	{
		//appOsdLib_OsdLayIconDraw(LCD_SIZE_X, LCD_SIZE_Y/2 - 10, SP5K_GFX_ALIGN_TOP_RIGHT, ID_ICON_MEDIA_RIGHT_ARROW);
		appIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_SHOW,SP5K_GFX_ALIGN_TOP_LEFT);
	}
	else
	{
		//appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, LCD_SIZE_X - APPPB_ICON_SIZE_X, LCD_SIZE_Y/2 - 10, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
		appIconShow(ID_ICON_MEDIA_RIGHT_ARROW,ICON_CLEAN,SP5K_GFX_ALIGN_TOP_LEFT);
	}


	appOsd_ColorDrawShapeColorSet(PAL_BLACK_1, PAL_BLEND_75, PAL_GRAY, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DZoom_Pan_X, DZoom_Pan_Y, DZoom_Pan_W, DZoom_Pan_H, 0, 0);
	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DZoom_Crop_X, DZoom_Crop_Y, DZoom_Crop_W, DZoom_Crop_H, 0, 0);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, AMP_X, AMP_Y, ALPHABET_FONT_SIZE_X * 6, ALPHABET_FONT_SIZE_Y, 0, 0);
	memset(AMPString, 0, 5);
	sprintf(AMPString,"%d.%02dx",ZoomFactor / 1000, ZoomFactor /10  - (ZoomFactor/1000)*100);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(AMP_X, AMP_Y, SP5K_GFX_ALIGN_TOP_LEFT, AMPString);

}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ModeIconDraw					Brian added
 *------------------------------------------------------------------------*/
void
appPlayOsd_ModeIconDraw
(
	BOOL bShow
)
{
	//appPlayOsd_IconDraw(bShow, PLAYBACK_ICON_X, PLAYBACK_ICON_Y, ID_ICON_MODE_PLAYBACK);
	appIconShow(ID_ICON_MODE_PLAYBACK,bShow,SP5K_GFX_ALIGN_TOP_LEFT);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_AudioBGDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_AudioBGDraw(
	void
)
{
	sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	#if (KIT_WITH_HDMI)
 	if(IS_HDMI_IN){
		UINT32 w = 1920, h = 1080;
		/*UINT32 arr[4] = {0,0, w, h};*/	/*nan.yang 20121108 for clear warning*/
		appModeSet(SP5K_MODE_STANDBY);
		appJpgDraw_ImgLayer(RES_JPG_AUDIO_BG, 0,0,w,h);
		appOsdLib_PIPLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, w, h, 0, 0);
/*		sp5kResourceFilePagePlay(SP5K_RES_TYPE_JPG,RES_JPG_AUDIO_BG , SP5K_GFX_PAGE_PIP_BASE+0, arr, 0);
		sp5kGfxAttrSet(SP5K_GFX_PIP_REFRESH_ACTIVE,1,0,0,0);	*/
	}else
 	#endif
	{
		appModeSet(SP5K_MODE_STANDBY);
		appJpgDraw_ImgLayer(RES_JPG_AUDIO_BG, 0, 0, 320, 240);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_OSDInfo_FastDraw
 *------------------------------------------------------------------------*/
void appPlayOsd_OSDInfo_FastDraw(
	pbMainCB_t *psPBImage,
	pbImgExifInfo_t *psPBCurImgExif,
	pbImgCb_t *pimgCb
)
{
	/*-------------------------------------------------------------
	 * To invoke this function, HOST shall guarantee the OSD have
	 * been clear completely. FastDraw wont clear any ICON/STRINGs
	 * but just draw what are necessary !!
	 *-------------------------------------------------------------
	 */
	UINT8 Lastfiletype;
	uiPara_t* puiPara = appUiParaGet();

	Lastfiletype = psPBImage->lastFileType;
	switch(psPBImage->osdType) 
	{
	case PB_DISP_FULL:
		switch(pimgCb->fileType)
		{
			case PB_FT_WAV:
				break;
			case PB_FT_JPG_MEMO:
				appPlayOsd_VoiceMemoDraw();
				appPlayOsd_DateTimeDraw(TRUE,puiPara->DateStyle,&pimgCb->dcfAttr.time);
				appPlayOsd_PicExifInfoDraw(TRUE,psPBCurImgExif,pimgCb);
				break;
			case PB_FT_JPG:
				appPlayOsd_DateTimeDraw(TRUE,puiPara->DateStyle,&pimgCb->dcfAttr.time);
				appPlayOsd_ResolutionDraw(TRUE,pimgCb->sizeGrade);
				appPlayOsd_PicExifInfoDraw(TRUE,psPBCurImgExif,pimgCb);
				appPlayOsd_HistogramDraw(TRUE, pimgCb->pHistBuf	);
				break;
			case PB_FT_MOV:
			case PB_FT_AVI:
				break;
			default:
				break;

		}
		appPlayOsd_FileNumDraw(TRUE,psPBImage->curImgIdx,psPBImage->totalImg);
		appPlayOsd_ModeIconDraw(TRUE);
		appPlayOsd_ProtectDraw(pimgCb->isProtected);
		appPlayOsd_DPOFDraw(pimgCb->isDpofSet);
		break;

	case PB_DISP_BASIC:
		switch(pimgCb->fileType)
		{
			case PB_FT_WAV:
				appPLayOsd_AudioPlayDraw(TRUE);
				break;
			case PB_FT_JPG_MEMO:
				appPlayOsd_VoiceMemoDraw();
				appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
				break;
			case PB_FT_JPG:		
				break;
			case PB_FT_MOV:
			case PB_FT_AVI:
			case PB_FT_MP4:
				appPlayOsd_VideoTrimCntrlDraw(TRUE,trimCount);
				appPlayOsd_VideoModeDraw(TRUE);
				break;
			default:
				break;
		}
		appPlayOsd_FileNumDraw(TRUE,psPBImage->curImgIdx,psPBImage->totalImg);
		appPlayOsd_ModeIconDraw(TRUE);
		appPlayOsd_ProtectDraw(pimgCb->isProtected);
		appPlayOsd_DPOFDraw(pimgCb->isDpofSet);
		break;

	case PB_DISP_NONE:
		switch(pimgCb->fileType)
		{
			case PB_FT_WAV:
				appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
				break;
			case PB_FT_JPG_MEMO:
				appPlayOsd_VoiceMemoDraw();
				appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
				break;
			case PB_FT_JPG:
				break;
			case PB_FT_MOV:
			case PB_FT_AVI:
			case PB_FT_MP4:
				appPlayOsd_VideoTrimCntrlDraw(TRUE,trimCount);
				appPlayOsd_VideoModeDraw(TRUE);
				break;
			default:
				break;
		}
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_MainInfoDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_MainInfoDraw(
	UINT32 osdDrawType
)
{
	PB_VERBOSE("\n*************************");
	PB_VERBOSE("\nMAIN_INFO_DRAW: osdType(%d) ...\n", gpbMainCb.osdType);
	PB_VERBOSE("*************************\n");

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		appOsdIconModeSet(APP_ICON_MODE_PLAYBACK_HDMI);
	}else{
		appOsdIconModeSet(APP_ICON_MODE_PLAYBACK);
	}
	#else
	appOsdIconModeSet(APP_ICON_MODE_PLAYBACK);
	#endif
	
	appOsd_FontIconFontSet(ID_FONT_ICON);

	if (gpbMainCb.isNoImage || gpbDiskCb.isDiskErr || gpbImgCb.isFileErr)
	{
		APP_OSD_REFRESH_ON;
		return;
	}

	APP_OSD_REFRESH_OFF;
	if (osdDrawType==PB_OSD_DRAW_FAST) 
	{
		appPlayOsd_OSDInfo_FastDraw(&gpbMainCb,&gpbExifInfo,&gpbImgCb);
	}
	else
	{
		appPlayOsd_OSDInfoDraw(&gpbMainCb,&gpbExifInfo,&gpbImgCb);
	}
	APP_OSD_REFRESH_ON;
	PB_LOG("MainInfoDraw done \n");
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_OSDInfoDraw
 *------------------------------------------------------------------------*/

void appPlayOsd_OSDInfoDraw(pbMainCB_t *psPBImage, pbImgExifInfo_t *psPBCurImgExif,	pbImgCb_t *pimgCb)
{
	UINT8 Lastfiletype;
	Lastfiletype = psPBImage->lastFileType;
	uiPara_t* puiPara = appUiParaGet();

	//CLEAR_OSD_SRC;
	//CLEAR_PIP_SRC;

	switch(psPBImage->osdType)
	{
		case PB_DISP_FULL:
			switch(pimgCb->fileType)
			{
				case PB_FT_WAV:
					break;
				case PB_FT_JPG_MEMO:
				case PB_FT_JPG:
					appPlayOsd_DateTimeDraw(TRUE,puiPara->DateStyle,&pimgCb->dcfAttr.time);
					#if CAM_TYPE_CVR
					appPlayOsd_CurrntFileSizeDraw(TRUE,pimgCb->dcfAttr.filesize);
					appPlayOsd_FileNumDraw(TRUE,psPBImage->curImgIdx,psPBImage->totalImg);
					appPlayOsd_ProtectDraw(pimgCb->isProtected);
					if(IS_HDMI_IN)
					{
						UINT32 coordinateMenuIcon_Y, coordinateMenuString_Y;

						coordinateMenuIcon_Y=302;
						coordinateMenuString_Y=302;
	

						//appOsdLib_OsdLayIconDraw(268-20, 313, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_LEFT);						
						//appOsdLib_OsdLayIconDraw(268-20, 313, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI);						
						//appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
						//appOsdLib_TextIDDraw(309-20,310,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_BACK);//lyc@add
						appOsdLib_OsdLayIconDraw(110-50, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKPRESS_HDMI);
						appOsdLib_OsdLayIconDraw(250, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI );
						appOsdLib_OsdLayIconDraw(390+50, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKHOLD_HDMI );

						appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

						appOsdLib_TextIDDraw(155-50,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_DISPLAY);
						appOsdLib_TextIDDraw(295,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_SELECT);
						appOsdLib_TextIDDraw(435+50+5,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_MENU);						

					}
					else
					{
						appOsdLib_OsdLayIconDraw(99, 209, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_LEFT );
						appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
						appOsdLib_TextIDDraw(133,207,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_BACK);
					}
					#else
					appPlayOsd_ResolutionDraw(TRUE,pimgCb->sizeGrade);
					appPlayOsd_PicExifInfoDraw(TRUE,psPBCurImgExif,pimgCb);
					appPlayOsd_HistogramDraw(TRUE, pimgCb->pHistBuf);
					#endif
					break;
				case PB_FT_MOV:
				case PB_FT_AVI:
				case PB_FT_MP4:
	                #if !CAM_TYPE_CVR
					appPlayOsd_ShowTimeInCenter(TRUE,gpbImgCb.mediaTotalTime);
					appPlayOsd_VideoSizeDraw(TRUE,gpbImgCb.imgWidth);
					#endif
					break;
				default:
					break;
			}
			break;
		case PB_DISP_BASIC:
			switch(pimgCb->fileType)
			{
				case PB_FT_WAV:
					appPLayOsd_AudioPlayDraw(TRUE);
					break;
				case PB_FT_JPG_MEMO:
					appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
					break;
				case PB_FT_JPG:
                    #if CAM_TYPE_CVR
					if(IS_HDMI_IN)
					{
						UINT32 coordinateMenuIcon_Y, coordinateMenuString_Y;

						coordinateMenuIcon_Y=302;
						coordinateMenuString_Y=302;

						///appOsdLib_OsdLayIconDraw(268-20, 313, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_LEFT);//lyc@add
						
						//appOsdLib_OsdLayIconDraw(268-20, 313, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI);
						//appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
						//appOsdLib_TextIDDraw(309-20,310,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_BACK);

						appOsdLib_OsdLayIconDraw(110-50, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKPRESS_HDMI);
						appOsdLib_OsdLayIconDraw(250, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI );
						appOsdLib_OsdLayIconDraw(390+50, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKHOLD_HDMI );

						appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

						appOsdLib_TextIDDraw(155-50,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_DISPLAY);
						appOsdLib_TextIDDraw(295,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_SELECT);
						appOsdLib_TextIDDraw(435+50+5,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_MENU);
						
					}
					else
					{
						appOsdLib_OsdLayIconDraw(99, 209, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_LEFT );
						appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
						appOsdLib_TextIDDraw(133,207,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_BACK);
					}
                    #endif
					break;
				case PB_FT_MOV:
				case PB_FT_AVI:
				case PB_FT_MP4:
				#if CAM_TYPE_CVR
					appPlayOsd_ModeIconDraw(TRUE);
					appPlayOsd_DateTimeDraw(TRUE,puiPara->DateStyle,&pimgCb->dcfAttr.time);
					appPlayOsd_ShowTimeInCenter(TRUE,gpbImgCb.mediaTotalTime);
					appPlayOsd_VideoFileIconDrow(TRUE);
				#else
					appPlayOsd_VideoTrimCntrlDraw(TRUE,trimCount);
					appPlayOsd_VideoModeDraw(TRUE);
				#endif
					break;
				default:
					break;
			}
			appPlayOsd_FileNumDraw(TRUE,psPBImage->curImgIdx,psPBImage->totalImg);
			appPlayOsd_ProtectDraw(pimgCb->isProtected);
			#if !CAM_TYPE_CVR
			appPlayOsd_ModeIconDraw(TRUE);
			appPlayOsd_DPOFDraw(pimgCb->isDpofSet);
			appPlayOsd_VoiceMemoDraw();
			#endif
			break;
		case PB_DISP_NONE:
			CLEAR_OSD_SRC;
			/*appPLayOsd_AudioPlayDraw(FALSE);
			appPlayOsd_HistogramDraw(FALSE, pimgCb->pHistBuf );
			appPlayOsd_ProtectDraw(FALSE);
			appPlayOsd_DPOFDraw(FALSE);
			appPlayOsd_ModeIconDraw(FALSE);
			appPlayOsd_DateTimeDraw(FALSE,puiPara->DateStyle,&pimgCb->dcfAttr.time);
			appPlayOsd_FileNumDraw(FALSE,psPBImage->curImgIdx,psPBImage->totalImg);
			appPlayOsd_ResolutionDraw(FALSE,pimgCb->sizeGrade);
			appPlayOsd_PicExifInfoDraw(FALSE,psPBCurImgExif,pimgCb);
			switch(pimgCb->fileType)
			{
				case PB_FT_WAV:
					appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
					break;
				case PB_FT_JPG_MEMO:
					appPlayOsd_VoiceMemoDraw(TRUE);
					appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
					break;
				case PB_FT_JPG:
					appPlayOsd_VoiceMemoDraw(void);
					break;
				case PB_FT_AVI:
					appPlayOsd_VideoTrimCntrlDraw(TRUE,TRUE);
					appPlayOsd_VideoModeDraw(TRUE);
					break;
				default:
					break;

			}
			break;*/
		default:
			break;
	}
}

/*-------------------------------------------------------------------------
*	File Name : appPlayOsd_PicExifInfoDraw
*------------------------------------------------------------------------*/
void appPlayOsd_PicExifInfoDraw(BOOL bShow, PpbImgExigInfo_t pCurImgExif,pbImgCb_t *pimgCb)
{
	pbImgExifInfo_t tmpExifInfo;

	if (pCurImgExif == NULL) 
	{
		pCurImgExif = &tmpExifInfo;
	}
	appPlayOsd_WhiteBalanceDraw(bShow, pCurImgExif->wbMode,pCurImgExif->lightSource);
	appPlayOsd_ExposureDraw(bShow, pCurImgExif->expBias[0]);
	appPlayOsd_ISODraw(bShow, pCurImgExif->iso);
	appPlayOsd_StrobeModeDraw(bShow, pCurImgExif->flash);
	appPlayOsd_MeteringModeDraw(bShow, pCurImgExif->meteringMode);
	appPlayOsd_CurrntExpFNumDraw(bShow,pCurImgExif);
	appPlayOSD_JPGImageIconDraw(bShow,pCurImgExif);
	appPlayOsd_CurrntFileNameDraw(bShow,pimgCb->dcfAttr.filename);
	appPlayOsd_CurrntFileSizeDraw(bShow,pimgCb->dcfAttr.filesize);
}
/*-------------------------------------------------------------------------
*	File Name : appPLayOsd_MemoRecOsdDraw
*------------------------------------------------------------------------*/
void appPLayOsd_MemoRecOsdDraw(BOOL bShow)
{
	UINT32 mediaStat;

	if(!bShow)
	{
		//appPlayOsd_VoiceMemoCntrlDraw(bShow,TRUE);
		appPlayOsd_ShowTimeInCenter(bShow,gpbImgCb.mediaTotalTime);
		appPlayOsd_ProgressBarFrameDraw(bShow);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_AUDIO_STRING_X-(PB_AUDIO_STRING_W/2), PB_AUDIO_STRING_Y, PB_AUDIO_STRING_W, 30, 0, 0);
		return;
	}
	appPlayOsd_ProgressBarFrameDraw(bShow);
	//if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_PLAYING){
	//	appPlayOsd_MediaPlayStatusDraw(FALSE,PAUSING);
	//}
	appPlayOsd_ShowTimeInCenter(bShow,gpbImgCb.mediaTotalTime);

	appOsd_FontIconFontSet(ID_FONT_ICON);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);

	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_AUDIO_STRING_X-(PB_AUDIO_STRING_W/2), PB_AUDIO_STRING_Y, PB_AUDIO_STRING_W, 30, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	mediaStat = appPbMediaUIStateGet();

	if(mediaStat==PB_MEDIA_UI_STAT_RECORDING){
		appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP, ID_STR_RECORDING_MEMO___);
		//appPlayOsd_VoiceMemoCntrlDraw(TRUE,FALSE);
		//appPlayOsd_MenuIconDraw(TRUE);
	}  else if (mediaStat==PB_MEDIA_UI_STAT_STANDBY){
		appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP, ID_STR_READY_TO_RECORD);
		//appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
		//appPlayOsd_MenuIconDraw(FALSE);
	}
}


/*-------------------------------------------------------------------------
*	File Name : appPLayOsd_AudioPauseResumeDraw
*------------------------------------------------------------------------*/
void appPLayOsd_AudioPauseResumeDraw(BOOL isPause)
{
	UINT32 strX = PB_AUDIO_STRING_X,strY = PB_AUDIO_STRING_Y;
	UINT32 strW=PB_AUDIO_STRING_W,strH=32;

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		strX = strX*2;
		strY = strY*3/2;
		strW = 260;
		strH = 42;
	}
	#endif
	
	appOsd_FontIconFontSet(ID_FONT_ICON);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, strX-(strW/2), strY, strW, strH, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	/*if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_PLAYING)
	{
		appPlayOsd_MediaPlayStatusDraw(FALSE,PAUSING);
	}*/
	
	appPlayOsd_ShowTimeInCenter(TRUE, gpbImgCb.mediaElapTime);

	if (isPause)
	{
		/*-- PAUSE */
		appOsdLib_TextIDDraw(strX,strY,SP5K_GFX_ALIGN_TOP,ID_STR_PAUSE);
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
		appPlayOsd_MenuIconDraw(TRUE);
		//appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\PB.SFN", 0x0002);
	} 
	else 
	{
		/*-- RESUME */
		appOsdLib_TextIDDraw(strX,strY,SP5K_GFX_ALIGN_TOP,ID_STR_PLAYING_AUDIO___);
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,FALSE);
		appPlayOsd_MenuIconDraw(TRUE);
	}

}

/*-------------------------------------------------------------------------
*	File Name : appPLayOsd_AudioPlayDraw
*------------------------------------------------------------------------*/
void appPLayOsd_AudioPlayDraw(BOOL bShow)
{
	UINT32 strX = PB_AUDIO_STRING_X,strY = PB_AUDIO_STRING_Y;
	UINT32 strW=PB_AUDIO_STRING_W,strH=30;
	
	#if KIT_WITH_HDMI
		if(IS_HDMI_IN){
			strX = strX*2;
			strY = strY*3/2;
			strW = 260;
			strH = 42;
		}
	#endif

	appPlayOsd_ProgressBarFrameDraw(bShow);
	
	/*if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_PLAYING)
	{
		appPlayOsd_MediaPlayStatusDraw(FALSE,PAUSING);
	}*/

	appPlayOsd_ShowTimeInCenter(bShow,gpbImgCb.mediaTotalTime);

	appOsd_FontIconFontSet(ID_FONT_ICON);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, strX-(strW/2), strY, strW, strH, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_PLAYING)
	{
		appOsdLib_TextIDDraw(strX,strY,SP5K_GFX_ALIGN_TOP,ID_STR_PLAYING_AUDIO___);
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,FALSE);
		appPlayOsd_MenuIconDraw(TRUE);
	}
	else if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_PAUSE)
	{
		appOsdLib_TextIDDraw(strX,strY,SP5K_GFX_ALIGN_TOP,ID_STR_PAUSE);
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
		appPlayOsd_MenuIconDraw(TRUE);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\PB.SFN", 0x0002);/*willow modify for reduce size@20100602*/
	}
	else if (appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_STANDBY)
	{
		appOsdLib_TextIDDraw(strX,strY,SP5K_GFX_ALIGN_TOP,ID_STR_READY_TO_PLAY);
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
		appPlayOsd_MenuIconDraw(FALSE);
	}
	else
	{
		appPlayOsd_VoiceMemoCntrlDraw(bShow,TRUE);
	}
}

/*-------------------------------------------------------------------------
*	 File Name : appPLayOsd_LeftRightArrow
*------------------------------------------------------------------------*/
void appPLayOsd_LeftRightArrow(BOOL bShow)
{
	if(bShow)
	{
	      #if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN){
			appOsd_FontIconFontSet(ID_FONT_ICON);
			appOsdLib_OsdLayIconDraw(FILE_LEFTRIGHT_LEFT_ARROW_X,FILE_LEFTRIGHT_LEFT_ARROW_Y+90, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_BUTTON_LEFT);
			appOsdLib_OsdLayIconDraw(FILE_LEFTRIGHT_RIGHT_ARROW_X+300,FILE_LEFTRIGHT_LEFT_ARROW_Y+90,SP5K_GFX_ALIGN_TOP_LEFT ,  ID_ICON_BUTTON_RIGHT);
		}
		else{
			appOsd_FontIconFontSet(ID_FONT_ICON);
			appOsdLib_OsdLayIconDraw(FILE_LEFTRIGHT_LEFT_ARROW_X,FILE_LEFTRIGHT_LEFT_ARROW_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_BUTTON_LEFT);
			appOsdLib_OsdLayIconDraw(FILE_LEFTRIGHT_RIGHT_ARROW_X,FILE_LEFTRIGHT_LEFT_ARROW_Y,SP5K_GFX_ALIGN_TOP_LEFT ,  ID_ICON_BUTTON_RIGHT);
		}
		#endif
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, FILE_LEFTRIGHT_LEFT_ARROW_X, FILE_LEFTRIGHT_LEFT_ARROW_Y,FILE_LEFTRIGHT_ARROW_CLEAR_W, FILE_LEFTRIGHT_ARROW_CLEAR_H, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, FILE_LEFTRIGHT_RIGHT_CLEAR_X, FILE_LEFTRIGHT_LEFT_ARROW_Y,FILE_LEFTRIGHT_ARROW_CLEAR_W, FILE_LEFTRIGHT_ARROW_CLEAR_H, 0, 0);
	}
}


void appPlayOsd_DlgMessageShow(UINT32 titleStrId,UINT32 dlgType)
{
	UINT32 dlgMsg_YesNo[2] = {ID_STR_YES, ID_STR_NO};
	UINT32 dlgMsg_Rotate[3] = {ID_STR_ROTATE, ID_STR_SAVE, ID_STR_BACK};
	UINT32 dlgMsg_MPbLock[4] = {ID_STR_SELECT, ID_STR_NO, ID_STR_UNPROTECT_ALL_, ID_STR_BACK};
	UINT32 dlgMsg_MpbDele[3] = {ID_STR_SELECT, ID_STR_ALL, ID_STR_BACK};
	UINT32 *pItem;
	UINT32 numOption;
	UINT32 actIndex,yOffset;
	actIndex = yOffset =0;
	switch(dlgType)
	{	
		case DLGTYPE_ROTATE:
			pItem = dlgMsg_Rotate;
			numOption = 3;
			break;
		case DLGTYPE_MPB_DELE:
			pItem = dlgMsg_MpbDele;
			numOption = 3;
			break;
		case DLGTYPE_MPB_LOCK:
			pItem = dlgMsg_MPbLock;
			numOption = 4;
			break;
		case DLGTYPE_YES_NO:
		default:
			pItem = dlgMsg_YesNo;
			numOption = 2;
			break;

	}
	CLEAR_OSD_SRC;
	appPlayOsd_DlgBoxConfig(titleStrId, numOption, pItem, actIndex, yOffset);
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(TRUE);
}

UINT8 appPlayOsd_DlgMessageProcess(UINT32 msg)
{
	UINT32 ret ;


	if(gpbDlgCb.dlgStatus == DLGSTATUS_SHOW)
	{
		ret = APPSTATE_MSG_IGNORE;
		switch(msg)
		{
			case APP_KEY_PRESS_UP:
				appPlayOsd_DlgBoxScroll(TRUE);
				break;
			case APP_KEY_PRESS_DOWN:
				appPlayOsd_DlgBoxScroll(FALSE);
				break;
			case APP_KEY_PRESS_SET:
				gpbDlgCb.dlgStatus = DLGSTATUS_SET;
				ret = APPSTATE_MSG_DEFAULT;
				break;
			default:
				break;
		}
	}
	else
	{
		ret = APPSTATE_MSG_DEFAULT;
	}

	return ret;
}

/*add by JQ*/
#if CAM_TYPE_CVR  
void
appPlayOsd_VideoInfoDraw(
	void
)
{	
	appPlayOsd_VideoFileIconDrow(TRUE);
	appPlayOsd_VideoFileNameDraw();
}

void
appPlayOsd_VideoFileNameDraw(
	void
)
{
	UINT32 dcfIdx,MaxCnt,start_idx=0,osdFileNameItem=0;
	UINT16 ncuridx = 0; 
	unsigned short *pIconId=NULL;
	unsigned short *pIconIdColor=NULL;
	unsigned short *pStrId=NULL;
	unsigned short *pIconIdHL=NULL;
	BOOL 	bEnable = FALSE;
	UINT8 dst[11];
	UINT32 X = 140,Y=41;
	UINT32 gapY=34;
	UINT32 StartX = 130,StartY = 40,W = 190,H = 140;
	

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		X = 260;
		Y = 66;
		gapY=48;
		StartX = 300;
		StartY = 60;
		W = 400;
		H = 240;
	}
	#endif
	

	if (gtotalImgCount >4)
	{
		MaxCnt = 4;
	}
	else
	{
		MaxCnt = gtotalImgCount;
	}


	if (ncuridx>0)
	{
		gbTKPlayMenuStartIndex = (ncuridx-1)/4;
	} 
	else
	{
	 	gbTKPlayMenuStartIndex = 0;
	}
	start_idx = gbTKPlayMenuStartIndex*4;
	gtotalImgCount = appPbTotalFileCountGet();
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,StartX,StartY,W,H,0,0);
	
	if(gtotalImgCount < 5){
		
		for(dcfIdx=gtotalImgCount; dcfIdx>0; dcfIdx--){
			appDcfFileNameGet(dcfIdx);
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
			#if 1
			strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
			dst[11]=0;
			appOsd_GfxStringDraw(X,Y+(osdFileNameItem*gapY),SP5K_GFX_ALIGN_TOP_LEFT,dst);
			#else
			appOsd_GfxStringDraw(140,45+(osdFileNameItem*32),SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);
			#endif
			osdFileNameItem++;
		}
	}
	else{
		
		for(dcfIdx=gtotalImgCount; dcfIdx>(gtotalImgCount-4); dcfIdx--){	
			appDcfFileNameGet(dcfIdx);
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);

			#if 1
			strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
			dst[11]=0;
			appOsd_GfxStringDraw(X,Y+(osdFileNameItem*gapY),SP5K_GFX_ALIGN_TOP_LEFT,dst);	/* DRAW STRING */
			#else
			appOsd_GfxStringDraw(140,45+(osdFileNameItem*32),SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);	/* DRAW STRING */
			#endif
			osdFileNameItem++;
		}
	}
	
	appDcfFileNameGet(gtotalImgCount);
	sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);
	appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	
	#if 1
	strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
	dst[11]=0;
	appOsd_GfxStringDraw(X,Y,SP5K_GFX_ALIGN_TOP_LEFT,dst);	/* DRAW STRING */ /* SC.LIU */
	#else
	appOsd_GfxStringDraw(140,45,SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);	/* DRAW STRING */ /* SC.LIU */
	#endif

}

 void
 appPlayOsd_VideoFileIconDrow(
 	unsigned char bShow
 )
{
	UINT32 coordinateMenuIcon_Y, coordinateMenuString_Y;
	if(IS_HDMI_IN){
		coordinateMenuIcon_Y=302;
		coordinateMenuString_Y=302;
	}
	else{
		coordinateMenuIcon_Y=208;
		coordinateMenuString_Y=208;
	}

	if (!bShow)
	{
		return ;
	}

#if 1//feng20120821
	appOsd_FontIconFontSet(ID_FONT_ICON);
	//Draw playback  ICON 
	//appOsdLib_OsdLayIconDraw(20, 2, SP5K_GFX_ALIGN_TOP_LEFT, 0x00b7);
#endif

#if KIT_WITH_HDMI
 if(IS_HDMI_IN){
		appOsd_FontIconFontSet(ID_FONT_ICON);
		//Draw OK , MENU ICON and String
		//#define ID_ICON_ZZPB_OKHOLD_HDMI		0x01a0
		//#define ID_ICON_ZZPB_OKPRESS_HDMI		0x01a1
		//#define ID_ICON_ZZPB_POWER_HDMI		0x01a2
		//appOsdLib_OsdLayIconDraw(110, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_MENU );
		//appOsdLib_OsdLayIconDraw(350, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);
		//appOsdLib_OsdLayIconDraw(110, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKHOLD_HDMI );
		
		//appOsdLib_OsdLayIconDraw(250, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI );
		
		//appOsdLib_OsdLayIconDraw(350, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKPRESS_HDMI);


		appOsdLib_OsdLayIconDraw(110-50, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKPRESS_HDMI);
		appOsdLib_OsdLayIconDraw(250, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_POWER_HDMI );
		appOsdLib_OsdLayIconDraw(390+50, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_ZZPB_OKHOLD_HDMI );

		
		
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

		appOsdLib_TextIDDraw(155-50,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_PLAY);
		appOsdLib_TextIDDraw(295,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_SELECT);
		appOsdLib_TextIDDraw(435+50+5,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_MENU);

		//appOsdLib_TextIDDraw(155,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_MENU);
		//appOsdLib_TextIDDraw(295,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_SELECT);
		//appOsdLib_TextIDDraw(435,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_PLAY);
	}
  else{
		appOsd_FontIconFontSet(ID_FONT_ICON);
		//Draw OK , MENU ICON and String
		appOsdLib_OsdLayIconDraw(40, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_MENU );
		appOsdLib_OsdLayIconDraw(215, coordinateMenuIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);

		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

		appOsdLib_TextIDDraw(78,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_MENU);
		appOsdLib_TextIDDraw(253,coordinateMenuString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_PLAY);
		}
#endif

}

void
appPlayOsd_VidFileNameChangeDrow(
	void
)
{

	UINT16 dcfIdx,MaxCnt,start_idx=0,osdFileNameItem=0;
	UINT16 ncuridx = gpbMainCb.totalImg - gpbMainCb.curImgIdx;
	UINT8 nClearShapeIndex=0;
	UINT16 PlayMenuLast_Index;
	UINT8 menuHLTab;

	unsigned short *pIconId=NULL;
	unsigned short *pIconIdColor=NULL;
	unsigned short *pStrId=NULL;
	unsigned short *pIconIdHL=NULL;
	BOOL 	bEnable = FALSE;
	UINT8 dst[11];
	UINT32 X = 140,Y=41;
	UINT32 gapY=34;
	UINT32 StartX = 130,StartY = 40,W = 190,H = 140;
	

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		X = 260;
		Y = 66;
		gapY=48;
		StartX = 300;
		StartY = 60;
		W = 400;
		H = 240;
	}
	#endif
	
	if (gtotalImgCount >4){
		MaxCnt = 4;
	}else{
		MaxCnt = gtotalImgCount;
	}

	// Look for 1st item in detail menu
	PlayMenuLast_Index= gbTKPlayMenuStartIndex;
	
	if (ncuridx>0){
		gbTKPlayMenuStartIndex = ncuridx / 4;
	}
	start_idx = gbTKPlayMenuStartIndex*4;		
	gtotalImgCount = appPbTotalFileCountGet();
	if (gbTKPlayMenuStartIndex > 0)
	{
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,StartX,StartY,W,H,0,0);
		for(dcfIdx=0; dcfIdx<MaxCnt; dcfIdx++){
			appDcfFileNameGet(gtotalImgCount-start_idx-dcfIdx);
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
			#if 1
			strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
			dst[11]=0;
			appOsd_GfxStringDraw(X,Y+(osdFileNameItem*gapY),SP5K_GFX_ALIGN_TOP_LEFT,dst);
			#else
			appOsd_GfxStringDraw(140,45+(osdFileNameItem*32),SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);
			#endif
			
			if(gpbImgCb.dcfIdx == 1)
				break;
			osdFileNameItem++;
		}
		menuHLTab = ncuridx % 4;
		appDcfFileNameGet(gtotalImgCount-ncuridx);
		appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		#if 1
		strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
		dst[11]=0;
		appOsd_GfxStringDraw(X,Y+(menuHLTab*gapY),SP5K_GFX_ALIGN_TOP_LEFT,dst);		
		#else
		appOsd_GfxStringDraw(140,45+(menuHLTab*32),SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);		
		#endif
	}
	else
	{
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,StartX,StartY,W,H,0,0);
		if(gtotalImgCount < 5){			
			for(dcfIdx=gtotalImgCount; dcfIdx>0; dcfIdx--){
				appDcfFileNameGet(dcfIdx);
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
				#if 1
				strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
				dst[11]=0;
				appOsd_GfxStringDraw(X,Y+(osdFileNameItem*gapY),SP5K_GFX_ALIGN_TOP_LEFT,dst);
				#else
				appOsd_GfxStringDraw(140,45+(osdFileNameItem*32),SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);
				#endif
				osdFileNameItem++;
			}
		}
		else{
			for(dcfIdx=gtotalImgCount; dcfIdx>(gtotalImgCount-4); dcfIdx--){
				appDcfFileNameGet(dcfIdx);
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
				#if 1
				strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
				dst[11]=0;
				appOsd_GfxStringDraw(X,Y+(osdFileNameItem*gapY),SP5K_GFX_ALIGN_TOP_LEFT,dst);
				#else
				appOsd_GfxStringDraw(140,45+(osdFileNameItem*32),SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);
				#endif
				osdFileNameItem++;
			}
		}
		appDcfFileNameGet(gtotalImgCount-ncuridx);
		sp5kDcfFsCurFileIdxSet(gpbImgCb.dcfIdx);
		appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		#if 1
		strncpy(dst,&gpbImgCb.dcfAttr.filename[4],strlen(&gpbImgCb.dcfAttr.filename[4])-4);
		dst[11]=0;
		appOsd_GfxStringDraw(X,Y+(ncuridx*gapY),SP5K_GFX_ALIGN_TOP_LEFT,dst);
		#else
		appOsd_GfxStringDraw(140,45+(ncuridx*32),SP5K_GFX_ALIGN_TOP_LEFT,&gpbImgCb.szDcfFileName[0]+17);
		#endif
	}
	osdFileNameItem=0;
}

void appPlayOsd_ImagePlay(void)
{
	if(gpbImgCb.fileType != PB_FT_JPG)
	{
		appPbImagePlay(gpbMainCb.curImgIdx);
	}
}

void
appDcfFileNameGet(
	UINT32 dcfIdx
)
{
	gpbImgCb.dcfIdx = dcfIdx;
	playImgDcfAttrGet(&gpbImgCb);
}
#endif
/*add by JQ*/

