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
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_dpof.h"
#include "app_ui_para.h"
#include "app_menu.h"
#include "app_cdfs_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

//#define DEL_KEY_REPEAT_INTERVAL			200 /* ms */

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	DEL_SXN_START,
	DEL_SXN_END,
	DEL_SXN_DO_ALL_START,
	DEL_SXN_DO_ALL_END,
} protUIKeyCtrl_t;

typedef enum {
	PB_DEL_RIGHT,
	PB_DEL_LEFT,
	//PB_DEL_UP,
	//PB_DEL_DOWN,
} pbProtShiftDir_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/*
static UINT32 gprotKeyIntvl = DEL_KEY_REPEAT_INTERVAL;
static UINT32 gprotKeyRptCnt = 0;
*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbImgDelStExit(UINT32 nextSt,	UINT32 stMsg);

#define DEL_ConfirmDlgShow()	DEL_ConfirmDlgShowEx(TRUE)
#define DEL_ConfirmDlgClear()	DEL_ConfirmDlgShowEx(FALSE)

/*-------------------------------------------------------------------------
 *  File Name : DEL_ConfirmDlgConfig
 *------------------------------------------------------------------------*/
static
void
DEL_ConfirmDlgConfig(
	void
)
{
	UINT32 strId;
	uiPara_t* puiPara = appUiParaGet();

	PB_VERBOSE("%s \n", __FUNCTION__);

	strId = (puiPara->selImgDelete==PB_UI_DELETE_SINGLE) ?
				ID_STR_DELETE_THIS_FILE_ : ID_STR_DELETE_ALL_FILE_;
	appPlayOsd_DlgConfirmYesNoConfig(strId, 1);
}

/*-------------------------------------------------------------------------
 *  File Name : DEL_ConfirmDlgShowEx
 *------------------------------------------------------------------------*/
