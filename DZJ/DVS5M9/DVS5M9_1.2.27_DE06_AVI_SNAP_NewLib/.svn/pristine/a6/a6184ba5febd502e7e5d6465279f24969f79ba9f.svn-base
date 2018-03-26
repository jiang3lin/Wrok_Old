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

#define PB_VIDEO_ELAPSED_TIMER_MS 500

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define VIDEO_CRITICAL_OP_POST_SLEEP	APP_SLEEP_MS(500)
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
	VDO_SXN_SNAPSHOT_START,
	VDO_SXN_SNAPSHOT_END,
} videoUIKeyCtrl_t;

typedef enum {
	VIDEO_ERR_OUT_OF_ELAPSE_TMR,
} videoErrorCode_t;

typedef enum {
	VIDEO_TRIM_START,
	VIDEO_TRIM_FIND_FIRST_PIONT,
	VIDEO_TRIM_SEC ,
	VIDEO_TRIM_FIND_END_PIONT,
	VIDEO_TRIM_VIDEO_REVIEW,
	VIDEO_TRIM_VIDEO_SAVE_START,
	VIDEO_TRIM_VIDEO_SAVE_END,	
} videoTrimUIStatusCtrl_t;



/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 gvideoElapTimer = TIMER_NULL;
static UINT8 gVideoTrimStatus = 0;
static UINT32 gVideoTrimStartTime =0;
static UINT32 gVideoTrimEndTime =0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
/*extern UINT32 dcfFsCurFileIdxSet(UINT32 fileIdx);*/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbTrimVideoStExit(UINT32 nextSt,	UINT32 stMsg);
void videoTrimBGBlack(void)
{
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_75, PAL_BLACK, PAL_BLEND_75);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,0, 0, LCD_SIZE_X, LCD_SIZE_Y, 0, 0);	
}
/*-------------------------------------------------------------------------
 *  File Name : videoUIKeyControl
 *------------------------------------------------------------------------*/
