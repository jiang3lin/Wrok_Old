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
#include "app_stringid.h"
#include "app_ui_para.h" 
#include "app_menu_tab.h"
#include "app_menu.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define PROT_KEY_REPEAT_INTERVAL			200 /* ms */

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	PROT_SXN_START,
	PROT_SXN_END,
	PROT_SXN_DO_ALL_START,
	PROT_SXN_DO_ALL_END,
} protUIKeyCtrl_t;

typedef enum {
	PB_PROT_RIGHT,
	PB_PROT_LEFT,
	//PB_PROT_UP,
	//PB_PROT_DOWN,
} pbProtShiftDir_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

//static UINT32 gprotKeyIntvl = PROT_KEY_REPEAT_INTERVAL;
static UINT32 gprotKeyRptCnt = 0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern BOOL gProtectAllFinish;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbImgProtStExit(UINT32 nextSt,	UINT32 stMsg);

/*-------------------------------------------------------------------------
 *  File Name : protUIKeyControl
 *------------------------------------------------------------------------*/

static
void
protUIKeyControl(
	UINT8 ctrl
)
{
	switch (ctrl){
	case PROT_SXN_START:
	case PROT_SXN_DO_ALL_START:
		appBtnDisable(BTN_ALL);
		break;
	case PROT_SXN_END:
	case PROT_SXN_DO_ALL_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("%s: undefined key session !!\n", __FUNCTION__);
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : PROTECT_ConfirmDialogConfig
 *------------------------------------------------------------------------*/
static
void
PROTECT_ConfirmDialogConfig(
	void
)
{
	UINT32 strId;
	uiPara_t* puiPara = appUiParaGet();

	PB_VERBOSE("%s \n", __FUNCTION__);

	if (puiPara->selImgProtect==PB_UI_PROTECT_SINGLE) {
		strId = (gpbImgCb.isProtected) ? ID_STR_UNPROTECT_FILE_ : ID_STR_PROTECT_FILE_;
	} else if (puiPara->selImgProtect==PB_UI_PROTECT_ALL) {
		strId = ID_STR_PROTECT_ALL_;
	} else {
		/* PB_UI_PROTECT_UNLOCK_ALL */
		strId = ID_STR_UNPROTECT_ALL_;
	}

	appPlayOsd_DlgConfirmYesNoConfig(strId, 1);

}

/*-------------------------------------------------------------------------
 *  File Name : PROTECT_ConfirmDialogShow
 *------------------------------------------------------------------------*/
static
void
PROTECT_ConfirmDialogShow(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();

	PB_VERBOSE("%s \n", __FUNCTION__);
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSet(TRUE);
	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ModeIconDraw(TRUE);
	appPlayOsd_ProtectDraw(gpbImgCb.isProtected);
	if (puiPara->selImgProtect==PB_UI_PROTECT_SINGLE) {
		appPLayOsd_LeftRightArrow(TRUE);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : PROTECT_ConfirmDialogUpdate
 *------------------------------------------------------------------------*/
static
void
PROTECT_ConfirmDialogUpdate(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();

	PB_VERBOSE("%s: isProtected(%d) \n", __FUNCTION__, gpbImgCb.isProtected);
	
	CLEAR_OSD_SRC;
	appPlayOsd_ModeIconDraw(TRUE);
	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ProtectDraw(gpbImgCb.isProtected);
	if (puiPara->selImgProtect==PB_UI_PROTECT_SINGLE) /*add for mantis bug 0047212*/
	{
		appPLayOsd_LeftRightArrow(TRUE);
	}
	if (gpbImgCb.isProtected) {
		appPlayOsd_DlgBoxTitleUpdate(ID_STR_UNPROTECT_FILE_);
	} else {
		appPlayOsd_DlgBoxTitleUpdate(ID_STR_PROTECT_FILE_);
	}
}



/*-------------------------------------------------------------------------
 *  File Name : PROTECT_ConfirmDialogClear
 *------------------------------------------------------------------------*/
static
void
PROTECT_ConfirmDialogClear(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);
	appPlayOsd_DlgBoxClear();
	appPbOsd_AuxNote_OKSelect(FALSE);
	appPlayOsd_FileNumDraw(FALSE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ModeIconDraw(FALSE);
	appPlayOsd_ProtectDraw(FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgProtValidityCheckup(
	void
)
{
	if (gpbDiskCb.isCardLock) {
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
		return FAIL;
	} else if (gpbImgCb.isFileErr) {
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		return FAIL;
	} if (gpbMainCb.isNoImage) {
		gpbMainCb.err = PB_ERR_NO_IMAGE;
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : protAllAsyncPostProc
 *------------------------------------------------------------------------*/
static
void
protAllAsyncPostProc(
	BOOL isSuccess
)
{
	PB_VERBOSE("PROT: Async PostProc (%d) \n", isSuccess);

	if (gpbMainCb.subStat != PB_PROT_STAT_ALL_BUSY) {
		/* Not Aysnc PROT_ALL operation, ignore */
		PB_DBG("PROT: not Async OP !!\n");
		return;
	}

	gpbMainCb.subStat = PB_PROT_STAT_STANDBY;

	if (isSuccess) {
		PB_VERBOSE("PROT: ASYNC OK !!\n");
		gpbMainCb.err = PB_ERR_NONE;
	} else {
		PB_VERBOSE("PROT: ASYNC NG !!\n");
		gpbMainCb.err = PB_ERR_PROT_ERROR;
	}

	/*-- Enable Key Control */
	protUIKeyControl(PROT_SXN_DO_ALL_END);
	#if CAM_TYPE_CVR
	appPbImgProtStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
	#else
	appPbImgProtStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtShift
 *------------------------------------------------------------------------*/
static
void
appPbImgProtShift(
	UINT8 shiftCtrl
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	PB_VERBOSE("appPbImgProtShift (%d) \n", shiftCtrl);

	if (gpbMainCb.subStat!=PB_PROT_STAT_CONFIRM_MENU && puiPara->selImgProtect!=PB_UI_PROTECT_SINGLE) {
		PB_VERBOSE("PbImgProt: (%d) busy !!\n", shiftCtrl);
		return;
	}

	protUIKeyControl(PROT_SXN_START);

	switch (shiftCtrl) {
	case PB_SHIFT_PRE:
	case PB_SHIFT_NEXT:
		if (FAIL==appPbImageShift_PreCheck()) {
			PB_DBG("Shift_PreCheck failed !!\n");
			break;
		}
		PB_BEEP;
		/*PB_OSD_CLR;*/
		if (FAIL==appPbImageShift(shiftCtrl)) {
			/*-- For PROTECT, the FILE_ERROR case just show BLACK BG
			 */
			//appPbErrorProcess(PB_ERR_FILE_ERROR);
			PB_BLACK_BKGND;
		}
		APP_OSD_REFRESH_OFF;
		PROTECT_ConfirmDialogUpdate();
		APP_OSD_REFRESH_ON;
		/*-- Emulate Key Repeat */
		appPbImageShiftRepeatCheck(0);
		break;

	default:
		PB_VERBOSE("PbImgProt: undefined navCtrl(%d) \n", shiftCtrl);
		break;
	}

	protUIKeyControl(PROT_SXN_END);

}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProt_Single
 *------------------------------------------------------------------------*/
static
void
appPbImgProt_Single(
	void
)
{
	/*-- UI process :
	 * LED, Key, Timeout timer, OSD, etc.
	 */
	gProtectAllFinish = FALSE;
	protUIKeyControl(PROT_SXN_START);
	PB_OSD_CLR;

	/*-- Go on */
	if (gpbImgCb.isProtected) {
		PB_DBG("\n UN_LOCK image !! \n\n");
		sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_UNLOCK, 0, PB_OP_SYNC, 0, 0);
		gpbImgCb.isProtected = FALSE;
	} else {
		PB_DBG("\n LOCK image !! \n\n");
		sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_LOCK, 0, PB_OP_SYNC, 0, 0);
		gpbImgCb.isProtected = TRUE;
	}
	gProtectAllFinish = TRUE;

	protUIKeyControl(PROT_SXN_END);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProt_LockAll
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgProt_LockAll(
	void
)
{
	PB_VERBOSE("PbImgProt: Lock ALL !!\n");
	gProtectAllFinish = FALSE;
	protUIKeyControl(PROT_SXN_DO_ALL_START);

	//if (gpbMainCb.totalImg > 100) {
		PB_OSD_CLR;
		appPbMsg_BusyWait();
	//}

	/*-- Go on */
	if (FAIL==sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_LOCKALL, 0, PB_OP_ASYNC, 0, 0)) {
		PB_DBG("PROT: (LockAll) FILEOP failed !!\n");
		gpbMainCb.err = PB_ERR_PROT_ERROR;
		protUIKeyControl(PROT_SXN_DO_ALL_END);

		#if PB_FILEOP_ASYNC_WORKAROUND /* MANTIS BUG : 20302 workaround */
		protAllAsyncPostProc(FALSE);
		#endif

		return FAIL;
	}
	gpbImgCb.isProtected = TRUE;
	gpbMainCb.subStat = PB_PROT_STAT_ALL_BUSY;

	#if PB_FILEOP_ASYNC_WORKAROUND /* MANTIS BUG : 20302 workaround */
	protAllAsyncPostProc(TRUE);
	#endif

	PB_VERBOSE("PbImgProt: Lock ALL DONE !!\n");

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProt_UnlockAll
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgProt_UnlockAll(
	void
)
{
	PB_VERBOSE("PbImgProt: Unlock ALL !!\n");
	gProtectAllFinish = FALSE;
	protUIKeyControl(PROT_SXN_DO_ALL_START);

	//if (gpbMainCb.totalImg > 100) {
		PB_OSD_CLR;
		appPbMsg_BusyWait();
	//}

	/*-- Go on */
	if (FAIL==sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_UNLOCKALL, 0, PB_OP_ASYNC, 0, 0)) {
		PB_DBG("PROT: (UN-LockAll) FILEOP failed !!\n");
		gpbMainCb.err = PB_ERR_PROT_ERROR;
		protUIKeyControl(PROT_SXN_DO_ALL_END);

		#if PB_FILEOP_ASYNC_WORKAROUND /* MANTIS BUG : 20302 workaround */
		protAllAsyncPostProc(FALSE);
		#endif

		return FAIL;
	}
	gpbImgCb.isProtected = FALSE;
	gpbMainCb.subStat = PB_PROT_STAT_ALL_BUSY;

	#if PB_FILEOP_ASYNC_WORKAROUND /* MANTIS BUG : 20302 workaround */
	protAllAsyncPostProc(TRUE);
	#endif

	PB_VERBOSE("PbImgProt: UnLock ALL DONE !!\n");

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtExecute
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgProtExecute(
	void
)
{
	/*-- Execute IMG Protect operation
	 */
	 uiPara_t* puiPara = appUiParaGet();
	
	switch (puiPara->selImgProtect) {
	default:
		break;
	case PB_UI_PROTECT_SINGLE:
		PB_VERBOSE("PROT: Single\n");
		appPbImgProt_Single();
		break;
	case PB_UI_PROTECT_ALL:
		PB_VERBOSE("PROT: Lock ALL\n");
		appPbImgProt_LockAll();
		break;
	case PB_UI_PROTECT_UNLOCK_ALL:
		PB_VERBOSE("PROT: UnLock ALL\n");
		gpbMainCb.subStat = PB_PROT_STAT_ALL_BUSY;
		appPbImgProt_UnlockAll();
		break;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgProtCreate(
	void
)
{
	/*-- Validity check	 */
	if (FAIL==appPbImgProtValidityCheckup()) {
		if (gpbMainCb.err==PB_ERR_CARD_LOCKED) {
			PB_VERBOSE("%s: DISK Locked !!\n", __FUNCTION__);
			appPbErr_CardLock();
		} else if (gpbMainCb.err==PB_ERR_FILE_ERROR) {
			PB_VERBOSE("%s: FILE Error !!\n", __FUNCTION__);
			//appPbErr_FileError();
			appPbErrorProcess(PB_ERR_FILE_ERROR);
		}
		#if 0
		else if (gpbMainCb.err==PB_ERR_NO_IMAGE) {
			PB_VERBOSE("%s: No IMG !!\n", __FUNCTION__);
			appPbErrorProcess(PB_ERR_NO_IMAGE);
		}
		#endif
		return FAIL;
	}

	/*-- Do Confirmation, before execute operation
	 */
	PROTECT_ConfirmDialogConfig();
	PROTECT_ConfirmDialogShow();

	gpbMainCb.subStat = PB_PROT_STAT_CONFIRM_MENU;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtStClose
 *------------------------------------------------------------------------*/
static
void
appPbImgProtStClose(
	void
)
{
	PB_DBG("PbImgProt: State closing \n");
	gpbMainCb.subStat = PB_PROT_STAT_NULL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtStExit
 *------------------------------------------------------------------------*/
static
void
appPbImgProtStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("PbImgProt: State exiting \n");
	gpbMainCb.mainStat = PB_MAIN_STAT_IMG_PROT_SUSPEND;
	appPbImgProtStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgProtStInit(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	puiPara->EmptyPara = puiPara->EmptyPara;
	PB_VERBOSE("%s: selImgProtect(%d) \n", __FUNCTION__, puiPara->selImgProtect);

	gpbMainCb.subStat = PB_PROT_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_PROT_CREATE, 0);
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtStOnMsg
 *------------------------------------------------------------------------*/
void
appPbImgProtStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("PROT_OnMsg: 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg) {
	case SP5K_MSG_STILLPLAY_DONE:
		PB_VERBOSE("SP5K_MSG_STILLPLAY_DONE(0x%08x)\n", param);
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR) {
			PB_LOG("!! WARNING !!, PB_DONE ERROR (0x%x) !!\n", tmpU32);
			//gpbImgCb.isFileErr = TRUE;
			//gpbImgCb.fileType = PB_FT_UNKNOWN;
			//gpbImgCb.dcfAttr.fileType=0;
			//gpbMainCb.err = PB_ERR_FILE_ERROR;
			//gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			//appPbErrorProcess(gpbMainCb.err);
			PB_BLACK_BKGND;
				/*-- In PROTEC case, FILE_ERROR wont show center message!!
				 */
			break;
		}

		if ( (IS_PB_QV_PLAYING && tmpU32==PB_DONE_QV_FINISH) ||
			 (IS_PB_THUMB_PLAYING && tmpU32==PB_DONE_THUMB_FINISH) )
		{
			PB_DBG("THUMB/QV Done !!\n");
			gpbImgCb.isThumbImgOK = TRUE;
			gpbMainCb.isPlayDone = TRUE;
			appPbJpgMainPlayStart();
		} else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH) {
			PB_DBG("MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
		}
		break;

	/*-- SP5K Async FileOP */
	case SP5K_MSG_DCF_FILEOP_COMPLETE:
		PB_VERBOSE("PROT: (all) FILEOP OK \n");
		protAllAsyncPostProc(TRUE);
		break;
	case SP5K_MSG_DCF_FILEOP_FAIL:
		PB_VERBOSE("PROT: (all) FILEOP NG \n");
		protAllAsyncPostProc(FALSE);
		break;

	/*-- IMG Shift */
	case APP_UI_MSG_PB_SHIFT_NEXT:
		PB_DBG("PbImgProt: APP_UI_MSG_PB_SHIFT_NEXT \n");
		appPbImgProtShift(PB_PROT_RIGHT);
		break;
	case APP_UI_MSG_PB_SHIFT_PRE:
		PB_DBG("PbImgProt: APP_UI_MSG_PB_SHIFT_PRE \n");
		appPbImgProtShift(PB_PROT_LEFT);
		break;

	/*-- PROTECT operation create */
	case APP_UI_MSG_PB_PROT_CREATE:
		if (FAIL==appPbImgProtCreate()) {
			appPbImgProtStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtStOnKey
 *------------------------------------------------------------------------*/
void
appPbImgProtStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	PB_VERBOSE("PROT_OnKey : [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_PROT_STAT_CONFIRM_MENU) 
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		}
		break;
	case APP_KEY_PRESS_SET:
		if (gpbMainCb.subStat != PB_PROT_STAT_CONFIRM_MENU) 
		{
			PB_VERBOSE("PROT: Ignore SET Key, Wrong state !! \n");
			break;
		}
		PB_BEEP;
		if (0==appPlayOsd_DlgBoxSelectGet()) 
		{
		#if !CAM_TYPE_CVR
			APP_OSD_REFRESH_OFF;
		#endif
			/*-- YES */
			if (puiPara->selImgProtect==PB_UI_PROTECT_SINGLE) 
			{
				/*-- Protect Single */
				appPbImgProtExecute();
				PROTECT_ConfirmDialogUpdate();
			} 
			else 
			{
				/*-- Protect/UnProtect ALL */
				PROTECT_ConfirmDialogClear();
				appPbImgProtExecute();
			}
		#if !CAM_TYPE_CVR
			APP_OSD_REFRESH_ON;
		#endif
		} 
		else 
		{
			/*-- NO */
			PROTECT_ConfirmDialogClear();
			appPbImgProtStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case APP_KEY_PRESS_DEL:
		  PROTECT_ConfirmDialogClear();
		 appPbImgProtStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
               break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		if (puiPara->selImgProtect==PB_UI_PROTECT_SINGLE)
		{
			gprotKeyRptCnt = 0;
			appPbImgProtShift(((msg==APP_KEY_PRESS_RIGHT) ? PB_SHIFT_NEXT : PB_SHIFT_PRE));
		}
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgProtState
 *------------------------------------------------------------------------*/
void
appPbImgProtState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbImgProtState");
		appPbImgProtStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appPbImgProtStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appPbImgProtStOnKey(msg,param);
		}
		else{
			appPbImgProtStOnMsg(msg,param);
		}
		break;
	}

}
