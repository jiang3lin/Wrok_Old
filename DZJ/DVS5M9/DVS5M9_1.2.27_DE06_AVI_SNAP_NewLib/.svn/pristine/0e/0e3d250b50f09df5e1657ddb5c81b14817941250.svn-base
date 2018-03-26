/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
 
/**
 * @file      app_menu_event.c
 * @brief     Brief
 * @author    Phil Lin
 * @since     2012-10-15
 * @date      2012-10-15
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include <stdio.h>

#include "app_com_def.h" 

#include "app_menu.h" 
#include "app_menu_core.h" 
#include "app_menu_tab.h" 
#include "app_menu_disp.h" 
#include "app_menu_event.h"
#include "app_ui_para.h" 
#include "app_setup_lib.h" 


#include "sp5k_cdsp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_af_api.h"
#include "sp5k_rsvblk_api.h"
#include "customization\app_init.h"	
#include "app_com_api.h"
#include "app_com_def.h"
#include "app_osd.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_playback_def.h"
#include "app_zoom_api.h"
#include "app_util.h"

#include "app_still.h"
#include "app_video.h"

#include "app_strobe.h"
#include "app_led.h"
#include "app_aaa.h"
#include "app_exif.h"
#include "app_view_osd.h"
#include "app_face_detect_ex.h"
#include "app_usb_dpslib.h"
#include "app_osd_api.h"
#include "app_playback_def.h"
#include "app_playback_uiparam.h"
#include "app_calib_api.h"

#include "api/sp5k_awb_api.h"

#include "middleware/sysg_resource_def.h"
#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
UINT8 rtc_init[3]={0, 1, 2};
UINT8 rtc_left[3][6]={{5, 0, 1, 2, 3, 4}, {2, 5, 1, 0, 3, 4}, {1, 2, 5, 0, 3, 4}};
UINT8 rtc_right[3][6]={{1, 2, 3, 4, 5, 0}, {3, 2, 0, 4, 5, 1}, {3, 0, 1, 4, 5, 2}};
static UINT8 sceneMenuMap[SENCEMENU_MAX][2]=
{		
	{SENCEMENU_PROGRAM, 			PASM_PROGRAM},
	{SENCEMENU_MOVIE		,		VIDEO_MODE},
	{SENCEMENU_SMILEDET ,			SCENE_SMILE_DETECTION},
	{SENCEMENU_SHAKE_FREE	,		SCENE_SHAKE_FREE},
	{SENCEMENU_LANDSCAPE	,		SCENE_LANDSCAPE},
	{SENCEMENU_BACK_LIGHT	,		SCENE_BACK_LIGHT},
	{SENCEMENU_NIGHT		,		SCENE_NIGHT},
	{SENCEMENU_BUILDING ,			SCENE_BUILDING},
	{SENCEMENU_CHILDREN ,			SCENE_CHILDREN},
	{SENCEMENU_PORTRAIT ,			SCENE_PORTRAIT},
	{SENCEMENU_FOOD 	,			SCENE_FOOD},
	{SENCEMENU_TEXT 	,			SCENE_TEXT},
	{SENCEMENU_BEACH_SNOW	,		SCENE_BEACH_SNOW},
	{SENCEMENU_FIRE_WORKS	,		SCENE_FIRE_WORKS},
	{SENCEMENU_SUNSET		,		SCENE_SUNSET},	
	{SENCEMENU_WDR			,		SCENE_WDR},	
	{SENCEMENU_HDR		,			SCENE_HDR},	
	{SENCEMENU_PANORAMA		,		SCENE_PANORAMA},	
	{SENCEMENU_BLINKDETECT,			SCENE_BLINK},
	{SENCEMENU_VOICEREC ,			AUDIO_MODE},
};
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 DPSImgIdxOld=1;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appStill_FeatureEffectSet(void);
extern void appIqSaturationFactorSet(UINT32 sat);
extern void appPlayOsd_FileNumDraw(UINT8 bShow,UINT32 CurrntIndex,UINT32	Total);
extern void _setupMenuFunctionResetAll(void);
extern UINT8 fileTypeSelect;
extern UINT8 IsSetImage;
extern UINT32 enPartial;
extern UINT8 *pszSlideMusic[SLSHOW_MUSIC_TOTAL];
extern uiSoundSetting_t *psoundcontrol;
extern appRTC_t rtcTime;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT32 appMenuEvt_ModeStill(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_ModeStill
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModeStill(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#if 0
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
		#endif
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

/**
 * @fn        UINT32 appMenuEvt_ModeVideo(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_ModeVideo
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModeVideo(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	
	layer = layer; /* just for clearing warning */
	item = item; /* just for clearing warning */
	param = param; /* just for clearing warning */
	
	switch (msg) {
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#if 0
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
		#endif
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
}

