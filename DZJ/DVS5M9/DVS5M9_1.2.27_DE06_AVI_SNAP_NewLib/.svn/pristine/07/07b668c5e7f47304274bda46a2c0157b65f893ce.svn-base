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
#include <stdlib.h>
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_global_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_pip_api.h" /*for clean warning*/
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_playback_uiparam.h"
#include "app_ui_para.h"
#include "app_menu.h"
#include "app_msg.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	SLIDE_SXN_START,
	SLIDE_SXN_END,
} slideUIKeyCtrl_t;

typedef struct {
	UINT32	musicHandle;	/* handle of preload music */
	UINT32	tmrHandle;		/* system timer ID for slide show */
	UINT32	tmrIntvl;		/* slide show time interval */

	UINT8 	effIdx;
	UINT8 	uiMusic;

	UINT8	shiftDir;	/* next shift control */
	BOOL	isRepeat;

	BOOL	isPaused;
	UINT8	rsvd;
} slideCB_t;


/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static slideCB_t gslideCb;

#if SOUND_PLAY_NEW_PROC
UINT32 slideSoundID[SLSHOW_MUSIC_TOTAL] = {
	SOUND_ID_NULL,
	SOUND_ID_SLIDE_1,
	SOUND_ID_SLIDE_2,
	SOUND_ID_SLIDE_3,
};	
#else
UINT8 *pszSlideMusic[SLSHOW_MUSIC_TOTAL] =
{
	#ifdef HW_PRJ_6330
	[SLSHOW_MUSIC1]="A:\\RO_RES\\WAV\\SLIDE003.WAV",
	[SLSHOW_MUSIC2]="A:\\RO_RES\\WAV\\SLIDE003.WAV",
	[SLSHOW_MUSIC3]="A:\\RO_RES\\WAV\\SLIDE003.WAV",
	#else
	[SLSHOW_MUSIC1]="A:\\RO_RES\\WAV\\SLIDE001.WAV",
	[SLSHOW_MUSIC2]="A:\\RO_RES\\WAV\\SLIDE002.WAV",
	[SLSHOW_MUSIC3]="A:\\RO_RES\\WAV\\SLIDE003.WAV",
	#endif
};
#endif
UINT32 gSlideShowBeginIndex =0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern BOOL isLastFileIdxFlag;
#if HDMI_MENU_BY_JPG
extern UINT32 pipHDMIMenuFlag;
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbSlideStExit(UINT32 nextSt, UINT32 stMsg);


/*-------------------------------------------------------------------------
 *  File Name :
 *------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 *  File Name : slideUIKeyControl
 *------------------------------------------------------------------------*/
#if 0
static
void
slideUIKeyControl(
	UINT8 ctrl
)
{
	switch (ctrl){
	case SLIDE_SXN_START:
		//appBtnDisable(BTN_ALL);
		break;
	case SLIDE_SXN_END:
		//appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("%s: undefined key session !!\n", __FUNCTION__);
		break;
	}
}
#endif

#if (IS_PB_MUSIC_SLIDE)
/*-------------------------------------------------------------------------
 *  File Name : slideMusicStop
 *------------------------------------------------------------------------*/
static
void
slideMusicStop(
	void
)
{
	PB_VERBOSE("%s !! \n", __FUNCTION__);

	/*-- Stop current Music play */
	playMusicDRAMPlayStop();
}

/*-------------------------------------------------------------------------
 *  File Name : slideMusicRelease
 *------------------------------------------------------------------------*/
