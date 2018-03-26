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
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_state_ctrl.h"
#include "app_zoom_api.h"
#include "app_ui_msg.h"
#include "app_audio.h"
#include "app_view_osd.h"
#include "app_osd_api.h"
#include "app_ui_para.h" 
#include "app_view_param.h" 
#include "app_still.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define APP_DZOOM_RATIO_TOTAL_NUM	(sizeof(gdzoomRatioArray)/sizeof(UINT32))
#define APP_DZOOM_MAX_RATIO_INDEX	(APP_DZOOM_RATIO_TOTAL_NUM-1)
#define APP_DZOOM_MID_RATIO_INDEX	(APP_DZOOM_RATIO_TOTAL_NUM-33) /*add for it  can only support zoom in <2 when 16 in 1 capture*/
#define APP_DZOOM_MIN_RATIO_INDEX	(0)

/*-- to advance ratio index by 1 according current dzoom direction */
#define ADV_NEXT_DZOOM_RATIO_INDEX(a)	do { \
											if (a==APP_ZOOM_TELE) gdzoomCb.ratioIdx++; \
											else if (a==APP_ZOOM_WIDE) gdzoomCb.ratioIdx--; \
										} while(0)

#define ADV_NEXT_DZOOM_SEG_INDEX(a)		do { \
											if (a==APP_ZOOM_TELE) gdzoomCb.curSeg++; \
											else if (a==APP_ZOOM_WIDE) gdzoomCb.curSeg--; \
										} while(0)

#if ICAT_WIFI
#define WIFI_DZOOM_RATIO_INDEX(a)	do { \
										if (a==APP_ZOOM_TELE) gdzoomCb.ratioIdx+=3; \
										else if (a==APP_ZOOM_WIDE) gdzoomCb.ratioIdx-=3; \
										} while(0)
											
#define WIFI_DZOOM_SEG_INDEX(a)		do { \
										if (a==APP_ZOOM_TELE) gdzoomCb.curSeg+=3; \
										else if (a==APP_ZOOM_WIDE) gdzoomCb.curSeg-=3; \
										} while(0)
											
#endif

#define IS_DZOOM_TELE_MAX_RATIO(a)	(a==APP_ZOOM_TELE && gdzoomCb.ratioIdx==APP_DZOOM_MAX_RATIO_INDEX)
#define IS_DZOOM_WIDE_MIN_RATIO(a)	(a==APP_ZOOM_WIDE && gdzoomCb.ratioIdx==APP_DZOOM_MIN_RATIO_INDEX)
/*add for it can onlysupport zoom in <2 when 16 in 1 capture*/
#define IS_DZOOM_TELE_MID_RATIO(a)	(a==APP_ZOOM_TELE && gdzoomCb.ratioIdx==APP_DZOOM_MID_RATIO_INDEX)

#define EXTRACT_DZOOM_RATIO(a)		(a & (~DZOOM_SEG_MARKER))
#define IS_PRIMARY_DZOOM_STOP(a)	(a & DZOOM_SEG_MARKER)


#define SET_UI_DZOOM_RATIO(m, r)	do {sp5kZoomFactorSet(((appDZoomUiMode_t)m)&0xFF, r); } while(0)

#define IS_DZOOM_TELE_REPEATED(a)	(a==APP_ZOOM_TELE && IS_KEY_TELE_PRESSED)
#define IS_DZOOM_WIDE_REPEATED(a)	(a==APP_ZOOM_WIDE && IS_KEY_WIDE_PRESSED)



/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static appDZoomCtrl_t gdzoomCb;

/*-- still & view DZOOM ratio table
 *   To mark the primary DZOOM stop, please add
 *   DZOOM_SEG_MARKER to the actual ratio.
 */
