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
#include "app_ui_para.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_media_api.h"

#include "app_tvlcd.h"
#include "app_sound.h"
#include "app_buzzer.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#if SOUND_PLAY_NEW_PROC 
#define APP_SOUND_PRI					16
#define APP_SOUND_TIME_SLICE			0
#define APP_SOUND_TX_AUTO_START			1

#define APP_SOUND_QUE_MSG_SIZE			1
#define APP_SOUND_QUE_SIZE			    (8*4*APP_SOUND_QUE_MSG_SIZE)
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32  preloadSoundPool[SOUND_ID_TOTAL];
static UINT32 ghndSound = 0;

#if SOUND_PLAY_NEW_PROC
static TX_THREAD *ptrdSound;
static TX_QUEUE queSound;  /* record which audio will to play */

/* as a audio play finished flag and ensure that the sound play is through new proc control */
static TX_EVENT_FLAGS_GROUP soundFinEvt;      

app_sound_info_t appSoundQue[]=
{
	[SOUND_ID_NULL]		=	{NULL,0},
	[SOUND_ID_BEEP]		=	{RES_BEEP_SOUND_1,0},
	[SOUND_ID_FOCUSOK]	=	{RES_AF_SOUND_1,0},
	[SOUND_ID_WARNING]	=	{RES_WARNING_SOUND_1,0},
	[SOUND_ID_SHUTTER]	=	{RES_SHUTTER_SOUND_1,0},
	[SOUND_ID_SLIDE_1]	=	{RES_SLIDE_SOUND_1,0},
	[SOUND_ID_SLIDE_2]	=	{RES_SLIDE_SOUND_2,0},
	[SOUND_ID_SLIDE_3]	=	{RES_SLIDE_SOUND_3,0},
	[SOUND_ID_START_1]	=	{RES_POWER_ON_SOUND_1,2},
	[SOUND_ID_START_2]	=	{RES_POWER_ON_SOUND_2,2},
	[SOUND_ID_START_3]	=	{RES_POWER_ON_SOUND_3,2},
	[SOUND_ID_POWEROFF] =	{RES_POWER_OFF,2},
};
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if SOUND_PLAY_NEW_PROC
/*  when state change,determined to clean queSound or wait sound play all done */
#define CLR_OR_WAIT		0      /*   0: clean  1:wait ( unfinished )  */    
extern UINT32 ros_queue_cnt_get(UINT32 queue_id, UINT32 *cnt, UINT32 *size);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
_mediaSoundPlayAbsoluteStop(
	void
)
{
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0 );
	sp5kResDRAMControl(SP5K_RES_DRAM_ABORT,0,0,0,0,0);
}

void
appAudioDevOn(
	void
)
{
	sp5kAudDevSrcSet(SP5K_AUD_DEV_PLAY, (IS_LCD_OUT) ? 
		AUD_PLAY_SRC_FOR_LCD : AUD_PLAY_SRC_FOR_TV);
}

void
appPreloadSoundInit(void)
{
#if SOUND_PLAY_NEW_PROC
	preloadSoundPool[SOUND_ID_BEEP] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,appSoundQue[SOUND_ID_BEEP].soundName,0,0,0,0);
	appSoundQue[SOUND_ID_BEEP].needPreLoad =1;

	preloadSoundPool[SOUND_ID_FOCUSOK] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,appSoundQue[SOUND_ID_FOCUSOK].soundName,0,0,0,0);
	appSoundQue[SOUND_ID_FOCUSOK].needPreLoad =1;
	
	preloadSoundPool[SOUND_ID_WARNING] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,appSoundQue[SOUND_ID_WARNING].soundName,0,0,0,0);
	appSoundQue[SOUND_ID_WARNING].needPreLoad =1;
#else
	preloadSoundPool[SOUND_ID_BEEP] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,RES_WAV_BEEP ,0,0,0,0);

	preloadSoundPool[SOUND_ID_FOCUSOK] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,RES_WAV_BEEP,0,0,0,0);

	preloadSoundPool[SOUND_ID_WARNING] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,RES_WAV_WARNING,0,0,0,0);
#endif
}

UINT32 
appAudDevStatusPolling(
	void
)
{
	UINT32 status, i=0;	
	
	sp5kAudDevStatusGet(&status);
	while (((status&SP5K_AUD_STATUS_PLAY_STOP)==0 
		&& (status&SP5K_AUD_STATUS_PLAY_PAUSE) == 0)&& i<500)
	{
		appTimeDelayMs(20);
		sp5kAudDevStatusGet(&status);
		i+=20;
	}	
	return status;
}	


