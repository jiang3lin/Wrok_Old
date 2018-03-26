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
#include "gpio_custom.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_menu.h"
#include "app_user_setting.h"
#include "app_disk.h"
#include "app_dcf.h"
#include "app_battery.h"
#include "app_error_msg.h"
#include "app_errorosd.h"
#include "app_btn_def.h"
#include "app_playback_def.h"
#include "app_still.h"
#include "app_rtc.h"
#include "app_sound.h"
#include "sp5k_capture_api.h"
#include "app_setup_lib.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_playback_def.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_ui_para.h" 
#include "app_audio.h"
 /**************************************************************************
  * 						  C O N S T A N T S 						   *
  **************************************************************************/ 

uiSoundSetting_t soundcontrol;
uiSoundSetting_t *psoundcontrol= &soundcontrol;
extern view_param_t *pViewParam;

void appSoundControl(SOUNDCONTROL_MODE_e soundmode );

struct {
	UINT8 *fntname, *strname;
} LangTbl[][2] =
{
	{{ ID_FONT_ENG22X28, ID_STRTK_TBL_ENG },			{ ID_FONT_ENG_HDMI, ID_STRTK_TBL_ENG }},
	{{ ID_FONT_SCH28X28, ID_STRTK_TBL_SCH },			{ ID_FONT_SCH_HDMI, ID_STRTK_TBL_SCH }},
	{{ ID_FONT_TCH28X28, ID_STRTK_TBL_TCH },			{ ID_FONT_TCH_HDMI, ID_STRTK_TBL_TCH }},
	{{ ID_FONT_ENG22X28, ID_STRTK_TBL_ENG},			{ ID_FONT_ENG_HDMI, ID_STRTK_TBL_ENG}},
	{{ ID_FONT_CZECH28x28, ID_STRTK_TBL_CZECH },		{ ID_FONT_CZECH_HDMI, ID_STRTK_TBL_CZECH }},
	{{ ID_FONT_DUTCH28x28, ID_STRTK_TBL_DUTCH },		{ ID_FONT_DUTCH_HDMI, ID_STRTK_TBL_DUTCH }},
	{{ ID_FONT_FRENCH28x28, ID_STRTK_TBL_FRENCH },		{ ID_FONT_FRENCH_HDMI, ID_STRTK_TBL_FRENCH }},
	{{ ID_FONT_GREEK28x28, ID_STRTK_TBL_GREEK },		{ ID_FONT_GREEK_HDMI, ID_STRTK_TBL_GREEK }},
	{{ ID_FONT_HUNGARIAN28x28, ID_STRTK_TBL_HUNGARIAN },	{ ID_FONT_HUNGARIAN_HDMI, ID_STRTK_TBL_HUNGARIAN }},
	{{ ID_FONT_ITALIAN28x28, ID_STRTK_TBL_ITALIAN },		{ ID_FONT_ITALIAN_HDMI, ID_STRTK_TBL_ITALIAN }},
	{{ ID_FONT_ENG22X28, ID_STRTK_TBL_INDONESIAN },	{ ID_FONT_ENG_HDMI, ID_STRTK_TBL_INDONESIAN }},
	{{ ID_FONT_MALAY28x28, ID_STRTK_TBL_MALAY },		{ ID_FONT_MALAY_HDMI, ID_STRTK_TBL_MALAY }},
	{{ ID_FONT_POLISH28x28, ID_STRTK_TBL_POLISH },		{ ID_FONT_POLISH_HDMI, ID_STRTK_TBL_POLISH }},
	{{ ID_FONT_PORTUGUESE28x28, ID_STRTK_TBL_PORTUGUESE },	{ ID_FONT_PORTUGUESE_HDMI, ID_STRTK_TBL_PORTUGUESE }},
	{{ ID_FONT_RUSSIAN28x28, ID_STRTK_TBL_RUSSIAN },		{ ID_FONT_RUSSIAN_HDMI, ID_STRTK_TBL_RUSSIAN }},
	{{ ID_FONT_ROMANIAN28x28, ID_STRTK_TBL_ROMANIAN },	{ ID_FONT_ROMANIAN_HDMI, ID_STRTK_TBL_ROMANIAN }},
	{{ ID_FONT_GERMAN28x28, ID_STRTK_TBL_GERMAN },		{ ID_FONT_GERMAN_HDMI, ID_STRTK_TBL_GERMAN }},
	{{ ID_FONT_SWEDISH28x28, ID_STRTK_TBL_SWEDISH },	{ ID_FONT_SWEDISH_HDMI, ID_STRTK_TBL_SWEDISH }},
	{{ ID_FONT_TURKISH28x28, ID_STRTK_TBL_TURKISH },		{ ID_FONT_TURKISH_HDMI, ID_STRTK_TBL_TURKISH }},
	{{ ID_FONT_CROATION28x28, ID_STRTK_TBL_CROATION },	{ ID_FONT_CROATION_HDMI, ID_STRTK_TBL_CROATION }},
	{{ ID_FONT_ARABIC28X28, ID_STRTK_TBL_ARABIC },	{ ID_FONT_ARABIC_HDMI, ID_STRTK_TBL_ARABIC }},
	{{ ID_FONT_THAI28X28, ID_STRTK_TBL_THAI },		{ ID_FONT_THAI_HDMI, ID_STRTK_TBL_THAI }},
	{{ ID_FONT_JAPAN28x28,ID_STRTK_TBL_JAPAN},		{ ID_FONT_JAPAN_HDMI,ID_STRTK_TBL_JAPAN}},
	{{ ID_FONT_KOREAN28X28,ID_STRTK_TBL_KOREAN},	{ ID_FONT_KOREAN_HDMI,ID_STRTK_TBL_KOREAN}}
};
#define RTC_VALUE_CHECK(var, min, max)      { \
				(var) = ((var) < (min)) ? (max) : (((var) > (max)) ? (min) : (var)); }