static UINT32 gdzoomRatioArray[] =
{
/*s Modify by Aries 10/02/04*/
#if 1 //(_CAM_A900_EVM)||(_CAM_M900_EVM)||(_CAM_A900_D3) ||(_CAM_CATHY_EVM)
	
	1000+DZOOM_SEG_MARKER,	  
	1022,					  
	1044+DZOOM_SEG_MARKER,	  
	1067,					  
	1091+DZOOM_SEG_MARKER,	  
	1114,					  
	1139+DZOOM_SEG_MARKER,	  
	1164,					  
	1189+DZOOM_SEG_MARKER,	  
	1215,					  
	1242+DZOOM_SEG_MARKER,	  
	1269,					  
	1297+DZOOM_SEG_MARKER,	  
	1325,					  
	1354+DZOOM_SEG_MARKER,	  
	1384,					  
	1414+DZOOM_SEG_MARKER,	  
	1445,					  
	1477+DZOOM_SEG_MARKER,	  
	1509,					  
	1542+DZOOM_SEG_MARKER,	  
	1576,					  
	1610+DZOOM_SEG_MARKER,	  
	1646,					  
	1682+DZOOM_SEG_MARKER,	  
	1719,					  
	1756+DZOOM_SEG_MARKER,	  
	1795,					  
	1834+DZOOM_SEG_MARKER,	  
	1874,					  
	1915+DZOOM_SEG_MARKER,	  
	1957,					  
	2000+DZOOM_SEG_MARKER,	  
	2044,					  
	2089+DZOOM_SEG_MARKER,	  
	2134,					  
	2181+DZOOM_SEG_MARKER,	  
	2229,					  
	2278+DZOOM_SEG_MARKER,	  
	2327,					  
	2378+DZOOM_SEG_MARKER,	  
	2430,					  
	2484+DZOOM_SEG_MARKER,
	#if !ICAT_WIFI /* only support to 2.5 x for WiFi streaming usage */
	2538,					  
	2594+DZOOM_SEG_MARKER,	  
	2650,					  
	2709+DZOOM_SEG_MARKER,	  
	2768,					  
	2828+DZOOM_SEG_MARKER,	  
	2890,					  
	2954+DZOOM_SEG_MARKER,	  
	3018,					  
	3084+DZOOM_SEG_MARKER,	  
	3152,					  
	3221+DZOOM_SEG_MARKER,	  
	3292,					  
	3364+DZOOM_SEG_MARKER,	  
	3437,					  
	3513+DZOOM_SEG_MARKER,	  
	3589,					  
	3668+DZOOM_SEG_MARKER,	  
	3748,					  
	3830+DZOOM_SEG_MARKER,	  
	3914,					  
	4000+DZOOM_SEG_MARKER,	  
	#endif
	
	#if 0
	(1000+DZOOM_SEG_MARKER),
	1078,
	1161,
	1251,
	(1348+DZOOM_SEG_MARKER),
	1452,
	1565,
	1686,
	(1817+DZOOM_SEG_MARKER),
	1958,
	2110,
	2273,
	(2449+DZOOM_SEG_MARKER),
	2639,
	2844,
	3064,
	(3302+DZOOM_SEG_MARKER),
	3558,
	3834,
	(4000+DZOOM_SEG_MARKER),
	#endif
/* Eric 100527 Digital Zoom fix to 4.0x */	
/*	4131, 
	(4451+DZOOM_SEG_MARKER),
	4796,
	5168,
	5568,
	(6000+DZOOM_SEG_MARKER)
*/	
#else

	(1000+DZOOM_SEG_MARKER),
	1040,
	1080,
	(1100+DZOOM_SEG_MARKER),
	1140,
	1180,
	1220,
	1260,
	(1320+DZOOM_SEG_MARKER),
    1360,
    1400,
    1460,
    1500,
    (1560+DZOOM_SEG_MARKER),
    1600,
    1680,
    1760,
    (1800+DZOOM_SEG_MARKER),
    1900,
    2060,
    2120,
    (2200+DZOOM_SEG_MARKER),
    2300,
    2400,
    2500,
    (2600+DZOOM_SEG_MARKER),
    2700,
    2800,
    2900,
    (3000+DZOOM_SEG_MARKER),
    3280,
    (3400+DZOOM_SEG_MARKER),
    3580,
    3780,
    (4000+DZOOM_SEG_MARKER)	/* PRV_DZOOM_MAX */
 #endif
/*e Modify by Aries 10/02/04*/

};

#define APP_DZOOM_DISTANCE (EXTRACT_DZOOM_RATIO(gdzoomRatioArray[APP_DZOOM_MAX_RATIO_INDEX])-1000)

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appIqSharpnessSet(UINT32 mode, UINT32 value); // for clean warning

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : zoomUIKeyControl
 *------------------------------------------------------------------------*/
static
void
zoomUIKeyControl(
	UINT8 zoomSxn
)
{
	/* %TODO% */
	return;
}


