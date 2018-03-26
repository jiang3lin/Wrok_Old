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
#include "app_com_api.h"
#include "sp5k_gfx_api.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_pal.h"
#include "app_osdfont.h"
#include "app_iconfont1.h"
#include "app_icon.h"
#include "app_icon_def.h"
#include "app_stringid.h"
#include "app_user_setting.h"
#include "app_menu.h"
#include "common.h"
#include "app_usb_dpslib.h" 
#include "app_usb_dps_osd.h"
#include "app_menu_frameosd.h"
#include "app_user_setting.h"
#include "app_playback_osd.h"
#include "app_playback_def.h"
#include "app_menu_tab.h"

 /**************************************************************************
  * 						  C O N S T A N T S 						   *
 **************************************************************************/
 
 /**************************************************************************
  * 							 M A C R O S							   *
  **************************************************************************/
 
#define TKTYPE_SSNAPSHAPE_MENU_OKSTART_X  /*70*/115
#define TKTYPE_SNAPSHAPE_MENU_OKSTART_Y  210
#define TKTYPE_MENUICON_OFFSET_X		36

 /**************************************************************************
  * 						 D A T A	T Y P E S						   *
  **************************************************************************/
extern dpsImage_t gDpsImage;
extern dpsInfo_t  gDpsInfo;


