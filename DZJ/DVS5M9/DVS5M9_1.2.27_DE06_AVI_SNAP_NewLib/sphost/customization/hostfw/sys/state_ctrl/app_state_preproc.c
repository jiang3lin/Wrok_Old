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
#include "sp5k_global_api.h"
#include "app_playback_def.h"
#include "app_tvlcd.h"
#include "app_still.h"
#include "app_view_osd.h"
#include "app_video.h"
#include "app_audio.h"
#include "app_still_timelapse.h"

#if ICAT_WIFI
#include "app_ptp.h"
#include "app_wifi_utility.h"
#endif

#include "app_osd_draw_lcm.h"

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern UINT32 appSenceMenuValidModeKeyCheck(void);


#if KIT_WITH_LENS
void appLensClose(void);
#endif
/**************************************************************************
 *                         L O C A L    D A T A                         *
 **************************************************************************/
void
appPowerSavingDispOff(
	void
)
{
	if(IS_LCD_OUT) 
	{
		//LCD_BACKLIGHT_OFF;
		//LED_FLICKER_LCD_OFF;
	}		
}

appState_msg_flow_e
appGroupSysProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	
	switch( msg )
	{
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			DBG_PRINT("APP_UI_MSG_POWER_SAVE_TIME_OUT....\n");			
			appPowerSavingDispOff();
			break;
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			DBG_PRINT("APP_UI_MSG_BATT_LEVEL_CHANGE(%d)\n",param);
			appLcmOsdDrawBattery(TRUE,param);
				
			#if ICAT_WIFI
			if (appPtpStatGet()){
				appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_BATTERY_LEVEL, param, 0);
			}
			#endif
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}
	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}

appState_msg_flow_e
appGroupViewProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	
	ret = appTimLapKeyMsgDispose(msg, param);
	
	if(ret == APPSTATE_MSG_IGNORE)
	{
		return ret;		
	}

	//appStillBackground(msg, param);
		
	switch( msg )
	{
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			DBG_PRINT("APP_UI_MSG_POWER_SAVE_TIME_OUT....\n");			
			appPowerSavingDispOff();
			break;
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			DBG_PRINT("APP_UI_MSG_BATT_LEVEL_CHANGE(%d)\n",param);
			if (!IS_MENU_STATE(appActiveStateGet()))
			{
				appLcmOsdDrawBattery(TRUE,param);
			}
			#if ICAT_WIFI
			if (appPtpStatGet()){
				appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_BATTERY_LEVEL, param, 0);
			}
			#endif
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}
	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}

appState_msg_flow_e
appGroupVideoProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;

	appStillBackground(msg, param);

	switch( msg )
	{
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			appPowerSavingDispOff();
			break;
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			if (!IS_MENU_STATE(appActiveStateGet()))
			{
				appLcmOsdDrawBattery(TRUE,param);
			}
			#if ICAT_WIFI
			if (appPtpStatGet()){
				appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_BATTERY_LEVEL, param, 0);
			}
			#endif
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}
	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}

appState_msg_flow_e
appGroupAudioProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	
	switch( msg )
	{
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			DBG_PRINT("APP_UI_MSG_POWER_SAVE_TIME_OUT....\n");
			//do power saving process
			//exception : audio recording
			appPowerSavingDispOff();
			break;
		case APP_UI_MSG_LENS_CLOSE:
			//close lens 
			//exception : during audio recording
			break;
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			DBG_PRINT("APP_UI_MSG_BATT_LEVEL_CHANGE(%d)\n",param);
			//update battery icon
			if (!IS_MENU_STATE(appActiveStateGet())) 
			{
				#if !CAM_TYPE_CVR
				appAudioOsd_BatteryDraw(TRUE,param);
				#endif
			}
			#if ICAT_WIFI
			if (appPtpStatGet()){
				appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_BATTERY_LEVEL, param, 0);
			}
			#endif
			ret = APPSTATE_MSG_IGNORE;
			break;
	}	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}

