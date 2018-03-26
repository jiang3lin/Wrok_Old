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
#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "app_timer.h"
#include "app_view_osd.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"

#include "app_audio.h"
#include "app_video.h"
#include "app_ui_para.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define PB_MEMO_REC_ELAPSED_TIMER_MS 	200

#define PB_MEMO_TIME_MAX		30
#define PB_MEMO_FILE_SIZE_KB	(8*PB_MEMO_TIME_MAX)	/* 8K*30sec, for u-Law */

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum 
{
	MEMO_ERR_OUT_OF_ELAPSE_TMR,
	MEMO_ERR_RECORD_ERROR,
	MEMO_ERR_OUT_OF_REC_TMR,
} videoErrorCode_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 gmrecElapTimer = TIMER_NULL;
static UINT32 gmrecTimer = TIMER_NULL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbMemoRecStExit(UINT32 nextSt, UINT32 stMsg);

/*-------------------------------------------------------------------------
 *  File Name : memoRec_uiOsd_RecElapse
 *------------------------------------------------------------------------*/
static
void
memoRec_uiOsd_RecElapse(
	UINT32 elapSec
)
{
	appPlayOsd_ProgressBarDraw(elapSec*1000, gpbImgCb.mediaTotalTime);
	appPlayOsd_ShowTimeInCenter(TRUE, elapSec*1000);
}
/*-------------------------------------------------------------------------
 *  File Name : memoRec_uiOsd_RecStart
 *------------------------------------------------------------------------*/
static
void
memoRec_uiOsd_RecStart(
	void
)
{
	appPbOsd_AuxNote_OKRec(FALSE);
	appPbOsd_AuxNote_MenuCancel(FALSE);
	appPbOsd_AuxNote_OKStop(TRUE);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, APPVIEW_AUDIO_STRING_X-(APPVIEW_AUDIO_STRING_W/2), APPVIEW_AUDIO_STRING_Y, APPVIEW_AUDIO_STRING_W, FONT_H, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,
							SP5K_GFX_ALIGN_TOP,
							ID_STR_RECORDING_MEMO___);
}
/*-------------------------------------------------------------------------
 *  File Name : memoDEL_ConfirmDlgConfig
 *------------------------------------------------------------------------*/
static
void
memoDEL_ConfirmDlgConfig(
	void
)
{
	appPlayOsd_DlgConfirmYesNoConfig(ID_STR_DELETE_VOICE_MEMO_, 0);
}
/*-------------------------------------------------------------------------
 *  File Name : memoDEL_ConfirmDlgShow
 *------------------------------------------------------------------------*/
static
void
memoDEL_ConfirmDlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSet(TRUE);
}
/*-------------------------------------------------------------------------
 *  File Name : memoRec_uiOsd_RecStandby
 *------------------------------------------------------------------------*/
static
void
memoRec_uiOsd_RecStandby(
	void
)
{
    
	appPLayOsd_MemoRecOsdDraw(TRUE);	  
	appPbOsd_AuxNote_OKRec(TRUE);	
	appPbOsd_AuxNote_MenuCancel(TRUE);
	

}

/*-------------------------------------------------------------------------
 *  File Name : memorecDiskCheckup
 *------------------------------------------------------------------------*/
static
UINT32
memorecDiskCheckup(
	void
)
{
	UINT32 cluz;

	/*-- calculate memo file size in clusters */
	cluz = appPbBytes2ClusterCalculate(((UINT32)PB_MEMO_FILE_SIZE_KB)<<10);

	if (cluz+1>=appPbDiskFreeClusterGet(gpbDiskCb.dskInfo.actDsk))
	{
		PB_LOG("MEMO_REC: Disk Full \n");
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : memorecValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
memorecValidityCheckup(
	void
)
{
	gpbMainCb.err = PB_ERR_NONE;
	if (gpbDiskCb.isCardLock) 
	{
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
	} 
	else if (gpbImgCb.isFileErr)
	{
		gpbMainCb.err = PB_ERR_FILE_ERROR;
	}

	if (gpbMainCb.err != PB_ERR_NONE)
	{
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecTerminate
 *------------------------------------------------------------------------*/
static
UINT32
appPbMemoRecTerminate(
	void
)
{
	/*-- Shall stop timers */
	appTimerClear(&gmrecElapTimer);
	appTimerClear(&gmrecTimer);

	/*-- Terminate audio/memo play */
	sp5kMediaMemoControl(gcurrDcfIdx, SP5K_MEDIA_MEMO_ABORT);

	gpbMainCb.subStat = PB_MEMOREC_STAT_STANDBY;
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecStart
 *------------------------------------------------------------------------*/
static
UINT32
appPbMemoRecStart(
	void
)
{
	UINT32 ret;


	/*-- Shall check remaining disk space */
	if (FAIL==memorecDiskCheckup())
	{
		gpbMainCb.err = PB_ERR_DISK_FULL;
		return FAIL;
	}

	/*-- Go on record memo */
	sp5kDcfFsCurFileIdxSet(gcurrDcfIdx);
	ret = sp5kMediaMemoControl(gcurrDcfIdx, SP5K_MEDIA_MEMO_RECORD);
		/*-- SP5KFW shall respond with SP5K_MSG_MEDIA_REC_OK/_FAIL
		 */
	if (FAIL==ret) 
	{
		sp5kHostMsgSend(APP_UI_MSG_PB_MEMO_REC_ERROR, MEMO_ERR_RECORD_ERROR);
		gpbMainCb.err = PB_ERR_AUDIO_ERROR;
		return FAIL;
	}
	gpbMainCb.subStat = PB_MEMOREC_STAT_STARTING;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecStCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbMemoRecStCreate(
	void
)
{
	/*-- Validity check */
	if (FAIL==memorecValidityCheckup()) 
	{
		return FAIL;
	}

	/*-- Configure MEMO REC attributes */
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_UNKNOWN);
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_WAV );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_MULAW );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, AUDIO_REC_SAMPLING_RATE );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, AUDIO_REC_SAMPLE_BITS );
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CHANNELS, VIDEO_AUDIO_CHANNEL); 

	/*-- Configure MEMO REC Volume */
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, 25);
	/*sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_ON);*/
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_HB, 500);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_LB, 100);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL, 20 );
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST, 0x80000016);

	gpbImgCb.mediaTotalTime = 30*1000;

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecStClose
 *------------------------------------------------------------------------*/
