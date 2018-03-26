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
#include "sp5k_dcf_api.h"
#include "app_timer.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define PB_AUDIO_ELAPSED_TIMER_MS 	500
#define PB_AUDIO_MIN_MS				500

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define AUDIO_CRITICAL_OP_POST_SLEEP	APP_SLEEP_MS(200)
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
#if 0
typedef enum {
	ADO_SXN_START,
	ADO_SXN_END,
	ADO_SXN_DO_ALL_START,
	ADO_SXN_DO_ALL_END,
} videoUIKeyCtrl_t;
#endif

typedef enum {
	AUDIO_ERR_OUT_OF_ELAPSE_TMR,
} videoErrorCode_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 gaudElapTimer = TIMER_NULL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbAudioStExit(UINT32 nextSt, UINT32 stMsg);

#if 0
/*-------------------------------------------------------------------------
 *  File Name : videoUIKeyControl
 *------------------------------------------------------------------------*/
static
void
videoUIKeyControl(
	UINT8 ctrl
)
{
	PB_VERBOSE("%s: ctrl(%d) \n", __FUNCTION__, ctrl);
	switch (ctrl){
	case ADO_SXN_START:
	case ADO_SXN_DO_ALL_START:
		appBtnDisable(BTN_ALL);
		break;
	case ADO_SXN_END:
	case ADO_SXN_DO_ALL_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("MPLAY: undefined key session !!\n");
		break;
	}
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : memoPlayAttrBuildup
 *------------------------------------------------------------------------*/
static
UINT32
memoPlayAttrBuildup(
	void
)
{
	UINT8 szWavName[PB_DCF_FILENAME_LEN_MAX];
	int i;

	/*-- Re-construct WAV filename for JPG Memo
	 */
	memset(szWavName, 0, PB_DCF_FILENAME_LEN_MAX);
	memcpy(szWavName, gpbImgCb.szDcfFileName, sizeof(gpbImgCb.szDcfFileName));
	i = 0;
	while( szWavName[i] != '\0' ){
		i++;
	}
	i = i - 3;
	memcpy(&szWavName[i], "WAV", 3);
	PB_VERBOSE("%s: MEMO file (%s) ...\n", __FUNCTION__, szWavName);

	/*-- Set MediaPlayFilename */
	sp5kMediaPlayFileNameSet(&szWavName[0]);

	/*-- Parse MediaPlayFile */
	if (FAIL==sp5kMediaPlayAttrParse()) {
		PB_VERBOSE("%s: MediaPlayParse failed !!\n", __FUNCTION__);
		return FAIL;
	}

	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbAudioResume
 *------------------------------------------------------------------------*/
static
void
appPbAudioResume(
	void
)
{
	PB_VERBOSE("%s: \n", __FUNCTION__);

	/*-- Retrieve elapsed time */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);

	/*-- Star ELAP_TMR */
	gaudElapTimer=appTimerSet(PB_AUDIO_ELAPSED_TIMER_MS, "ELAP_TMR");
	if (TIMER_NULL==gaudElapTimer) {
		PB_DBG("%s: Out of ADO elapsed TMR !! \n", __FUNCTION__);
		sp5kHostMsgSend(APP_UI_MSG_PB_AUDIO_ERROR, AUDIO_ERR_OUT_OF_ELAPSE_TMR);
		return;
	}

	/*-- Go on Resume */
	if (gpbImgCb.fileType==PB_FT_JPG_MEMO) {
		sp5kMediaMemoControl(gcurrDcfIdx, SP5K_MEDIA_MEMO_RESUME);
	} else {
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);
	}
	/*s Modify by Aries 10/06/08*/
	#if 0 //(KIT_WITH_HDMI)	
	if(IS_HDMI_IN){
		sp5kAudDevSpeakerCtrl(0);
    		WRITE8(0xb000605c, 3);
		WRITE8(0xb0006068, 2);
		WRITE8(0xb0006070, 1);
		WRITE8(0xb00000b0, 0x1f);
		WRITE8(0xb0000074, 1);
		WRITE8(0xb0000090, 5);
	}	
	#endif
	/*e Modify by Aries 10/06/08*/
	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbMainCb.subStat = PB_AUDIO_STAT_PLAYING;

	/*-- Refresh elapsed time immediately !! */

}

