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
#include "sp5k_disp_api.h"
#include "app_timer.h"
#include "sp5k_rsvblk_api.h"

#include "app_still.h"
#include "app_sound.h"
#include "app_view_param.h"
#include "app_osd_api.h"
#include "app_view_osd.h"
#include "gpio_custom.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SELF_TIMER_PERIOD 50
#define SELF_TIMER_ONE_SECOND (1000/SELF_TIMER_PERIOD)
#define SELF_TIMER_500_MS (500/SELF_TIMER_PERIOD)
#define SELF_TIMER_250_MS (250/SELF_TIMER_PERIOD)
#define SELF_TIMER_200_MS (200/SELF_TIMER_PERIOD)
#define SELF_TIMER_6S (6000/SELF_TIMER_PERIOD)
#define SELF_TIMER_2S (2000/SELF_TIMER_PERIOD)
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 selfSnapLedStatus; 
static UINT32 selfSnapTimer;
static int selfCountDown;
static int BeepFrequency;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
appStillSelfTimerInit(
	UINT32 delayTime
)
{
	CLEAR_OSD_SRC;
	appViewOsdShow(0,0,gStillCB.osd);
	BeepFrequency = 0;
	selfSnapLedStatus = 0;
	selfCountDown = delayTime*SELF_TIMER_ONE_SECOND;
	selfSnapTimer = appTimerSet(SELF_TIMER_PERIOD, "Self Snap Timer");
}

void 
appStillSelfTimerStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 next_state,next_param;

	switch (msg) {
	case SP5K_MSG_TIMER: 
		if (param==selfSnapTimer) {
			/* time number show */				
			if(!(selfCountDown%SELF_TIMER_ONE_SECOND)) {
				 appViewOsd_DriveModeCountDownDraw(selfCountDown/SELF_TIMER_ONE_SECOND);
			}

			/* beep sound control */
			if (BeepFrequency == 0) {
				appBeep(TRUE, FALSE);
				if(selfSnapLedStatus) {
					selfSnapLedStatus=0;
					GREEN_LED_OFF
					RED_LED_OFF					
				}	
				else {
					selfSnapLedStatus=1;
					GREEN_LED_ON
					RED_LED_ON					
				}	
				BeepFrequency = selfCountDown>SELF_TIMER_6S? SELF_TIMER_ONE_SECOND: 
					selfCountDown>SELF_TIMER_2S? SELF_TIMER_500_MS: 
					selfCountDown>1? SELF_TIMER_250_MS:0xffff;
			}
			selfCountDown--;
			BeepFrequency--;
			if (selfCountDown<=0) {
				LED_NORMAL_ON
				appTimerClear(&selfSnapTimer);
				appStillFlow_GetNextState(&next_state, &next_param);
				appStateChange(next_state, next_param);
			}
		}
		break;	
	case APP_STATE_MSG_ABORT:
		appTimerClear(&selfSnapTimer);
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		break;
	default:
		break;
	}	
}

void 
appStillSelfTimerStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_KEY_PRESS_S1:
	      #if KIT_WITH_HDMI     /*Add by Lation@20130717*/
		if(IS_HDMI_IN)    /*add for mantis bug 0047469*/
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 248, 108, 144, 144, 0, 0);
		}
		else
		{
			appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 110, 69, 100, 104, 0, 0);
		}
		#endif
		if(gStillCB.osd == OSD_VIEW_GRID)
		{
			appViewOsd_AllClean();
			appViewOsd_GuideLineDraw(TRUE);
		}
		appTimerClear(&selfSnapTimer);
		appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
		break;	
	default:
		break;
	}
}

void 
appStillSelfTimerState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appStillSelfTimerState\n");
		appStillSelfTimerInit(FLOW_EXTRCT_ST_PAR(param));
		break;		
	case APP_STATE_MSG_CLOSE:
		appTimerClear(&selfSnapTimer);
		appStateCloseDone();
		break;			
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appStillSelfTimerStateOnKey(msg,param);
		}
		else{
			appStillSelfTimerStateOnMsg(msg,param);
		}
		break;
	}

}