/*-------------------------------------------------------------------------
 *  File Name : dzoomRatioSyncAssociatedMode
 *------------------------------------------------------------------------*/
static
void
dzoomRatioSyncAssociatedMode(
	UINT32	uiMode,
	UINT32	ratio
)
{
	if (uiMode==APP_DZOOM_UI_MODE_STILL_VIEW) {
		sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, ratio);
	} else if (uiMode==APP_DZOOM_UI_MODE_VIDEO_VIEW) {
		sp5kZoomFactorSet(APP_DZOOM_UI_MODE_VIDEO_REC, ratio);
	} else if (uiMode==APP_DZOOM_UI_MODE_VIDEO_REC) {
		sp5kZoomFactorSet(APP_DZOOM_UI_MODE_VIDEO_VIEW, ratio);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appDZoomTeleWide
 *------------------------------------------------------------------------*/
static
UINT32
appDZoomTeleWide(
	UINT8	zoomDir,
	UINT32	uiMode
)
{
	UINT32 comboRatio, realRatio;
	UINT32 ratioDist;
	UINT32 ulsp5kmode = 0;
	uiPara_t* puiPara = appUiParaGet();
	/*-- Param validatiy check
	 */
	if (gdzoomCb.isEnabled==FALSE) 
	{
		DBG_PRINT("\n%s: DZOOM Disabled !!\n\n", __FUNCTION__);
		return SUCCESS;
	}

	if (zoomDir!=APP_ZOOM_TELE && zoomDir!=APP_ZOOM_WIDE) 
	{
		printf("\n%s: !! ERROR !!, illegal zoomDir(%d) \n\n", __FUNCTION__, zoomDir);
		return FAIL;
	}
/*add for it  can only support zoom in <2 when 16 in 1 capture*/
	if(puiPara->FeatureEffect == UI_FEATURE_EFFECT_16IN1) 
	{
		if (IS_DZOOM_TELE_MID_RATIO(zoomDir)) 
		{
			DBG_PRINT("Skipped, Midmax DZOOM ratio!!\n");
			appViewOsd_ZoomFrameDraw(TRUE);
			appViewOsd_DigZoomPosDraw(100);
			return SUCCESS;
		} 
		else if (IS_DZOOM_WIDE_MIN_RATIO(zoomDir)) 
		{
			DBG_PRINT("Skipped, Midmin DZOOM ratio!!\n");
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,  VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, DIGITAL_ZOOM_BAR_X, DIGITAL_ZOOM_BAR_Y, 0, 0);
			return SUCCESS;
		}
	}
	else
	{
		if (IS_DZOOM_TELE_MAX_RATIO(zoomDir)) 
		{
			DBG_PRINT("Skipped, Max DZOOM ratio!!\n");
			appViewOsd_ZoomFrameDraw(TRUE);
			appViewOsd_DigZoomPosDraw(100);
			return SUCCESS;
		} 
		else if (IS_DZOOM_WIDE_MIN_RATIO(zoomDir)) 
		{
			DBG_PRINT("Skipped, Min DZOOM ratio!!\n");
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,  VIEW_ZOOM_FRAME_X, VIEW_ZOOM_FRAME_Y, DIGITAL_ZOOM_BAR_X, DIGITAL_ZOOM_BAR_Y, 0, 0);
			if(gStillCB.osd == OSD_VIEW_GRID) /*add for manrtis bug 0047356*/
			{
				appViewOsd_AllClean();
				appViewOsd_GuideLineDraw(TRUE);
			}
			return SUCCESS;
		}
	}
	/*-- Disable keys
	 */
	zoomUIKeyControl(APP_ZOOM_SESSION_START);

	/*-- Approaching DZOOM
	 */
	switch(uiMode){
		case APP_DZOOM_UI_MODE_STILL_VIEW:
			ulsp5kmode = SP5K_MODE_STILL_PREVIEW;
		    break;
		case APP_DZOOM_UI_MODE_VIDEO_VIEW:
			ulsp5kmode = SP5K_MODE_VIDEO_PREVIEW;
		    break;
		case APP_DZOOM_UI_MODE_VIDEO_REC:
			ulsp5kmode = SP5K_MODE_VIDEO_RECORD;
		    break;
		default:
			break;
	}

	comboRatio = gdzoomRatioArray[gdzoomCb.ratioIdx];
	realRatio = EXTRACT_DZOOM_RATIO(comboRatio);