/*-------------------------------------------------------------------------
 *  File Name : appPbAudioPause
 *------------------------------------------------------------------------*/
static
UINT32
appPbAudioPause(
	void
)
{
	PB_VERBOSE("%s: \n", __FUNCTION__);

	/*-- Constraint :
	 * A. The very beginning and ending 1.1 sec, NO Pause.
	 */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
	if ( IS_MEDIA_BEGINNING_CRITICAL_TIME(gpbImgCb.mediaElapTime, PB_AUDIO_CRITICAL_TIME) 
		||IS_MEDIA_ENDING_CRITICAL_TIME(gpbImgCb.mediaElapTime, PB_AUDIO_CRITICAL_TIME, gpbImgCb.mediaTotalTime) )
	{
		PB_VERBOSE("ADO: Critical period, ignore PAUSE !!\n");
		return FAIL;
	}

	/*-- Go On Puase operation */
	if (gpbImgCb.fileType==PB_FT_JPG_MEMO) 
	{
		sp5kMediaMemoControl(gcurrDcfIdx, SP5K_MEDIA_MEMO_PAUSE);
	} 
	else 
	{
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_PAUSE, 0, 0);
	}
	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbMainCb.subStat = PB_AUDIO_STAT_PAUSE;

	/*-- Stop ELAP_TMR */
	appTimerClear(&gaudElapTimer);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbAudioTerminate
 *------------------------------------------------------------------------*/
static
UINT32
appPbAudioTerminate(
	void
)
{
	PB_VERBOSE("%s: audioStat(%d) \n", __FUNCTION__, gpbMainCb.subStat);

	/*-- Shall stop timer */
	appTimerClear(&gaudElapTimer);

	/*-- Terminate audio/memo play */
	if (gpbImgCb.fileType==PB_FT_JPG_MEMO) {
		sp5kMediaMemoControl(gcurrDcfIdx, SP5K_MEDIA_MEMO_ABORT);
	} else {
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);
	}

	gpbMainCb.subStat = PB_AUDIO_STAT_STANDBY;
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbAudioPlayStart
 *------------------------------------------------------------------------*/
