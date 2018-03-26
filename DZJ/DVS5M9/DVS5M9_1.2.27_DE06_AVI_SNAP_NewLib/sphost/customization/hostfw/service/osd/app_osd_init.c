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
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"
#include "app_setup_lib.h"
#include "app_ui_para.h"

#include "app_osd_api_lcm.h"
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

extern SINT32 gbOsdFactorXNumerator ;
extern SINT32 gbOsdFactorXDenominator ;
extern SINT32 gbOsdFactorYNumerator ;
extern SINT32 gbOsdFactorYDenominator ;

extern appOsdColorSetting_t appOsdCtrl;


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/



UINT32 osdwidth, osdheight;

static UINT8 blendTbl[16] __attribute__ ((unused)) ={4,4,4,4, 4,4,4,4,4,4,4,4,4,4,4,4};
static UINT16 pltTbl[]= {
	/* 0 - 7 */
	//RGB888_TO_RGB565(COLOR_TRANSPARENT),
	RGB888_TO_RGB565(COLOR_BLACK),
	RGB888_TO_RGB565(COLOR_GREEN_2),
	RGB888_TO_RGB565(COLOR_GREEN_3),
	RGB888_TO_RGB565(COLOR_GREEN_4),
	RGB888_TO_RGB565(COLOR_YELLOW_1),
	RGB888_TO_RGB565(COLOR_BROWN_1),
	RGB888_TO_RGB565(COLOR_RED),
	/* 8- */
	RGB888_TO_RGB565(COLOR_DARKBLUE_1),
	RGB888_TO_RGB565(COLOR_WHITE),
	RGB888_TO_RGB565(COLOR_GRAY_5),
	RGB888_TO_RGB565(COLOR_LIGHTBLUE_1),
	RGB888_TO_RGB565(COLOR_GRAY_2),
	RGB888_TO_RGB565(COLOR_BLACK_1),
	RGB888_TO_RGB565(COLOR_GRAY_1),
	RGB888_TO_RGB565(COLOR_BLUE),
	/* 16- */
	RGB888_TO_RGB565(COLOR_GRAY_3),
	RGB888_TO_RGB565(COLOR_GRAY),
	RGB888_TO_RGB565(COLOR_GREEN),
	RGB888_TO_RGB565(COLOR_ORANGE),
	RGB888_TO_RGB565(COLOR_BLUE_1),
	RGB888_TO_RGB565(COLOR_PURPLE_1),
	RGB888_TO_RGB565(COLOR_DARK_BLUE),
	RGB888_TO_RGB565(COLOR_IRON),
	/* 24- */
	RGB888_TO_RGB565(COLOR_GRAY_4),
	RGB888_TO_RGB565(COLOR_RED_1),
	RGB888_TO_RGB565(COLOR_GREEN_1),
	RGB888_TO_RGB565(COLOR_YELLOW),
	RGB888_TO_RGB565(COLOR_BLUE_2),
	RGB888_TO_RGB565(COLOR_PINK_4),
	RGB888_TO_RGB565(COLOR_LIGHT_BLUE),
	RGB888_TO_RGB565(COLOR_BROWN),
	/* 32- */

};




/*-------------------------------------------------------------------------
 *  File Name : appOsd_OSDColorBlendingInit
 *------------------------------------------------------------------------*/
 void appOsd_OSDColorBlendingInit(void)
 {
	UINT32 i;

	appOsdCtrl.blendTbl[0]= 0;
	for (i=1; i<16; i++)
	{
		appOsdCtrl.blendTbl[i]= 4;
	}
	sp5kDispBlendTblSet(appOsdCtrl.blendTbl);

	sp5kDispColorKeySet(SP5K_DISP_COLOR_KEY_RGB565, 0x000000, 0);

 }


void appPalInit(void)
{
	sp5kDispOsdPaletteSet(pltTbl,1, COUNT_OF(pltTbl));
	appOsd_OSDColorBlendingInit();

}