/**
 * @fn        UINT32 appMenuEvt_VideoSizeSet(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_VideoSizeSet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_VideoSizeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;	/* just for clear warning */
	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_MENU:
			appVidSizeSet(puiPara->VideoSize);
			break;

		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			if(puiPara->VideoSize == UI_VID_SIZE_FHD)
			{
				puiPara->VideoFormat = VIDEO_FORMAT_H264;
			}
			appVidSizeSet(puiPara->VideoSize);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoQualitySet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;	/* just for clear warning */
	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
			
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
			
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appVideoSetQuality(puiPara->VideoQuality);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_SeamlessSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	
	item = item;	/* just for clear warning */
	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoWBModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	item = item;	/* just for clear warning */
	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			appStill_SetWB(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			appStill_SetWB(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_MENU:
			appStill_SetWB(puiPara->WBMode);
			break;

		case APP_KEY_PRESS_SET:
			if(item == UI_WB_CUSTOM)
			{
				appStateChange(APP_STATE_VIDEO_CWB,STATE_PARAM_NORMAL_INIT); /*junqiong.hu@20121225 add for mantis bug 0046601*/
				//appStateChange(APP_STATE_STILL_CWB, STATE_INIT);					
			}
			else
			{
				menuProcParaSet(layer, item);
				appStill_SetWB(puiPara->WBMode);
			}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_RollingShutterSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clear warning */
	item = item;	/* just for clear warning */
	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			//if(appActiveStateGet() == APP_STATE_VIDEO_SUBMENU){
			menuProcParaSet(layer, item);
			//appVideo_SetRSC(puiPara->RollingShutter, puiPara->DisMode);
			//}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoMeteringSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clear warning */
	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetMetering(puiPara->AEMetering);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoFaceTrackingSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clear warning */
	param =param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == 0){	//video face tracking on
				puiPara->VideoFaceTracking = 0;
			}
			else{			//video face tracking off
				puiPara->VideoFaceTracking = 1;
			}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoStampModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clear warning */
	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == 0){		//datestamp off
				puiPara->VideoStampMode = UI_STAMP_OFF;
			}
			else if(item == 1){	//datestamp date
				puiPara->VideoStampMode = UI_STAMP_DATE;
			}
			else if(item == 2){	//datestamp date&time
				puiPara->VideoStampMode = UI_STAMP_DATE_TIME;
			}
			break;
	}
	
	return ret;
}

UINT32
appMenuEvt_VideoDistModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clear warning */
	param =param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			//if(appActiveStateGet() == APP_STATE_VIDEO_SUBMENU){
			menuProcParaSet(layer, item);
			//appVideo_SetDist(puiPara->DisMode, puiPara->RollingShutter);
			//}
			break;
	}
	
	return ret;
}

UINT32
appMenuEvt_AdvanceDenoise(	//not ready
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clear warning */
	param =param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == 0){	//advance denoise on
				puiPara->AdvanceDenoise = 0;
			}
			else{
				puiPara->AdvanceDenoise = 1;
			}
			break;
	}
	
	return ret;
}


/**
 * @fn        UINT32 appMenu_ModePlayback(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenu_ModePlayback
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModePlayback(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	
	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#if 0
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
		#endif
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

/**
 * @fn        UINT32 appMenuEvt_ModeSetup(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_ModeSetup
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModeSetup(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	
	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */

	switch (msg) {
		#if 0
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;		
		#endif
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			appMenu_PosInit();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 
}

UINT32
appMenuEvt_SoundInit(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
		case APP_KEY_PRESS_RIGHT:
			puiPara->soundcontrol = SOUND_OFF;/*junqiong.hu@20130107 add for mantis bug 0046732 */
			menuProcChild(0);
			menuProcDisp2Layer();
			psoundcontrol->shutter = puiPara->ShutterSound;
			psoundcontrol->powerOn = puiPara->StartupSound;
			psoundcontrol->beep = puiPara->BeepSound;
			psoundcontrol->volume = puiPara->Volume;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

UINT32
appMenuEvt_SoundSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_UP:
			if(puiPara->BeepSound == BEEP_ON) 
			{
				appBeep(TRUE,TRUE); 
			}
			menuProcMoveUp ();
			menuProcDisp2Layer();
			appSoundTerminate();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			if(puiPara->BeepSound == BEEP_ON) 
			{
				appBeep(TRUE,TRUE); 
			}
			menuProcMoveDown ();
			menuProcDisp2Layer();
			appSoundTerminate();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			appSoundTerminate();
			appSetupMenu_Sounn_Adjust(item, psoundcontrol, 0);
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent(); 
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			appSoundTerminate();
			appSetupMenu_Sounn_Adjust(item, psoundcontrol, 1);
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent(); 
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_SET:
			if(puiPara->BeepSound == BEEP_ON)  
			{
				appBeep(TRUE,TRUE);
				puiPara->soundcontrol = SOUND_ON;
			}
			if(puiPara->ShutterSound != SHUTTER_SOUND_NONE)
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, RES_WAV_SHUTTER);	
			}
			else 
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, NULL);
			}
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_MENU:  
			if(puiPara->BeepSound == BEEP_ON) 
			{
				appBeep(TRUE,TRUE);
				puiPara->soundcontrol = SOUND_ON;
			}
			if(puiPara->ShutterSound != SHUTTER_SOUND_NONE) /*add for mantis bug 0047527*/
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, RES_WAV_SHUTTER);	
			}
			else 
			{
				sp5kStillCapCfgSet(SP5K_CAPTURE_SHUTTER_SND_NAME, NULL);
			}
			break;
	}
	
	return ret;
}

UINT32
appMenuEvt_AutoReviewSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	
	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == 0){		//autoreview off
				puiPara->AutoReviewTime = QV_TIME_OFF;
			}
			else if(item == 1){	//autoreview 1s
				puiPara->AutoReviewTime = QV_TIME_1SEC;
			}
			else if(item == 2){	//autoreview 3s
				puiPara->AutoReviewTime = QV_TIME_3SEC;
			}
			else if(item == 3){	//autoreview 5s
				puiPara->AutoReviewTime = QV_TIME_5SEC;
			}
			if((puiPara->AutoReviewTime != QV_TIME_OFF && puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT)\
				||(puiPara->AutoReviewTime != QV_TIME_OFF && puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)) /*add for mantis bug 0047602*/
			{
				puiPara->FaceTracking = UI_FACETRACK_OFF;
			}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_LightFreqSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			applightFrequencySetting(puiPara->LightFreq);
			break;
		}

	return ret;
}
UINT32
appMenuEvt_VideoFormatSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			puiPara->VideoFormat = item;
			if((puiPara->VideoFormat == VIDEO_FORMAT_MJPG)
				&& (puiPara->VideoSize == UI_VID_SIZE_FHD))
			{
				puiPara->VideoSize = UI_VID_SIZE_HD_30FPS;
			}
			if(puiPara->VideoFormat == VIDEO_FORMAT_H264)
			{
				puiPara->TimeLapse = UI_TIMELAPSE_OFF;
			}
			break;
		}
	return ret; 
}