static
UINT32
appPbAudioPlayStart(
	void
)
{
	UINT32 ret;

	PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- UI OSD...
	 */
	/*-- Go on play audio/memo */
	if (gpbImgCb.fileType==PB_FT_JPG_MEMO) 
	{
		ret = sp5kMediaMemoControl(gcurrDcfIdx, SP5K_MEDIA_MEMO_PLAY);
	} 
	else 
	{
		ret = sp5kMediaPlayControl(SP5K_MEDIA_PLAY_START, 0, 0);
	}
	
	if (FAIL==ret) 
	{
		return FAIL;
	}
	gpbMainCb.subStat = PB_AUDIO_STAT_PLAY_STARTING;
		/*-- SP5KFW will send SP5K_MSG_MEDIA_PLAY_OK or
		 * SP5K_MSG_MEDIA_PLAY_FAIL to notify the result of PLAY_START cmd.
		 * Go and wait for the message !!
		 */
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbAudioStCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbAudioPlayStCreate(
	void
)
{
	PB_VERBOSE("ADO: Play Create !! \n");
    
	/*-- If MEMO, build up MEMO attributes */
	if (gpbImgCb.fileType==PB_FT_JPG_MEMO) 
	{
		if (FAIL==memoPlayAttrBuildup()) 
		{
			PB_VERBOSE("%s: MEMO PlayAttrBuildup failed \n", __FUNCTION__);
			return FAIL;
		}
	}

	/*-- Retrieve media information */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &gpbImgCb.mediaTotalTime);
	PB_VERBOSE("SP5K_MEDIA_ATTR_DURATION [%d]\n", gpbImgCb.mediaTotalTime);

	if(0==gpbImgCb.mediaTotalTime || gpbImgCb.mediaTotalTime<PB_AUDIO_MIN_MS) 
	{
		PB_LOG("\n%s: Zero length or less than(%d)ms !! \n", __FUNCTION__, PB_AUDIO_MIN_MS);
		return FAIL;
	}

	/*-- Register TIMER for elapsed time process */
	gaudElapTimer = appTimerSet(PB_AUDIO_ELAPSED_TIMER_MS, "ADO_ELAP");
	if (gaudElapTimer==TIMER_NULL) 
	{
		PB_LOG("%s: Out of ADO_ELAP timer failed !! \n", __FUNCTION__);
		sp5kHostMsgSend(APP_UI_MSG_PB_AUDIO_ERROR, AUDIO_ERR_OUT_OF_ELAPSE_TMR);
		return FAIL;
	}

	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbImgCb.mediaElapTime = 0;
	appPowerSavingEnableSet(FALSE);/*++ for not power saving while playing video*/

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbAudioStClose
 *------------------------------------------------------------------------*/
static
void
appPbAudioStClose(
	void
)
{
	PB_DBG("ADO: State closing \n");

	if (gpbMainCb.subStat!=PB_AUDIO_STAT_STANDBY) {
		appPbAudioTerminate();
	}

	/*-- Shall close timer */
	if (gaudElapTimer!=TIMER_NULL) {
		appTimerClear(&gaudElapTimer);
	}

	gpbMainCb.subStat = PB_AUDIO_STAT_STANDBY;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbAudioStExit
 *------------------------------------------------------------------------*/
static
void
appPbAudioStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("ADO: State exiting \n");
	appPbAudioStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbAudioStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbAudioStInit(
	UINT32 param
)
{
	PB_VERBOSE("ADO: State initing, param(%d) ...\n", param);

	gpbMainCb.subStat = PB_AUDIO_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_AUDIO_CREATE, 0);
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbAudioStOnMsg
 *------------------------------------------------------------------------*/
void
appPbAudioStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("ADO: OnMsg [0x%08x], [0x%08x] ... \n", msg, param);

	switch (msg) 
	{
	case SP5K_MSG_TIMER:
		PB_VERBOSE("ADO: SP5K_MSG_TIMER(0x%08x)\n", param);
		if (gaudElapTimer!=param) 
		{
			PB_VERBOSE("ADO: Not Elapse Timer(%d) !! \n", gaudElapTimer);
			break;
		}

		if (gpbMainCb.subStat==PB_AUDIO_STAT_PLAYING 
			||gpbMainCb.subStat==PB_AUDIO_STAT_FF 
			||gpbMainCb.subStat==PB_AUDIO_STAT_REW)
		{
			sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
			PB_VERBOSE("ADO: Elapsed time (%d) \n", gpbImgCb.mediaElapTime);
		}
		
		if(gpbImgCb.mediaElapTime)
		{
			appPlayOsd_ProgressBarDraw(gpbImgCb.mediaElapTime,gpbImgCb.mediaTotalTime);
			appPlayOsd_ShowTimeInCenter(TRUE,gpbImgCb.mediaElapTime);
		}
		else
		{
			appPlayOsd_ShowTimeInCenter(TRUE,0);
		}
		break;

	case SP5K_MSG_MEDIA_PLAY_END:
		PB_VERBOSE("ADO: SP5K_MSG_MEDIA_PLAY_END(0x%08x)\n", param);
		appPbAudioTerminate();
		appPlayOsd_ProgressBarDraw(gpbImgCb.mediaTotalTime,gpbImgCb.mediaTotalTime);
		appPlayOsd_ShowTimeInCenter(TRUE,gpbImgCb.mediaTotalTime);
		appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case SP5K_MSG_MEDIA_PLAY_START:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY) 
		{
			PB_VERBOSE("ADO: PLAY_START(OKAY)\n");
			if (gpbMainCb.subStat != PB_AUDIO_STAT_PLAY_STARTING) 
			{
				break;
			}
			gpbMainCb.err = PB_ERR_NONE;
			gpbMainCb.subStat = PB_AUDIO_STAT_PLAYING;

			/*-- UI */
			APP_OSD_REFRESH_OFF;
			CLEAR_OSD_SRC;
			appPLayOsd_AudioPlayDraw(TRUE);
			appPlayOsd_ShowTimeInCenter(TRUE,gpbImgCb.mediaElapTime);
			APP_OSD_REFRESH_ON;
		} 
		else 
		{
			PB_VERBOSE("ADO: PLAY_START(FAIL)\n");
			if (gpbMainCb.subStat != PB_AUDIO_STAT_PLAY_STARTING) 
			{
				break;
			}
			gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
			appPbErrorProcess(gpbMainCb.err);
			appPbAudioTerminate();
			appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case SP5K_MSG_MEDIA_PLAY_STOP:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY) 
		{
			PB_VERBOSE("ADO: PLAY_STOP(OKAY)\n");
		} 
		else 
		{
			PB_VERBOSE("ADO: PLAY_STOP(FAIL)\n");
				/*-- There wont have this msg out of middleware.
				 * Instead, middleware will send SP5K_MSG_DISK_ERROR.
				 */
		}
		break;
	case SP5K_MSG_MEDIA_PLAY_FILE_ERROR:
		PB_VERBOSE("ADO: SP5K_MSG_MEDIA_PLAY_FILE_ERROR(0x%08x)\n", param);
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		appPbErrorProcess(gpbMainCb.err);
		appPbAudioTerminate();
		appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;


	case APP_UI_MSG_PB_AUDIO_CREATE:
		PB_VERBOSE("ADO: APP_UI_MSG_PB_AUDIO_CREATE(0x%08x)\n", param);
		if (FAIL==appPbAudioPlayStCreate()) 
		{
			PB_DBG("ADO: Play State creation FAILED !!\n");
			appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}

		if (FAIL==appPbAudioPlayStart()) 
		{
			PB_DBG("ADO: Play failed !! \n");
			appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;

	/*----- Video ERROR Handling */
	case APP_UI_MSG_PB_AUDIO_ERROR:
		#if 0
		if (param==VIDEO_ERR_OUT_OF_ELAPSE_TMR) {
			appPbVideoTerminate();
			gpbMainCb.err = PB_ERR_VIDEO_ERROR;
			appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		#endif
		break;

	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbAudioStOnKey
 *------------------------------------------------------------------------*/
void
appPbAudioStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("ADO: OnKey [0x%x] [0x%x]\n", msg,param);

	switch (msg) 
	{
		case APP_KEY_PRESS_UP:
			PB_VERBOSE("ADO: APP_KEY_PRESS_UP \n");
			if (gpbMainCb.subStat==PB_AUDIO_STAT_PLAYING) 
			{
				PB_VERBOSE("ADO: Going PAUSE !!\n");
				if (SUCCESS==appPbAudioPause()) 
				{
					appPLayOsd_AudioPauseResumeDraw(TRUE);
				}
			}
			else if (gpbMainCb.subStat==PB_AUDIO_STAT_PAUSE) 
			{
				PB_VERBOSE("ADO: Going RESUME !!\n");
				appPbAudioResume();
				appPLayOsd_AudioPauseResumeDraw(FALSE);
				appPlayOsd_ProgressBarLastTimeReset();
				appPlayOsd_ProgressBarDraw(gpbImgCb.mediaElapTime,gpbImgCb.mediaTotalTime);
			}
			else  
			{
				PB_VERBOSE("ADO: Ignore UP key !!\n");
				return;
			}
			break;
		case APP_KEY_PRESS_MENU:
			PB_VERBOSE("ADO: APP_KEY_PRESS_MENU \n");
			appPbAudioTerminate();
			appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			break;
		default:
			break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbAudioState
 *------------------------------------------------------------------------*/
void
appPbAudioState(
	UINT32 msg,
	UINT32 param
)
{

	switch (msg) 
	{
		case APP_STATE_MSG_INIT:
			RPrintf("appPbAudioState");
			appPbAudioStInit(param);
			break;
		case APP_STATE_MSG_CLOSE:
			appPbAudioStClose();
			appStateCloseDone();
			break;
		case APP_STATE_MSG_ABORT:   
			appPbAudioTerminate();
			appPbAudioStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			break;
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appPbAudioStOnKey(msg,param);
			}
			else
			{
				appPbAudioStOnMsg(msg,param);
			}
			break;
	}

}
