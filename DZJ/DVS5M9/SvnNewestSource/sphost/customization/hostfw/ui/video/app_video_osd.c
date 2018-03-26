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
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"

/* for display osd */
#include "sp5k_gfx_api.h"
#include "app_pal.h"
#include "app_osd.h"

#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"

#include "app_still.h"
#include "app_video.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_osd_api.h"
#include "app_view_osd.h"
#include "app_battery.h"
#include "app_ui_para.h"
#include "gpio_custom.h"

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
void
appVideoOsdElsTimeShow(
	void
)
{
	appViewOsd_VideoTotalFreeTimeDraw(1,gVideoCB.CapSec);
}

void
appVideoOsdDiskShow(
	BOOL bShow
)
{
	if(bShow && gVideoCB.isDiskReady) {
		appViewOsd_AudioTotalFreeTimeDraw(TRUE, 0, appVideoRemainSecGet());
	}
	else {
		appViewOsd_AudioTotalFreeTimeDraw(FALSE, 0, 0);
	}
	
	DBG_PRINT("appVideoOsdDiskShow: dskfree=%d KB, remaintime=%d sec\n", pDisk->freeSz, n);
}


void
appVideoOsdShow(
	UINT8 clean,
	UINT8 outInOsdFreeze,
	UINT8 osd
)
{	
	uiPara_t* puiPara = appUiParaGet();

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		appOsdIconModeSet(APP_ICON_MODE_VIDEOVIEW_HDMI);
	}else{
		appOsdIconModeSet(APP_ICON_MODE_VIDEOVIEW);
	}
	#else
	appOsdIconModeSet(APP_ICON_MODE_VIDEOVIEW);
	#endif
	
	if (gVideoCB.fCapture==0) 
	{
		APP_OSD_REFRESH_OFF;
		if(clean) 
		{
			appViewOsd_AllClean();	
		}
		appViewOsd_ZoomBarDestroy();
		//osd-type
		switch(osd) 
		{
			case OSD_VIEW_S_ONE_HOLD:
				// this is not effect, the app_still_aaa will call appViewOsdShow();
				// Then appViewOsdShow() will check if this is video mode.
				break;
			case OSD_VIEW_VERBOSE:
				appViewOsd_WBModeDraw(TRUE, puiPara->WBMode);
				appViewOsd_MeteringDraw(TRUE, puiPara->AEMetering);			
			default:	
			case OSD_VIEW_SIMPLE:
				appViewOsd_GuideLineDraw(FALSE);
				appViewOsd_ViewModeDraw(TRUE, gStillCB.cap.CurViewMode, gStillCB.cap.CurScene);			
				appViewOsd_PicSizeVideoDraw(TRUE,puiPara->VideoSize);
				appViewOsd_VideoQtyDraw(TRUE, puiPara->VideoQuality);
				appViewOsd_BatteryDraw(TRUE, appBatteryLevelGet());
				#if !ICAT_FCWS
				appViewOsd_FDModeDraw(TRUE, puiPara->VideoFaceTracking);
				#endif
				#if GPS_SUPPORT
				appViewOsd_GPSModeDraw(TRUE, puiPara->GPS);
				#endif
				appVideoOsdDiskShow(TRUE);
				break;
			case OSD_VIEW_NONE:			
				appViewOsd_WBModeDraw(FALSE,0);
				appViewOsd_MeteringDraw(FALSE,0);
				appViewOsd_GuideLineDraw(FALSE);
				appViewOsd_ViewModeDraw(FALSE, 0, 0);
				appViewOsd_PicSizeVideoDraw(FALSE,0);
				appViewOsd_VideoQtyDraw(TRUE, puiPara->VideoQuality);
				appViewOsd_FocusModeDraw(FALSE,0);
				appViewOsd_BatteryDraw(FALSE,0);
				appVideoOsdDiskShow(FALSE);
				appViewOsd_AllClean();	
				break;
			case OSD_VIEW_GRID:
				appViewOsd_GuideLineDraw(TRUE);
				break;	
		}			
		//post-osd
		appViewOsd_FocusWindowDraw(TRUE, FOCUS_STATUS_VIEW);
		if(gVideoCB.err > VIEW_ERR_NO_ERROR) 
		{
			appViewOsd_WarmingMsgDraw(TRUE, gVideoCB.err, FALSE);	
		}
		
		if(osd==OSD_VIEW_VERBOSE && gVideoCB.err <= VIEW_ERR_NO_ERROR) 
		{
			appViewOsd_HistogramCreat();
		}	
		else 
		{			
			appViewOsd_HistogramDestroy();
		}	
		
		VIEW_PERF_LOG(0, "osd refresh -s");
		if(!outInOsdFreeze) 
		{
			APP_OSD_REFRESH_ON;
		}	
		VIEW_PERF_LOG(0, "osd refresh -e");
	} 
	else 
	{
		//appOsd_GfxStringDraw(20, 200, SP5K_GFX_ALIGN_BOTTOM_LEFT,gVideoCB.fPause? "SH1: Stop, UP: Record":"SH1: Stop, UP: Pause");
		appVideoOsdElsTimeShow();	
	}
}