UINT32
appMenuEvt_MediaFormatSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_SleepTimeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_PeriodicalCapSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

void
appMenuEvt_DayItemCheck(
	UINT8 item,
	UINT32 msg
)
{
	UINT8 dayMax;
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->rtcMonth+1)
	{
		case 2:
		//modify for leep year problem @2008-04-11 wei.sun begin
		//if ((puiPara->rtcYear) && (!(puiPara->rtcYear & 0x03)))
		if (!(puiPara->rtcYear & 0x03))
		//modify for leep year problem @2008-04-11 wei.sun end
			dayMax = 28;
		else
			dayMax = 27;
		if (puiPara->rtcDay > dayMax)
		{
			if ((item == 2) && (msg == APP_KEY_PRESS_UP))
				puiPara->rtcDay = 0;
			else
				puiPara->rtcDay = dayMax;
		}
		break;

		case 4:
		case 6:
		case 9:
		case 11:
		dayMax = 29;
		if (puiPara->rtcDay > dayMax)
		{
			if ((item == 2) && (msg == APP_KEY_PRESS_UP))
				puiPara->rtcDay = 0;
			else
				puiPara->rtcDay = dayMax;
		}
		break;
	}
	if(puiPara->rtcYear == 7 && msg == APP_KEY_PRESS_DOWN)
		puiPara->rtcYear = 30;
	if(puiPara->rtcYear == 0 && msg == APP_KEY_PRESS_UP)
		puiPara->rtcYear = 8;

}
UINT32
appMenuEvt_RTCInit(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			appRtcGet(&rtcTime);
			puiPara->rtcYear = rtcTime.year - 2000;
			puiPara->rtcMonth = rtcTime.mon - 1;
			puiPara->rtcDay = rtcTime.day - 1;
			puiPara->rtcHour = rtcTime.hour;
			puiPara->rtcMinute = rtcTime.min;
			menuProcChild (rtc_init[puiPara->DateStyle]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		}

	return ret;
}

