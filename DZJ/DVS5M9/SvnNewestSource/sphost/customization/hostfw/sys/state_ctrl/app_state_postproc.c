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
#include "app_playback_def.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_still.h"
#include "app_dpof.h"
#include "app_video.h"
#include "app_view_osd.h"
#include "app_osd_api.h"
#include "sp5k_capture_api.h"

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


/**************************************************************************
 *                         L O C A L    D A T A                         *
 **************************************************************************/
void
_CheckSavingUiData(
	void
)
{
	UINT32 next_state = appNextStateGet(); 
	
	if(!IS_VIDEO_GROUP(next_state) 
		&& !IS_AUDIO_GROUP(next_state) 
		&& !IS_VIEW_GROUP(next_state))
	{
		UiParamViewFlush();		
		#if 1
		sp5kAeModeSet(SP5K_AE_MODE_OFF);
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);	
		#endif
	}
	if(IS_AUDIO_GROUP(next_state)) 
	{
		#if 1
		sp5kAeModeSet(SP5K_AE_MODE_OFF);
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);	
		#endif
	}
}	

void
appGroupViewPostProc(
	void
)
{
	DBG_PRINT("%s enter...\n",__FUNCTION__);

	#if 1

	#else
	
	if(appNextStateGet() == APP_STATE_AUDIO_REC || appNextStateGet() == APP_STATE_POWER_OFF) 
	{
		return ;
	}
	
	LED_NORMAL_ON;
	_CheckSavingUiData();
	appStill_SetFlash(UI_FLASH_OFF);
	appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	APP_OSD_REFRESH_OFF;
	appStill_SetPhotoFrame(UI_PHOTOFRAME_OFF);
	appStillOsd_UpdateFaceOsd(TRUE);
	appStillOsd_UpdateObjectOsd(TRUE);		/* Add by sc.liu */
	appViewOsd_ZoomBarDestroy();
	appViewOsd_HistogramDestroy();
	appStillWaitJobFinish();
	if(appStill_GetMiniatureStatus())
		sp5kPreviewUrgentCallbackSet(1, NULL);
	
	#endif
}

void
appGroupVideoPostProc(
	void
)
{
	DBG_PRINT("%s enter...\n",__FUNCTION__);

	#if 1

	#else
	
	LED_NORMAL_ON;
	_CheckSavingUiData();
	if(!gVideoCB.isAutoPowerOffOn)
	{
		appPowerSavingEnableSet(TRUE);
		gVideoCB.isAutoPowerOffOn=1;
	}
	
	APP_OSD_REFRESH_OFF;	
	appStill_SetFaceTracking(VIEW_FT_STATUS_DESTROY);
	appStillOsd_UpdateFaceOsd(TRUE);
	appViewOsd_ZoomBarDestroy();	
	appViewOsd_HistogramDestroy();

	#endif
}

void
appGroupAudioPostProc(
	void
)
{
	DBG_PRINT("%s enter...\n",__FUNCTION__);

	#if 1

	#else
	
	if(appNextStateGet()==APP_STATE_PB_MAIN)
	{		
		appModeSet(SP5K_MODE_STANDBY);   /* avoid running mode is preview in middleware ,mantis 46811*/
		appJpgFileDraw(SP5K_RES_TYPE_JPG, RES_JPG_BLACK, 0, 0, 0, 0);
	//	appViewOsd_BlackOsdLayerDraw();
	}
	LED_NORMAL_ON;
	_CheckSavingUiData();

	#endif
}

void
appGroupPlaybackPostProc(
	void
)
{
	pbMainCB_t *ppbMainCb;
	DBG_PRINT("%s enter...\n",__FUNCTION__);

	#if 1

	#else
	
//	appViewOsd_BlackOsdLayerDraw();
	
	ppbMainCb = appPb_MainCbPtrGet();
	if (ppbMainCb->closeLensTmrId != TIMER_NULL)
	{
		DBG_PRINT("%s: Clearing TMR for closeLens \n", __FUNCTION__);
		appTimerClear(&ppbMainCb->closeLensTmrId);
	}
/*	CLEAR_OSD_SRC ;
	APP_OSD_REFRESH_ON;	*/
	
	#endif
}

void
appGroupUsbPostProc(
	void
)
{
	DBG_PRINT("%s enter...\n",__FUNCTION__);
}

void
appGroupCalibrationPostProc(
	void
)
{
	DBG_PRINT("%s enter...\n",__FUNCTION__);
}

void
appStateGroupCloseProc(
	void
)
{
	switch( STATE_GROUP(appActiveStateGet()) )
	{
		case APP_STATE_GROUP_VIEW:
			appGroupViewPostProc();
			break;
		case APP_STATE_GROUP_VIDEO:
			appGroupVideoPostProc();
			break;
		case APP_STATE_GROUP_AUDIO:
			appGroupAudioPostProc();
			break;
		case APP_STATE_GROUP_PLAYBACK:
			appGroupPlaybackPostProc();
			break;
		case APP_STATE_GROUP_USB:
			appGroupUsbPostProc();
			break;
		case APP_STATE_GROUP_CALIB:
			appGroupCalibrationPostProc();
			break;
		default:
			break;
	}

}