static
void
slideMusicRelease(
	void
)
{
	PB_VERBOSE("%s !! \n", __FUNCTION__);

	#if SOUND_PLAY_NEW_PROC
	uiPara_t* puiPara = appUiParaGet();
	
	appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
	#else
	/*-- Stop current Music play */
	playMusicDRAMPlayStop();

	if (gslideCb.musicHandle) {
		PB_VERBOSE("%s: Unload ResDRAM \n", __FUNCTION__);
		sp5kResDRAMUnload(gslideCb.musicHandle);
		gslideCb.musicHandle = 0;
	}
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : slideMusicPreload
 *------------------------------------------------------------------------*/
#if !SOUND_PLAY_NEW_PROC
static
UINT32
slideMusicPreload(
	UINT8 musicId
)
{
	UINT8 szMusic[]="A:\\RO_RES\\WAV\\SLIDE001.WAV";

	PB_VERBOSE("%s !! \n", __FUNCTION__);

	slideMusicRelease();

	/*-- Music preload
	 */
	#ifdef HW_PRJ_6330
		if (musicId == SLSHOW_NONE){
			musicId = SLSHOW_NONE;
			}
		else{
			musicId = SLSHOW_MUSIC3;
			}
	#endif

	if (musicId>SLSHOW_NONE && musicId<SLSHOW_MUSIC_TOTAL) {
		sprintf(szMusic, "A:\\RO_RES\\WAV\\SLIDE00%01d.WAV", musicId); /*-- Hah, lazy fingers */
		gslideCb.musicHandle = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV, szMusic, 0, 0, 0, 0);
	} else {
		gslideCb.musicHandle = 0;
	}

	if (gslideCb.musicHandle==0 && musicId!=SLSHOW_NONE) {
		PB_VERBOSE("%s: !!ERROR!!, Music preload failed !! \n", __FUNCTION__);
		return FAIL;
	}
	return SUCCESS;

}
#endif
/*-------------------------------------------------------------------------
 *  File Name : slideMusicPlayStart
 *------------------------------------------------------------------------*/
static
void
slideMusicPlayStart(
	void
)
{
	#if !SOUND_PLAY_NEW_PROC
	if (!gslideCb.musicHandle) {
		PB_VERBOSE("%s: Null music handle \n", __FUNCTION__);
		return;
	}

	if (FAIL==sp5kResDRAMControl(SP5K_RES_DRAM_PLAY, gslideCb.musicHandle, TRUE, 0, 0, 0)) {
		PB_VERBOSE("%s: Music play failed !!\n", __FUNCTION__);
		return;
	}

	PB_VERBOSE("%s: Music play OK !! \n", __FUNCTION__);
	#else
	uiPara_t* puiPara = appUiParaGet();

	if(puiPara->selSlideMusic == SLSHOW_NONE){
		return;
	}
	appAddSoundToPlay(slideSoundID[puiPara->selSlideMusic]);
	#endif
}

#endif

/*-------------------------------------------------------------------------
 *  File Name : slideEffectConfig
 *------------------------------------------------------------------------*/
static
UINT32
slideShowIntervalLookup(
	UINT8 selIntvl
)
{
	if (selIntvl>SLSHOW_INTERVAL_TOTAL-1) {
		PB_DBG("%s: Illegal intvl(%d) \n", __FUNCTION__, selIntvl);
		return 1000;
	}

	return (1000*(selIntvl+1));
}

/*-------------------------------------------------------------------------
 *  File Name : slideEffectConfig
 *------------------------------------------------------------------------*/
static
UINT32
slideEffectIdTransform(
	SLSHOW_TRANSITION_e eff_id
)
{
	switch (eff_id) {
	default:
	case SLSHOW_TRANSITION_TOTAL:
		PB_VERBOSE("%s: !!ERROR!!, Unsupported Effect (%d) !!\n", __FUNCTION__, eff_id);
		return SP5K_SLIDE_EFFECT_NONE;
	case SLSHOW_TRANSITION_TYPE1:
		return SP5K_SLIDE_EFFECT_FRAME_DRAG;
	case SLSHOW_TRANSITION_TYPE2:
		return SP5K_SLIDE_EFFECT_SLOPE_PROGRESS;
	case SLSHOW_TRANSITION_TYPE3:
		return SP5K_SLIDE_EFFECT_DRAPE_DRAG;
	case SLSHOW_TRANSITION_TYPE4:
		return SP5K_SLIDE_EFFECT_DRAPE_PROGRESS;
	case SLSHOW_TRANSITION_TYPE5:
		return SP5K_SLIDE_EFFECT_STRIP_DRAG;
	/* DONT PARSE THIS case !!
	case SLSHOW_TRANSITION_MIX:
		return SP5K_SLIDE_EFFECT_NONE;
	*/
	}
}