static
void
videoTrimUIKeyControl(
	UINT8 ctrl
)
{
	PB_VERBOSE("%s: ctrl(%d) \n", __FUNCTION__, ctrl);
	switch (ctrl){
	case VDO_SXN_SNAPSHOT_START:
		appBtnDisable(BTN_ALL);
		break;
	case VDO_SXN_SNAPSHOT_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("VDO: undefined key session !!\n");
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
videoTrimValidityCheckup(
	void
)
{
	/*-- Minimum video length !? */
	/*-- File Error, perhaps, we shall check this in PB_MAIN UP key */

	return SUCCESS;
}

#if 0
/*-------------------------------------------------------------------------
 *  File Name : videoSnapValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
videoSnapValidityCheckup(
	void
)
{
	UINT32 imgCluz, freeCluz;

	if (gpbDiskCb.isCardLock) {
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
		return FAIL;
	} else if (gpbDiskCb.isDcfFull) {
		gpbMainCb.err = PB_ERR_DCF_KEY_FULL;
		return FAIL;
	}

	imgCluz = appPbBytes2ClusterCalculate(gpbImgCb.imgWidth*gpbImgCb.imgHeight);
		/*-- maximun estimation, W*H*2/2 */
	freeCluz = appPbDiskFreeClusterGet(gpbDiskCb.dskInfo.actDsk);

	PB_VERBOSE("\n==== %s ================\n", __FUNCTION__);
	PB_VERBOSE("imgCluz(%d), freeCluz(%d) \n", imgCluz, freeCluz);
	PB_VERBOSE("============================\n");


	if (imgCluz > freeCluz) {
		gpbMainCb.err = PB_ERR_DISK_FULL;
		return FAIL;
	}

	return SUCCESS;
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : videoFlyingPlayExecute
 *------------------------------------------------------------------------*/
static
void
videoTrimFlyingPlayExecute(
	pbMediaFlyingDir_t flyingDir
)
{
	UINT32 flyingCtrl, flyingSpeed;
	BOOL isInverseFlying = FALSE;

	PB_VERBOSE("%s: flyingDir(%s) \n", __FUNCTION__, (flyingDir==PB_MEDIA_FLYING_FF ? "FF" : "REW") );

	/*-- flying speed control */
	flyingSpeed = gpbImgCb.mediaPlaySpeed;
	if (flyingDir==PB_MEDIA_FLYING_FF) {
		if (gpbMainCb.subStat==PB_VIDEO_STAT_REW) {
			/* Inverse flying !! */
			isInverseFlying = TRUE;
		}
		flyingCtrl = SP5K_MEDIA_PLAY_FORWARD;
		gpbMainCb.subStat = PB_VIDEO_STAT_FF;
	} else if (flyingDir==PB_MEDIA_FLYING_REW) {
		if (gpbMainCb.subStat==PB_VIDEO_STAT_FF) {
			/* Inverse flying !! */
			isInverseFlying = TRUE;
		}
		flyingCtrl = SP5K_MEDIA_PLAY_REWIND;
		gpbMainCb.subStat = PB_VIDEO_STAT_REW;
	}  else if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION) {
		if (gpbMainCb.subStat==PB_VIDEO_STAT_SLOW_MOTION) {
			/* Inverse flying !! */
			isInverseFlying = TRUE;
		}
		flyingCtrl = SP5K_MEDIA_PLAY_SLOW_MOTION;
		gpbMainCb.subStat = PB_VIDEO_STAT_SLOW_MOTION;
	}else {
		printf("%s: Damned !!, Illegal input !! \n", __FUNCTION__);
		return;
	}

	if (isInverseFlying) {
		/*-- Reset to ACC_1 speed for inverse flying */
		PB_DBG("InvFlying !\n");
		gpbImgCb.mediaPlaySpeed = flyingSpeed = PB_PLAY_SPEED_ACCELERAT_1;
	} else {
		PB_INDEX_INC(flyingSpeed, PB_PLAY_SPEED_ACCELERAT_1, PB_PLAY_SPEED_ACCELERAT_MAX);
		gpbImgCb.mediaPlaySpeed = flyingSpeed;
	}

	/*-- go on media control for flying play */
	if (flyingSpeed==PB_PLAY_SPEED_NORMAL) {
		sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);
		gpbMainCb.subStat = PB_VIDEO_STAT_PLAYING;
		PB_DBG("%s: Resume to NORMAL Spped !! \n", __FUNCTION__);
	} else {
		
		flyingSpeed *= 2; /* mapping enum to real multiple */
		if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION)
			   flyingSpeed *= 2;
		sp5kMediaPlayControl(flyingCtrl, flyingSpeed, 0);
		PB_DBG("%s: flyingCtrl(%d), speedX(%d) \n", __FUNCTION__, flyingCtrl, flyingSpeed);
	}

	if (flyingDir==PB_MEDIA_FLYING_FF && gpbMainCb.subStat==PB_VIDEO_STAT_FF) {
		; /* do noting */
	} else if (flyingDir==PB_MEDIA_FLYING_REW && gpbMainCb.subStat==PB_VIDEO_STAT_REW) {
		; /* do noting */
	} else if (flyingDir==PB_MEDIA_FLYING_SLOW_MOTION && gpbMainCb.subStat==PB_MEDIA_FLYING_SLOW_MOTION) {
		; /* do noting */
	}else {
		/*- SP5K does not allow any operation during this period */
		VIDEO_CRITICAL_OP_POST_SLEEP;
	}

	return;
}

/*-------------------------------------------------------------------------
 *  File Name : videoFlyingConstraintCheckup
 *------------------------------------------------------------------------*/
