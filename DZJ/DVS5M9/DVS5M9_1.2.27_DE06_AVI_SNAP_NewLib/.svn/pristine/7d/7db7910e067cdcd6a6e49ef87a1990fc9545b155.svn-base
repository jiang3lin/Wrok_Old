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
#include "sp5k_rsvblk_api.h"
#include "sp5k_modesw_api.h"
#include "sp5k_gfx_api.h"
#include "app_osd_api.h"
#include "app_ui_msg.h"
#include "app_pal.h"
#include "app_cvrws_api.h"
#include "app_osd.h"

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
static UINT8 dayNightFlag = 1;
appFCWSInfo_t fcwsInfo;
UINT16 Vehicle_LV;

#if ICAT_CVR_WARN_SYS_DBG
static UINT8 fcwsLog[100] = {0};
#if CVR_WARN_SYS_INT_DBG
static UINT8 fcwsWarnFlag = 1;
UINT32 fcwsWarnCnt = 0;
#endif
#endif

UINT8 FCWS_LUT[768] = {	/* Y: 0-255, U: 256-511, V: 512-767 */
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};

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
appForwardCollisionWarnSysParamSet(
	void
)
{
	printf("%s dayNightFlag=%d\n", FCWS_STR, dayNightFlag);

	/* 1. Setting camera focal length parameter */
	sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_SENSOR_FOCAL_SET, FCWS_SENSOR_FOCAL);

	/* 2. Setting unit pixel width paramter */
	sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_SENSOR_PIXEL_UNIT_SET, FCWS_SENSOR_PIXEL_UNIT);

	/* 3. Setting warning distance parameter */
	sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_WARN_DISTANCE_SET, FCWS_WARN_DISTANCE);

	/* 4. Setting crop factor and crop offset parameter
	* This setting affects the size of the vehicle can be detected
	* Total width: 1024
	*/
	/*
		<---------------------- 1024 ------------------->
					<--- ROI_CROP_FACTOR --->
					 _________________________
					|						   |
					|					          |
					|						   |
		<-OFFSET->	|_________________________|
	 */
	sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_ROI_CROP_FACTOR_SET, FCWS_ROI_CROP_FACTOR);
	sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_ROI_CROP_OFFSET_SET, FCWS_ROI_CROP_OFFSET);

	/* 5. Setting YUV threshold LUT table for night mode */
	sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_NIGHT_YUV_LUT_TABLE_SET, (UINT32)(&FCWS_LUT[0]));

 	/* 6. Setting day or night flag */
	sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_DAYNIGHT_SET, dayNightFlag);
}

void
appForwardCollisionWarningDayNightExchange(
	UINT16 lvIdx
)
{
	UINT8 newdayNightFlag;

	Vehicle_LV = lvIdx * 1000 / 16;

	if (Vehicle_LV < 6000) {
		newdayNightFlag = 0; /* night */
	}
	else {
		newdayNightFlag = 1; /* day */
	}

	if (dayNightFlag != newdayNightFlag) {
		printf("%s DAYNIGHT_EXCHANGE dayNightFlag=%d newdayNightFlag=%d\n", FCWS_STR, dayNightFlag, newdayNightFlag);
		dayNightFlag = newdayNightFlag;
		sp5kVehicleDetectCfgSet(SP5K_FORWARD_COLLISION_DAYNIGHT_SET, dayNightFlag);
	}
}

#if ICAT_CVR_WARN_SYS_DBG
void
appForwardCollisionWarningInfo(
	void
)
{
	vehicleDetectGetInfo_t vdInfor;

	#if CVR_WARN_SYS_INT_DBG
	UINT32 modeGet;
	sp5kModeGet(&modeGet);
	#endif

	memset(fcwsLog, 0, 100);
	appOsd_GfxFontIDSet(SP5K_RES_OSD_FONT);
	sp5kVehicleDetectResultGet(&vdInfor);
	//printf("%s vdInfor.vehicleDistance=%d\n", FCWS_STR, vdInfor.vehicleDistance);
	//printf("%s vdInfor.vehicleWarn=%d\n", FCWS_STR, vdInfor.vehicleWarn);

	if (vdInfor.vehicleWarn == 1) { /* FCWS alert */
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 50, 50, 25, 40, 0, 0);
		sp5kGfxStringDraw(SP5K_GFX_PAGE_OSD_0, 50, 50, SP5K_GFX_ALIGN_TOP_LEFT, SP5K_GFX_ENCODE_ASCII, "W");
		sprintf(fcwsLog, "D:%d Warn, LV:%d", vdInfor.vehicleDistance, Vehicle_LV);
		#if CVR_WARN_SYS_INT_DBG
		if (modeGet == SP5K_MODE_VIDEO_RECORD) {
			if (fcwsWarnFlag) {
				fcwsWarnCnt++;
				fcwsWarnFlag = 0;
			}
		}
		#endif
	}
	else {
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 50, 50, 25, 40, 0, 0);
		sprintf(fcwsLog, "D:%d Safe, LV:%d", vdInfor.vehicleDistance, Vehicle_LV);
		#if CVR_WARN_SYS_INT_DBG
		fcwsWarnFlag = 1;
		#endif
	}
}

void
appFCWS_DbgMsg(
	UINT32 stampPageId
)
{
	sp5kGfxStringDraw(stampPageId, 60, 120, SP5K_GFX_ALIGN_TOP_LEFT, SP5K_GFX_ENCODE_ASCII, fcwsLog);
}
#endif

