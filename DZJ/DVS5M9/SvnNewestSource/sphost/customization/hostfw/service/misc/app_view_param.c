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
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_view_param.h"

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

/**************************************************************************
 *                          L O C A L    D A T A                          *
 **************************************************************************/

param_info_t PmodeParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,					PRM_SAVE,				UI_PIC_SIZE_FULL,		0 ),
	DEF_VIEW_PRM_INF(vidsize,				PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,				PRM_SAVE,				UI_QUALITY_FINE,		0 ),
	DEF_VIEW_PRM_INF(metering,				PRM_SAVE,				UI_METER_MULTI,			0 ),
	DEF_VIEW_PRM_INF(wb,					PRM_SAVE,				UI_WB_AUTO,				0 ),
	DEF_VIEW_PRM_INF(iso,					PRM_SAVE,				UI_ISO_AUTO,			0 ),
	DEF_VIEW_PRM_INF(ev,					PRM_SAVE,				UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,					PRM_NOSAVE,				UI_DRIVE_OFF,			0 ),
	DEF_VIEW_PRM_INF(aeb,					PRM_NOSAVE,				UI_AEB_OFF,				0 ),
	DEF_VIEW_PRM_INF(highlight, 			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(edge,					PRM_SAVE,				UI_SHARP_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(effect,				PRM_NOSAVE,				UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,					PRM_SAVE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(stablizer,				PRM_SAVE,				UI_STABILIZER_OFF,		0 ),
	DEF_VIEW_PRM_INF(facetrack,				PRM_SAVE,				UI_FACETRACK_OFF,		0 ),
	DEF_VIEW_PRM_INF(photoframe,			PRM_NOSAVE,				UI_PHOTOFRAME_OFF,		0 ),
	DEF_VIEW_PRM_INF(dzoom,					PRM_SAVE,				UI_DZOOM_ON,			0 ),
	DEF_VIEW_PRM_INF(af,					PRM_NOSAVE,				UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,				PRM_NOSEL,				SHUTTER_AUTO,			0 ),
	DEF_VIEW_PRM_INF(aperture,				PRM_NOSEL,				APERTURE_AUTO,			0 ),
	DEF_VIEW_PRM_INF(flash, 				PRM_SAVE,				UI_FLASH_OFF,			0 ),
	DEF_VIEW_PRM_INF(osd,					PRM_NOSAVE,				0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,			PRM_SAVE,				UI_PV_FLICKERMODE_60HZ,	0 ),
	DEF_VIEW_PRM_INF(viddist,				PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    			PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,		PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama  ,    		PRM_SAVE,				UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,				PRM_SAVE,				UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack, 		PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF, 	0 ),
	DEF_VIEW_PRM_INF(videoStamp, 			PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,			PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE, 	0 ),
	DEF_VIEW_PRM_INF(vidrsc,				PRM_NOUSE,				0,					 	0 ),
};

param_info_t WDRmodeParamInfo[VIEW_PRM_COUNT]={
	#if !CAM_TYPE_CVR
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,		UI_PIC_SIZE_5M,			0 ),
	#endif
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,					0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_SHARE|PRM_MODE_P,		UI_QUALITY_FINE,		0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_SHARE|PRM_MODE_P,		UI_METER_MULTI,			0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_SHARE|PRM_MODE_P,		UI_WB_AUTO,				0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_SHARE|PRM_MODE_P,		UI_ISO_AUTO,			0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_SHARE|PRM_MODE_P,		UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				0  ,						UI_DRIVE_OFF,			0 ),
	DEF_VIEW_PRM_INF(aeb,				0,							UI_AEB_OFF,				0 ),
	DEF_VIEW_PRM_INF(highlight, 		0,							UI_HIGHLIGHT_OFF,		0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_SHARE|PRM_MODE_P,		UI_SHARP_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(effect,			0,							UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_SHARE|PRM_MODE_P,		UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_SHARE|PRM_MODE_P,		UI_STABILIZER_OFF,		0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_SHARE|PRM_MODE_P,		UI_FACETRACK_ON,		0 ),
	DEF_VIEW_PRM_INF(photoframe,		0,							UI_PHOTOFRAME_OFF,		0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,		UI_DZOOM_ON,			0 ),
	DEF_VIEW_PRM_INF(af,				0,							UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_NOSEL,					SHUTTER_AUTO,			0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_NOSEL,					APERTURE_AUTO,			0 ),
	DEF_VIEW_PRM_INF(flash, 			PRM_SHARE|PRM_MODE_P,		UI_FLASH_OFF,			0 ),
	DEF_VIEW_PRM_INF(osd,				0,							0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,		UI_PV_FLICKERMODE_60HZ,	0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,					0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,		UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,					0,						0 ),
	DEF_VIEW_PRM_INF(panorama  ,    	PRM_SHARE|PRM_MODE_P,		UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,		UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack,	PRM_SHARE|PRM_MODE_P,		UI_FACETRACKING_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoStamp,		PRM_NOUSE,					UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,		UI_QUALITY_SUPERFINE,	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,					0, 						0 ),
};


param_info_t AmodeParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_SHARE|PRM_MODE_P,	UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(aeb,				0,						UI_AEB_OFF,				0x1 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(effect,			0,						UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_SAVE,				0,						0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(af,				PRM_SHARE|PRM_MODE_P,	UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			0,						SHUTTER_1_2000S,		0 ),
	DEF_VIEW_PRM_INF(aperture,			0,						APERTURE_2_8,			0 ),
	DEF_VIEW_PRM_INF(flash, 			0,						UI_FLASH_OFF,			0xfffe ),
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama,    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack,	PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoStamp,		PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE,	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,				0,				 		0 ),
};

param_info_t SmodeParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_SHARE|PRM_MODE_P,	UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(aeb,				0,						UI_AEB_OFF,				0x1 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(effect,			PRM_NOSEL,				UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_NOSEL			,	UI_STABILIZER_OFF,		0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(af,				PRM_SHARE|PRM_MODE_P,	UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_SHARE|PRM_MODE_A,	SHUTTER_1_2000S,		0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_SHARE|PRM_MODE_A,	APERTURE_2_8,			0 ),
	DEF_VIEW_PRM_INF(flash, 			0,						UI_FLASH_OFF,			0xfff6 ),
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama,    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack,	PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoStamp,		PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE,	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,				0, 						0 ),
};

param_info_t MmodeParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(iso,				0,						UI_ISO_100,				0xfffe ),
	DEF_VIEW_PRM_INF(ev,				PRM_NOSEL,				UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(aeb,				PRM_NOSEL,				UI_AEB_OFF,				0 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(effect,			PRM_NOSEL,				UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_NOSEL,				UI_STABILIZER_OFF,		0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(af,				PRM_SHARE|PRM_MODE_P,	UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_SHARE|PRM_MODE_A,	SHUTTER_1_2000S,		0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_SHARE|PRM_MODE_A,	APERTURE_2_8,			0 ),
	DEF_VIEW_PRM_INF(flash, 			0,						UI_FLASH_OFF,			0xfff6 ),
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama,    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack, 	PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF, 	0 ),
	DEF_VIEW_PRM_INF(videoStamp, 		PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE, 	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,				0, 						0 ),
};

param_info_t QmodeParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_NOSEL,				UI_QUALITY_SUPERFINE,	0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_NOSEL,				UI_METER_MULTI,			0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_NOSEL,				UI_WB_AUTO,				0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_NOSEL,				UI_ISO_AUTO,			0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_NOSEL,				UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(aeb,				PRM_NOSEL,				UI_AEB_OFF,				0 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_NOSEL,				UI_HIGHLIGHT_OFF,		0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_NOSEL,				UI_SHARP_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(effect,			PRM_NOSEL,				UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_NOSEL,				UI_STAMP_DATE_TIME,		0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_NOSEL,				UI_DZOOM_OFF,			0 ),
	DEF_VIEW_PRM_INF(af,				PRM_NOSEL,				UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_NOSEL,				SHUTTER_AUTO,			0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_NOSEL,				APERTURE_AUTO,			0 ),
	DEF_VIEW_PRM_INF(flash, 			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama  ,    	PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack, 	PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF, 	0 ),
	DEF_VIEW_PRM_INF(videoStamp, 		PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE, 	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,				0, 						0 ),
};

param_info_t SmileParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(metering,			0,						UI_METER_MULTI,			0x3 ),
	DEF_VIEW_PRM_INF(wb,				PRM_NOSEL,				UI_WB_AUTO,				0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_NOSEL,				UI_ISO_AUTO,			0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_NOSEL,				UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_NOSEL,				UI_DRIVE_OFF,			0 ),
	DEF_VIEW_PRM_INF(aeb,				PRM_NOSEL,				UI_AEB_OFF,				0 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_NOSEL,				UI_HIGHLIGHT_OFF,		0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_NOSEL,				UI_SHARP_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(effect,			0,						UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_NOSEL,				UI_FACETRACK_ON,		0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_NOSEL,				UI_PHOTOFRAME_OFF,		0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(af,				PRM_NOSEL,				UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_NOSEL,				SHUTTER_AUTO,			0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_NOSEL,				APERTURE_AUTO,			0 ),
	DEF_VIEW_PRM_INF(flash, 			0,						UI_FLASH_RED_EYE,		0x12 ),
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama  ,    	PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack, 	PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF, 	0 ),
	DEF_VIEW_PRM_INF(videoStamp, 		PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE, 	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,				0, 						0 ),
};

param_info_t VideoParamInfo[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_SAVE,				UI_VID_SIZE_FHD,		0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_SHARE|PRM_MODE_P,	UI_METER_MULTI,			0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_SHARE|PRM_MODE_P,	UI_WB_AUTO,				0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_NOSEL,				UI_ISO_AUTO,			0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_NOSEL,				UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(aeb,				PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(effect,			PRM_SAVE,				0,						0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_SAVE,				1,						0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_SAVE,				1,						0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(af,				PRM_NOSEL,				UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_NOSEL,				SHUTTER_AUTO,			0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_NOSEL,				APERTURE_AUTO,			0 ),
	DEF_VIEW_PRM_INF(flash, 			PRM_NOSEL,				UI_FLASH_OFF,			0 ),/*willow modify hang up after reset@20100714*/
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_SAVE,				UI_STABILIZER_OFF,		0 ),/*willow modify for dist off@20100714*/
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_SAVE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama  ,    	PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack,	PRM_SAVE,				UI_FACETRACKING_ON,		0 ),
	DEF_VIEW_PRM_INF(videoStamp,		PRM_SAVE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality  ,    PRM_SAVE,				UI_QUALITY_SUPERFINE,	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_SAVE,				UI_ROLLING_SHUTTER_OFF, 0 ),
};


param_info_t SceneParamInfo_People[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_NOSEL,				0,						0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_NOSEL,				UI_WB_AUTO,				0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_NOSEL,				UI_ISO_AUTO,			0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_NOSEL,				UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_SHARE|PRM_MODE_P,	UI_DRIVE_OFF,			0 ),
	DEF_VIEW_PRM_INF(aeb,				PRM_NOSEL,				UI_AEB_OFF,				0 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_NOSEL,				UI_HIGHLIGHT_OFF,		0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_NOSEL,				0,						0 ),
	DEF_VIEW_PRM_INF(effect,			PRM_NOSEL,				UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(af,				PRM_NOSEL,				UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_NOSEL,				SHUTTER_AUTO,			0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_NOSEL,				APERTURE_AUTO,			0 ),
	DEF_VIEW_PRM_INF(flash, 			PRM_NOSEL,				UI_FLASH_OFF,			0 ),
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama  ,    	PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack,	PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoStamp,		PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE,	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,				0, 						0 ),
};

param_info_t SceneParamInfo_NotPeople[VIEW_PRM_COUNT]={
	DEF_VIEW_PRM_INF(size,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(vidsize,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(quality,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(metering,			PRM_NOSEL,				0,						0 ),
	DEF_VIEW_PRM_INF(wb,				PRM_NOSEL,				UI_WB_AUTO,				0 ),
	DEF_VIEW_PRM_INF(iso,				PRM_NOSEL,				UI_ISO_AUTO,			0 ),
	DEF_VIEW_PRM_INF(ev,				PRM_NOSEL,				UI_EXP_BIAS_ZERO,		0 ),
	DEF_VIEW_PRM_INF(drive,				PRM_SHARE|PRM_MODE_P,	UI_DRIVE_OFF,			0 ),
	DEF_VIEW_PRM_INF(aeb,				PRM_NOSEL,				UI_AEB_OFF,				0 ),
	DEF_VIEW_PRM_INF(highlight, 		PRM_NOSEL,				UI_HIGHLIGHT_OFF,		0 ),
	DEF_VIEW_PRM_INF(edge,				PRM_NOSEL,				0,						0 ),
	DEF_VIEW_PRM_INF(effect,			PRM_NOSEL,				UI_EFFECT_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(stamp,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(stablizer,			PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(facetrack,			PRM_NOSEL,				0,						0 ),
	DEF_VIEW_PRM_INF(photoframe,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(dzoom,				PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(af,				PRM_NOSEL,				UI_FOCUS_NORMAL,		0 ),
	DEF_VIEW_PRM_INF(shutter,			PRM_NOSEL,				SHUTTER_AUTO,			0 ),
	DEF_VIEW_PRM_INF(aperture,			PRM_NOSEL,				APERTURE_AUTO,			0 ),
	DEF_VIEW_PRM_INF(flash, 			PRM_NOSEL,				UI_FLASH_OFF,			0 ),
	DEF_VIEW_PRM_INF(osd,				0,						0,						0 ),
	DEF_VIEW_PRM_INF(pvflickermode,		PRM_SHARE|PRM_MODE_P,	0,						0 ),
	DEF_VIEW_PRM_INF(viddist,			PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(WDR  ,	    		PRM_SHARE|PRM_MODE_P,	UI_WDR_OFF,				0 ),
	DEF_VIEW_PRM_INF(VideoSeamlessMode,	PRM_NOUSE,				0,						0 ),
	DEF_VIEW_PRM_INF(panorama  ,    	PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(feature,			PRM_SHARE|PRM_MODE_P,	UI_FEATURE_EFFECT_OFF,	0 ),
	DEF_VIEW_PRM_INF(videoFacetrack, 	PRM_SHARE|PRM_MODE_P,	UI_FACETRACKING_OFF, 	0 ),
	DEF_VIEW_PRM_INF(videoStamp, 		PRM_NOUSE,				UI_STAMP_OFF,			0 ),
	DEF_VIEW_PRM_INF(videoQuality,		PRM_SHARE|PRM_MODE_P,	UI_QUALITY_SUPERFINE, 	0 ),
	DEF_VIEW_PRM_INF(vidrsc,			PRM_NOUSE,				0, 						0 ),
};



ui_param_t ViewParamSet[PRM_MODE_MAX]={
	{ "UIPMODE.BIN", 	NULL,		PmodeParamInfo },
	{ "UIAMODE.BIN", 	NULL,		AmodeParamInfo },
	{ "UISMODE.BIN", 	NULL,		SmodeParamInfo },
	{ "UIMMODE.BIN", 	NULL,		MmodeParamInfo },
	{ "UIQMODE.BIN", 	NULL,		QmodeParamInfo },
	{ "UISMILE.BIN", 	NULL,		SmileParamInfo },
	{ "UIVIDEO.BIN", 	NULL,		VideoParamInfo },
	{ "UISCEN1.BIN", 	NULL,		SceneParamInfo_People },
	{ "UISCEN2.BIN", 	NULL,		SceneParamInfo_NotPeople },	
};

/*e Modify by Aries 10/03/08*/