static
UINT32
videoTrimFlyingConstraintCheckup(
	UINT8 opCmd
)
{
	UINT32 videoStat;
	UINT32 elapT, totalT, critT;

	PB_VERBOSE("%s: opCmd(%d) \n", __FUNCTION__, opCmd);

	videoStat = gpbMainCb.subStat;
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elapT);
	gpbImgCb.mediaElapTime = elapT;
	totalT = gpbImgCb.mediaTotalTime;
	critT = PB_VIDEO_CRITICAL_TIME;
	PB_VERBOSE("%s: videoStat(%d), elapT(%d), critT(%d), totalT(%d) \n", __FUNCTION__, videoStat, elapT, critT, totalT);

	switch (opCmd) {
	default:
		return FAIL;
	case PB_MEDIA_FLYING_FF:
		/* The constraints :
		 * A. The end of 2 sec while PLAYING, NO FF !!
		 * B. The end of 2 sec while REWINDING, NO FF !!
		 * C. The begining 2 sec while REWINDING, NO FF !!
		 * D. 500ms of critical constraining time is needed for reverse operation,
		 *    --> e.g., Ask to FF while MEDIA is rewinding.
		 */
		if (videoStat==PB_VIDEO_STAT_PLAYING) {
			/*-- Media is playing */
			if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT)) {
				PB_VERBOSE("VDO: FF - Rule (A), ignored !!\n");
				return FAIL;
			}
		}
		else if (videoStat==PB_VIDEO_STAT_REW) {
			/*-- Media is rewinding */
			if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT)) {
				PB_VERBOSE("VDO: FF - Rule (B), ignored !!\n");
				return FAIL;
			} else if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT)) {
				PB_VERBOSE("VDO: FF - Rule (C), ignored !!\n");
				return FAIL;
			}
		}
		break;
	case PB_MEDIA_FLYING_REW:
		/* The constraints :
		 * A. The beginning of 2 sec while PLAYING, NO REW !!
		 * B. The endinf 2 sec while PLAYING, NO REW !!
		 * C. The begining 2 sec while F.FORWARDING, NO REW !!
		 * D. The ending 2 sec while F.FORWARDING, NO REW !!
		 * D. 500ms of critical constraining time is needed for reverse operation,
		 *    --> e.g., Ask to FF while MEDIA is rewinding.
		 */
		if (videoStat==PB_VIDEO_STAT_PLAYING) {
			/*-- Media is playing */
			if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT)) {
				PB_VERBOSE("VDO: REW - Rule (A), ignored !!\n");
				return FAIL;
			} else if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT)) {
				PB_VERBOSE("VDO: REW - Rule (B), ignored !!\n");
				return FAIL;
			}
		}
		else if (videoStat==PB_VIDEO_STAT_FF) {
			if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT)) {
				PB_VERBOSE("VDO: REW - Rule (C), ignored !!\n");
				return FAIL;
			} else if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT)) {
				PB_VERBOSE("VDO: REW - Rule (D), ignored !!\n");
				return FAIL;
			}
		}
		break;
	case PB_MEDIA_FLYING_SLOW_MOTION:
		#if 0
		/* The constraints :
		 * A. The beginning of 2 sec while PLAYING, NO REW !!
		 * B. The endinf 2 sec while PLAYING, NO REW !!
		 * C. The begining 2 sec while F.FORWARDING, NO REW !!
		 * D. The ending 2 sec while F.FORWARDING, NO REW !!
		 * D. 500ms of critical constraining time is needed for reverse operation,
		 *    --> e.g., Ask to FF while MEDIA is rewinding.
		 */
		if (videoStat==PB_VIDEO_STAT_PLAYING) {
			/*-- Media is playing */
			if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT)) {
				PB_VERBOSE("VDO: REW - Rule (A), ignored !!\n");
				return FAIL;
			} else if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT)) {
				PB_VERBOSE("VDO: REW - Rule (B), ignored !!\n");
				return FAIL;
			}
		}
		else if (videoStat==PB_VIDEO_STAT_FF) {
			if (IS_MEDIA_BEGINNING_CRITICAL_TIME(elapT, critT)) {
				PB_VERBOSE("VDO: REW - Rule (C), ignored !!\n");
				return FAIL;
			} else if (IS_MEDIA_ENDING_CRITICAL_TIME(elapT, critT, totalT)) {
				PB_VERBOSE("VDO: REW - Rule (D), ignored !!\n");
				return FAIL;
			}
		}
		#endif
		break;	
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoResume
 *------------------------------------------------------------------------*/
