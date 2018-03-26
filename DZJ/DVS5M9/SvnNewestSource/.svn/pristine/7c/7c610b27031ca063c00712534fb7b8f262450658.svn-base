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

#include "app_osd_api_lcm.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"
#include "gpio_custom.h"
#include "app_tvlcd.h"

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
void appLcmRefresh(void)
{
	dispRefresh();
}

void appLcmClean(void)
{
	dispClean();
}

void appLcmFlip(BOOL bFlip)
{
	dispFlip(bFlip);
}

void appLcmBrightness(UINT32 duty)
{
	#if 1
	
	#define LCM_PWM_CTRL 5

	sp5kPwmCfgSet(LCM_PWM_CTRL, 10, duty);
	sp5kPwmTrig( 1 << LCM_PWM_CTRL, 0 );

	#else

	if(duty == 30)
	{
		LCD_BACKLIGHT_OFF;
	}
	else if(IS_LCD_OUT)
	{
		LCD_BACKLIGHT_ON;
	}

	#endif
}

void appLcmOsdInit(void)
{
	profLogAdd(0, "appLcmOsdInit s");
		
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);		
	appOsd_FontTextStringSet(ID_STRING_TABLE_ENGLISH);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);

	appOsdSFNFileLoad(SP5K_RES_OSD_ICON, ID_FONT_ICON_32X24);

	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_5, ID_ICON_LCM_TEST);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_3, ID_FONT_ICON_16X8);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_2, ID_ICON_FOCUS);
	
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_4, "UI\\FONT\\LOGO.SFN");
	appOsdSFNFileLoad(SP5K_RES_DATE_STAMP_OSD, "UI\\FONT\\DATSTAMP.SFN");

	profLogAdd(0, "appLcmOsdInit e");
}

void appLcmOsdIconColorInvert(BOOL bColorInvert)
{
	if(bColorInvert == TRUE)
	{
		appOsd_ColorIconColorSet(PAL_WHITE,PAL_BLACK);
		appOsd_ColorIconColorSet(PAL_BLACK,PAL_WHITE);
	}
	else
	{
		appOsd_ColorIconColorSet(PAL_WHITE,PAL_WHITE);
		appOsd_ColorIconColorSet(PAL_BLACK,PAL_BLACK);
	}
}

void appLcmOsdTextColorInvert(BOOL bColorInvert)
{
	if(bColorInvert == FALSE)
	{
		appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_WHITE, PAL_BLEND_100);
	}
	else
	{
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_BLACK, PAL_BLEND_100);
	}
}

void appLcmOsdClean(void)
{
	#ifdef LCM_USE_OSD_LAYER
	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM, 0, 0);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_UsrLayerClearDraw();
	//appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	//appOsdLib_UsrLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 0, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM, 0, 0);
	#endif
}

void appLcmOsdBatteryIconShow(UINT32 BatteryIconId, BOOL bColorInvert)
{
	//appOsd_FontIconFontSet(ID_FONT_ICON_16X8);
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON_3);
	appLcmOsdIconColorInvert(bColorInvert);
	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_OsdLayIconDraw(LCM_OSD_BATTERY_X, LCM_OSD_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, BatteryIconId);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_UsrLayIconDraw(LCM_OSD_BATTERY_X, LCM_OSD_BATTERY_Y, SP5K_GFX_ALIGN_TOP_LEFT, BatteryIconId);
	#endif
}