/*-------------------------------------------------------------------------
 *  File Name : slideRandomEffectGen
 *------------------------------------------------------------------------*/
static
void
slideRandomEffectGen(
	UINT8 *peffIdx
)
{
	UINT8 selEff;

	selEff = ((UINT8)rand()) % SLSHOW_TRANSITION_MIX;
	if (selEff==*peffIdx) {
		if (gslideCb.shiftDir==PB_SHIFT_NEXT) {
			PB_INDEX_INC(selEff, SLSHOW_TRANSITION_TYPE1, SLSHOW_TRANSITION_MIX-1);
		} else {
			PB_INDEX_DEC(selEff, SLSHOW_TRANSITION_TYPE1, SLSHOW_TRANSITION_MIX-1);
		}
	}
	*peffIdx = selEff;

	PB_VERBOSE("\nSLSHOW : Random Effect (%d) ...\n", selEff);
}

/*-------------------------------------------------------------------------
 *  File Name : slideEffectConfig
 *------------------------------------------------------------------------*/
static
void
slideEffectConfig(
	SLSHOW_TRANSITION_e eff_id
)
{
	PB_VERBOSE("%s: effect(%d) \n", __FUNCTION__, eff_id);

	sp5kSlideCfgSet(SP5K_SLIDE_FRAME_COLOR, 0, 0, 0, 0);
	sp5kSlideCfgSet(SP5K_SLIDE_UNIT_SIZE, 16, 0, 0, 0);
	sp5kSlideCfgSet(SP5K_SLIDE_STEP_SIZE, 4, 0, 0, 0);

	switch(eff_id) {
		default:
		case SLSHOW_TRANSITION_TOTAL:
			PB_VERBOSE("%s: Unknow Effect (%d) !!\n", __FUNCTION__, eff_id);
			sp5kSlideCfgSet(SP5K_SLIDE_EFFECT_ID,SP5K_SLIDE_EFFECT_NONE,0,0,0);
			return;
		case SLSHOW_TRANSITION_TYPE1:
			PB_VERBOSE("SLSHOW_TRANSITION_TYPE1\n");
			sp5kSlideCfgSet(SP5K_SLIDE_UNIT_TIME, 40, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_DIR, SLIDE_DIR_LEFT, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_MOVE, SLIDE_MOVE_IN, 0, 0, 0);
			break;
		case SLSHOW_TRANSITION_TYPE2:
			PB_VERBOSE("SLSHOW_TRANSITION_TYPE2\n");
			sp5kSlideCfgSet(SP5K_SLIDE_UNIT_TIME, 80, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_CORNER, SLIDE_CORNER_TOP_LEFT, 0, 0, 0);
			break;
		case SLSHOW_TRANSITION_TYPE3:
			PB_VERBOSE("SLSHOW_TRANSITION_TYPE3\n");
			sp5kSlideCfgSet(SP5K_SLIDE_UNIT_TIME, 70, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_AXIS, SLIDE_AXIS_HORI, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_MOVE, SLIDE_MOVE_IN, 0, 0, 0);
			break;
		case SLSHOW_TRANSITION_TYPE4:
			PB_VERBOSE("SLSHOW_TRANSITION_TYPE4\n");
			sp5kSlideCfgSet(SP5K_SLIDE_UNIT_TIME, 70, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_AXIS, SLIDE_AXIS_VERTI, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_MOVE, SLIDE_MOVE_OUT, 0, 0, 0);
			break;
		case SLSHOW_TRANSITION_TYPE5:
			PB_VERBOSE("SLSHOW_TRANSITION_TYPE4\n");
			sp5kSlideCfgSet(SP5K_SLIDE_UNIT_TIME, 70, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_AXIS, SLIDE_AXIS_HORI, 0, 0, 0);
			sp5kSlideCfgSet(SLIDE_MOVE, SLIDE_MOVE_OUT, 0, 0, 0);
			break;
		case SLSHOW_TRANSITION_MIX:
			PB_VERBOSE("SLSHOW_TRANSITION_TYPE4\n");
			/*-- Nothing to Config, RETURN immediately !! */
			return;
	}

	sp5kSlideCfgSet(SP5K_SLIDE_EFFECT_ID, slideEffectIdTransform(eff_id),0,0,0);

}