static
void
appPbTrimVideoResume(
	void
)
{
	PB_VERBOSE("%s: \n", __FUNCTION__);

	/*-- Retrieve elapsed time */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);

	/*-- Star ELAP_TMR */
	if (gvideoElapTimer==TIMER_NULL) {
		/*-- If resume from FF || REW, the ElapTimer is still working !!
		 */
		gvideoElapTimer=appTimerSet(PB_VIDEO_ELAPSED_TIMER_MS, "ELAP_TMR");
	}
	if (TIMER_NULL==gvideoElapTimer) {
		PB_DBG("%s: Out of VDO elapsed TMR !! \n", __FUNCTION__);
		sp5kHostMsgSend(APP_UI_MSG_PB_VIDEO_ERROR, VIDEO_ERR_OUT_OF_ELAPSE_TMR);
		return;
	}
	//PB_LOG("PbVideo: TMR(%d) created \n", gvideoElapTimer);


	/*-- Go on Resume */
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);
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
	gpbMainCb.subStat = PB_VIDEO_STAT_PLAYING;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoPause
 *------------------------------------------------------------------------*/
static
UINT32
appPbTrimVideoPause(
	void
)
{
	PB_VERBOSE("%s: \n", __FUNCTION__);

	/*-- Constraint :
	 * A. The very beginning and ending 2 sec, NO Pause.
	 */
	sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
	/*-- Go On Puase operation */
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_PAUSE, 0, 0);
	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbMainCb.subStat = PB_VIDEO_STAT_PAUSE;

	/*-- Stop ELAP_TMR */
	//PB_LOG("PbVideo: TMR(%d) clearing \n", gvideoElapTimer);
	appTimerClear(&gvideoElapTimer);

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoSnap
 *------------------------------------------------------------------------*/