void appLcmOsdLargeIconShow(lcmOsdIconIndex_t IconIndex, UINT32 IconID, BOOL bColorInvert)
{
	appOsd_FontIconFontSet(ID_FONT_ICON_32X24);
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
	appLcmOsdIconColorInvert(bColorInvert);

	if(IconIndex == LcmOsdIconIndex0)
	{
		#ifdef LCM_USE_OSD_LAYER
		appOsdLib_OsdLayIconDraw(LCM_OSD_ICON0_X, LCM_OSD_ICON0_Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
		#elif defined LCM_USE_USR_LAYER
		appOsdLib_UsrLayIconDraw(LCM_OSD_ICON0_X, LCM_OSD_ICON0_Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
		#endif
	}
	else if(IconIndex == LcmOsdIconIndex1)
	{
		#ifdef LCM_USE_OSD_LAYER
		appOsdLib_OsdLayIconDraw(LCM_OSD_ICON0_X, LCM_OSD_ICON0_Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
		#elif defined LCM_USE_USR_LAYER
		appOsdLib_UsrLayIconDraw(LCM_OSD_ICON1_X, LCM_OSD_ICON1_Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
		#endif
	}
	else if(IconIndex == LcmOsdIconIndex2)
	{
		#ifdef LCM_USE_OSD_LAYER
		appOsdLib_OsdLayIconDraw(LCM_OSD_ICON0_X, LCM_OSD_ICON0_Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
		#elif defined LCM_USE_USR_LAYER
		appOsdLib_UsrLayIconDraw(LCM_OSD_ICON2_X, LCM_OSD_ICON2_Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
		#endif
	}
}

void appLcmOsdLargeTextShow(CHAR* String, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_24);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextDraw(LCM_OSD_LARGE_TEXT_X, LCM_OSD_LARGE_TEXT_Y, SP5K_GFX_ALIGN_TOP_RIGHT, String);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextUsrDraw(LCM_OSD_LARGE_TEXT_X, LCM_OSD_LARGE_TEXT_Y, SP5K_GFX_ALIGN_TOP_RIGHT, String);
	#endif
}

void appLcmOsdLargeStringShow(UINT32 StringID, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_24);	
	appOsd_FontTextStringSet(ID_STRING_TABLE_ENGLISH);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextIDDraw(LCM_OSD_LARGE_TEXT_X, LCM_OSD_LARGE_TEXT_Y, SP5K_GFX_ALIGN_TOP_RIGHT, StringID);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextIDUsrDraw(LCM_OSD_LARGE_TEXT_X, LCM_OSD_LARGE_TEXT_Y, SP5K_GFX_ALIGN_TOP_RIGHT, StringID);
	#endif
}

void appLcmOsdModeTextShow(CHAR* String, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextDraw(LCM_OSD_MODE_TEXT_X, LCM_OSD_MODE_TEXT_Y, SP5K_GFX_ALIGN_CENTER, String);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextUsrDraw(LCM_OSD_MODE_TEXT_X, LCM_OSD_MODE_TEXT_Y, SP5K_GFX_ALIGN_CENTER, String);
	//appOsdLib_TextUsrDraw(LCM_OSD_MODE_TEXT_X, LCM_OSD_MODE_TEXT_Y, SP5K_GFX_ALIGN_TOP_LEFT, String);
	#endif
}

void appLcmOsdModeStringShow(UINT32 StringID, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);		
	appOsd_FontTextStringSet(ID_STRING_TABLE_ENGLISH);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextIDDraw(LCM_OSD_MODE_TEXT_X, LCM_OSD_MODE_TEXT_Y, SP5K_GFX_ALIGN_CENTER, StringID);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextIDUsrDraw(LCM_OSD_MODE_TEXT_X, LCM_OSD_MODE_TEXT_Y, SP5K_GFX_ALIGN_CENTER, StringID);
	#endif
}

void appLcmOsdTipsTextShow(CHAR* String, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextDraw(LCM_OSD_TIPS_TEXT_X, LCM_OSD_TIPS_TEXT_Y, SP5K_GFX_ALIGN_TOP, String);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextUsrDraw(LCM_OSD_TIPS_TEXT_X, LCM_OSD_TIPS_TEXT_Y, SP5K_GFX_ALIGN_TOP, String);
	#endif
}

void appLcmOsdTipsStringShow(UINT32 StringID, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);		
	appOsd_FontTextStringSet(ID_STRING_TABLE_ENGLISH);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextIDDraw(LCM_OSD_TIPS_TEXT_X, LCM_OSD_TIPS_TEXT_Y, SP5K_GFX_ALIGN_TOP, StringID);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextIDUsrDraw(LCM_OSD_TIPS_TEXT_X, LCM_OSD_TIPS_TEXT_Y, SP5K_GFX_ALIGN_TOP, StringID);
	#endif
}

void appLcmOsdBottomTextShow(CHAR* String, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextDraw(LCM_OSD_BOTTOM_TEXT_X, LCM_OSD_BOTTOM_TEXT_Y, SP5K_GFX_ALIGN_TOP, String);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextUsrDraw(LCM_OSD_BOTTOM_TEXT_X, LCM_OSD_BOTTOM_TEXT_Y, SP5K_GFX_ALIGN_TOP, String);
	#endif
}

void appLcmOsdBottomStringShow(UINT32 StringID, BOOL bColorInvert)
{
	appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);		
	appOsd_FontTextStringSet(ID_STRING_TABLE_ENGLISH);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextIDDraw(LCM_OSD_BOTTOM_TEXT_X, LCM_OSD_BOTTOM_TEXT_Y, SP5K_GFX_ALIGN_TOP, StringID);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextIDUsrDraw(LCM_OSD_BOTTOM_TEXT_X, LCM_OSD_BOTTOM_TEXT_Y, SP5K_GFX_ALIGN_TOP, StringID);
	#endif
}

void appLcmOsdIconShow(SINT32 x, SINT32 y, UINT32 align, UINT32 IconId, BOOL bLargeFont, BOOL bColorInvert)
{
	if(bLargeFont == TRUE)
	{
		appOsd_FontIconFontSet(ID_FONT_ICON_32X24);
		appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON);
	}
	else
	{
		//appOsd_FontIconFontSet(ID_FONT_ICON_16X8);
		appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON_3);
	}	
	appLcmOsdIconColorInvert(bColorInvert);
	
	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_OsdLayIconDraw(x, y, align, IconId);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_UsrLayIconDraw(x, y, align, IconId);
	#endif
}