UINT32
appMenuEvt_RTCSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_REPEAT_UP:
			if (item == 5)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
			
		case APP_KEY_PRESS_UP:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			appMenuEvt_DayItemCheck(item, APP_KEY_PRESS_UP);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_REPEAT_DOWN:
			if(item == 5)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}

		case APP_KEY_PRESS_DOWN:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			appMenuEvt_DayItemCheck(item, APP_KEY_PRESS_DOWN);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_LEFT:
			menuProcParent();
			menuProcChild (rtc_left[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_RIGHT:
			menuProcParent();
			menuProcChild (rtc_right[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_SET:
			rtcTime.year = puiPara->rtcYear + 2000;
			rtcTime.mon = puiPara->rtcMonth + 1;
			rtcTime.day = puiPara->rtcDay + 1;
			rtcTime.hour = puiPara->rtcHour;
			rtcTime.min = puiPara->rtcMinute;
			rtcTime.sec = 0;
			appRtcSet(&rtcTime);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		}
	
	return ret;
}


/**
 * @fn        UINT32 appMenuEvt_LanguageSet(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_LanguageSet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_LanguageSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	appSetupMenu_SetCurLanguage(puiPara->Language);
	
	return ret; 
}

UINT32
appMenuEvt_TVModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_USBModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_Format(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == FORMAT_YES){
				if(_setupMenuFunctionFormat()) 
	        		{
		        		appPbBackdoorEventCBF(PB_APP_EVENT_DISK_FORMAT);
		        		if(IS_PLAYBACK_GROUP(appPreviousStateGet()))
		        		{
		        			appMenu_MenuExit(0);
		        		}
		        	}
			}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_StartUpPicSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
		}

	return ret;
}

UINT32
appMenuEvt_ResetAll(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara;

	layer = layer; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == RESET)
			{
				_setupMenuFunctionResetAll();
				puiPara = appUiParaGet();
				if(IS_VIDEO_MODE) 
				{
					appVidSizeSet(puiPara->VideoSize);
				}
				else if(IS_PASM_OR_SCENE_MODE) 
				{
					appStill_SetResolution(puiPara->ImageSize);
				}
			}
			break;
	}

	return ret;
}

void
appMenuEvt_SceneMenuBackToStill(
	void
)
{
	UINT8 count;
	uiPara_t* puiPara = appUiParaGet();

	for(count = 0; count < SENCEMENU_MAX; count++)
	{
		if(puiPara->SceneMode == sceneMenuMap[count][0])
		{
			gStillCB.lastcap.CurViewMode = gStillCB.cap.CurViewMode;
			gStillCB.lastcap.CurScene = gStillCB.cap.CurScene;
			if((puiPara->SceneMode == SENCEMENU_PROGRAM)
				||(puiPara->SceneMode == SENCEMENU_MOVIE)
				||(puiPara->SceneMode == SENCEMENU_VOICEREC))
			{
				gStillCB.cap.CurViewMode = sceneMenuMap[count][1];
				gStillCB.cap.CurScene  = SCENE_NONE;
			}
			else
			{
				gStillCB.cap.CurViewMode = SCENE_MODE;
				gStillCB.cap.CurScene  = sceneMenuMap[count][1];
 			}
		}
	}
}

UINT32
appMenuEvt_SceneModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	
	appViewInit();
	if(gStillCB.cap.CurViewMode == AUDIO_MODE)
	{
		appJpgDraw_ImgLayer("A:\\RO_RES\\UI\\JPG\\AUDIOBK.JPG", 0,0,320,240);
	}

	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appMenuEvt_SceneMenuBackToStill();
			if (gStillCB.cap.CurViewMode == AUDIO_MODE)
			{
				appStateChange(APP_STATE_AUDIO_REC, STATE_PARAM_NORMAL_INIT);
			}
			else if (gStillCB.cap.CurViewMode == VIDEO_MODE)
			{
				appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			}
			else 
			{
				appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
			}
			break;
		}

	return ret;
}
/**************************************************************************
 * @fn        UINT32 appMenuEvt_ModeDPS(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_ModeSetup
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    
 * @since     2012-10-31
 * @todo      N/A
 * @bug       N/A
**************************************************************************/		
UINT32
appMenuEvt_ModeDPS(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */
#if 1
	switch (msg) {
#if 0
		case SP1K_MSG_TP_UP:
			menuProcPageUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
		case SP1K_MSG_TP_DOWN:
			menuProcPageDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
#endif
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

	}
#endif	
	return ret; 
}
UINT32
appMenuEvt_DPSPrintSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			pDpsMenuOpt->Print.SelOption = (UINT16)item;
			printf("appMenuEvt_DPSPrintSet item =%x\n",item);
			if(item == 0)
			{
				appUsbDps_StandardPrintingConfigSet();
			}
			else if(item == 1)
			{
				appUsbDps_IndexPrintingConfigSet();
			}
			else if(item == 2)
			{
				enPartial = 0;
				appUsbDps_DPOFPrintingConfigSet();
			}
			
			appStateChange(APP_STATE_USB_PRINTING,STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
}

UINT32
appMenuEvt_DPSImageSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();
	dpsInfo_t *pgDpsInfo=appUsbDps_gDpsInfoPtrGet();
	dpsImage_t *pgDpsImage=appUsbDps_gDpsImagePtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pgDpsInfo = pgDpsInfo; /* just for clearing waring */
	pgDpsImage = pgDpsImage; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_LEFT:
			break;
		case APP_KEY_PRESS_RIGHT:
			break;	
		case APP_KEY_PRESS_UP:
			break;
		case APP_KEY_PRESS_DOWN:
			break;
		case APP_KEY_PRESS_SET:
			#if 1
			appUsbDps_MenuOptReset();
			menuProcParaSet(layer, item);
			printf("currItem =%d\n",puiPara->DPSImageMode);
			pDpsMenuOpt->Image.SelOption= puiPara->DPSImageMode;
			CLEAR_OSD_SRC;
			CLEAR_PIP_SRC;
			appStateChange(APP_STATE_USB_PRINTIMAGESEL, STATE_PARAM_NORMAL_INIT);				
			#else
			if(item!=2){
				if(item==DPS_IMAGE_SIGNLE)
				{
					printf("APP_KEY_PRESS_SET currIdx=%x\n",pgDpsImage->curImgIdx);
					pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]= pgDpsInfo->SelPic ;
				}
				else if(item==DPS_IMAGE_ALL)
				{
					UINT8 count = 0;
					for (count=0; count<pgDpsImage->totalImg; count++)
					{
						pgDpsInfo->PicBuf[count]= pgDpsInfo->SelPic ;
					}
	 			}
				pgDpsInfo->TotalSelPics=appUsbDps_TotalPrintSelImg(pgDpsInfo,pgDpsImage->totalImg);
			}
			else{
				printf("layer=%x,item=%x,para=%x\n",layer,item,menuProcParaGet(layer, item));
				CLEAR_PIP_SRC;
				menuProcChild (menuProcParaGet(layer, item));
				menuProcDispYesNo(ID_STR_PICTBRIDGE);
				DPSImgIdxOld=pgDpsImage->curImgIdx;
				appPlayOsd_FileNumDraw(1,pgDpsImage->curImgIdx,pgDpsImage->totalImg);
				appUsbDps_OsdSelectIconDraw(pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]);
				ret = MENU_IGNORE_DEFAULT;
			}
			pDpsMenuOpt->Image.OsdEnOption = (UINT16)item;
			IsSetImage = 1 ;
			#endif
			printf("APP_KEY_PRESS_SET ITEM=%x\n",item);
			break;
	}
	return ret; 

}
#if 0
UINT32
appMenuEvt_DPSImageChoose(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();
	dpsInfo_t *pgDpsInfo=appUsbDps_gDpsInfoPtrGet();
	dpsImage_t *pgDpsImage=appUsbDps_gDpsImagePtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	//pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_LEFT:
			PB_INDEX_DEC(pgDpsImage->curImgIdx, 1, pgDpsImage->totalImg);
			menuProcDispYesNo(ID_STR_PICTBRIDGE);
			appPbImagePlay(pgDpsImage->curImgIdx);
			appPlayOsd_FileNumDraw(1,pgDpsImage->curImgIdx,pgDpsImage->totalImg);
			appUsbDps_OsdSelectIconDraw(pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			PB_INDEX_INC(pgDpsImage->curImgIdx, 1, pgDpsImage->totalImg);
			menuProcDispYesNo(ID_STR_PICTBRIDGE);
			appPbImagePlay(pgDpsImage->curImgIdx);
			appPlayOsd_FileNumDraw(1,pgDpsImage->curImgIdx,pgDpsImage->totalImg);
			appUsbDps_OsdSelectIconDraw(pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]);
			ret = MENU_IGNORE_DEFAULT;
			break;	
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDispYesNo(ID_STR_PICTBRIDGE);
			appPlayOsd_FileNumDraw(1,pgDpsImage->curImgIdx,pgDpsImage->totalImg);
			appUsbDps_OsdSelectIconDraw(pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDispYesNo(ID_STR_PICTBRIDGE);
			appPlayOsd_FileNumDraw(1,pgDpsImage->curImgIdx,pgDpsImage->totalImg);
			appUsbDps_OsdSelectIconDraw(pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]);
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			if(item == 1)
			{
				pgDpsImage->curImgIdx=DPSImgIdxOld;
				DPSImgIdxOld=1;
			}
			pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]= (UINT8)pgDpsInfo->SelPic ;
			appUsbDps_OsdSelectIconDraw(pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]);
			pgDpsInfo->TotalSelPics=appUsbDps_TotalPrintSelImg(pgDpsInfo,pgDpsImage->totalImg);
			IsSetImage = 1 ;
			if(item == 0)
			{
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}
	
	return ret; 

}
UINT32
appMenuEvt_DPSCopiesInit(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	//pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		//case SP1K_MSG_KEY_PRESS_RIGHT:
		case APP_KEY_PRESS_SET:
			menuProcChild(0);
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}