#define VAR_LIMIT(var, min, max)  { \
				(var) = ((var) < (min)) ? (min) : (((var) > (max)) ? (max) : (var)); }

 /**************************************************************************
  * 						G L O B A L    D A T A						   *
  **************************************************************************/
#if 0
static UINT32 month[2][12] = 
{ 
	{ 31,28,31,30,31,30,31,31,30,31,30,31 },
	{ 31,29,31,30,31,30,31,31,30,31,30,31 } 
};
#endif
appRTC_t rtcTime;

 /**************************************************************************
  * 			  F U N C T I O N	 D E C L A R A T I O N S			   *
  **************************************************************************/
BOOL
_setupMenuFunctionFormat(
	void
)
{
	UINT32 ret;
	UINT32 time_t0 = 0, time_t1 = 0;
	uiPara_t *psPara = appUiParaGet();
	
	appBtnDisable(BTN_ALL);	
	
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	if (IS_CARD_EXIST && IS_CARD_LOCK) 
	{
		printf("[WARN] Card Lock, Fail to format!!\n");
		appErrMsgWithSound(ERR_MSG_CARD_LOCKED,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_2SEC);
		ret= FALSE;
	}
	else 
	{
		LED_FLICKER_FOMATE;
		appOsdMessageBoxShow(TRUE,ID_STR_BUSY___PLEASE_WAIT);
		time_t0 = appMsTimeGet();
		DBG_PRINT("Format disk%d start ..\n",appActiveDiskGet());	
		ret = appDiskFormat(appActiveDiskGet());
		DBG_PRINT("Format disk%d done ..\n",appActiveDiskGet());
		if (psPara->fileNumber  == FILENUM_RESET) 
		{
			psPara->dcfDirKey = 0;
			psPara->dcfFileKey = 0;
		}
		appDcfNexDcfKeySet((psPara->fileNumber  == FILENUM_RESET),psPara->dcfDirKey,psPara->dcfFileKey);
		
		time_t1 = appMsTimeGet() - time_t0;
		if (time_t1 < 500)
		{
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS , (500 - time_t1));
		}
		appOsdMessageBoxShow(FALSE,ID_STR_BUSY___PLEASE_WAIT);
		//appPbShowBG();
		if (ret == SUCCESS) 
		{			
			appErrMsgWithSound(ERR_MSG_COMPLETED,ERR_MSG_ONLY,ERR_MSG_SHOW_1SEC);
		}
		else 
		{			
			appErrMsgWithSound(ERR_MSG_FORMAT_ERRPR,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_1SEC);
		}
		LED_NORMAL_ON;		
		ret = TRUE;
	}

	appBtnEnable(BTN_ALL);
	
	return ret;

}

