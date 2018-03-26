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
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_sensor_api.h"

#include "gpio_custom.h"
#include "app_simple_menu.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define STEXT_OSD_FONT_CONST_WIDTH	18
#define STEXT_OSD_FONT_CONST_HEIGHT 32

#define SMENU_COLOR_TITLE_H		41
#define SMENU_NULL_TITLE_H		8
#define SMENU_DATA_CONT_H		31
#define SMENU_DATA_LAST_H		38
#define SMENU_WIDTH				292
#define SMENU_HL_BAR_W			264
#define SMENU_HL_BAR_H			26

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static simpleMenuInfo_t gsmenuCb;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern UINT32 appBtnMsgToKey(UINT32 msgId,UINT32 msgParam);

/*-------------------------------------------------------------------------
 *  File Name : stext_Clear
 *------------------------------------------------------------------------*/
 void stext_Clear(UINT32 x, UINT32 y, UINT8 *pszText)
 {
 	UINT32 textLen;

 	textLen = strlen(pszText);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x-(((textLen+1)>>1)*STEXT_OSD_FONT_CONST_WIDTH), y-(STEXT_OSD_FONT_CONST_HEIGHT>>1), (textLen+1)*STEXT_OSD_FONT_CONST_WIDTH, STEXT_OSD_FONT_CONST_HEIGHT, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : stext_Print
 *------------------------------------------------------------------------*/
 void stext_Print(UINT32 x, UINT32 y, 	const char *format, ...)
 {
 	char buf[256];
	va_list	ap;

	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	stext_Clear(x, y, buf);
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
 	appOsdLib_TextDraw(x, y, SP5K_GFX_ALIGN_CENTER, buf);
 }

/*-------------------------------------------------------------------------
 *  File Name : stext_Blink
 *------------------------------------------------------------------------*/
 void stext_Blink(UINT32 x, UINT32 y, const char *format, ...)
 {
  	char buf[256];
	va_list	ap;

	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

 	int i;
	for (i=3; i>0; i--) {
		/* show */
		APP_SLEEP_MS(300); stext_Print(x, y, buf); APP_SLEEP_MS(300);
		/* clear */
		stext_Clear(x, y, buf);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : dbgOsd_Print
 *------------------------------------------------------------------------*/
 void dbgOsd_Print(UINT32 row, const char *format, ...)
 {
 	char buf[64];
	va_list	ap;

	if (row >= 8) {
		printf("\n%s: Illegal row(%d) \n\n", __FUNCTION__, row);
		return;
	}

	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	stext_Clear(160, row*30, buf);
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
 	appOsdLib_TextDraw(160, row*30, SP5K_GFX_ALIGN_CENTER, buf);
 }

/*-------------------------------------------------------------------------
 *  File Name : SMENU_CBGet
 *------------------------------------------------------------------------*/
simpleMenuInfo_t
*SMENU_CBGet(
	void
)
{
	return &gsmenuCb;
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_SelectSet
 *------------------------------------------------------------------------*/
void
SMENU_SelectSet(
	UINT32 selIndex
)
{
	gsmenuCb.lastActIndex = gsmenuCb.actIndex = selIndex;
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_SelectGet
 *------------------------------------------------------------------------*/
UINT8
SMENU_SelectGet(
	void
)
{
	return gsmenuCb.actIndex;
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Clear
 *------------------------------------------------------------------------*/
void
SMENU_Clear(
	void
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, gsmenuCb.boxX, gsmenuCb.boxY, gsmenuCb.boxW, gsmenuCb.boxH, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_TitleSet
 *------------------------------------------------------------------------*/
void
SMENU_TitleSet(
	UINT8 *pszTitle
)
{
	gsmenuCb.pszTitleText = pszTitle;
}


/*-------------------------------------------------------------------------
 *  File Name : SMENU_TitleUpdate
 *------------------------------------------------------------------------*/
void
SMENU_TitleUpdate(
	UINT8 *pszTitle
)
{
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	SMENU_TitleSet(pszTitle);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, gsmenuCb.boxX, gsmenuCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_TXT2_TITLE_ICON, 0x0000);
	appOsdLib_TextDraw(gsmenuCb.titleX, gsmenuCb.titleY, SP5K_GFX_ALIGN_BOTTOM, gsmenuCb.pszTitleText);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_ActiveBarChange
 *------------------------------------------------------------------------*/
void
SMENU_ActiveBarChange(
	void
)
{
	UINT8 index;

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);

	/*-- Inactivate last option */
	index = gsmenuCb.lastActIndex;
	//PB_VERBOSE("\n==> lastActIndex (%d) ..\n\n", index);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,
							(320-SMENU_HL_BAR_W)>>1, gsmenuCb.itemY[index],
							SP5K_GFX_ALIGN_TOP_LEFT,
							"A:\\RO_RES\\UI\\ICON\\MU2CLBAR.SFN", 0x0000);
	appOsdLib_TextDraw(gsmenuCb.itemX[index], gsmenuCb.itemY[index],
						SP5K_GFX_ALIGN_TOP_LEFT,
						gsmenuCb.pszItemText[index]);

	/*-- Activate current option */
	index = gsmenuCb.actIndex;
	//PB_VERBOSE("\n==> actIndex (%d) ..\n\n", index);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (320-SMENU_HL_BAR_W)>>1, gsmenuCb.itemY[index], SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\MUDLGBAR.SFN", 0x0000);
	appOsdLib_TextDraw(gsmenuCb.itemX[index], gsmenuCb.itemY[index], SP5K_GFX_ALIGN_TOP_LEFT, gsmenuCb.pszItemText[index]);

	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_NOCOLOR);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Scroll
 *------------------------------------------------------------------------*/
void
SMENU_Scroll(
	BOOL isUp
)
{
	gsmenuCb.lastActIndex = gsmenuCb.actIndex;

	if (isUp) {
		gsmenuCb.actIndex = (gsmenuCb.actIndex==0) ? (gsmenuCb.numItems-1) : gsmenuCb.actIndex-1;
	} else {
		gsmenuCb.actIndex = (gsmenuCb.actIndex>=(gsmenuCb.numItems-1)) ? 0 : gsmenuCb.actIndex+1;
	}

	/*SMENU_ActiveBarChange();*/
	SMENU_Show();
	appBeep(TRUE, FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Show
 *------------------------------------------------------------------------*/
void
SMENU_Show(
	void
)
{
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);
	appOsd_ColorBlendingSet(PAL_GRAY_1, PAL_BLEND_75);

	/*-- Title TEXT */
	if (gsmenuCb.pszTitleText) {
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,
								gsmenuCb.boxX, gsmenuCb.boxY,
								SP5K_GFX_ALIGN_TOP_LEFT,
								"A:\\RO_RES\\UI\\ICON\\DLGTITLL.SFN", 0x0000);
		appOsdLib_TextDraw(gsmenuCb.titleX, gsmenuCb.titleY,
							SP5K_GFX_ALIGN_BOTTOM, gsmenuCb.pszTitleText);
	} else {
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, gsmenuCb.boxX, gsmenuCb.boxY,
								SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_NULL_TITLE_ICON, 0x0000);
	}

	/*-- Options TEXT & ICON, except the last */
	UINT32 i;
	for (i=0; i<gsmenuCb.numItems-1; i++) {
		/*-- TEXT BG ICONT */
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,
								gsmenuCb.boxX, gsmenuCb.titleY+(i*SMENU_DATA_CONT_H),
								SP5K_GFX_ALIGN_TOP_LEFT,
								ID_DLGBOX_OPTION_TXT_ICON, 0x0000);
		/*-- Show marker ICON */
		if (i==gsmenuCb.actIndex) {
			appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,
									(320-SMENU_HL_BAR_W)>>1, gsmenuCb.itemY[i],
									SP5K_GFX_ALIGN_TOP_LEFT,
									"A:\\RO_RES\\UI\\ICON\\CALIB_BAR.SFN", 0x0000);
		}
		appOsdLib_TextDraw(gsmenuCb.itemX[i], gsmenuCb.itemY[i],
							SP5K_GFX_ALIGN_TOP_LEFT, gsmenuCb.pszItemText[i]);
	}

	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,
							gsmenuCb.boxX, gsmenuCb.titleY+(i*SMENU_DATA_CONT_H),
							SP5K_GFX_ALIGN_TOP_LEFT,
							ID_DLGBOX_OPTION_BOTTOM_ICON, 0x0000);

	if (gsmenuCb.actIndex==i) {
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,
								(320-SMENU_HL_BAR_W)>>1, gsmenuCb.itemY[i],
								SP5K_GFX_ALIGN_TOP_LEFT,
								"A:\\RO_RES\\UI\\ICON\\CALIB_BAR.SFN", 0x0000);
	}
	appOsdLib_TextDraw(gsmenuCb.itemX[i], gsmenuCb.itemY[i],
						SP5K_GFX_ALIGN_TOP_LEFT, gsmenuCb.pszItemText[i]);

	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_NOCOLOR);

}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Config
 *------------------------------------------------------------------------*/