static
UINT32
appPbTrimVideoSnap(
	void
)
{
	videoTrimUIKeyControl(VDO_SXN_SNAPSHOT_START);
	appOsdMessageBoxShow(TRUE, ID_STR_IMAGE_SAVED);

	if (FAIL==sp5kMediaPlayControl(SP5K_MEDIA_PLAY_FRAME_CAPTURE, 0, 0)) 
	{
		PB_VERBOSE("%s: Video Snap failed \n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_DISK_ERROR;
		videoTrimUIKeyControl(VDO_SXN_SNAPSHOT_END);
		return FAIL;
	}

	videoTrimUIKeyControl(VDO_SXN_SNAPSHOT_END);

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoTerminate
 *------------------------------------------------------------------------*/
static
UINT32
appPbTrimVideoTerminate(
	void
)
{
	PB_VERBOSE("%s: videoStat(%d) \n", __FUNCTION__, gpbMainCb.subStat);

	/*-- UI OSD */

	/*-- Shall stop timer */
	appTimerClear(&gvideoElapTimer);
	//PB_LOG("PbVideo: TMR(%d) cleared \n", gvideoElapTimer);

	/*-- Terminate video play */
	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);

	gpbMainCb.subStat = PB_VIDEO_STAT_STANDBY;
	return SUCCESS;
	appPbTrimVideoSnap(); /* nobody invoke appPbTrimVideoSnap(), for clean compiling warning */
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoFlyingPlay
 *------------------------------------------------------------------------*/
static
void
appPbTrimVideoFlyingPlay(
	pbMediaFlyingDir_t flyingDir
)
{
	//PB_VERBOSE("%s: flyingDir(%s) \n", __FUNCTION__, (flyingDir==PB_MEDIA_FLYING_FF ? "FF" : "REW") );

	if (FAIL==videoTrimFlyingConstraintCheckup(flyingDir)) {
		PB_VERBOSE("%s: Operation ignored, media constraint !!", __FUNCTION__);
		return;
	}

	videoTrimFlyingPlayExecute(flyingDir);
	return;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoPlayStart
 *------------------------------------------------------------------------*/
static
UINT32
appPbTrimVideoPlayStart(
	void
)
{
	videoTrimBGBlack();
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	/*appOsdLib_TextDraw(LCD_SIZE_X/2,60,SP5K_GFX_ALIGN_CENTER,"Find start");
	appOsdLib_TextDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,"and press");
	appOsdLib_TextDraw(LCD_SIZE_X/2,140,SP5K_GFX_ALIGN_CENTER,"OK");*/	
	appOsdLib_TextIDDraw(LCD_SIZE_X/2,60,SP5K_GFX_ALIGN_CENTER,ID_STR_FIND_START);
	appOsdLib_TextIDDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,ID_STR_AND_PRESS_OK);
	APP_SLEEP_MS(3000); 
	CLEAR_OSD_SRC ;
	CLEAR_PIP_SRC ;
	gVideoTrimStatus = VIDEO_TRIM_START;
	appPlayOsd_VideoCntrlDraw(TRUE,FALSE,TRUE,TRUE);
	appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
	if (FAIL==sp5kMediaPlayControl(SP5K_MEDIA_PLAY_START, 0, 0)) 
	{
		gpbMainCb.err = PB_ERR_MEDIA_ERROR;
		return FAIL;
	}
	gpbMainCb.subStat = PB_VIDEO_STAT_PLAY_STARTING;
		/*-- SP5KFW will send SP5K_MSG_MEDIA_PLAY_OK or
		 * SP5K_MSG_MEDIA_PLAY_FAIL to notify the result of PLAY_START cmd.
		 * Go and wait for the message !!
		 */
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbTrimVideoStCreate(
	void
)
{
	if (FAIL==videoTrimValidityCheckup())
	{
		return FAIL;
	}

	if(0==gpbImgCb.mediaTotalTime) 
	{
		PB_VERBOSE("\n%s: Zero length media !! \n", __FUNCTION__);
		return FAIL;
	}

	/*-- Register TIMER for elapsed time process */
	gvideoElapTimer = appTimerSet(PB_VIDEO_ELAPSED_TIMER_MS, "VDO_ELAP");
	if (gvideoElapTimer==TIMER_NULL)
	{
		PB_DBG("%s: Register VDO_ELAP timer failed !! \n", __FUNCTION__);
		return FAIL;
	}
	gpbImgCb.mediaPlaySpeed = PB_PLAY_SPEED_NORMAL;
	gpbImgCb.mediaElapTime = 0;

	appPowerSavingEnableSet(FALSE);/*++ for not power saving while playing video*/

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStClose
 *------------------------------------------------------------------------*/
static
void
appPbTrimVideoStClose(
	void
)
{
	PB_DBG("VDO: State closing \n");

	if (gpbMainCb.subStat != PB_VIDEO_STAT_STANDBY) {
		appPbTrimVideoTerminate();
	}

	gpbMainCb.subStat = PB_VIDEO_STAT_STANDBY;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStExit
 *------------------------------------------------------------------------*/
 #define CALLSTACK(str, arg...)	{ printf("%s ", __FUNCTION__); printf(str, ##arg); callStack(); }
static
void
appPbTrimVideoStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("VDO: State exiting \n");
	appPbTrimVideoStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbTrimVideoStInit(
	void
)
{
	gpbMainCb.subStat = PB_VIDEO_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_VIDEO_CREATE, 0);
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStOnMsg
 *------------------------------------------------------------------------*/
void
appPbTrimVideoStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
	case SP5K_MSG_TIMER:
		if (gvideoElapTimer!=param)
		{
			break;
		}

		if (gpbMainCb.subStat==PB_VIDEO_STAT_PLAYING 
		 	|| gpbMainCb.subStat==PB_VIDEO_STAT_FF 
		 	|| gpbMainCb.subStat==PB_VIDEO_STAT_REW)
		{
			sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gpbImgCb.mediaElapTime);
		}
		appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
		break;
	case SP5K_MSG_MEDIA_PLAY_START:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY) 
		{
			PB_VERBOSE("VDO: PLAY_START(OKAY)\n");
			if (gpbMainCb.subStat != PB_VIDEO_STAT_PLAY_STARTING)
			{
				break;
			}
			gpbMainCb.err = PB_ERR_NONE;
			gpbMainCb.subStat = PB_VIDEO_STAT_PLAYING;
		} 
		else
		{
			if (gpbMainCb.subStat != PB_VIDEO_STAT_PLAY_STARTING)
			{
				break;
			}
			gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
			appPbErrorProcess(gpbMainCb.err);
			appPbTrimVideoTerminate();
			appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case SP5K_MSG_MEDIA_PLAY_STOP:
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{
			PB_VERBOSE("VDO: PLAY_STOP(OKAY)\n");
		}
		else
		{
			PB_VERBOSE("VDO: PLAY_STOP(FAIL)\n");
				/*-- There wont have this msg out of middleware.
				 * Instead, middleware will send SP5K_MSG_DISK_ERROR.
				 */
		}
		break;
	case SP5K_MSG_MEDIA_PLAY_FILE_ERROR:
		PB_VERBOSE("VDO: SP5K_MSG_MEDIA_PLAY_FILE_ERROR(0x%08x)\n", param);
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		appPbErrorProcess(gpbMainCb.err);
		appPbTrimVideoTerminate();
		appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;

	case SP5K_MSG_MEDIA_PLAY_END:
	case SP5K_MSG_MEDIA_FF_TO_END:
	case SP5K_MSG_MEDIA_RW_TO_END:
		if(gVideoTrimStatus == VIDEO_TRIM_VIDEO_SAVE_START)
		{	
			appPlayOsd_DlgMessageShow(ID_STR_SAVE_THIS_FILE_,DLGTYPE_YES_NO);
			gVideoTrimStatus = VIDEO_TRIM_VIDEO_SAVE_END;
		}
		else
		{
			appPbTrimVideoTerminate();
			appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}	
		break;

	case APP_UI_MSG_PB_VIDEO_CREATE:
		if (FAIL==appPbTrimVideoStCreate()) 
		{
			appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}

		if (FAIL==appPbTrimVideoPlayStart()) 
		{
			appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;
	/*----- Video ERROR Handling */
	case APP_UI_MSG_PB_VIDEO_ERROR:
		if (param==VIDEO_ERR_OUT_OF_ELAPSE_TMR) 
		{
			appPbTrimVideoTerminate();
			gpbMainCb.err = PB_ERR_VIDEO_ERROR;
			appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;

	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbVideoStOnKey
 *------------------------------------------------------------------------*/
void
appPbTrimVideoStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 TrimVideoTime;

	if(appPlayOsd_DlgMessageProcess(msg) == APPSTATE_MSG_IGNORE)
	{
		return;
	}
	switch (msg) {
	case APP_KEY_PRESS_S2:
		break;
	case APP_KEY_PRESS_UP:
		break;
	case APP_KEY_PRESS_DOWN:
		break;
	case APP_KEY_PRESS_SET:
		//dlg box process start
		if(appPlayOsd_DlgBoxStatusGet() == DLGSTATUS_SET)
		{
			appPlayOsd_DlgBoxStatusSet(DLGSTATUS_IDLE);
			switch(gVideoTrimStatus)
			{
				case VIDEO_TRIM_FIND_END_PIONT://review this file?
					if(gVideoTrimEndTime - gVideoTrimStartTime > 1000) /*add for mantis bug 0048523*/
					{
						if (0==appPlayOsd_DlgBoxSelectGet())//yes---> review
						{
				           	sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_START_TIME, gVideoTrimStartTime);
							sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_END_TIME, gVideoTrimEndTime);	
							CLEAR_OSD_SRC ;
							CLEAR_PIP_SRC ;
							sp5kMediaPlayControl(SP5K_MEDIA_PLAY_START, 0, 0);
							sp5kMediaPlayControl(SP5K_MEDIA_PLAY_RESUME, 0, 0);			
							gVideoTrimStatus = VIDEO_TRIM_VIDEO_SAVE_START;
						}
						else//no--->save confirm
						{
							appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
							appPlayOsd_DlgMessageShow(ID_STR_SAVE_THIS_FILE_,DLGTYPE_YES_NO);
							gVideoTrimStatus = VIDEO_TRIM_VIDEO_SAVE_END;
						}
					}
					else
					{
						printf("TRIM_VIDEO:time interval=%d us,fail to review!!\n",gVideoTrimEndTime-gVideoTrimStartTime);  
						appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
						appPlayOsd_DlgMessageShow(ID_STR_SAVE_THIS_FILE_,DLGTYPE_YES_NO);
						gVideoTrimStatus = VIDEO_TRIM_VIDEO_SAVE_END;
					}
					break;
				case VIDEO_TRIM_VIDEO_SAVE_END://save this file?
					if (0==appPlayOsd_DlgBoxSelectGet())//yes--->save
					{
						UINT32 fileIdx;
						CLEAR_PIP_SRC ;
						videoTrimBGBlack();
						appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
						/*appOsdLib_TextDraw(LCD_SIZE_X/2,60,SP5K_GFX_ALIGN_CENTER,"Writing");
						appOsdLib_TextDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,"Video");*/
						appOsdLib_TextIDDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,ID_STR_WRITING_VIDEO);
						#if 1 /* Fast Trim function */
						sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_START_TIME, gVideoTrimStartTime);
						sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_END_TIME, gVideoTrimEndTime);		
						TrimVideoTime = gVideoTrimEndTime-gVideoTrimStartTime;
						if(TrimVideoTime > 1000)  /*add for mantis bug 0048524 */
						{
							if( FAIL == sp5kMediaPlayControl(SP5K_MEDIA_PLAY_FAST_TRIM, 0, 0))
							#else
							if( FAIL == sp5kMediaEdit(MEDIA_EDIT_TRIM, gVideoTrimStartTime,gVideoTrimEndTime,0,0))
							#endif
							{
								BPrintf("VIDEO_TRIM_VIDEO write file fail!");
								CLEAR_OSD_SRC ;
								videoTrimBGBlack();
								/*appOsdLib_TextDraw(LCD_SIZE_X/2,60,SP5K_GFX_ALIGN_CENTER,"Writing File Fail");
								appOsdLib_TextDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,"Trim Video is too Short");*/
								appOsdLib_TextIDDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,ID_STR_WRITING_FILE_FAIL);
								APP_SLEEP_MS(3000); 
								CLEAR_OSD_SRC ;
								CLEAR_PIP_SRC ;
								gVideoTrimStatus = 0;
								appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
								gVideoTrimStatus = VIDEO_TRIM_START;
								return;
							}
						}
						else
						{
							printf("VIDEO_TRIM_VIDEO:file time =%d us ,write file fail!!\n",TrimVideoTime);
							CLEAR_OSD_SRC ;
							videoTrimBGBlack();
							appOsdLib_TextIDDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,ID_STR_WRITING_FILE_FAIL);
							APP_SLEEP_MS(3000); 
							CLEAR_OSD_SRC ;
							CLEAR_PIP_SRC ;
							gVideoTrimStatus = 0;
							appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
							gVideoTrimStatus = VIDEO_TRIM_START;
							return;
						}
						sp5kDcfFsCurFileIdxGet(&fileIdx);
						sp5kDcfFsCurFileIdxSet(fileIdx);			
						gVideoTrimStatus = VIDEO_TRIM_START;
						gpbMainCb.mainStat = PB_MAIN_STAT_NULL;
						sp5kModeSet(SP5K_MODE_STANDBY);
				 		sp5kModeWait(SP5K_MODE_STANDBY);
						appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
					}
					else//no--->exit
					{
						CLEAR_OSD_SRC ;
						CLEAR_PIP_SRC ;
						gVideoTrimStatus = 0;
						appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
						gVideoTrimStatus = VIDEO_TRIM_START;
					}
					break;
			}
			
			break;
		}
		//dlg box process end
		
		if (gVideoTrimStatus == VIDEO_TRIM_START && gpbMainCb.playStat == PB_PLAY_STAT_VIDEO_TRIM_P2P) 
		{//set start position in p2p mode
			appPbTrimVideoPause();
			sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gVideoTrimStartTime); 
			videoTrimBGBlack();
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
			/*appOsdLib_TextDraw(LCD_SIZE_X/2,60,SP5K_GFX_ALIGN_CENTER,"Find end");
			appOsdLib_TextDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,"and press");
			appOsdLib_TextDraw(LCD_SIZE_X/2,140,SP5K_GFX_ALIGN_CENTER,"OK");*/
			appOsdLib_TextIDDraw(LCD_SIZE_X/2,60,SP5K_GFX_ALIGN_CENTER,ID_STR_FIND_END);
			appOsdLib_TextIDDraw(LCD_SIZE_X/2,100,SP5K_GFX_ALIGN_CENTER,ID_STR_AND_PRESS_OK);
			APP_SLEEP_MS(3000); 
			CLEAR_OSD_SRC ;
			CLEAR_PIP_SRC ;
			appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
			appPlayOsd_VideoCntrlDraw(TRUE,FALSE,TRUE,TRUE);
			gVideoTrimStatus = VIDEO_TRIM_FIND_FIRST_PIONT;
			appPbTrimVideoResume();
		}
		else if(gVideoTrimStatus == VIDEO_TRIM_FIND_FIRST_PIONT) 
		{//set end positon in p2p mode
			appPbTrimVideoPause();
			sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gVideoTrimEndTime);
			sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_START_TIME, gVideoTrimStartTime);
			sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_END_TIME, gVideoTrimEndTime);
			sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);
			appPlayOsd_DlgMessageShow(ID_STR_REVIEW_THIS_FILE_,DLGTYPE_YES_NO);
			gVideoTrimStatus = VIDEO_TRIM_FIND_END_PIONT;
		}		
		else if(gVideoTrimStatus == VIDEO_TRIM_START &&  gpbMainCb.playStat == PB_PLAY_STAT_VIDEO_TRIM_SEC ) 
		{//Set start position in sec mode
			appPbTrimVideoPause();
			sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &gVideoTrimStartTime); 
			gVideoTrimEndTime = gVideoTrimStartTime +5000 ;
			if(gVideoTrimEndTime > gpbImgCb.mediaTotalTime)
			{
				gVideoTrimEndTime = gpbImgCb.mediaTotalTime ;
			}
			sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_START_TIME, gVideoTrimStartTime);
			sp5kMediaPlayAttrSet(MEDIA_ATTR_PLAY_END_TIME, gVideoTrimEndTime);
			sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0);
			appPlayOsd_DlgMessageShow(ID_STR_REVIEW_THIS_FILE_,DLGTYPE_YES_NO);
			gVideoTrimStatus = VIDEO_TRIM_FIND_END_PIONT;
		}
		break;
	case APP_KEY_PRESS_MODE:	
		if( gVideoTrimStatus == VIDEO_TRIM_VIDEO_REVIEW
		  	|| gVideoTrimStatus == VIDEO_TRIM_VIDEO_SAVE_START
		  	|| gVideoTrimStatus == VIDEO_TRIM_VIDEO_SAVE_END)
		{			

			CLEAR_OSD_SRC ;
			CLEAR_PIP_SRC ;
			gVideoTrimStatus = 0;
			appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			gVideoTrimStatus = VIDEO_TRIM_START;
		}
		break ;	
	case APP_KEY_PRESS_DEL:	
		break ;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		if (gpbMainCb.subStat != PB_VIDEO_STAT_PLAYING
			&&gpbMainCb.subStat != PB_VIDEO_STAT_FF 
			&&gpbMainCb.subStat != PB_VIDEO_STAT_REW)
		{
			break;
		}
		
		if (msg==APP_KEY_PRESS_LEFT) 
		{
			appPbTrimVideoFlyingPlay(PB_MEDIA_FLYING_REW);
		} 
		else if(msg==APP_KEY_PRESS_RIGHT)
		{
			appPbTrimVideoFlyingPlay(PB_MEDIA_FLYING_FF);
		}
		else if(msg==APP_KEY_PRESS_DEL)
		{
			appPbTrimVideoFlyingPlay(PB_MEDIA_FLYING_SLOW_MOTION);
		}
		
		appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);

		if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_FF)
		{
			appPlayOsd_MediaPlayStatusDraw( TRUE, FWDING);
		}
		else if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_REW)
		{
			appPlayOsd_MediaPlayStatusDraw( TRUE, RWDING);
		}
		else if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_SLOWMOTION)
		{
			appPlayOsd_MediaPlayStatusDraw( TRUE, RWDING);
		}
		break;
	case APP_KEY_PRESS_MENU:
		appPbTrimVideoTerminate();
		appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbVideoState
 *------------------------------------------------------------------------*/
void
appPbTrimVideoState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) 
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appPbTrimVideoState");
		appPbTrimVideoStInit();
		break;
	case APP_STATE_MSG_CLOSE:
		appPbTrimVideoStClose();
		appStateCloseDone();
		break;
	case APP_STATE_MSG_ABORT:   
		appPbTrimVideoTerminate();
		appPbTrimVideoStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appPbTrimVideoStOnKey(msg,param);
		}
		else
		{
			appPbTrimVideoStOnMsg(msg,param);
		}
		break;
	}

}