UINT32
appMenuEvt_DPSCopiesSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();
	dpsInfo_t *pgDpsInfo=appUsbDps_gDpsInfoPtrGet();
	dpsImage_t *pgDpsImage=appUsbDps_gDpsImagePtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			pgDpsInfo->SelPic=(UINT32)item;
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			pgDpsInfo->SelPic=(UINT32)item;
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
		case APP_KEY_PRESS_UP:
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			//menuProcChild (menuProcParaGet(layer, item));
			//item = menuProcCurrItemGet(layer);
			//pgDpsInfo->SelPic=item;
			printf("pDpsMenuOpt->Image.OsdEnOption = %x\n",pDpsMenuOpt->Image.OsdEnOption);
			if(pDpsMenuOpt->Image.OsdEnOption!=DPS_IMAGE_ALL)
			{
				pgDpsInfo->PicBuf[pgDpsImage->curImgIdx-1]= pgDpsInfo->SelPic ;
			}
			else
			{
				UINT8 count = 0;
				for (count=0; count<pgDpsImage->totalImg; count++)
				{
					pgDpsInfo->PicBuf[count]= pgDpsInfo->SelPic ;
				}
			}
			pgDpsInfo->TotalSelPics=appUsbDps_TotalPrintSelImg(pgDpsInfo,pgDpsImage->totalImg);
			menuProcParent ();
			menuProcDisp();
			
			ret = MENU_IGNORE_DEFAULT;
			break;

	}
	
	return ret; 

}
#endif
#if 1
UINT32
appMenuEvt_DPSQualitySet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			pDpsMenuOpt->Quality.SelOption = (UINT16)puiPara->DPSQuality;
			appUsbDps_PaperQualityConfigSet(pDpsMenuOpt);
			//ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}
UINT32
appMenuEvt_DPSLayOutSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			pDpsMenuOpt->Layout.SelOption = (UINT16)puiPara->DPSLayout;
			appUsbDps_PaperQualityConfigSet(pDpsMenuOpt);
			//ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}
UINT32
appMenuEvt_DPSPageSizeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();
	
	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			pDpsMenuOpt->PaperSize.SelOption = (UINT16)puiPara->DPSPageSize;
			appUsbDps_PaperQualityConfigSet(pDpsMenuOpt);
			//ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}
UINT32
appMenuEvt_DPSPaperTypeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();
	
	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			pDpsMenuOpt->PaperType.SelOption = (UINT16)puiPara->DPSPageType;
			appUsbDps_PaperQualityConfigSet(pDpsMenuOpt);
			//ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}
UINT32
appMenuEvt_DPSDateSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();
	
	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			pDpsMenuOpt->DateTime.SelOption = (UINT16)puiPara->DPSDate;
			appUsbDps_PaperQualityConfigSet(pDpsMenuOpt);
			//ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}
UINT32
appMenuEvt_DPSFileNameSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			pDpsMenuOpt->FileNo.SelOption = (UINT16)puiPara->DPSFileName;
			appUsbDps_PaperQualityConfigSet(pDpsMenuOpt);
			//ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}

UINT32
appMenuEvt_DPSDateTypeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			pDpsMenuOpt->DateType.SelOption = (UINT16)puiPara->DPSDateType;
			appUsbDps_PaperQualityConfigSet(pDpsMenuOpt);
			//ret = MENU_IGNORE_DEFAULT;		
			break;
	}
	
	return ret; 

}
#endif