#if 0 /*function isn't ready*/
	if(realRatio == 2000 && zoomDir == APP_ZOOM_TELE)
	{
		sp5kSensorModeCfgSet(ulsp5kmode, 0x31);
		sp5kModeSet(ulsp5kmode);
		sp5kModeWait(ulsp5kmode);
	}
	else if(realRatio == 2000 && zoomDir == APP_ZOOM_WIDE)
	{
		sp5kSensorModeCfgSet(ulsp5kmode, 0x30);
		sp5kModeSet(ulsp5kmode);
		sp5kModeWait(ulsp5kmode);
	}

#endif	
	do 
	{
		ADV_NEXT_DZOOM_RATIO_INDEX(zoomDir);
		/* to advance DZOOM ratio one step according to
		 * current DZOOM direction. E.g., for DZOOM_TELE --> +1,
		 * for DZOOM_WIDE --> -1
		 */

		comboRatio = gdzoomRatioArray[gdzoomCb.ratioIdx];
		realRatio = EXTRACT_DZOOM_RATIO(comboRatio);

		SET_UI_DZOOM_RATIO(uiMode, realRatio);
	} while (!IS_PRIMARY_DZOOM_STOP(comboRatio));

	/*-- Advance current DZOOM segment index by 1 stop according to zoomDir
	 */
	ADV_NEXT_DZOOM_SEG_INDEX(zoomDir);

	/*-- Synchronize associated mode DZOOM ratio */
	dzoomRatioSyncAssociatedMode(uiMode, realRatio);
		
	/*-- Post MSG for DZOOM OSD Bar update */
	ratioDist = realRatio - 1000;
	ratioDist = (ratioDist*100) / APP_DZOOM_DISTANCE;
	sp5kHostMsgSend(APP_UI_MSG_DZOOM_POS_UPDATE, ratioDist);
	/*-- check if Key repeated
	 */
	if (IS_DZOOM_TELE_REPEATED(zoomDir)) 
	{
		/*DBG_PRINT("DZOOM (T) Sending MSG \n"); */
		sp5kHostMsgSend(APP_UI_MSG_LENS_DZOOM_TELE, 0);
	} 
	else if (IS_DZOOM_WIDE_REPEATED(zoomDir)) 
	{
		/*DBG_PRINT("DZOOM (W) Sending MSG \n"); */
		sp5kHostMsgSend(APP_UI_MSG_LENS_DZOOM_WIDE, 0);
	}
	/*-- Enable keys */
	zoomUIKeyControl(APP_ZOOM_SESSION_END);  /*0x%08x*/

	DBG_PRINT("\nDZOOM : seg(%d), ratio(0x%08x) @ %u \n\n", gdzoomCb.curSeg, comboRatio, appMsTimeGet());

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appZoomTeleWide
 *------------------------------------------------------------------------*/
static
void
appZoomTeleWide(
	UINT8	zoomDir,
	UINT32	uiMode
)
{
	#if KIT_WITH_LENS
	UINT32 qryResult;
	#endif

	/*-- Param Validity check
	 */
	if (uiMode!=APP_DZOOM_UI_MODE_STILL_VIEW &&
		uiMode!=APP_DZOOM_UI_MODE_STILL_SNAP &&
		uiMode!=APP_DZOOM_UI_MODE_VIDEO_VIEW &&
		uiMode!=APP_DZOOM_UI_MODE_VIDEO_REC)
	{
		DBG_PRINT("\n%s: !! WARNING !!, illegal uiMode(%d) \n\n", __FUNCTION__, uiMode);
		return;
	}

	if (zoomDir!=APP_ZOOM_TELE && zoomDir!=APP_ZOOM_WIDE) 
	{
		DBG_PRINT("\n%s: !! WARNING !!, illegal zoomDir(%d) \n\n", __FUNCTION__, zoomDir);
		return;
	}

	/*-- Disable keys
	 */
	zoomUIKeyControl(APP_ZOOM_SESSION_START);

	/*-- Check whether go Optical ZOOM or DigiZOOM
	 */
	if (zoomDir==APP_ZOOM_TELE) 
	{
		#if KIT_WITH_LENS
		/* ZOOM-IN !! */
		qryResult = appLensQuery(LENS_QUERY_ZOOM_TELE_PARKED);
		if (!qryResult) 
		{
			/*-- Do optical zoom */
			if (SUCCESS==appLensTele()) 
			{
				#ifdef VIDEO_REC_MUTE_AT_LENS_ZOOMING
				if (APP_STATE_VIDEO_REC==appActiveStateGet()) 
				{
					appAudioRecVolumeMute();
				}
				#endif
			}
			zoomUIKeyControl(APP_ZOOM_SESSION_END);
			return;
		}
		#endif
	} 
	else 
	{
		/* ZOOM-OUT !! */
		if (gdzoomCb.curSeg==0) {
			/*-- DZOOM not active, go Lens Zoom */
			#if KIT_WITH_LENS
			qryResult = appLensQuery(LENS_QUERY_ZOOM_WIDE_PARKED);
			if (!qryResult) 
			{
				/*-- Do optical zoom */
				if (SUCCESS==appLensWide()) 
				{
					#ifdef VIDEO_REC_MUTE_AT_LENS_ZOOMING
					if (APP_STATE_VIDEO_REC==appActiveStateGet()) 
					{
						appAudioRecVolumeMute();
					}
					#endif
				}
				zoomUIKeyControl(APP_ZOOM_SESSION_END);
				return;
			}
			#endif
		}
	}

	/*-- Do DZoom */
	appDZoomTeleWide(zoomDir, uiMode);

	/*-- Enable keys */
	zoomUIKeyControl(APP_ZOOM_SESSION_END);
}