void
appPreloadSoundPlay(
	UINT32 sound_id
)
{
	UINT32 status;

	status = appAudDevStatusPolling();
	if((status&SP5K_AUD_STATUS_PLAY_STOP)==0)
	{
		return;
	}

	status=sp5kResDRAMControl(SP5K_RES_DRAM_PLAY,preloadSoundPool[sound_id],0,0,0,0);
	/*printf("appPreloadSoundPlay:0x%x\n",status);*/

	do 
	{
		appTimeDelayMs(20);
		sp5kAudDevStatusGet(&status);
	} while ((status&SP5K_AUD_STATUS_PLAY_STOP)==0);


}


void
appPreloadSoundAbort(
	void
)
{
	sp5kResDRAMControl(SP5K_RES_DRAM_ABORT,0,0,0,0,0);
}



void
appSundVolumeSet(
	soundVolume_e vol
)
{
	UINT32 volumeTbl[SOUND_VOLUME_MAX] = {0,9,16,30};
	UINT32 volume;

	if (vol >= SOUND_VOLUME_MAX) 
	{
		return;
	}
	if(IS_LCD_OUT) 
	{
		volume = volumeTbl[vol];
	}
	else 
	{
		volume = volumeTbl[vol] >> 1;
	}	
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_PLAY,volume);	
}


/*-------------------------------------------------------------------------
 *  File Name : appSoundUnload
 *------------------------------------------------------------------------*/