static
void
appPbMemoRecStClose(
	void
)
{
	appBtnEnable(BTN_ALL);
	/*-- Shall stop timers */
	if (gmrecElapTimer!=TIMER_NULL)
	{
		appTimerClear(&gmrecElapTimer);
	}
	
	if (gmrecTimer!=TIMER_NULL)
	{
		appTimerClear(&gmrecTimer);
	}

	gpbMainCb.subStat = PB_MEMOREC_STAT_STANDBY;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecStExit
 *------------------------------------------------------------------------*/
static
void
appPbMemoRecStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	appPbMemoRecStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbMemoRecStInit(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	gpbMainCb.subStat = PB_MAIN_STAT_MEMOREC_SUSPEND;

	if (puiPara->selVoiceMemo==PB_UI_VOICE_MEMO_DELETE) 
	{
		/*-- Delete MEMO */
		sp5kHostMsgSend(APP_UI_MSG_PB_MEMO_DEL_CREATE, 0);
	} 
	else 
	{
		/*-- Add MEMO */
		sp5kHostMsgSend(APP_UI_MSG_PB_MEMO_REC_CREATE, 0);
	}
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecStOnMsg
 *------------------------------------------------------------------------*/
static
void
appPbMemoRecStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	switch (msg) 
	{
	case SP5K_MSG_TIMER:
		if (gmrecElapTimer!=param && gmrecTimer!=param)
		{
			break;
		}

		if (gpbMainCb.subStat != PB_MEMOREC_STAT_RECORDING) 
		{
			break;
		}

		if (gmrecElapTimer==param) 
		{
			/*-- Update Elapsed Timer */
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &tmpU32);
			PB_VERBOSE("MEMO_REC: Elapsed time (%d) \n", tmpU32);
			if (((tmpU32+500)/1000)>((gpbImgCb.mediaElapTime+500)/1000)) 
			{
				gpbImgCb.mediaElapTime = tmpU32;
				tmpU32 = (tmpU32+500)/1000;
				memoRec_uiOsd_RecElapse(tmpU32);
			}
			return;
		}
		else if (gmrecTimer==param) 
		{
			/*-- REC Time is up */
			appPbMemoRecTerminate();
			appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;
	case SP5K_MSG_MEDIA_REC_START:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY) 
		{
			/*-- Register TIMER for REC Timeout */
			gmrecTimer = appTimerSet(PB_MEMO_TIME_MAX*1000+0.5*1000, "MEMO_REC_TIME"); /*modify for mantis bug 0047607*/
			if (gmrecTimer==TIMER_NULL) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_MEMO_REC_ERROR, MEMO_ERR_OUT_OF_REC_TMR);
				break;
			}
			gpbImgCb.mediaElapTime = 0;
			/*-- Register TIMER for elapsed time process */
			gmrecElapTimer = appTimerSet(PB_MEMO_REC_ELAPSED_TIMER_MS, "MEMO_REC_ELAP");
			if (gmrecElapTimer==TIMER_NULL) 
			{
				sp5kHostMsgSend(APP_UI_MSG_PB_MEMO_REC_ERROR, MEMO_ERR_OUT_OF_ELAPSE_TMR);
				break;
			}
			appBtnDisable(BTN_DEL|BTN_MENU|BTN_DOWN|BTN_UP|BTN_RIGHT|BTN_LEFT|BTN_MODE|BTN_TELE|BTN_WIDE);
			gpbMainCb.subStat = PB_MEMOREC_STAT_RECORDING;
		}
		else 
		{
			gpbMainCb.err = PB_ERR_MEMOREC_ERROR;
			appPbMemoRecTerminate();
			appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case SP5K_MSG_MEDIA_REC_STOP:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY) 
		{
			DBG_PRINT("MEMO_REC: REC_STOP(OKAY)\n");
		} 
		else
		{
			DBG_PRINT("MEMO_REC: REC_STOP(FAIL) !!\n");
				/*-- There wont have this msg out of middleware.
				 * Instead, middleware will send SP5K_MSG_DISK_ERROR.
				 */
		}
		break;
	case APP_UI_MSG_PB_MEMO_REC_CREATE:
		if (FAIL==appPbMemoRecStCreate()) 
		{
			appPbErrorProcess(gpbMainCb.err);
			appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}		
		memoRec_uiOsd_RecStandby();		
		gpbMainCb.subStat = PB_MEMOREC_STAT_STANDBY;
		break;

	case APP_UI_MSG_PB_MEMO_DEL_CREATE:
		if (FAIL==memorecValidityCheckup()) 
		{
			appPbErrorProcess(gpbMainCb.err);
			appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			break;
		}
		memoDEL_ConfirmDlgConfig();
		memoDEL_ConfirmDlgShow();
		gpbMainCb.subStat= PB_MEMOREC_STAT_DEL_CONFIRM_MENU;
		break;

	/*----- MEMO_REC ERROR Handling */
	case APP_UI_MSG_PB_MEMO_REC_ERROR:
		break;
	case APP_STATE_MSG_ABORT: /*add for mantis bug 0047610*/
		 if(param == STATE_PARAM_CARD_PLUG)
	 	 {
			profLogPrintf(0,"APP_STATE_MSG_ABORT");
			gpbMainCb.err = PB_ERR_MEMOREC_ERROR;
			appPbErrorProcess(gpbMainCb.err);
			appPbMemoRecTerminate();
			appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		  }
	    break;
	default :
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : pbMemo_KEY_SET_Service
 *------------------------------------------------------------------------*/