void
SMENU_Config(
	UINT8 *pszTitle,
	UINT32 numItems,
	UINT32 *pszOptText,
	UINT32 actIndex,
	SINT32 yOffset
)
{
	UINT32 i;

	memset(&gsmenuCb, 0, sizeof(simpleMenuInfo_t));

	gsmenuCb.pszTitleText= pszTitle;
	gsmenuCb.numItems = (numItems>SMENU_ITEM_NUM_TOTAL) ? SMENU_ITEM_NUM_TOTAL : numItems;
	gsmenuCb.actIndex = (actIndex>=SMENU_ITEM_NUM_TOTAL) ? SMENU_ITEM_NUM_TOTAL-1 : actIndex;
	gsmenuCb.lastActIndex = gsmenuCb.actIndex;

	/*-- Construct coordinates & dimension */
	gsmenuCb.boxW = SMENU_WIDTH;
	gsmenuCb.boxH = ((gsmenuCb.numItems-1)*SMENU_DATA_CONT_H)+SMENU_DATA_LAST_H;
	gsmenuCb.boxH += ((NULL==pszTitle) ? SMENU_NULL_TITLE_H : SMENU_COLOR_TITLE_H);

	gsmenuCb.boxX = (320-gsmenuCb.boxW)>>1;
	gsmenuCb.boxY = ((240-gsmenuCb.boxH)>>1);
	gsmenuCb.boxY += yOffset;

	gsmenuCb.titleX = 160;	/*-- title text is BOTTOM-CENTER aligned */
	gsmenuCb.titleY = (NULL==pszTitle) ? gsmenuCb.boxY+SMENU_NULL_TITLE_H : gsmenuCb.boxY+SMENU_COLOR_TITLE_H;

	UINT32 itemX = gsmenuCb.boxX+32;
	UINT32 itemY = gsmenuCb.titleY+4;
	UINT8 *pszTmp;
	for (i=0; i<numItems; i++) {
		pszTmp = (UINT8 *)(*(pszOptText+i));
		gsmenuCb.pszItemText[i] = pszTmp;
		gsmenuCb.itemX[i] = itemX;
		gsmenuCb.itemY[i] = itemY+(i*SMENU_DATA_CONT_H);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_YesNoConfig
 *------------------------------------------------------------------------*/
static UINT8 smenuYesNo_TitleText[24]="Yes or No ?";
static UINT32 smenuYesNo_SelList[] =
		{(UINT32)"Yes",
		 (UINT32)"No",
		};
void SMENU_YesNoConfig(UINT8 *pszTitle)
{
	if (pszTitle) {
		memset(smenuYesNo_TitleText, 0, 24);
		strcpy(smenuYesNo_TitleText, pszTitle);
	}
	SMENU_Config(smenuYesNo_TitleText, 2, smenuYesNo_SelList, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_YesNoShow
 *------------------------------------------------------------------------*/
void SMENU_YesNoShow(void)
{
	SMENU_Show();
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_YesNoClear
 *------------------------------------------------------------------------*/
void SMENU_YesNoClear(void)
{
	SMENU_Clear();
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_SelScan
 *------------------------------------------------------------------------*/
UINT32 SMENU_SelScan(void)
{
	UINT32 msg, param;
	UINT32 key;

	while(1) {
		sp5kHostMsgReceive(&msg, &param);

		if (msg==SP5K_MSG_AE_READY) {
			continue;
		}

		if (!IS_SP5K_BTN_MSG(msg)) {
			continue;
		}
		key = appBtnMsgToKey(msg, param);

		if (key==APP_KEY_PRESS_UP) {
			SMENU_Scroll(TRUE);
			continue;
		} else if (key==APP_KEY_PRESS_DOWN) {
			SMENU_Scroll(FALSE);
			continue;
		} else if (key==APP_KEY_PRESS_SET) {
			appBeep(TRUE, FALSE);
			return SMENU_SelectGet();
		}
	}
}