/*-------------------------------------------------------------------------
 *  File Name : appDZoomReset
 *------------------------------------------------------------------------*/
void
appDZoomReset(
	UINT32 uiMode
)
{
	memset(&gdzoomCb, 0, sizeof(appDZoomCtrl_t));

	switch ((uiMode&0xFF)) {
	default :
		DBG_PRINT("%s: !! WARNING !!, illegal uiMode(%x) \n", __FUNCTION__, uiMode);
		break;
	case SP5K_MODE_STILL_PREVIEW :
	case SP5K_MODE_STILL_SNAP:
		sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 1000);
		sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, 1000);
		/*s Modify by Aries 10/08/17*/
		#if 1
		appIqSharpnessSet(0,4); 
		#else
		appIqSharpnessSet(0,0);
		#endif
		/*e Modify by Aries 10/08/17*/
		break;
	case SP5K_MODE_VIDEO_PREVIEW:
	case SP5K_MODE_VIDEO_RECORD:
		sp5kZoomFactorSet(SP5K_MODE_VIDEO_PREVIEW, 1000);
	//	sp5kZoomFactorSet(SP5K_MODE_VIDEO_RECORD, 1000);
		/*s Modify by Aries 10/08/17*/
		#if 1
		appIqSharpnessSet(0,4); 
		#else
		appIqSharpnessSet(0,0);
		#endif
		/*e Modify by Aries 10/08/17*/
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appDZoomEnableCtrl
 *------------------------------------------------------------------------*/
void
appDZoomEnableCtrl(
	BOOL isEnable
)
{
	gdzoomCb.isEnabled = (isEnable) ? TRUE : FALSE;
}

/*-------------------------------------------------------------------------
 *  File Name : appZoomStop
 *------------------------------------------------------------------------*/