UINT32
appMenuEvt_DPSReSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	dpsMenuOption_t* pDpsMenuOpt=appUsbDps_MenuOptPtrGet();

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	pDpsMenuOpt = pDpsMenuOpt; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_SET:
			pDpsMenuOpt->Reset.SelOption = (UINT16)item;		
			if(item == 0){
				appUsbDps_MenuOptReset();
			}
			break;
	}
	
	return ret; 

}	

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_FisrstItemProcess (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_LastItemProcess (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcParent();
			menuProcDisp(); 
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 
appMenuEvt_ImageSizeSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();		
			menuProcDisp();
			/*appStill_SetResolution(menuProcCurrItemGet(layer));*/   /*junqiong.hu@20121121*/
			ret = MENU_IGNORE_DEFAULT;		
			break;
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp();
			/*appStill_SetResolution(menuProcCurrItemGet(layer));*/  /*junqiong.hu@20121121*/
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_MENU:
			/*appStill_SetResolution(puiPara->ImageSize);*/  /*junqiong.hu@20121121*/
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetResolution(puiPara->ImageSize);
			appStill_SetDateStamp(puiPara->StampMode, puiPara->DateStyle);
			appStill_SetQuality(puiPara->ImageQuality);	
			menuProcParent();
			menuProcDisp();			
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 
appMenuEvt_ImageQaulitySet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetQuality(puiPara->ImageQuality);			
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 
appMenuEvt_AEMeteringSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetMetering(puiPara->AEMetering);
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 
appMenuEvt_WBModeSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			appStill_SetWB(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp();
			appStill_SetWB(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_MENU:
			appStill_SetWB(puiPara->WBMode);
			break;
		case APP_KEY_PRESS_SET:
			if(item == UI_WB_CUSTOM)
			{			
				appStateChange(APP_STATE_STILL_CWB,STATE_PARAM_NORMAL_INIT); /*junqiong.hu@20121225 add for mantis bug 0046601*/
				//appStateChange(APP_STATE_STILL_CWB, STATE_INIT);					
			}	
			else 
			{
				menuProcParaSet(layer, item);
				appStill_SetWB(puiPara->WBMode);
				/*appAwbInit();*/
			}
			/*menuProcParent();
			menuProcDisp();	
			ret = MENU_IGNORE_DEFAULT;*/
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 
appMenuEvt_IsoSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetISO(puiPara->Iso);
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}
UINT32 appMenuEvt_AEModeSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();			
			/*menuProcParaSet(layer, menuProcCurrItemGet(layer));*/  
			appStill_SetExposure(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp();			
			/*menuProcParaSet(layer, menuProcCurrItemGet(layer));*/
			appStill_SetExposure(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_MENU:
			/*menuProcParaSet(layer, item);*/
			appStill_SetExposure(puiPara->AEMode);			
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetExposure(puiPara->AEMode);
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 
appMenuEvt_DriverModeSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetDriveMode(puiPara->DriverMode);
			if(puiPara->DriverMode != UI_DRIVE_OFF) 
			{
				puiPara->AEB = UI_AEB_OFF;
				appStill_SetAEB(puiPara->AEB);
			}
			if((puiPara->DriverMode != UI_DRIVE_OFF\
				&&puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT)\
				||(puiPara->DriverMode != UI_DRIVE_OFF\
				&&puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)) /*modify for mantis bug 0047342*/
			{
				puiPara->FaceTracking = UI_FACETRACK_OFF;
			}
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_AEBSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if(puiPara->DriverMode == UI_DRIVE_OFF)
			{
				menuProcParaSet(layer, item);
				appStill_SetAEB(puiPara->AEB);
			}
			if(puiPara->AEB != UI_AEB_OFF) /*add for mantis bug 0047495*/
			{
				puiPara->FaceTracking = UI_FACETRACK_OFF;
			}
			menuProcParent();
			menuProcDisp();

			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32 
appMenuEvt_SharpnessSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetSharpness(puiPara->Sharpness);
			menuProcParent();
			menuProcDisp();	
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_ImageEffectSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			/*menuProcParaSet(layer, menuProcCurrItemGet(layer));*/  /*modify for mantis bug 0047193 */
			menuProcDisp();
			if(item == UI_EFFECT_SATURATION)
			{
				appIqSaturationFactorSet(150);
			}
			else
			{
				appIqSaturationFactorSet(100);
			}
			
			if(puiPara->ImageEffect==UI_EFFECT_SKETCH){	
			   	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,0);/*kellogs modify 2011.12.1*/

			}
			else{
			   	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,1);/*kellogs modify 2011.12.1*/

			}

			
			if (puiPara->ImageEffect ==UI_EFFECT_LOMO || puiPara->ImageEffect==UI_EFFECT_SKETCH){/*kellogs modify 2011.10.20 need reset zoom*/
                appDZoomReset(APP_DZOOM_UI_MODE_STILL_VIEW);
		        appStill_SetDZoom(puiPara->DigitalZoom);
			}
			
			if(item==UI_EFFECT_MINIATURE){
				printf("miniature effect : pv urgent cb start\n");
				sp5kPreviewUrgentCallbackSet(1, appMiniatureEffectPvCb);
			}
			else{
				sp5kPreviewUrgentCallbackSet(1, NULL);
			}
			
			appIqEffectSet(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			/*menuProcParaSet(layer, menuProcCurrItemGet(layer));*/
			menuProcDisp();
			#if 1
			if(item == UI_EFFECT_SATURATION)
			{
				appIqSaturationFactorSet(150);
			}
			else
			{
				appIqSaturationFactorSet(100);
			}
			#endif
			if(puiPara->ImageEffect==UI_EFFECT_SKETCH){	
			   	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,0);/*kellogs modify 2011.12.1*/

			}
			else{
			   	sp5kStillCapAdvCfgSet(SP5K_CAPTURE_CROP_SCALE_IN_JPEG ,1);/*kellogs modify 2011.12.1*/

			}

			
			if (puiPara->ImageEffect ==UI_EFFECT_LOMO || puiPara->ImageEffect==UI_EFFECT_SKETCH){/*kellogs modify 2011.10.20 need reset zoom*/
                appDZoomReset(APP_DZOOM_UI_MODE_STILL_VIEW);
		        appStill_SetDZoom(puiPara->DigitalZoom);
			}
			
			if(item==UI_EFFECT_MINIATURE){
				printf("miniature effect : pv urgent cb start\n");
				sp5kPreviewUrgentCallbackSet(1, appMiniatureEffectPvCb);
			}
			else{
				sp5kPreviewUrgentCallbackSet(1, NULL);
			}
			
			appIqEffectSet(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_MENU:
			if(puiPara->ImageEffect!=UI_EFFECT_MINIATURE){
				printf("miniature effect : pv urgent cb stop\n");
				sp5kPreviewUrgentCallbackSet(1, NULL);
			}
			#if 1
			if(puiPara->ImageEffect == UI_EFFECT_SATURATION)
			{
				appIqSaturationFactorSet(150);
			}
			else
			{
				appIqSaturationFactorSet(100);
			}
			#endif
			appIqSaturationFactorSet(100);
			appIqEffectSet(puiPara->ImageEffect);
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			if(puiPara->ImageEffect==UI_EFFECT_MINIATURE){
				appStill_SetMiniatureStatus(1);
			}
			else{
				appStill_SetMiniatureStatus(0);
				printf("miniature effect : pv urgent cb stop\n");
				sp5kPreviewUrgentCallbackSet(1, NULL);
			}
			#if 0
			if(puiPara->ImageEffect == UI_EFFECT_SATURATION)
			{
				appIqSaturationFactorSet(150);
			}
			else
			{
				appIqSaturationFactorSet(100);
			}
			#endif
			appIqSaturationFactorSet(100);
			appIqEffectSet(puiPara->ImageEffect);

			menuProcParent();
			menuProcDisp();	

			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_FeatureEffectSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_FeatureEffectSet();
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_StampModeSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetDateStamp(puiPara->StampMode, puiPara->DateStyle);
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_StabilizerSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			/*If stabilizer enabled,face tracking  will be disable*/
			if(puiPara->Stabilizer != UI_STABILIZER_OFF){
				puiPara->FaceTracking = UI_FACETRACK_OFF;
			}
			appStill_SetStabilizer(puiPara->Stabilizer);
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_FaceTrackingSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	layer=layer;//just for clearing waring
	item=item;//just for clearing waring

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			/*If FaceTracking enabled,stabilizer  will be disable*/
			if(puiPara->FaceTracking != UI_FACETRACK_OFF){
				puiPara->Stabilizer = UI_STABILIZER_OFF;
			}
			if((puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)||(puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT))			
			/*junqiong.hu@20121127*/
			{
				puiPara->AutoReviewTime = QV_TIME_OFF;
				puiPara->DriverMode = UI_DRIVE_OFF;
				appStill_SetDriveMode(puiPara->DriverMode);
				puiPara->AEB = UI_AEB_OFF;
				appStill_SetAEB(puiPara->AEB);
			}
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;			
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_PhotoFrameSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			appStill_SetPhotoFrame(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			appStill_SetPhotoFrame(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
		case APP_KEY_PRESS_MENU:
			appStill_SetPhotoFrame(puiPara->PhotoFrame);
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_DigitalZoomSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetDZoom(puiPara->DigitalZoom);
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32 
appMenuEvt_WDRModeSet (
	UINT8 layer, 
	UINT8 item, 
	UINT32 msg, 
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
#ifndef _EVB_HW_
			appIqSetWdr(item);
#endif
			menuProcParent();
			menuProcDisp();	

			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 

}

UINT32 
appMenuEvt_MDModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
) 
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		#if SP5K_MOTION_DETECT
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appMotionDetectSet(APP_MD_EN, item);
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
	}
	return ret; 
}

UINT32 
appMenuEvt_TimeLapseSet( 
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
) 
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetTimeLapse(puiPara->TimeLapse);
			if(puiPara->TimeLapse != UI_TIMELAPSE_OFF)
			{			
				printf("\033[35m appMenuEvt_TimelapseSet dsc \033[0m\n");
				puiPara->ImageSize = UI_PIC_SIZE_2M_169;
				puiPara->VideoFormat = VIDEO_FORMAT_MJPG;
			}
			else
			{
				puiPara->ImageSize = UI_PIC_SIZE_FULL;
				puiPara->VideoFormat= VIDEO_FORMAT_H264;
			}
			menuProcParent();
			menuProcDisp();				
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret; 
}

UINT32 
appMenuEvt_PbFileProtect (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgProtect= puiPara->selImgProtect;
			gpbMainCb->mainStat = PB_MAIN_STAT_IMG_PROT_SUSPEND;
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_PB_IMG_PROT, STATE_PARAM_NORMAL_INIT);	
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 	
}
UINT32 appMenuEvt_PbFileDelete (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbMainCb->isDelHotKey= FALSE;
			gpbUiParam->selImgDelete= puiPara->selImgDelete;
			gpbMainCb->mainStat = PB_MAIN_STAT_IMG_DEL_SUSPEND;
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_PB_IMG_DEL, STATE_PARAM_NORMAL_INIT);			
			break;
	}

	return ret; 

}

