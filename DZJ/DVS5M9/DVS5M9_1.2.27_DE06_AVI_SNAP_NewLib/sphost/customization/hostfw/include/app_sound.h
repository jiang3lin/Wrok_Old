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
#ifndef APP_SOUND_H
#define APP_SOUND_H

#include "app_user_setting.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/* The new process can improve the system speed and provide more info after the sound play finished,
   when you open it,you can use appAddSoundToPlay() to play sound,and use appReleaseSound()
   to stop.
   There a special situation that in the still capture state,the shutter voice was played in the middleware, 
   so in the meantime,pls don't play any voices !
*/ 
#define SOUND_PLAY_NEW_PROC    0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	SOUND_ID_NULL,
	SOUND_ID_BEEP,
	SOUND_ID_FOCUSOK,
	SOUND_ID_WARNING,
	SOUND_ID_SHUTTER,
	SOUND_ID_SLIDE_1,
	SOUND_ID_SLIDE_2,
	SOUND_ID_SLIDE_3,
	SOUND_ID_START_1,
	SOUND_ID_START_2,
	SOUND_ID_START_3,
	SOUND_ID_POWEROFF,
	SOUND_ID_TOTAL,
} preloadSoundIdDef_t;

#if SOUND_PLAY_NEW_PROC
typedef struct app_sound_info_s{
	UINT8* soundName;
	UINT32 needPreLoad;  /*0: need to preload  1:has loaded  2: use sp5kResourceFilePlay,not to preload */ 
}app_sound_info_t;
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appAudioDevOn(void);
void appPreloadSoundInit(void);
void appPreloadSoundPlay(UINT32 sound_id);
/*s Add by Aries 09/07/21*/
void appPreloadSoundAbort(void);
/*e Add by Aries 09/07/21*/
void appSundVolumeSet(soundVolume_e vol);

void appSoundUnload(void);
void appSoundStop(void);
void appSoundTerminate(	void);
UINT32 appSoundLoad(UINT8 *pszSoundFilename);
UINT32 appSoundPlay(void);
void appBeep(BOOL waitFinish,	BOOL forceBeep);

#if SOUND_PLAY_NEW_PROC
void appSoundInit();
UINT32 appAddSoundToPlay(UINT32 soundID);
UINT32 appReleaseSound(UINT32 soundID);
UINT32 appSoundPlayEvtSet(void);
UINT32 appWaitSoundQueEmpty(void);

#endif

#endif