void
_setupMenuFunctionResetAll(
	void
)
{
	appBtnDisable(BTN_ALL);	

	appFactorySettingApply();	
	CLEAR_OSD_SRC;
	CLEAR_PIP_SRC;
	appErrMsgWithSound(ERR_MSG_COMPLETED,ERR_MSG_ONLY,ERR_MSG_SHOW_2SEC);
	appBtnEnable(BTN_ALL);
}

 

 /*-------------------------------------------------------------------------
*	File Name : appSetupMenu_Sounn_Adjust
*------------------------------------------------------------------------*/
void
appSetupMenu_Sounn_Adjust(
	SOUNDS_e soundItem,
	uiSoundSetting_t *psound,
	UINT32 dir
)
{
	#if SOUND_PLAY_NEW_PROC
	UINT32 powerOnSoundID[] = {
		SOUND_ID_NULL,
		SOUND_ID_START_1,
		SOUND_ID_START_2,
		SOUND_ID_START_3,
	};
	#else
	UINT8 *powerOnSound[POWERON_SOUND_MAX] = 
	{ 
		NULL,
		RES_WAV_POWER_ON1,
		RES_WAV_POWER_ON2,
		RES_WAV_POWER_ON3
	};
	#endif
	/*uiPara_t * puiPara = appUiParaGet();*/

	switch(soundItem)
	{
		#if !CAM_TYPE_CVR
		case SOUNDS_SHTTUER:
			 if (dir) 
			 {
				 psound->shutter++;
				if ( psound->shutter == SHUTTER_SOUND_MAX) 
				{
					psound->shutter = SHUTTER_SOUND_NONE;
				}
			}
			else 
			{
				if (psound->shutter == SHUTTER_SOUND_NONE)
				{
					psound->shutter = SHUTTER_SOUND_MAX -1;
				}
				else
				{
					psound->shutter--;
				}
			}
			
			if(psound->shutter != SHUTTER_SOUND_NONE) 
			{
			#if SOUND_PLAY_NEW_PROC
				appAddSoundToPlay(SOUND_ID_SHUTTER);
			#else
				if(appSoundLoad(RES_WAV_SHUTTER) == SUCCESS)
				{
					appSoundPlay();
				}
			#endif	
			}
			break;
		case SOUNDS_STARTUP:
			 if (dir) 
			 {
				 psound->powerOn++;
				if ( psound->powerOn == POWERON_SOUND_MAX) 
				{
					psound->powerOn = POWERON_SOUND_NONE;
				}
			}
			else 
			{
				if (psound->powerOn == POWERON_SOUND_NONE) 
				{
					psound->powerOn = POWERON_SOUND_MAX -1;
				}
				else
				{
					psound->powerOn--;
				}
			}
			if(psound->powerOn != POWERON_SOUND_NONE)
			{
			#if SOUND_PLAY_NEW_PROC
				appAddSoundToPlay(powerOnSoundID[psound->powerOn]);
			#else
				if(appSoundLoad(appRootGet(powerOnSound[psound->powerOn])) == SUCCESS)
				{
					appSoundPlay();
				}
			#endif	
			}
			break;
			#endif
		case SOUNDS_BEEP:
			 if (dir)
			 {
				 psound->beep++;
				if ( psound->beep == BEEP_TOTAL) 
				{
					psound->beep = BEEP_OFF;
				}
			}
			else
			{
				if (psound->beep == BEEP_OFF)
				{
					psound->beep = BEEP_TOTAL -1;
				}
				else
				{
					psound->beep--;
				}
			}

			
			if(psound->beep == BEEP_ON)
			{
				/*puiPara->soundcontrol = SOUND_ON;*/ 
				#if SOUND_PLAY_NEW_PROC
					appAddSoundToPlay(SOUND_ID_BEEP);
				#else
					appPreloadSoundPlay(SOUND_ID_BEEP);
				#endif
			}
			else
			{
				/*puiPara->soundcontrol = SOUND_OFF;*/  /*mark for mantis bug 0046732 */
			}
			break;
		case SOUNDS_VOULUME:
			 if (dir)
			 {
				psound->volume++;
				if ( psound->volume == SOUND_VOLUME_MAX)
				{
					psound->volume = SOUND_VOLUME_OFF;
				}
			}
			else 
			{
				if (psound->volume == SOUND_VOLUME_OFF)
				{
					psound->volume = SOUND_VOLUME_MAX -1;
				}
				else
				{
					psound->volume--;
				}
			}
			appSundVolumeSet(psound->volume);
			#if SOUND_PLAY_NEW_PROC
				appAddSoundToPlay(SOUND_ID_BEEP);
			#else
				appPreloadSoundPlay(SOUND_ID_BEEP);
			#endif
			break;
		#if CAM_TYPE_CVR
		case SOUNDS_REC:
			psound->voicerec = !psound->voicerec;
			if(psound->voicerec==VOICEREC_ON)
			{
				appAudioRecVolumeRestore();
			}
			else
			{
				appAudioRecVolumeMute();
			}
			 break;
		#endif
		default:
			break;
	}	
}
/*-------------------------------------------------------------------------
*	File Name : appSetupMenu_SetCurLanguage
*------------------------------------------------------------------------*/
void appSetupMenu_SetCurLanguage(UINT8 nCurLanguage)
{
	if (nCurLanguage>=LANG_MAX)
	{
		return;
	}
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		appOsd_FontTextFontSet(appRootGet(LangTbl[nCurLanguage][1].fntname));		
		appOsd_FontTextStringSet(appRootGet(LangTbl[nCurLanguage][1].strname));
	}
	else{
		appOsd_FontTextFontSet(appRootGet(LangTbl[nCurLanguage][0].fntname));		
		appOsd_FontTextStringSet(appRootGet(LangTbl[nCurLanguage][0].strname));
	}
	#endif
}