/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuItenSetandDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuItenSetandDraw( unsigned short wTitle,  unsigned short SelectIndex)
{
	UINT32 strID = 0;
	switch (wTitle)
	{
		default:
		case DPS_MENU_CHANGE:
		case DPS_MENU_DATE:
		case DPS_MENU_QUALITY:
		case DPS_MENU_LAYOUT:
		case DPS_MENU_PAPER_SIZE:
		case DPS_MENU_PAPER_TYPE:
		case DPS_MENU_PRINT:
		case DPS_MENU_FILE_NO:
		case DPS_MENU_DATETYPE:
		case DPS_MENU_RESET:
			break;
		case DPS_MENU_IMAGE:
			switch(SelectIndex)
			{
				case DPS_IMAGE_SIGNLE:
					strID =ID_STR_CHOOSE_THIS_PHOTO_;
					break;
				case DPS_IMAGE_ALL:
					strID =ID_STR_PRINT_ALL_PHOTO_;
					break;
				case DPS_IMAGE_CHOOSE:
					strID =ID_STR_CHOOSE_THIS_PHOTO_;
					break;
				
			}
			appPlayOsd_DlgConfirmYesNoConfig(strID,1);
			appPlayOsd_DlgBoxShow();
			appDPSMenu_DPS2ndMenuHelpItemDraw(TRUE);
		break;
	}
}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuHelpItemDraw
*------------------------------------------------------------------------*/
void
appDPSMenu_DPS2ndMenuHelpItemDraw(BOOL bShow)
{
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);

	appOsdLib_OsdLayIconDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+2,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);
	appOsdLib_TextIDDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+3/*10*/,SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_SET);
	appPlayOsd_ModeIconDraw(bShow);
	/*pbMainCB_t *pmainCb;
	pmainCb = appPb_MainCbPtrGet();*/
	printf("gDpsImage->totalImg = %d",gDpsImage.totalImg);
	if(gDpsInfo.PicBuf[gDpsImage.curImgIdx-1])
	{
		appDPSMenu_DpsOSDIconDraw(TRUE);
	}
	else
	{
		appDPSMenu_DpsOSDIconDraw(FALSE);
	}
	if(gDpsImage.totalImg>0){
		__FUNC_TRACK__;
		appPLayOsd_LeftRightArrow(bShow);
	}
	appPlayOsd_FileNumDraw(bShow,gDpsImage.curImgIdx,gDpsImage.totalImg);
}
/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuImageFrameDraw(unsigned short SelectIndex,UINT16 Num)
{
	char szStr[8];
#if 0
	/*   BACKGROUND*/
	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_TITLE_FRAME_X, DPS_TK2NDMENU_TITLE_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT,ID_DLGBOX_NULL_TITLE_ICON, 0x0000);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_TITLE_FRAME_X, DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_OPTION_TXT_ICON, 0x0000);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_TITLE_FRAME_X, DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_OPTION_BOTTOM_ICON, 0x0000);
	/*	TITLE  */
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DPS_TK2NDMENU_TITLE_STR_X,DPS_TK2NDMENU_TITLE_STR_Y, SP5K_GFX_ALIGN_CENTER, ID_STR_SET_PRINT_COPIES);
#endif
	/* BOTTOM HELP BAR */
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayIconDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+2,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);
	appOsdLib_TextIDDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+3,SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_SET);

	/* Icon */
	appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_PRINT_ICON_X, DPS_TK2NDMENU_PRINT_ICON_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_PRINT);


	if(SelectIndex==DPS_MENU_2ND_SELECT0){
		/* ARROW*/
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_LEFT_ARROW_X, DPS_TK2NDMENU_LEFT_ARROW_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_LEFT);
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_RIGHT_ARROW_X, DPS_TK2NDMENU_RIGHT_ARROW_Y,	SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_RIGHT);

		appOsd_ColorDrawShapeColorSet(PAL_YELLOW_1, PAL_BLEND_100, PAL_YELLOW_1, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,DPS_TK2NDMENU_SELBAR_W,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, szStr);
		#if 0
		/*	 back string */
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextIDDraw(DPS_TK2NDMENU_STR_BACK_X,DPS_TK2NDMENU_STR_BACK_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
		#endif
	}else{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,100,25, 0, 0);/**/
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, szStr);
		#if 0
		/*	 back string */
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextIDDraw(DPS_TK2NDMENU_STR_BACK_X,DPS_TK2NDMENU_STR_BACK_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
		#endif
	}
}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuImageSelChangeDraw(unsigned short SelectIndex,UINT16 Num)
{
	char szStr[8];
	if(SelectIndex==DPS_MENU_2ND_SELECT0){
		appOsd_ColorDrawShapeColorSet(PAL_YELLOW_1, PAL_BLEND_100, PAL_YELLOW_1, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,DPS_TK2NDMENU_SELBAR_W,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, szStr);
	}

}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuImageUpDownChangeDraw(unsigned short SelectIndex,UINT16 Num)
{
	char szStr[8];


	if(SelectIndex==DPS_MENU_2ND_SELECT0){
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_LEFT_ARROW_X, DPS_TK2NDMENU_LEFT_ARROW_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_LEFT);
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_RIGHT_ARROW_X, DPS_TK2NDMENU_RIGHT_ARROW_Y,	SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_RIGHT);

		appOsd_ColorDrawShapeColorSet(PAL_YELLOW_1, PAL_BLEND_100, PAL_YELLOW_1, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,DPS_TK2NDMENU_SELBAR_W,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, szStr);
		/*appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_STR_BACK_X-15,DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H+5,264,26, 0, 0);*/

	}else{
		RPrintf("show RES_ICON_MU2NDBAR");
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,DPS_TK2NDMENU_SELBAR_W,25, 0, 0);/**/
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, szStr);
	}
	#if 0
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DPS_TK2NDMENU_STR_BACK_X,DPS_TK2NDMENU_STR_BACK_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_STR_BACK);
	#endif
}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
#define DPS_DPOF_ICON_X								10/*6*/
#define DPS_DPOF_ICON_Y								10/*45*/
void
appDPSMenu_DpsOSDIconDraw
(
	UINT8 bShow
)
{
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
	/*appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_DPOF_ICON_X, DPS_DPOF_ICON_Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);*/
	if (bShow)
	{
		appOsd_FontIconFontSet(ID_FONT_ICON);
		appOsdLib_OsdLayIconDraw(DPS_DPOF_ICON_X, DPS_DPOF_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_SELECT/*ID_ICON_MODE_PICTBRIDGE*/);
	}
}


 /*-------------------------------------------------------------------------
*  File Name : appDPSMenu_DlgConfirmSetBackConfig
*------------------------------------------------------------------------*/
static UINT32 strID_DlgConfirmSetBack[2] = {(UINT32)NULL, ID_STR_BACK};
void
appDPSMenu_DlgConfirmSetBackConfig(
	UINT32 strId,
	UINT32 actIdx
)
{
	appPlayOsd_DlgBoxConfig(strId, 2, strID_DlgConfirmSetBack, actIdx, 40);

}