void appLcmOsdTextShow(SINT32 x, SINT32 y, UINT32 align, UINT8 *szStr, BOOL bLargeFont, BOOL bColorInvert)
{
	if(bLargeFont == TRUE)
	{
		appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_24);
	}
	else
	{
		appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);
	}	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextDraw(x, y, align, szStr);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextUsrDraw(x, y, align, szStr);
	#endif
}

void appLcmOsdStringShow(SINT32 x, SINT32 y, UINT32 align, UINT32 StrId, BOOL bLargeFont, BOOL bColorInvert)
{
	if(bLargeFont == TRUE)
	{
		appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_24);
	}
	else
	{
		appOsd_FontTextFontSet(ID_FONT_TEXT_ENGLISH_8);
	}	
	appOsd_FontTextStringSet(ID_STRING_TABLE_ENGLISH);	
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	appLcmOsdTextColorInvert(bColorInvert);

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_TextIDDraw(x, y, align, StrId);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_TextIDUsrDraw(x, y, align, StrId);
	#endif
}

void appLcmOsdTest(BOOL bColorInvert)
{
	//appOsd_FontIconFontSet(ID_ICON_LCM_TEST);
	appOsd_GfxIconIDSet(SP5K_RES_OSD_ICON_5);

	if(bColorInvert == TRUE)
	{
		appOsd_ColorIconColorSet(PAL_WHITE,PAL_BLACK);
		appOsd_ColorIconColorSet(PAL_BLACK,PAL_WHITE);
	}
	else
	{
		appOsd_ColorIconColorSet(PAL_WHITE,PAL_WHITE);
		appOsd_ColorIconColorSet(PAL_BLACK,PAL_BLACK);
	}

	#ifdef LCM_USE_OSD_LAYER
	appOsdLib_OsdLayIconDraw(0, 0, SP5K_GFX_ALIGN_TOP_LEFT, 0);
	#elif defined LCM_USE_USR_LAYER
	appOsdLib_UsrLayIconDraw(0, 0, SP5K_GFX_ALIGN_TOP_LEFT, 0);
	#endif
}

