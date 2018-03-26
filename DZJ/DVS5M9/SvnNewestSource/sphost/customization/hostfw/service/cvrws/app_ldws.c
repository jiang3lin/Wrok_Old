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
#include "api/sp5k_global_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_modesw_api.h"
#include "sp5k_cvr_warning_system_api.h"
#include "sp5k_cvr_warning_system_private.h"
#include "sp5k_rsvblk_api.h"
#include "app_com_api.h"
#include "app_osd_api.h"
#include "app_pal.h"
#include "app_cvrws_api.h"

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
static appLDWSInfo_t ldwsInfo;
static UINT8 ldwsWarnMsgCnt = 0;
static UINT32 LaneX1 = 0, LaneX2 = 0, LaneX3 = 0, LaneX4 = 0;

#if ICAT_CVR_WARN_SYS_DBG
static UINT8 ldwsLog[100] = {0};
#if CVR_WARN_SYS_INT_DBG
static UINT8 ldwsWarnFlag = 1;
UINT32 ldwsBothCnt = 0, ldwsRightCnt = 0, ldwsLeftCnt = 0;
#endif
#endif


/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

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
appLaneDepartureWarnSysParamSet(
	void
)
{
	sp5kCvrWarningSystemCfgSet(SP5K_LANE_DEPARTURE_WARNING_THRESHOLD_SET, LDWS_ANGLE, LDWS_THR);
}

UINT32
appCvrWarningSystemFrameProc(
	UINT32 frmId,
	UINT8 *buf,
	UINT32 width,
	UINT32 height
)
{
	sp5kCvrWarningSystemFunDo(buf, width, height);
	return 0;
}

void
appLaneDepartureWarnSysCoorTranslate(
	void
)
{
	sp5kCvrWarningSystemCfgGet(SP5K_LANE_DEPARTURE_COORD_INFOR_GET, &ldwsInfo);

	ldwsInfo.ftx1 = (ldwsInfo.ftx1*320)>>10;
	ldwsInfo.ftx2 = (ldwsInfo.ftx2*320)>>10;
	ldwsInfo.ftx3 = (ldwsInfo.ftx3*320)>>10;
	ldwsInfo.ftx4 = (ldwsInfo.ftx4*320)>>10;
}

void
appLaneDepartureWarningDraw(
	UINT32 param
)
{
	UINT32 modeGet;
	sp5kModeGet(&modeGet);

	#if ICAT_FCWS
	appCvrwsOsdDraw();
	#endif

	appLaneDepartureWarnSysCoorTranslate();
	LaneX1 = ldwsInfo.ftx1;
	LaneX2 = ldwsInfo.ftx2;
	LaneX3 = ldwsInfo.ftx3;
	LaneX4 = ldwsInfo.ftx4;
	#if 0 && ICAT_CVR_WARN_SYS_DBG
	printf("%s[0] LaneX1=0x%x\n", LDWS_STR, LaneX1);
	printf("%s[0] LaneX2=0x%x\n", LDWS_STR, LaneX2);
	printf("%s[0] LaneX3=0x%x\n", LDWS_STR, LaneX3);
	printf("%s[0] LaneX4=0x%x\n", LDWS_STR, LaneX4);
	#endif

	if ((param == 3 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG_REC && modeGet == SP5K_MODE_VIDEO_RECORD) ||
		(param == 3 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG && modeGet == SP5K_MODE_VIDEO_PREVIEW)) { /* Both shift */
		RPrintf("Both Shift Sound Play");
		appBeep(TRUE, FALSE);
	}
	else if ((param == 1 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG_REC && modeGet == SP5K_MODE_VIDEO_RECORD) ||
		(param == 1 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG && modeGet == SP5K_MODE_VIDEO_PREVIEW)) { /* Left shift */
		RPrintf("Left Shift Sound Play");
		appBeep(TRUE, FALSE);
	}
	else if ((param == 2 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG_REC && modeGet == SP5K_MODE_VIDEO_RECORD) ||
		(param == 2 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG && modeGet == SP5K_MODE_VIDEO_PREVIEW)) { /* Right shift */
		RPrintf("Right Shift Sound Play");
		appBeep(TRUE, FALSE);
	}

	if (ldwsInfo.ftx1 == 0xff || ldwsInfo.ftx2 == 0xff || ldwsInfo.ftx3 == 0xff || ldwsInfo.ftx4 == 0xff ||
		ldwsInfo.ftx1 < 85 || ldwsInfo.ftx2 < 85 || ldwsInfo.ftx3 > 235 || ldwsInfo.ftx4 > 235) {
		return;
	}

	if ((param == 3 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG_REC && modeGet == SP5K_MODE_VIDEO_RECORD) ||
		(param == 3 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG && modeGet == SP5K_MODE_VIDEO_PREVIEW)) { /* Both shift */
		RPrintf("Both Shift");
		ldwsWarnMsgCnt = 0;
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX1, 138, LaneX2, 166);
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX3, 138, LaneX4, 166);
		#if 0
		if(!SOUND_ONing)
		{
			appPreloadSoundPlay(SOUND_ID_WARNING_DAS);
		 	SOUND_ONing=1;
		}
		#endif
	}
	else if ((param == 1 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG_REC && modeGet == SP5K_MODE_VIDEO_RECORD) ||
		(param == 1 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG && modeGet == SP5K_MODE_VIDEO_PREVIEW)) { /* Left shift */
		RPrintf("Left Shift");
		ldwsWarnMsgCnt = 0;
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX1, 138, LaneX2, 166);
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX3, 138, LaneX4, 166);
		#if 0
		if(!SOUND_ONing)
		{
			appPreloadSoundPlay(SOUND_ID_WARNING_DAS);
		 	SOUND_ONing=1;
		}
		#endif
	}
	else if ((param == 2 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG_REC && modeGet == SP5K_MODE_VIDEO_RECORD) ||
		(param == 2 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG && modeGet == SP5K_MODE_VIDEO_PREVIEW)) { /* Right shift */
		RPrintf("Right Shift");
		ldwsWarnMsgCnt = 0;
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX1, 138, LaneX2, 166);
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_WARN_COLOR, 0, 0, 0);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX3, 138, LaneX4, 166);
		#if 0
		if(!SOUND_ONing)
		{
			appPreloadSoundPlay(SOUND_ID_WARNING_DAS);
		 	SOUND_ONing=1;
		}
		#endif
	}
	else if ((param == 0 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG_REC && modeGet == SP5K_MODE_VIDEO_RECORD) ||
		(param == 0 && ldwsWarnMsgCnt >= LDWS_WARN_CNT_DBG && modeGet == SP5K_MODE_VIDEO_PREVIEW)) { /* Safe */
		RPrintf("No Shift");
		ldwsWarnMsgCnt = 0;
		sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
		sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, LDWS_SAFE_COLOR, 0, 0, 0);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX1, 138, LaneX2, 166);
		sp5kGfxLineDraw(SP5K_GFX_PAGE_OSD_0, LaneX3, 138, LaneX4, 166);
	}
}