UINT32
appMenuEvt_PbSlideCfg(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	
	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcChild(0);
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}


UINT32 
appMenuEvt_PbSlideStart (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if (item==PB_UI_SLIDE_START) 
			{
				gpbMainCb->mainStat = PB_MAIN_STAT_SLIDE_SUSPEND;
				CLEAR_OSD_SRC;
				/*appBeep(TRUE, FALSE);*/  /*junqiong.hu@20121226  for mantis bug 0046650*/
				appStateChange(APP_STATE_PB_SLIDE_SHOW, STATE_PARAM_NORMAL_INIT);
			}	
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;			
	}

	return ret; 

}

UINT32 
appMenuEvt_PbSlideInterval (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideInteval=puiPara->selSlideInteval;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideInteval=puiPara->selSlideInteval;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 

}

UINT32 
appMenuEvt_PbSlideMusic (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			
			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif
			
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			
			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif
			
			ret = MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
			
			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif
			
			if (puiPara->selSlideMusic>SLSHOW_NONE)
			{
			#if SOUND_PLAY_NEW_PROC
				appAddSoundToPlay(slideSoundID[puiPara->selSlideMusic]);
			#else
				if (FAIL==appSoundLoad(pszSlideMusic[puiPara->selSlideMusic]))
				{
					printf("%s: !!FATAL!!, sound load failed \n", __FUNCTION__);
					return ret;
				}
				if (FAIL==appSoundPlay())
				{
					printf("%s: !!FATAL!!, sound play failed \n", __FUNCTION__);
					return ret;
				}
			#endif	
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
			
			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif
			
			if (puiPara->selSlideMusic>SLSHOW_NONE)
			{
			#if SOUND_PLAY_NEW_PROC
				appAddSoundToPlay(slideSoundID[puiPara->selSlideMusic]);
			#else	
				if (FAIL==appSoundLoad(pszSlideMusic[puiPara->selSlideMusic]))
				{
					printf("%s: !!FATAL!!, sound load failed \n", __FUNCTION__);
					return ret;
				}
				if (FAIL==appSoundPlay())
				{
					printf("%s: !!FATAL!!, sound play failed \n", __FUNCTION__);
					return ret;
				}
			#endif	
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

UINT32 
appMenuEvt_PbSlideTransition (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent(); 
			menuProcDisp2Layer();

			gpbUiParam->selSlideTrans=puiPara->selSlideTrans;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideTrans=puiPara->selSlideTrans;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 

}


UINT32 
appMenuEvt_PbSlideRepeat (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;			
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent(); 
			menuProcDisp2Layer();

			gpbUiParam->selSlideRepeat=puiPara->selSlideRepeat;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent(); 
			menuProcDisp2Layer();

			gpbUiParam->selSlideRepeat=puiPara->selSlideRepeat;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	
	return ret; 

}

UINT32 
appMenuEvt_PbCopyToCard (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selCopy2Card= puiPara->selCopy2Card;
			gpbMainCb->mainStat = PB_MAIN_STAT_C2C_SUSPEND;
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_PB_C2C, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 

}

UINT32 
appMenuEvt_PbPhotoEffect (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgEffect= puiPara->selImgEffect;
			appPbEffectPreview(gpbUiParam->selImgEffect);
			if (FAIL==appPbEffectExecute(ppbImgCb)) {
				DBG_PRINT("Saving Effect(%d) failed \n", item);
				appPbErrorProcess(gpbMainCb->err);
				/*
				appStateChange(APP_STATE_PLAY_SUBMENU, STATE_PARAM_NORMAL_INIT);
				*/
				menuProcDisp();
				ret = MENU_IGNORE_DEFAULT;
				return ret;
			} else {
				appPbEffectDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}
			if(gpbUiParam->selImgEffect == PB_UI_EFFECT_RED_EYE || gpbUiParam->selImgEffect ==PB_UI_EFFECT_SKIN_BEAUTY)
			{ 
				sp5kImageEditCallbackSet(NULL);
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 

}

UINT32 
appMenuEvt_PbPhotoRotInit (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 
	param = param;
	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbMainCb->isDelHotKey= FALSE;
			gpbMainCb->mainStat = PB_MAIN_STAT_JPG_ROTATE;
			CLEAR_OSD_SRC;
			/*appPbImagePlay(gpbMainCb->curImgIdx);
			menuProcChild(0);
			menuProcDispYesNo(ID_STR_ROTATE);
			ret = MENU_IGNORE_DEFAULT;*/
			appStateChange(APP_STATE_PB_IMG_ROTATE, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 	
}

UINT32 
appMenuEvt_PbPhotoRotate (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgRotate= puiPara->selImgRotate;
			if (item==PB_UI_ROTATE_SELECT_SET) 
			{
				if (FAIL==appPbRotatePreview(PB_ROT_CW)) 
				{
					appPbErrorProcess(gpbMainCb->err);
					appPbRotateDestroy(ppbImgCb);
					appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
					return MENU_IGNORE_DEFAULT;
				}
			}
			else if (item==PB_UI_ROTATE_SELECT_SAVE) 
			{
				if (FAIL==appPbRotateExecute(ppbImgCb, PB_ROT_SAVE)) 
				{
					appPbErrorProcess(gpbMainCb->err);
				}
				appPbRotateDestroy(ppbImgCb);
				/*
				appStateChange(APP_STATE_PLAY_MAINMENU, STATE_PARAM_NORMAL_INIT);
				*/
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;;
			} 
			else if (item==PB_UI_ROTATE_SELECT_BACK) 
			{
				appPbRotateExecute(ppbImgCb, PB_ROT_CANCEL);
				gpbMainCb->err = PB_ERR_NONE;
				appPbRotateDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return MENU_IGNORE_DEFAULT;;
			} 
			else
			{
				DBG_PRINT("\nFATAL: It should not happen !!\n");
				gpbMainCb->err = PB_ERR_ROTATE_ERROR;
				appPbRotateDestroy(ppbImgCb);
				/*
				appStateChange(APP_STATE_PLAY_MAINMENU, STATE_PARAM_NORMAL_INIT);
				*/
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;
			}	
			break;
	}

	return ret; 

}


UINT32 
appMenuEvt_PbPhotoResize (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgResize= puiPara->selImgResize;
			if (FAIL==appPbResizeExecute(ppbImgCb, item)) 
			{
				DBG_PRINT("Saving RESIZE(%d) failed \n", item);
				appPbErrorProcess(gpbMainCb->err);
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;
			} 
			else 
			{
				appPbResizeDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				ret = MENU_IGNORE_DEFAULT;
			}	
			break;
	}

	return ret; 

}


UINT32 
appMenuEvt_PbPhotoVoiceMemo (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			if(gpbImgCb.isProtected)  /*add for mantis bug 0047522*/
			{
				gpbMainCb->err = PB_ERR_IMG_PROTECTED;
				appPbErrorProcess(gpbMainCb->err);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);	
			}
			else
			{
				gpbUiParam->selVoiceMemo= puiPara->selVoiceMemo;
				gpbMainCb->mainStat = PB_MAIN_STAT_MEMOREC_SUSPEND;
				CLEAR_OSD_SRC;
				appStateChange(APP_STATE_PB_MEMO_REC, APP_UI_MSG_PB_MEMO_REC_CREATE);	
			}
			ret = MENU_IGNORE_DEFAULT;
			break;	
	}

	return ret; 

}


UINT32 
appMenuEvt_PbStartImageSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selStartImage= puiPara->selStartImage;
			gpbMainCb->mainStat = PB_MAIN_STAT_START_IMG_SUSPEND;
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_PB_START_IMG, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 

}


UINT32 
appMenuEvt_PbPhotoFrame (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet(); 
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selPhotoframe= puiPara->selPhotoframe;
			if (FAIL==appPbPhotoframeExecute(ppbImgCb, item)) {
				DBG_PRINT("Saving Photoframe(%d) failed \n", item);
				appPbErrorProcess(gpbMainCb->err);
				//appPbPhotoframeDestroy(ppbImgCb);
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;
			} else {
				appPbPhotoframeDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}	
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 

}

UINT32 
appMenuEvt_FileTypeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg)
	{
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			printf("filetype=%d\n",puiPara->fileType);
			appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			fileTypeSelect=0;
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret; 

}



