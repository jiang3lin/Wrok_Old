/**************************************************************************
 *
 *       Copyright (c) 2014-2015 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
#define HOST_DBG 0

#include "common.h"
#include <stdio.h>
#include <string.h>
#include "sp5k_gfx_api.h"
#include "sp5k_modesw_api.h"
#include "sp5k_cvr_warning_system_api.h"
#include "app_com_api.h"
#include "app_osd_api.h"
#include "app_still.h"
#include "app_cvrws_api.h"
#include "sp5k_capture_api.h"
#include "app_video.h"
#include "app_ui_para.h"

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
UINT32 centralPointX = CENTRALPOINT_X;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 gVideoSizeTag;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void
appCvrwsOsdDraw(
	void
)
{
	APP_OSD_REFRESH_OFF;

	#if 0
	/* Draw ROI area with yellow color */
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_YELLOWGREEN, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 85, 80, 150, 80, 0, 0);
	#endif


	/* Clear the lines */
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 80, 105, 160, 30, 0, 0);

	/* Draw horizontal line with green color */
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_GREEN, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, PAL_GREEN, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, 2, 0, 0, 0);
	sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, 85, 120, 235, 120);

	/* Draw vertical line with red color */
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PAL_RED, 0, 0, 0);
	#if 0
	sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, 160, 110, 160, 130);
	#else
	sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, centralPointX, CENTRALPOINT_Y-10, centralPointX, CENTRALPOINT_Y+10);
	#endif

	APP_OSD_REFRESH_ON;
}

void
appCvrwsCentralPointCal(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	
#if ICAT_FCWS || ICAT_LDWS
	if (puiPara->cvrwsRightCnt == 0 && puiPara->cvrwsLeftCnt == 0) {
		centralPointX = CENTRALPOINT_X;
	}
	else if (puiPara->cvrwsRightCnt > 0) {
		centralPointX = CENTRALPOINT_X + (puiPara->cvrwsRightCnt*CENTRALPOINT_UNIT);
	}
	else if (puiPara->cvrwsLeftCnt > 0) {
		centralPointX = CENTRALPOINT_X - (puiPara->cvrwsLeftCnt*CENTRALPOINT_UNIT);
	}
	printf("%s %s RightCnt=%d, LeftCnt=%d\n", CVRWS_STR, __FUNCTION__, puiPara->cvrwsRightCnt, puiPara->cvrwsLeftCnt);
	printf("%s X=%d, Y=%d(320x240)\n", CVRWS_STR, centralPointX, CENTRALPOINT_Y);
#endif
}

void
appCvrwsCentralPointUpdate(
	appCvrwsCalib_e param
)
{
	uiPara_t* puiPara = appUiParaGet();
#if ICAT_FCWS || ICAT_LDWS
	
	printf("%s %s RightCnt=%d, LeftCnt=%d\n", CVRWS_STR, __FUNCTION__, puiPara->cvrwsRightCnt, puiPara->cvrwsLeftCnt);
	if ((param == CVRWS_CALIB_MOVE_RIGHT && puiPara->cvrwsRightCnt < CENTRALPOINT_CNT_MAX) ||
		(param == CVRWS_CALIB_MOVE_LEFT && puiPara->cvrwsLeftCnt < CENTRALPOINT_CNT_MAX)) {

		if (param == CVRWS_CALIB_MOVE_RIGHT) { /* right move */
			if ((puiPara->cvrwsRightCnt == 0 && puiPara->cvrwsLeftCnt == 0) || puiPara->cvrwsRightCnt > 0) {
				puiPara->cvrwsRightCnt++;
			}
			else {
				puiPara->cvrwsLeftCnt--;
			}
		}
		else if (param == CVRWS_CALIB_MOVE_LEFT) { /* left move */
			if ((puiPara->cvrwsLeftCnt == 0 && puiPara->cvrwsRightCnt == 0) || puiPara->cvrwsLeftCnt > 0) {
				puiPara->cvrwsLeftCnt++;
			}
			else {
				puiPara->cvrwsRightCnt--;
			}
		}

		appCvrwsCentralPointCal();
		appCvrwsOsdDraw();
		appCvrwsCentralPointSet();
	}
#endif
}

void appCvrwsCentralPointSet(
	void
)
{
	UINT32 cvrwsPointX = 0, cvrwsPointY = 0;

	/* translate to 1024x1024 domain from 320x240 domain */
	cvrwsPointX = (centralPointX*1024) / 320;
	cvrwsPointY = (CENTRALPOINT_Y*1024) / 240;

	printf("%s X=%d, Y=%d(1024x1024)\n", CVRWS_STR, cvrwsPointX, cvrwsPointY);
	sp5kCvrWarningSystemCfgSet(SP5K_CVR_WARNING_SYSTEM_CENTRAL_POINT_SET, cvrwsPointX, cvrwsPointY);
}

void
appCvrwsEnableSet(
	UINT8 enable
)
{
	#if ICAT_LDWS || ICAT_LDWS
	if (enable) {
		if (appVidFrameRateGet(gVideoSizeTag) == 30) {
			sp5kPreviewCallbackSet(1, appCvrWarningSystemFrameProc, 640, 360);
		}
		else {
			sp5kPreviewCallbackSet(2, appCvrWarningSystemFrameProc, 640, 360);
		}
		appStill_SetFaceTracking(VIEW_FT_STATUS_RUN);
		appCvrwsOsdDraw();
	}
	else {
		sp5kPreviewCallbackSet(1, NULL, 0, 0);
		appStill_SetFaceTracking(VIEW_FT_STATUS_PAUSE);
		APP_OSD_REFRESH_OFF;
		APP_SLEEP_MS(2000);
		/* clear CVRWS OSD */
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 50, 50, 250, 100, 0, 0);
		APP_OSD_REFRESH_ON;
	}
	#endif
}