static void pbMemo_KEY_SET_Service(void)
{
	uiPara_t* puiPara = appUiParaGet();
	UINT32 elsTime;
	if (puiPara->selVoiceMemo==PB_UI_VOICE_MEMO_SET) 
	{
		/*-- ADD MEMO -------------------
		 */
		if (gpbMainCb.subStat==PB_MEMOREC_STAT_RECORDING) 
		{
			/*-- stop rec */
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
			if (elsTime>500)
			{
				appPbMemoRecTerminate();
				appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				printf("time is too short,forbid stop!!\n");
			}
		} 
		else if (gpbMainCb.subStat==PB_MEMOREC_STAT_STANDBY) 
		{
			/*-- start rec */
			if (FAIL==appPbMemoRecStart()) 
			{
				appPbErrorProcess(gpbMainCb.err);
				appPbMemoRecStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
				return;
			}
			memoRec_uiOsd_RecStart();
		}
	}
	else 
	{
		/*-- DEL MEMO --------------------
		 */
		if (0==appPlayOsd_DlgBoxSelectGet())
		{
			/*-- YES : DEL MEMO
			 */
			if (SUCCESS==sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DEL, 1/*MEMO*/, 0/*SYNC-OP*/, 0, 0)) 
			{
				gpbMainCb.err = PB_ERR_NONE;
			}
			else 
			{
				gpbMainCb.err = PB_ERR_MEMOREC_ERROR;
			}
			appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		} 
		else 
		{
			/*-- NO : DONT DEL MEMO
			 */
			//PB_OSD_CLR;
			appPbMemoRecStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecStOnKey
 *------------------------------------------------------------------------*/
static
void
appPbMemoRecStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	switch (msg) 
	{
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_MEMOREC_STAT_DEL_CONFIRM_MENU)
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(msg==APP_KEY_PRESS_UP);
		}
		break;

	case APP_KEY_PRESS_DEL:
	case APP_KEY_PRESS_MENU:
		if (gpbMainCb.subStat== PB_MEMOREC_STAT_STANDBY 
			&&puiPara->selVoiceMemo==PB_UI_VOICE_MEMO_SET)
		{
			appPbMemoRecStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case APP_KEY_PRESS_SET:
		pbMemo_KEY_SET_Service();
		break;
	case APP_KEY_PRESS_LEFT:
		break;
	case APP_KEY_PRESS_RIGHT:
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbMemoRecState
 *------------------------------------------------------------------------*/
void
appPbMemoRecState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbMemoRecState");
		appPbMemoRecStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:	
		appPbMemoRecStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appPbMemoRecStOnKey(msg,param);
		}
		else
		{
			appPbMemoRecStOnMsg(msg,param);
		}
		break;
	}

}