appState_msg_flow_e
appGroupPlaybackProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
	pbMainCB_t *pmainCb;
	pbImgCb_t *pimgCb;

	pmainCb = appPb_MainCbPtrGet();
	pimgCb = appPb_ImageCbPtrGet();
	
	/*-- To skip KEY RELEASE Msg for Playback */
	if (IS_APP_KEY_MSG(msg) && ((msg&0x03UL)==0x01)) 
	{
		ret = APPSTATE_MSG_IGNORE;
		return ret;
	}

	switch( msg )
	{
		case SP5K_MSG_STILLPLAY_DONE:
			ret = appPbStillPlayDoneParse(param);
			if (ret & PB_DONE_ERROR) 
			{
				DBG_PRINT("!! WARNING !!, PB_DONE ERROR (0x%x) !!\n", ret);
				gpbImgCb.isFileErr = TRUE;
				gpbImgCb.fileType = PB_FT_UNKNOWN;
				gpbImgCb.dcfAttr.fileType=0;
				gpbMainCb.err = PB_ERR_FILE_ERROR;
				gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			}
			ret = APPSTATE_MSG_DEFAULT;
			break;

		case SP5K_MSG_AE_READY:
		case SP5K_MSG_AWB_READY:
			ret = APPSTATE_MSG_IGNORE;
			break;
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
			appPowerSavingDispOff();
			break;
		case SP5K_MSG_TIMER:
			#if KIT_WITH_LENS
			if (param==pmainCb->closeLensTmrId) 
			{
				sp5kHostMsgSend(APP_UI_MSG_LENS_CLOSE, 0);
				appTimerClear(&pmainCb->closeLensTmrId);
				pmainCb->closeLensTmrId = TIMER_NULL;
			}
			#endif
			break;
		case APP_UI_MSG_LENS_CLOSE:
			/*-- close lens
			 * exception : during voice memo recording
			 */
			DBG_PRINT("\nAPP_UI_MSG_LENS_CLOSE !!\n\n");
			#if KIT_WITH_LENS
			if (PB_SUB_STAT_PRIME_NUM(pmainCb->subStat)!=PB_SUB_STAT_PRIME_MEMOREC) 
			{
				appLensClose();
			}
			#endif
			break;
		case SP5K_MSG_DISK_MOUNT_COMPLETE:
		case SP5K_MSG_DISK_MOUNT_FAIL:
			DBG_PRINT("Preprocess [SP5K_MSG_DISK_MOUNT_COMPLETE/_FAIL] ....\n");
			if (pmainCb->mainStat!=PB_MAIN_STAT_C2C_SUSPEND) 
			{
				ret = APPSTATE_MSG_IGNORE;
			}
			break;

		case SP5K_MSG_MODE_READY:
			DBG_PRINT("Preprocess [SP5K_MSG_MODE_READY] ...param=%x \n",param);
			if (param==SP5K_MODE_STANDBY && FALSE==appPb_IsViewIdle()) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_INIT, 0);
			} 
			else if (param==SP5K_MODE_VIDEO_PLAYBACK 
				&&pmainCb->playStat==PB_PLAY_STAT_VIDEO_TITLE_PLAYING)
			{   
				printf("PB_PreProc: Video QV ready !!\n");
				pmainCb->playStat = PB_PLAY_STAT_STANDBY;
				pimgCb->isThumbImgOK = TRUE;
				pmainCb->isPlayDone = TRUE;
				sp5kHostMsgSend(APP_UI_MSG_PB_VIDEO_TITLE_DONE, 0);
				ret = APPSTATE_MSG_IGNORE;
			}
			break;

		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			DBG_PRINT("APP_UI_MSG_BATT_LEVEL_CHANGE(%d)\n",param);
			ret = APPSTATE_MSG_IGNORE;			
			break;
	}		
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}

appState_msg_flow_e
appGroupUsbProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
		
	switch( msg )
	{
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}
	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}

appState_msg_flow_e
appGroupCalibrationProc(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 ret = APPSTATE_MSG_DEFAULT;
		
	switch( msg )
	{
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
			ret = APPSTATE_MSG_IGNORE;
			break;
		default:
			break;
	}
	
	if (ret == APPSTATE_MSG_IGNORE)
	{
		DBG_PRINT("Msg[0x%x] is ignored after %s \n",msg,__FUNCTION__);
	}

	return ret;
}		