void
appZoomStop(
	void
)
{
	#if KIT_WITH_LENS
	appLensStop();
//	appLensStopWait();
	/*-- 0218, alex :
	 * Probably, we dont have to wait Lens stop still.
	 * appLensTele/Wide/Close will check and reject the command
	 * if Lens is still moving.
	 * Waiting Lens stop completely might block HOST cease the MSG popping.
	 * For example, HOST CANNOT update ZOOM bar at this period.
	 */
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appZoomNextModeQuery
 *------------------------------------------------------------------------*/
UINT32
appZoomNextModeQuery(
	UINT8	zoomDir
)
{
	#if KIT_WITH_LENS
	UINT32 qryResult;
	#endif

	/*-- Param Validity check
	 */
	#if KIT_WITH_LENS
	qryResult = appLensQuery(LENS_QUERY_MOTION_STATE);
	if (qryResult) {
		/*-- Lens is busy */
		return APP_ZOOM_NEXT_UNKNOWN;
	}
	#endif

	if (zoomDir!=APP_ZOOM_TELE && zoomDir!=APP_ZOOM_WIDE) {
		DBG_PRINT("\n%s: !! WARNING !!, illegal zoomDir(%d) \n\n", __FUNCTION__, zoomDir);
		return APP_ZOOM_NEXT_UNKNOWN;
	}

	/*-- Check whether go Optical ZOOM or DigiZOOM
	 */
	if (zoomDir==APP_ZOOM_TELE) {
		/* ZOOM-IN !! */
		#if KIT_WITH_LENS
			qryResult = appLensQuery(LENS_QUERY_ZOOM_TELE_PARKED);
			if (!qryResult) {
				/*-- Do optical zoom */
				return APP_ZOOM_NEXT_LENS;
			} else if (gdzoomCb.isEnabled) {
				/*-- Parked @ TELE_STOP & DZOOM is enabled */
				return APP_ZOOM_NEXT_DZOOM;
			} else {
				/*-- Parked @ TELE_STOP BUT DZOOM is disabled */
				return APP_ZOOM_NEXT_UNKNOWN;
			}
		#else
			/*-- KIT w/o LENS
			 */
			return ((gdzoomCb.isEnabled) ? APP_ZOOM_NEXT_DZOOM : APP_ZOOM_NEXT_UNKNOWN);
		#endif
	} else {
		/* ZOOM-OUT !! */
		#if KIT_WITH_LENS
			if (gdzoomCb.curSeg==0) {
				/*-- DZOOM not active, go Lens Zoom */
				qryResult = appLensQuery(LENS_QUERY_ZOOM_WIDE_PARKED);
				if (qryResult) {
					/*-- Already @ WIDE_STOP */
					return APP_ZOOM_NEXT_UNKNOWN;
				} else {
					return APP_ZOOM_NEXT_LENS;
				}
			} else {
				/*-- DZOOM on-going */
				return APP_ZOOM_NEXT_DZOOM;
			}
		#else
			/*-- KIT w/o LENS
			 */
			return ((gdzoomCb.isEnabled) ? APP_ZOOM_NEXT_DZOOM : APP_ZOOM_NEXT_UNKNOWN);
		#endif
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomTele
 *------------------------------------------------------------------------*/
void
appStillDZoomTele(
	void
)
{
	appDZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_STILL_VIEW);
}


/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomWide
 *------------------------------------------------------------------------*/
void
appStillDZoomWide(
	void
)
{
	appDZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_STILL_VIEW);
}

/*-------------------------------------------------------------------------
 *  File Name : appStillZoomTele
 *------------------------------------------------------------------------*/
void
appStillZoomTele(
	void
)
{
	appZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_STILL_VIEW);
}

/*-------------------------------------------------------------------------
 *  File Name : appStillZoomWide
 *------------------------------------------------------------------------*/
void
appStillZoomWide(
	void
)
{
	appZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_STILL_VIEW);
}

/*s Modify by Aries 10/03/16*/

/*-------------------------------------------------------------------------
 *  File Name : appVideZoomTele
 *------------------------------------------------------------------------*/
 void appVideoZoomTele(void)
{
	UINT32 State;
	State = appActiveStateGet();
	if (State == APP_STATE_VIDEO_REC)
	{
		appZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_REC);
	}
	else
	{
		appZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appVideZoomWide
 *------------------------------------------------------------------------*/
 void appVideoZoomWide(void)
{
	UINT32 State;
	State = appActiveStateGet();
	if (State == APP_STATE_VIDEO_REC)
	{
		appZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_REC);
	}
	else
	{
		appZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomTele
 *------------------------------------------------------------------------*/
void appVideoDZoomTele(void)
{
	UINT32 State;
	State = appActiveStateGet();
	if (State == APP_STATE_VIDEO_REC)
	{
		appDZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_REC);
	}
	else
	{
		appDZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomWide
 *------------------------------------------------------------------------*/
 void appVideoDZoomWide(void)
{
	UINT32 State;
	State = appActiveStateGet();
	if (State == APP_STATE_VIDEO_REC)
	{
		appDZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_REC);
	}
	else
	{
		appDZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
}
/*e Modify by Aries 10/03/16*/

/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomWide
 *------------------------------------------------------------------------*/
UINT32 
appDZoomGetRation(
	void
)
{
	return EXTRACT_DZOOM_RATIO(gdzoomRatioArray[gdzoomCb.ratioIdx]);
}