/*-------------------------------------------------------------------------
 *  File Name : slideEffectStop
 *------------------------------------------------------------------------*/
static
void
slideEffectStop(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	sp5kStillPlayAbort();
	slideEffectConfig(SLSHOW_TRANSITION_TOTAL);
}

/*-------------------------------------------------------------------------
 *  File Name : slideTimerStart
 *------------------------------------------------------------------------*/
static
UINT32
slideTimerStart(
	void
)
{
	if (gslideCb.tmrHandle!=TIMER_NULL) 
	{
		PB_VERBOSE("\n%s: !!WARNING!!, TMR existed @ (%d), ignored \n\n", __FUNCTION__, gslideCb.tmrHandle);
		/*-- While calling after RESUME, the timer could have been created by RESUME.
		 */
		return SUCCESS;
	}

	gslideCb.tmrHandle = appTimerSet(gslideCb.tmrIntvl, "PbSlide");

	if (gslideCb.tmrHandle==TIMER_NULL)
	{
		return FAIL;
	}

	PB_VERBOSE("%s: TMR(%d) started !! \n", __FUNCTION__, gslideCb.tmrHandle);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : slideTimerStop
 *------------------------------------------------------------------------*/
static
void
slideTimerStop(
	void
)
{
	PB_VERBOSE("%s: TMR(%d) \n", __FUNCTION__, gslideCb.tmrHandle);

	if (gslideCb.tmrHandle==TIMER_NULL) return;

	appTimerClear(&gslideCb.tmrHandle);
}

/*-------------------------------------------------------------------------
 *  File Name : slideNextConfig
 *------------------------------------------------------------------------*/
static
UINT32
slideNextConfig(
	pbImageShiftDir_t 	shiftDir
)
{
	uiPara_t* puiPara = appUiParaGet();

	if (!(shiftDir==PB_SHIFT_NEXT || shiftDir==PB_SHIFT_PRE))
	{
		PB_DBG("%s: !!error!!, Unknown shift(%d) !!\n", __FUNCTION__, shiftDir);
		return FAIL;
	}

	/*-- Scan for Next GOOD JPG IMG */
	if (FAIL==appPbScanNextJPG(shiftDir, TRUE /*skip err JPG*/)) 
	{
		PB_LOG("%s: Next JPG scan failed \n", __FUNCTION__);
		return FAIL;
	}

	/*-- Start TIMER to countdown EFFECT display time */
	if (FAIL==slideTimerStart())
	{
		return FAIL;
	}

	/*-- Configure Next Slide Effect */
	if (puiPara->selSlideTrans==SLSHOW_TRANSITION_MIX) 
	{
		slideRandomEffectGen(&gslideCb.effIdx);
	} 
	slideEffectConfig(gslideCb.effIdx);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbSlideTerminate
 *------------------------------------------------------------------------*/
static
void
appPbSlideTerminate(
	UINT32 param
)
{
	/*-- Note: The input argument is reserved for extended purpose to
	 *   pass specific error code, if necessary.
	 *   0(ZEOR) is reserved for NO_ERROR.
	 */
	#if (IS_PB_MUSIC_SLIDE)
	slideMusicRelease();
	#endif /* IS_PB_MUSIC_SLIDE */
	slideTimerStop();
	slideEffectStop();

}

/*-------------------------------------------------------------------------
 *  File Name : appPbSlidePauseResume
 *------------------------------------------------------------------------*/
static
void
appPbSlidePauseResume(
	void
)
{
	PB_LOG("PbSlide: %s ...\n", ((gslideCb.isPaused==TRUE) ? "Resume" : "Pause"));

	//PB_BEEP;  /* mantis 46991 */
	if (gslideCb.isPaused==TRUE) 
	{
		gslideCb.isPaused = FALSE;
		slideMusicPlayStart();
	//	slideTimerStart();
		sp5kSlideResume();
		appPbOsd_AuxNote_OKPlay(FALSE);
		appPbOsd_AuxNote_OKPause(TRUE);
	}
	else
	{
		gslideCb.isPaused = TRUE;
	//	slideTimerStop();
		slideMusicStop();
		sp5kSlidePause();
		appPbOsd_AuxNote_OKPause(FALSE);
		appPbOsd_AuxNote_OKPlay(TRUE);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbSlideNextConfig
 *------------------------------------------------------------------------*/
static
UINT32
appPbSlideNextConfig(
	void
)
{

	if (FAIL==slideNextConfig(gslideCb.shiftDir)) 
	{
		return FAIL;
	}
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbSlideCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbSlideCreate(
	void
)
{

	/*-- At the moment of create, we suppose the current IMG
	 *   has been played completely.
	 */
	gSlideShowBeginIndex=gcurrDcfIdx;
	
	if (FAIL==appPbSlideNextConfig())
	{
		return FAIL;
	}

	#if HDMI_MENU_BY_JPG
	#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
	if(IS_HDMI_IN){
		pipHDMIMenuFlag = 0;
		sp5kPipTerm();
	}
	#endif
	#endif
	
	appPbOsd_AuxNote_OKPause(TRUE);
	appPbOsd_AuxNote_MenuCancel(TRUE);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbSlideStClose
 *------------------------------------------------------------------------*/
static
void
appPbSlideStClose(
	void
)
{

	appPbSlideTerminate(0);

	/*-- Shall resume APO while doing slide show */
	appPowerSavingEnableSet(ENABLE);

	gpbMainCb.subStat = PB_SLIDE_STAT_NULL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbSlideStExit
 *------------------------------------------------------------------------*/
static
void
appPbSlideStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	appPbSlideStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbSlideStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbSlideStInit(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	/*-- Validity check :
	 * (1) NO_IMG shall be checked before entering SLIDE state.
	 * (2) FILE_ERROR !?
	 */

	/*-- Initialize SLIDE Control structure
	 */
	memset(&gslideCb, 0, sizeof(slideCB_t));

	/*-- Preconfig Music for SlideShow
	 */
	#if (IS_PB_MUSIC_SLIDE)
	#if !SOUND_PLAY_NEW_PROC
	if (FAIL==slideMusicPreload(puiPara->selSlideMusic)) 
	{
		return FAIL;
	}
	#endif

	slideMusicPlayStart();
	#endif /* IS_PB_MUSIC_SLIDE */

	/*-- Preconfig Effect for SlideShow
	 */
	gslideCb.tmrHandle = TIMER_NULL;
	gslideCb.tmrIntvl = slideShowIntervalLookup(puiPara->selSlideInteval);
	gslideCb.shiftDir = PB_SHIFT_NEXT;
	gslideCb.effIdx = (puiPara->selSlideTrans>=SLSHOW_TRANSITION_MIX) ? SLSHOW_TRANSITION_TYPE1 : puiPara->selSlideTrans;
	gslideCb.isRepeat = (puiPara->selSlideRepeat==SLSHOW_REPEAT_ON) ? TRUE : FALSE;
	slideEffectConfig(gslideCb.effIdx);

	gpbMainCb.subStat = PB_SLIDE_STAT_NULL;
	sp5kHostMsgSend(APP_UI_MSG_PB_SLIDE_CREATE, 0);

	/*-- Shall Disable APO while doing slide show */
	appPowerSavingEnableSet(FALSE);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbSlideStOnMsg
 *------------------------------------------------------------------------*/
void
appPbSlideStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;
	#if SOUND_PLAY_NEW_PROC
	uiPara_t* puiPara = appUiParaGet();
	#endif
	PB_VERBOSE("SLIDE: OnMsg [0x%08x], [0x%08x] ... \n", msg, param);

	switch (msg) {
	case SP5K_MSG_MODE_READY:
		appBtnEnable(BTN_ALL);
		break;
	case SP5K_MSG_STILLPLAY_DONE:
		PB_VERBOSE("SLIDE: SP5K_MSG_STILLPLAY_DONE(0x%08x)\n", param);
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
		tmpU32 = appPbStillPlayDoneParse(param);

		if (tmpU32==PB_DONE_MAIN_FINISH) 
		{
			PB_DBG("MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
			gpbMainCb.subStat = PB_SLIDE_STAT_EFFECT_SLIDING;

		} 
		else if (tmpU32 & PB_DONE_ERROR)
		{
			PB_DBG("MAIN ERROR !!\n");
			/* FileError, we shall shift next IMG immediately !!
			 *-- Scan for Next GOOD JPG IMG, and Play new JPG immediately !!
			 */
			if (FAIL==appPbScanNextJPG(gslideCb.shiftDir, TRUE /*skip err JPG*/)) 
			{
				PB_LOG("%s: !!Oops!!, line %d \n", __FILE__, __LINE__);
				/*-- Oops ! Some kind of impossible case !! */
				sp5kHostMsgSend(APP_UI_MSG_PB_SLIDE_TERMINATE, 0);
				break;
			}

			sp5kHostMsgSend(APP_UI_MSG_PB_SLIDE_ERR_PLAY_NEXT, 0);
		}

		break;

	case APP_UI_MSG_PB_SLIDE_CREATE:
		PB_VERBOSE("SLIDE: APP_UI_MSG_PB_SLIDE_CREATE(0x%08x)\n", param);
		if (FAIL==appPbSlideCreate())
		{
			PB_DBG("SLIDE: !!ERROR!!, Create failed !! \n");
			gpbMainCb.err = PB_ERR_SLIDE_ERROR;
			sp5kHostMsgSend(APP_UI_MSG_PB_SLIDE_TERMINATE, 0);
			break;
		}
		gpbMainCb.subStat = PB_SLIDE_STAT_EFFECT_DONE;
		break;

	case SP5K_MSG_TIMER:
		PB_VERBOSE("SLIDE: SP5K_MSG_TIMER(0x%08x)\n", param);
		if (param!=gslideCb.tmrHandle) 
		{
			PB_VERBOSE("SLIDE: OnMsg, Skipping TMR(%d) msg \n", param);
			return;
		}
		PB_LOG("SLIDE: TMR(%d) \n", param);
		goto lbl_sshow_play_next;
	case APP_UI_MSG_PB_SLIDE_ERR_PLAY_NEXT:
		PB_DBG("SLIDE: APP_UI_MSG_PB_SLIDE_ERR_PLAY_NEXT(0x%08x)\n", param);
		goto lbl_sshow_play_next;
	case APP_UI_MSG_PB_SLIDE_TIMEOUT:
		PB_DBG("SLIDE: APP_UI_MSG_PB_SLIDE_TIMEOUT(0x%08x)\n", param);
lbl_sshow_play_next:
		/*-- Timer has timed out, go play next image
		 * Note : the current dcf index shall have been set at the PLAY_DONE
		 *        of last playback.
		 */
		/*-- STOP slide timer */
		slideTimerStop();
		gpbImgCb.imgOrientDim = appPbImageOrientDimGet(gpbImgCb.imgWidth,gpbImgCb.imgHeight);/*jj add for mantis BUG 0047211*/
		printf("gpbImgCb.imgOrientDim = %d,gpbImgCb.imgWidth = %d,gpbImgCb.imgHeight = %d\n",gpbImgCb.imgOrientDim,gpbImgCb.imgWidth,gpbImgCb.imgHeight);

		/*-- Playback MAIN IMG */
		gpbImgCb.isMainImgOK = gpbImgCb.isThumbImgOK = FALSE;
		gpbMainCb.isPlayDone = FALSE;
		gpbMainCb.playStat = PB_PLAY_STAT_JPG_MAIN_PLAYING;
		sp5kStillPlayCfgSet(SP5K_SPB_IMG_TYPE, SP5K_SPB_IMG_PRIMARY);
		sp5kModeSet(SP5K_MODE_STILL_PLAYBACK);
		gpbMainCb.subStat = PB_SLIDE_STAT_IMG_DECODING;
		PB_VERBOSE("SLIDE: Playing IMG (%s) \n", &gpbImgCb.szDcfFileName[0]);
		break;

	case SP5K_MSG_SLIDE_READY:
		PB_DBG("SLIDE: SP5K_MSG_SLIDE_READY(0x%08x)\n", param);
		/*-- Slide effect has completed, go configure next effect.
		 */
		 /*s modified by Leo @20100803*/
		 if(!gslideCb.isRepeat)
		 {
         		 if(isLastFileIdxFlag)
     		 	{
	     		 	isLastFileIdxFlag=FALSE;
				sp5kHostMsgSend(APP_UI_MSG_PB_SLIDE_TERMINATE, 0);	
     		 	}
			else
         		{
	         		if (FAIL==appPbSlideNextConfig()) 
	         		{
	         		}
	         		gpbMainCb.subStat = PB_SLIDE_STAT_EFFECT_DONE;
         		}
		 }
		 else
		 {
    		 	  if (FAIL==appPbSlideNextConfig()) 
    		 	  {
        		  }
        				
        		      gpbMainCb.subStat = PB_SLIDE_STAT_EFFECT_DONE;
		 }
		break;

	case APP_UI_MSG_PB_SLIDE_TERMINATE:
		/*-- Note: the param can be used to indicate extra information
		 *         for terminating SLIDE SHOW STAT !!
		 */
		appPbSlideTerminate(param);
		appPbSlideStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);		   	
		break;
	#if (IS_PB_MUSIC_SLIDE)
	#if SOUND_PLAY_NEW_PROC
		case APP_UI_MSG_AUDIO_PLAY_FINISH:	
			if( (gslideCb.isPaused==FALSE) && (param == slideSoundID[puiPara->selSlideMusic]))
			{
				slideMusicPlayStart();
			}
			break;
	#else
		case SP5K_MSG_MEDIA_SOUND_FINISH:	
		/*if(gslideCb.isPaused==FALSE)
		{
			playMusicDRAMPlayStop();
			slideMusicPlayStart();
		}*/  /*mask by GQ,use loop play */ 
		break;
	#endif
	#endif

	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbSlideStOnKey
 *------------------------------------------------------------------------*/
void
appPbSlideStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	static UINT8 bPauseFlag=0;

	PB_VERBOSE("SLIDE: OnKey [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_DEL:
	case APP_KEY_PRESS_MENU:
		appPbSlideTerminate(param);
		PB_BEEP;
		appPbSlideStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_SET:
		if(gpbMainCb.subStat != PB_SLIDE_STAT_EFFECT_SLIDING)   /*add for mantis bug 0047354*/
		{
			slideTimerStop();
			 bPauseFlag=1 ;
			appPbSlidePauseResume();
			gpbMainCb.subStat = PB_SLIDE_STAT_EFFECT_SLIDING ;
		}
		else
		{
			if(bPauseFlag==1)
			{
				appPbSlideNextConfig();
				gpbMainCb.subStat = PB_SLIDE_STAT_IMG_DECODING ;
			}
			appPbSlidePauseResume();
			bPauseFlag=0;
		}		
		break;
	case APP_KEY_PRESS_DOWN:
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
 *  File Name : appPbSlideState
 *------------------------------------------------------------------------*/
void
appPbSlideState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s: [0x%x] [0x%x]\n", __FUNCTION__, msg, param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbSlideState");
		appBtnDisable(BTN_ALL);
		appMsgFlush(APP_MSG_KEY_QUE);
		appPbSlideStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appPbSlideStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appPbSlideStOnKey(msg,param);
		}
		else
		{
			appPbSlideStOnMsg(msg,param);
		}
		break;
	}

}