void
appSoundUnload(
	void
)
{
	if (ghndSound) 
	{
		sp5kResDRAMUnload(ghndSound);
		ghndSound = 0;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appSoundStop
 *------------------------------------------------------------------------*/
void
appSoundStop(
	void
)
{
	DBG_PRINT("%s !! \n", __FUNCTION__);

	if (ghndSound) 
	{
		DBG_PRINT("%s: Unload ResDRAM \n", __FUNCTION__);
		sp5kResDRAMControl(SP5K_RES_DRAM_ABORT, 0, 0, 0, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appSoundTerminate
 *------------------------------------------------------------------------*/
void
appSoundTerminate(
	void
)
{
	DBG_PRINT("%s !! \n", __FUNCTION__);

	appSoundStop();
	appSoundUnload();
}


/*-------------------------------------------------------------------------
 *  File Name : appSoundLoad
 *------------------------------------------------------------------------*/
UINT32
appSoundLoad(
	UINT8 *pszSoundFilename
)
{
	DBG_PRINT("%s: %s !! \n", __FUNCTION__, pszSoundFilename);

	if (pszSoundFilename==NULL) 
	{
		DBG_PRINT("%s: Null PTR \n", __FUNCTION__);
		return FAIL;
	}

	/*-- Release previoud sound resource, if any */
	appSoundTerminate();

	/*-- Music preload	 */
	ghndSound = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV, pszSoundFilename, 0, 0, 0, 0);

	if (ghndSound==0) 
	{
		DBG_PRINT("%s: !!ERROR!!, Music preload failed !! \n", __FUNCTION__);
		return FAIL;
	}

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appSoundPlay
 *------------------------------------------------------------------------*/
UINT32
appSoundPlay(
	void
)
{
	if (!ghndSound) 
	{
		DBG_PRINT("%s: Null sound handle \n", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==sp5kResDRAMControl(SP5K_RES_DRAM_PLAY, ghndSound, 0, 0, 0, 0)) 
	{
		DBG_PRINT("%s: Sound play failed !!\n", __FUNCTION__);
		return FAIL;
	}

	DBG_PRINT("%s: Sound play OK !! \n", __FUNCTION__);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appBeep
 *------------------------------------------------------------------------*/
void
appBeep(
	BOOL waitFinish,
	BOOL forceBeep
)
{
	uiPara_t * puiPara = appUiParaGet();
	UINT8 sysBeep = puiPara->soundcontrol;
	_mediaSoundPlayAbsoluteStop();

	if (sysBeep || forceBeep) 
	{
		
		#if !SOUND_PLAY_NEW_PROC	
		appPreloadSoundPlay(SOUND_ID_BEEP);

		if (waitFinish) 
		{
			UINT32 param;
			appHostMsgWait(SP5K_MSG_MEDIA_SOUND_FINISH, &param, 2000);
		}
		#else
		waitFinish = waitFinish;
		appAddSoundToPlay(SOUND_ID_BEEP);
		#endif
	}
}

#if SOUND_PLAY_NEW_PROC

UINT32 appWaitSoundQueEmpty(void)
{
	UINT32 cnt,tmp;
	#if CLR_OR_WAIT
	
	#else
	while(1){
		ros_queue_cnt_get(queSound,&cnt,&tmp);
		if(cnt){
			sp5kOsQueueReceive(&queSound,&tmp,TX_WAIT_FOREVER);
		}else{
			return SUCCESS;
		}
	}	
	#endif
	
	return SUCCESS;
} 
UINT32 appAddSoundToPlay(UINT32 soundID)
{
	return sp5kOsQueueSend(&queSound,&soundID,TX_NO_WAIT);
}

UINT32 appReleaseSound(UINT32 soundID)
{
	UINT32 ret;
	ret = sp5kResDRAMControl(SP5K_RES_DRAM_ABORT, 0, 0, 0, 0, 0);
	
	if(preloadSoundPool[soundID]){
		appSoundQue[soundID].needPreLoad = 0;
		return sp5kResDRAMUnload(preloadSoundPool[soundID]);
	}
	return ret;
}

UINT32 appSoundPlayEvtSet(void)
{
	ULONG event = 1;
	return sp5kOsEventFlagsSet(&soundFinEvt,event,TX_OR);
}

UINT32 appSoundQueInit()
{
	return sp5kOsQueueCreate(&queSound,"appSoundPlayQue",APP_SOUND_QUE_MSG_SIZE,NULL,APP_SOUND_QUE_SIZE);
}

void appSoundTask(ULONG para)
{
	UINT32 soundID;
	ULONG reqFlag = 1,soundFlag = 1;;
	
	para = para;
	
	while(1){
		if(sp5kOsQueueReceive(&queSound,&soundID,TX_WAIT_FOREVER) == SUCCESS){
			printf("sound -- %s  %d\n",appSoundQue[soundID].soundName,appSoundQue[soundID].needPreLoad);
			if(!appSoundQue[soundID].needPreLoad){
				preloadSoundPool[soundID] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,appSoundQue[soundID].soundName,0,0,0,0);
				appSoundQue[soundID].needPreLoad = 1;
			}

			if(preloadSoundPool[soundID]){
				sp5kResDRAMControl(SP5K_RES_DRAM_ABORT, 0, 0, 0, 0, 0);
				sp5kOsEventFlagsGet(&soundFinEvt,reqFlag,TX_OR_CLEAR,&soundFlag,TX_NO_WAIT);
				appPreloadSoundPlay(soundID);
				sp5kOsEventFlagsGet(&soundFinEvt,reqFlag,TX_OR_CLEAR,&soundFlag,TX_WAIT_FOREVER);
				sp5kHostMsgSend(APP_UI_MSG_AUDIO_PLAY_FINISH,soundID);
			}

			if(appSoundQue[soundID].needPreLoad == 2){
				sp5kOsEventFlagsGet(&soundFinEvt,reqFlag,TX_OR_CLEAR,&soundFlag,TX_NO_WAIT);
				sp5kResourceFilePlay(SP5K_RES_TYPE_WAV, appSoundQue[soundID].soundName, NULL);
				sp5kOsEventFlagsGet(&soundFinEvt,reqFlag,TX_OR_CLEAR,&soundFlag,TX_WAIT_FOREVER);
				sp5kHostMsgSend(APP_UI_MSG_AUDIO_PLAY_FINISH,soundID);
			}
		}
	}
}

void appSoundInit()
{
	if(appSoundQueInit() != SUCCESS){
		printf("sound play queue create fail!!!\n");
		return;
	}
	
	sp5kOsEventFlagsCreate(&soundFinEvt,"appsoundEvent");
	if(!soundFinEvt){
		printf("soundFinMut create fail\n");
		return;
	}

	ptrdSound = sp5kOsThreadCreate(
						"soundPlay",
						appSoundTask,
						0,
						APP_SOUND_PRI,
						APP_SOUND_PRI,
						APP_SOUND_TIME_SLICE,
						APP_SOUND_TX_AUTO_START);	
}
#endif