void
appOsdResPreload(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();

	profLogAdd(0, "osd preload :s");
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON, ID_FONT_ICON);
	appOsdSFNFileLoad(SP5K_RES_DATE_STAMP_OSD, "UI\\FONT\\DATSTAMP.SFN");
	appSetupMenu_SetCurLanguage(puiPara->Language);
	//appOsdSFNFileLoad(SP5K_RES_OSD_ICON_5, ID_ICON_ZOOM);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_5, ID_ICON_LCM_TEST);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_3,ID_ICON_WARNMEDIASTANDBY);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_2,ID_ICON_FOCUS);
	#if CAM_TYPE_CVR
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_4, "UI\\FONT\\LOGO.SFN");
	#endif
	profLogAdd(0, "osd preload :e");
}

void
appOsdInit(
	void
)
{
	//UINT32 width,height;

	profLogAdd(0, "osd init :s");
	appPalInit();
	appOsdLib_OsdLayerClearDraw();
	sp5kGfxAttrSet(SP5K_GFX_TRANS_COLOR, 0, 0, 0, 0);
	appOsd_ColorTextColorSet(PAL_WHITE,PAL_NOCOLOR,PAL_NOCOLOR);
	sp5kDispDevAttrSet(SP5K_DISP_BG_COLOR, 0,0,0,0);

	sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);

	sp5kGfxAttrSet(SP5K_GFX_FILL_ENABLE, 1, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_ENABLE, 1, 0, 0, 0);

	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_NOCOLOR);

	profLogAdd(0, "osd init :e");

	//appOsdResPreload();
}


/*-------------------------------------------------------------------------
 *  File Name : appOsdInit_TVModeSet
 *------------------------------------------------------------------------*/
void
appOsdInit_TVModeSet(
	unsigned char dispMode
)
{
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0); /*add for mantis bug 0048519 */

	sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FRAME_SIZE,OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);

	sp5kGfxInitCfgSet(SP5K_GFX_PF_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FRAME_SIZE,OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WITH_ALPHA_PLANE, 1, 0, 0, 0);

	sp5kGfxInit();

	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0, 1, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_BG_COLOR, 0,0,0,0);

	gbOsdFactorXNumerator = OSD_FACTOR_X_NUMERATOR;
	gbOsdFactorXDenominator = OSD_FACTOR_X_DENOMINATOR;
	gbOsdFactorYNumerator = OSD_FACTOR_Y_NUMERATOR;
	gbOsdFactorYDenominator = OSD_FACTOR_Y_DENOMINATOR;

}


/*-------------------------------------------------------------------------
 *  File Name : appOsdInit_LCDModeSet
 *------------------------------------------------------------------------*/
void appOsdInit_LCDModeSet(
	unsigned char dispMode
)
{
	sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM, 0, 0);

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WINDOW, 0, 0, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM);
	sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, 1000, 1000, 0, 0); /*add for mantis bug 0048519 */
	
	/*need to set PIP0 layer buffer size to 320X240 for timelapse datestamp*/
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FRAME_SIZE,OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_WINDOW, 0, 0, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H);

	sp5kGfxInitCfgSet(SP5K_GFX_PF_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FRAME_SIZE,OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_WINDOW, 0, 0, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM);

	sp5kGfxInitCfgSet(SP5K_GFX_USR_PAGE_ID, 0, 0, 0, 0);
	#ifdef LCM_USE_OSD_FORMAT_IDX8
	sp5kGfxInitCfgSet(SP5K_GFX_USR_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
	#elif defined LCM_USE_OSD_FORMAT_RGB565
	sp5kGfxInitCfgSet(SP5K_GFX_USR_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	#endif
	sp5kGfxInitCfgSet(SP5K_GFX_USR_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_USR_WINDOW, 0, 0, OSD_DEFAULT_FRAMSIZE_W_LCM, OSD_DEFAULT_FRAMSIZE_H_LCM);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_USR_0+1, 0, 0, 0);//需要关掉USR1 page，不然会和OSD0 page混合

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WITH_ALPHA_PLANE, 1, 0, 0, 0);

	sp5kGfxInit();

	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0, 1, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_BG_COLOR, 0,0,0,0);

	gbOsdFactorXNumerator = OSD_FACTOR_X_NUMERATOR_LCM;
	gbOsdFactorXDenominator = OSD_FACTOR_X_DENOMINATOR_LCM;
	gbOsdFactorYNumerator = OSD_FACTOR_Y_NUMERATOR_LCM;
	gbOsdFactorYDenominator = OSD_FACTOR_Y_DENOMINATOR_LCM;

}