void
appLaneDepartureWarningUpdate(
	UINT32 param
)
{
	ldwsWarnMsgCnt++;
	//printf("P:%d,C:%d\n",param,ldwsWarnMsgCnt);
	APP_OSD_REFRESH_OFF;

	if (ldwsWarnMsgCnt == 5) {
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 50, 135, 250, 45, 0, 0);
	}
	appLaneDepartureWarningDraw(param);

	#if ICAT_CVR_WARN_SYS_DBG
	appLaneDepartureWarningDbg(param);
	#endif
	APP_OSD_REFRESH_ON;
}

#if ICAT_CVR_WARN_SYS_DBG
void
appLaneDepartureWarningDbg(
	UINT32 param
)
{
	static UINT32 preParam;

	memset(ldwsLog, 0, 100);
	#if CVR_WARN_SYS_INT_DBG
	UINT32 modeGet;
	sp5kModeGet(&modeGet);
	#endif
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);

	if (param == 3) { /* Both shift */
		sprintf(ldwsLog, "B Lane: %d %d %d %d", LaneX1, LaneX2, LaneX3, LaneX4);
		#if CVR_WARN_SYS_INT_DBG
		if (modeGet == SP5K_MODE_VIDEO_RECORD) {
			if (ldwsWarnFlag) {
				ldwsBothCnt++;
				ldwsWarnFlag = 0;
			}
		}
		#endif
	}
	else if (param == 1) { /* Left shift */
		sprintf(ldwsLog, "L Lane: %d %d %d %d", LaneX1, LaneX2, LaneX3, LaneX4);
		#if CVR_WARN_SYS_INT_DBG
		if (modeGet == SP5K_MODE_VIDEO_RECORD) {
			if (ldwsWarnFlag) {
				ldwsLeftCnt++;
				ldwsWarnFlag = 0;
			}
		}
		#endif
	}
	else if (param == 2) { /* Right shift */
		sprintf(ldwsLog, "R Lane: %d %d %d %d", LaneX1, LaneX2, LaneX3, LaneX4);
		#if CVR_WARN_SYS_INT_DBG
		if (modeGet == SP5K_MODE_VIDEO_RECORD) {
			if (ldwsWarnFlag) {
				ldwsRightCnt++;
				ldwsWarnFlag = 0;
			}
		}
		#endif
	}
	else { /* Safe */
		sprintf(ldwsLog, "Lane: %d %d %d %d", LaneX1, LaneX2, LaneX3, LaneX4);
		#if CVR_WARN_SYS_INT_DBG
		if (modeGet == SP5K_MODE_VIDEO_RECORD) {
			ldwsWarnFlag = 1;
		}
		#endif
	}

	if (preParam != param) {
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 80, 50, 50, 40, 0, 0);
		preParam = param;
	}
	else {
		return;
	}

	if (param == 3) { /* Both shift */
		sp5kGfxStringDraw(SP5K_GFX_PAGE_OSD_0, 80, 50, SP5K_GFX_ALIGN_TOP_LEFT, SP5K_GFX_ENCODE_ASCII, "B");
	}
	else if (param == 1) { /* Left shift */
		sp5kGfxStringDraw(SP5K_GFX_PAGE_OSD_0, 80, 50, SP5K_GFX_ALIGN_TOP_LEFT, SP5K_GFX_ENCODE_ASCII, "L");
	}
	else if (param == 2) { /* Right shift */
		sp5kGfxStringDraw(SP5K_GFX_PAGE_OSD_0, 80, 50, SP5K_GFX_ALIGN_TOP_LEFT, SP5K_GFX_ENCODE_ASCII, "R");
	}
}

void
appLDWS_DbgMsg(
	UINT32 stampPageId
)
{
	sp5kGfxStringDraw(stampPageId, 280, 120, SP5K_GFX_ALIGN_TOP_LEFT, SP5K_GFX_ENCODE_ASCII, ldwsLog);
}
#endif