static
void
DEL_ConfirmDlgShowEx(
	BOOL bShow
)
{
	uiPara_t* puiPara = appUiParaGet();
	/*CLEAR_OSD_SRC;*/  //mask for mantis bug 0048025 
	PB_VERBOSE("%s \n", __FUNCTION__);
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(bShow);
	appPlayOsd_FileNumDraw(bShow, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ModeIconDraw(bShow);
	
	if (bShow) 
	{
		appPlayOsd_ProtectDraw(gpbImgCb.isProtected);
	} 
	else 
	{
		appPlayOsd_ProtectDraw(FALSE);
	}
	if(gpbMainCb.totalImg>0 && puiPara->selImgDelete==PB_UI_DELETE_SINGLE)
	{
		appPLayOsd_LeftRightArrow(TRUE);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : delUIKeyControl
 *------------------------------------------------------------------------*/
static
void
delUIKeyControl(
	UINT8 ctrl
)
{
	PB_VERBOSE("DEL: KeySxN(%d) !!\n", ctrl);

	switch (ctrl){
	case DEL_SXN_START:
	case DEL_SXN_DO_ALL_START:
		appBtnDisable(BTN_ALL);
		break;
	case DEL_SXN_END:
	case DEL_SXN_DO_ALL_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("DEL: undefined key session !!\n");
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : delValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
delValidityCheckup(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- My opinion, the following shall be checked
	 * before entering PB_DEL state !!
	 */
	if (gpbDiskCb.isDiskErr) {
		gpbMainCb.err = PB_ERR_CARD_ERROR;
		return FAIL;
	} else if (gpbDiskCb.isCardLock) {
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
		return FAIL;
	}
	#if 0
	else if (gpbMainCb.isNoImage) {
		gpbMainCb.err = PB_ERR_NO_IMAGE;
		return FAIL;
	}
	#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : delAllAsyncPostProc
 *------------------------------------------------------------------------*/
static
void
delAllAsyncPostProc(
	BOOL isSuccess
)
{
	UINT32 totalBeforeDelAll;

	PB_DBG("DEL: (all) Async PostProc (%d) \n", isSuccess);

	if (gpbMainCb.subStat != PB_DEL_STAT_ALL_BUSY) {
		/* Not Aysnc DEL_ALL operation, ignore */
		PB_DBG("DEL: (all) not Async OP !!\n");
		return;
	}

	totalBeforeDelAll = gtotalImgCount;
	gtotalImgCount = appPbTotalFileCountGet();
	if (isSuccess) {
		PB_DBG("DEL: (all) FileOp OK !!\n");
		gcurrDcfIdx = gtotalImgCount;
		if (gtotalImgCount>0) {
			/* some files are protected !! */
			gpbMainCb.err = PB_ERR_IMG_PROTECTED;
			gpbMainCb.isNoImage = FALSE;
		} else {
			/* all files deleted */
			gpbMainCb.err = PB_ERR_NO_IMAGE;
			gpbMainCb.isNoImage = TRUE;
		}
	} else {
		PB_DBG("DEL: (all) FileOp NG !!\n");
		gpbMainCb.err = PB_ERR_DEL_ERROR;
	}

	gpbMainCb.subStat = PB_DEL_STAT_STANDBY;
	/*-- Enable Key Control */
	delUIKeyControl(DEL_SXN_DO_ALL_END);

}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelShift
 *------------------------------------------------------------------------*/
static
void
appPbImgDelShift(
	UINT8 shiftCtrl
)
{
	UINT32 ret=SUCCESS;

	PB_DBG("DEL: SHIFT(%d) \n", shiftCtrl);


	if (gpbMainCb.subStat!=PB_DEL_STAT_CONFIRM_MENU) {
		PB_DBG("DEL: (%s) busy !!\n", (shiftCtrl ? "Left" : "Right"));
		return;
	}
	if (gtotalImgCount==1) {
		PB_DBG("DEL: (%s) only 1 IMG, ignore !!\n", (shiftCtrl ? "Left" : "Right"));
		return;
	}

	switch (shiftCtrl) {
	case PB_SHIFT_NEXT:
	case PB_SHIFT_PRE:
		if (FAIL==appPbImageShift_PreCheck()) {
			PB_LOG("Shift_PreChk failed \n");
			break;
		}
		//PB_BEEP;
		//ret = appPbImageShift(shiftCtrl);
		PB_BEEP;
		if (FAIL==appPbImageShift(shiftCtrl)) {
			/*-- For PROTECT, the FILE_ERROR case just show BLACK BG
			 */
			//appPbErrorProcess(PB_ERR_FILE_ERROR);
			PB_BLACK_BKGND;
		}
		/*-- Emulate Key Repeat */
		appPbImageShiftRepeatCheck(0);
		break;

	default:
		PB_DBG("DEL: undefined shiftCtrl(%d) \n", shiftCtrl);
		return;
	}

	/*-- Update FileNum & Protect ICON  */
	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ProtectDraw(gpbImgCb.isProtected);

	if (ret==FAIL) 
	{
		appPbErrorProcess(gpbMainCb.err);
		DEL_ConfirmDlgShow();
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelSingle
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelSingle(
	void
)
{
	UINT32 ret;

	/*-- Validity check for Del Single
	 * is File Protected ?
	 */
	if (gpbImgCb.isProtected) 
	{
		PB_DBG("DEL: (Single) IMG Protected !!\n");
		gpbMainCb.err = PB_ERR_IMG_PROTECTED;
		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}

	/*-- Abort current IMG Playback !?
	 * DO NOT NEED to abort, because the request will be ignored
	 * at OnKey service.
	 */

	/*-- Go on */
	PB_DBG("\nDEL: Deleting image (%s) !! \n\n", gpbImgCb.szDcfFileName);
		/*-- Del DPOF as well */
	if((gpbImgCb.fileType==PB_FT_JPG) && (gpbImgCb.isDpofSet ==TRUE))
	{

		appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);
		
		gpbImgCb.isDpofSet=FALSE;
		gpbImgCb.dpofPrintNum = 0;
		gpbImgCb.isDpofStamp = FALSE;
		sp5kDpofFileWrite();	

	}
	//dpofDebugDump(1/*isQryDpof*/);

	
	ret = sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DEL, 0, PB_OP_SYNC, 0, 0);
	
	if (ret == FAIL) 
	{
		PB_LOG("DEL: (Single) Failed !!\n");
		gpbMainCb.err = PB_ERR_DEL_ERROR;
		return FAIL;
	}



	/*-- Re-calculate DCF index */
	if (gtotalImgCount==1) 
	{
		gtotalImgCount = 0;
		gcurrDcfIdx = 0;
		gpbMainCb.err = PB_ERR_NO_IMAGE;
		gpbMainCb.isNoImage = TRUE;
		return SUCCESS;
	}

	gtotalImgCount--;
	if (gcurrDcfIdx>=gtotalImgCount) 
	{
		gcurrDcfIdx=gtotalImgCount;
	}

	sp5kDcfFsCurFileIdxSet(gcurrDcfIdx);

	/*-- Playback current image */
	if (FAIL==appPbImagePlay(gcurrDcfIdx)) 
	{
		PB_LOG("\n%s: Play failed !! \n\n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_FILE_ERROR;

		/*appPbErrorProcess(gpbMainCb.err);*//*mark for resolve mantis bug 49098*/
	}

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelAll
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelAll(
	void
)
{
	PB_DBG("DEL: (ALL-OneByOne) !!\n");

	delUIKeyControl(DEL_SXN_DO_ALL_START);

	sp5kHostMsgSend(APP_UI_MSG_PB_DELALL_1BY1, gtotalImgCount);
	gpbMainCb.subStat = PB_DEL_STAT_ALL_BUSY;

	PB_VERBOSE("========================================\n");
	PB_DBG("DEL: (All-1by1) total(%d) go on !!\n", gtotalImgCount);
	PB_VERBOSE("========================================\n");
	PB_BLACK_BKGND;
	appPbMsg_BusyWait();

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelExecute
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelExecute(
	void
)
{
	UINT32 ret;
	uiPara_t* puiPara = appUiParaGet();
	
	ret = FAIL;
	switch (puiPara->selImgDelete) 
	{
		default:
			break;
		case PB_UI_DELETE_SINGLE:
			delUIKeyControl(DEL_SXN_START);
			ret = appPbImgDelSingle();
			delUIKeyControl(DEL_SXN_END);
			break;
		case PB_UI_DELETE_ALL:
			ret = appPbImgDelAll();
			break;
	}

	if (FAIL==ret) 
	{
		return FAIL;
	}
	
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelCreate(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- Validity check :
	 */
	if (FAIL==delValidityCheckup()) 
	{
		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}
	
	/*-- Configure Confirm Dialog Box
	 */
	DEL_ConfirmDlgConfig();
	DEL_ConfirmDlgShow();
	gpbMainCb.subStat = PB_DEL_STAT_CONFIRM_MENU;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStClose
 *------------------------------------------------------------------------*/
static
void
appPbImgDelStClose(
	void
)
{
	PB_DBG("DEL: State closing \n");
	gpbMainCb.lastImgIdx = gcurrDcfIdx;
	gpbMainCb.subStat = PB_DEL_STAT_NULL;
	gpbMainCb.mainStat= PB_MAIN_STAT_IMG_DEL_SUSPEND;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStExit
 *------------------------------------------------------------------------*/
static
void
appPbImgDelStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("DEL: State exiting \n");
	appPbImgDelStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelStInit(
	void
)
{
	PB_VERBOSE("DEL: State initing \n");
	if(gpbMainCb.isDelHotKey)
	{
		gpbMainCb.mainStat = PB_MAIN_STAT_IMG_DEL_SUSPEND;
	}
	gpbMainCb.subStat = PB_DEL_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_DEL_CREATE, 0);
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStOnMsg
 *------------------------------------------------------------------------*/
void
appPbImgDelStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("DEL_OnMsg: 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg) 
	{
	case APP_UI_MSG_PB_DELALL_1BY1:
		PB_VERBOSE("APP_UI_MSG_PB_DELALL_1BY1(0x%08x)\n", param);
		sp5kDcfFsCurFileIdxSet(param);
		gpbImgCb.dcfIdx = param;
		playImgDcfAttrGet(&gpbImgCb);
		//printf("isdpofset=%d,isprotect=%d\n",gpbImgCb.isDpofSet,gpbImgCb.isProtected);
		if((gpbImgCb.fileType==PB_FT_JPG) &&(!gpbImgCb.isProtected))
		{
			#if SP5K_CDFS_OPT
			#else
			appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);
			#endif
		}
		//dpofDebugDump(1/*isQryDpof*/);
		
		sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DELALL, 0, PB_OP_SYNC, 0, 0);

		sp5kDpofFileWrite();
		sp5kHostMsgSend(APP_UI_MSG_PB_DELALL_DONE, 0);
		break;

	case APP_UI_MSG_PB_DELALL_DONE:
		delUIKeyControl(DEL_SXN_DO_ALL_END);
		PB_OSD_CLR;
		gtotalImgCount = appPbTotalFileCountGet();
		gcurrDcfIdx = gtotalImgCount;
		if (gtotalImgCount>0) 
		{
			appPbImagePlay(gcurrDcfIdx);
			if(gpbImgCb.isProtected){
				gpbMainCb.err = PB_ERR_IMG_PROTECTED;
				__FUNC_TRACK__;
				appPbErrorProcess(gpbMainCb.err);
				DEL_ConfirmDlgShow();
				gpbMainCb.subStat=PB_DEL_STAT_CONFIRM_MENU;
			}	
			else
			{
				appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}			
		} 
		else
		{
			appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;

	case SP5K_MSG_STILLPLAY_DONE:
		PB_VERBOSE("SP5K_MSG_STILLPLAY_DONE(0x%08x)\n", param);
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR) 
		{
			DBG_PRINT("%s: !! WARNING !!, PB_DONE error(0x%x) !!\n", __FUNCTION__, tmpU32);
			PB_BLACK_BKGND;
				/*-- In PROTEC case, FILE_ERROR wont show center message!!
				 */
			break;
		}

		if ( (IS_PB_QV_PLAYING && tmpU32==PB_DONE_QV_FINISH) 
			 ||(IS_PB_THUMB_PLAYING && tmpU32==PB_DONE_THUMB_FINISH) )
		{
			PB_DBG("THUMB/QV Done !!\n");
			gpbImgCb.isThumbImgOK = TRUE;
			gpbMainCb.isPlayDone = TRUE;
			appPbJpgMainPlayStart();
		}
		else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH)
		{
			PB_DBG("MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
		}
		break;


	case SP5K_MSG_DCF_FILEOP_COMPLETE:
		PB_DBG("DEL: (all) FILEOP OK \n");
		delAllAsyncPostProc(TRUE);
		if (gpbMainCb.err != PB_ERR_NO_IMAGE) 
		{
			/* Delete ALL has encountered ERROR !! */
			__FUNC_TRACK__;
			appPbErrorProcess(gpbMainCb.err);
		}
		appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case SP5K_MSG_DCF_FILEOP_FAIL:
		PB_DBG("DEL: (all) FILEOP NG \n");
		delAllAsyncPostProc(FALSE);
		__FUNC_TRACK__;
		appPbErrorProcess(gpbMainCb.err);
		appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case APP_UI_MSG_PB_SHIFT_NEXT:
		PB_DBG("DEL: APP_UI_MSG_PB_SHIFT_NEXT \n");
		appPbImgDelShift(PB_DEL_RIGHT);
		break;
	case APP_UI_MSG_PB_SHIFT_PRE:
		PB_DBG("DEL: APP_UI_MSG_PB_SHIFT_PRE \n");
		appPbImgDelShift(PB_DEL_LEFT);
		break;
	case APP_UI_MSG_PB_DEL_CREATE:
		if (FAIL==appPbImgDelCreate()) 
		{
			PB_DBG("DEL: Creation failed !!\n");
			appPbImgDelStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;

	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStOnKey
 *------------------------------------------------------------------------*/
void
appPbImgDelStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	PB_VERBOSE("DEL_OnKey : [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_DEL_STAT_CONFIRM_MENU) 
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		}
		break;
	case APP_KEY_PRESS_SET:
		PB_VERBOSE("DEL: APP_KEY_PRESS_SET \n");

		if (gpbMainCb.playStat!=PB_PLAY_STAT_STANDBY) 
		{
			PB_VERBOSE("DEL: Ignore SET Key, Playing busy !! \n");
			break;
		}

		if (gpbMainCb.subStat != PB_DEL_STAT_CONFIRM_MENU) 
		{
			PB_VERBOSE("DEL: Ignore SET Key, Wrong DEL state !! \n");
			break;
		}

		PB_BEEP;
		if (0==appPlayOsd_DlgBoxSelectGet()) 
		{
			/*-- YES */
			if (FAIL==appPbImgDelExecute()) 
			{
				PB_LOG("%s: DEL excute failed !! \n", __FUNCTION__);
			} 
			else 
			{
				/*-- Execute SUCCESS !! */
				/*-- Doing DelAll 1by1,
				 * DO NOTHING while Execute returns
				 */
					__FUNC_TRACK__;
				if (puiPara->selImgDelete==PB_UI_DELETE_ALL) 
				{
					break;
				}

				if (PB_ERR_NO_IMAGE==gpbMainCb.err ) 
				{
					PB_OSD_CLR;
					appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
					break;
				}
			}
			__FUNC_TRACK__;

			/*-- UI */
			if (puiPara->selImgDelete==PB_UI_DELETE_SINGLE) 
			{
				APP_OSD_REFRESH_OFF;
				/*-- UI : Prepare DLG box */
				__FUNC_TRACK__;
				appPlayOsd_DlgBoxSelectSet(1); /* change default */
				__FUNC_TRACK__;
				DEL_ConfirmDlgShow();
				__FUNC_TRACK__;
				/*-- UI : Update FileNum & Protect ICON */
				appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
				__FUNC_TRACK__;
				appPlayOsd_ProtectDraw(gpbImgCb.isProtected);
				__FUNC_TRACK__;
				APP_OSD_REFRESH_ON;
			}

		}
		else 
		{
			/*-- NO */
			PB_OSD_CLR;
			if (gpbMainCb.isDelHotKey) 
			{
				appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			} 
			else 
			{
				appPbImgDelStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			}
		}
		break;
	case APP_KEY_PRESS_DEL:
		appPbImgDelStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
	break;
	case APP_KEY_PRESS_LEFT:
		if (puiPara->selImgDelete==PB_UI_DELETE_SINGLE) 
		{
			PB_VERBOSE("DEL: APP_KEY_PRESS_LEFT \n");
			delUIKeyControl(DEL_SXN_START);
			appPbImgDelShift(PB_SHIFT_PRE);
			delUIKeyControl(DEL_SXN_END);
		}
		break;
	case APP_KEY_PRESS_RIGHT:
		if (puiPara->selImgDelete==PB_UI_DELETE_SINGLE) 
		{
			PB_VERBOSE("DEL: APP_KEY_PRESS_RIGHT \n");
			delUIKeyControl(DEL_SXN_START);
			appPbImgDelShift(PB_SHIFT_NEXT);
			delUIKeyControl(DEL_SXN_END);
		}
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelState
 *------------------------------------------------------------------------*/
void
appPbImgDelState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbImgDelState");
		appPbImgDelStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appPbImgDelStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appPbImgDelStOnKey(msg,param);
		}
		else
		{
			appPbImgDelStOnMsg(msg,param);
		}
		break;
	}

}