/*-------------------------------------------------------------------------
*	File Name : applightFrequencySetting
*------------------------------------------------------------------------*/
void applightFrequencySetting(LIGHT_FREQ_e lightfreq)
{
	switch(lightfreq)
	{
		case LIGHT_FREQ_50HZ:
			appStill_SetExpFlickerMode(0);
			break;
		case LIGHT_FREQ_60HZ:
			appStill_SetExpFlickerMode(1);
			break;
		case LIGHT_FREQ_AUTO:
			sp5kModeCfgSet(SP5K_MODE_CFG_BAND_DETECT_PEAK, 200 , 310 ,400);
			sp5kModeCfgSet(SP5K_MODE_CFG_BAND_DETECT_ENABLE, 1);
			return;
		default:
			break;
	}
	appAePvReset();

}

/********************************************
*Functiion name:appSoundControl
********************************************/
void appSoundControl(SOUNDCONTROL_MODE_e soundmode )
{

memcpy(psoundcontrol,&pUiSetting->sound,sizeof(uiSoundSetting_t));
switch(soundmode)
  {
 	case SOUND_OFF:
              /*psoundcontrol->volume= SOUND_VOLUME_OFF;
              appSundVolumeSet(psoundcontrol->volume);
              memcpy(&pUiSetting->sound,psoundcontrol,sizeof(uiSoundSetting_t));*/
              IO_SPEAKER_OFF;
	         break;
	case SOUND_ON:
              /*psoundcontrol->volume= SOUND_VOLUME_MIDDLE;
              appSundVolumeSet(psoundcontrol->volume);
              appPreloadSoundPlay(SOUND_ID_BEEP);
              memcpy(&pUiSetting->sound,psoundcontrol,sizeof(uiSoundSetting_t));*/
              IO_SPEAKER_ON;
	       break;
	default:
		 break;
  }
}

