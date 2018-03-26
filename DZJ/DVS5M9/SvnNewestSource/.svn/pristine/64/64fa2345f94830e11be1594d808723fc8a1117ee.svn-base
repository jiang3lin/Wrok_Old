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

#if 0
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define DPOF_DpofOsdIconDraw(bshow)	appPlayOsd_IconDraw( bshow, \
										6, 45, \
										ID_ICON_PLAYBACK_THUMBNAIL_DPOF)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	DPOF_SXN_START,
	DPOF_SXN_END,
	DPOF_SXN_DO_ALL_START,
	DPOF_SXN_DO_ALL_END,
} dpofUIKeyCtrl_t;

typedef enum {
	PB_DPOF_RIGHT,
	PB_DPOF_LEFT,
	PB_DPOF_UP,
	PB_DPOF_DOWN,
} pbDpofShiftDir_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 extern BOOL isStExiting;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
 void appPbDpofStExit(UINT32 nextSt,	UINT32 stMsg);

#if 0
/*-------------------------------------------------------------------------
 *  File Name : dpofUIKeyControl
 *------------------------------------------------------------------------*/
static
void
dpofUIKeyControl(
	UINT8 ctrl
)
{
#if 1
	switch (ctrl){
	case DPOF_SXN_START:
	case DPOF_SXN_DO_ALL_START:
		appBtnDisable(BTN_ALL);
		break;
	case DPOF_SXN_END:
	case DPOF_SXN_DO_ALL_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("%s: undefined key session !!\n", __FUNCTION__);
		break;
	}
#endif
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : dpofDebugDump
 *------------------------------------------------------------------------*/
static
void
dpofDebugDump(
	UINT8 isQryDpof
)
{
#if HOST_DBG
	UINT8 isSet;
	UINT32 jobQty;
	UINT8 isStamp;
	UINT8 szDateTime[24];

	if (isQryDpof) {
		isSet = appDpofJobExistQuery(APP_DPOF_MODE_STD);
		jobQty = appDpofPrintQuantityGet(APP_DPOF_MODE_STD);
		isStamp = appDpofDateStampExistQuery(APP_DPOF_MODE_STD);
	} else {
		isSet = gpbImgCb.isDpofSet;
		jobQty = gpbImgCb.dpofPrintNum;
		isStamp = gpbImgCb.isDpofStamp;
	}
	appDpofDateStampGet(APP_DPOF_MODE_STD, szDateTime);

	PB_DBG("\n\n====== DPOF Param (%s) \n", (isQryDpof ? "DRAM" : "pbLOCAL"));
	PB_DBG("isExist(%d) Qty(%d) isStamp(%d) ... \n", isSet, jobQty, isStamp);
	PB_DBG("DateStamp TEXT [%s] ... \n\n", szDateTime);
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : dpofExifDateTimeTextFormat
 *------------------------------------------------------------------------*/
void
dpofExifDateTimeTextFormat(
	sp5kDcfTime_t *pdcfTime,
	UINT8 *pszDateTime
)
{
	sprintf(pszDateTime, "%4d:%02d:%02d",
			pdcfTime->year, pdcfTime->mon, pdcfTime->day);

	#if HOST_DBG
	PB_VERBOSE("DPOF: DateTime(%s) \n", pszDateTime);
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : DPOF_ConfirmDlgConfig
 *------------------------------------------------------------------------*/
static
void
DPOF_ConfirmDlgConfig(
	UINT32 strId
)
{
	appPlayOsd_DlgConfirmYesNoConfig(strId, 0);

}

/*-------------------------------------------------------------------------
 *  File Name : DPOF_ConfirmDlgShow
 *------------------------------------------------------------------------*/
static
void
DPOF_ConfirmDlgShow(
	void
)
{
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSet(TRUE);
	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ModeIconDraw(TRUE);
	DPOF_DpofOsdIconDraw(gpbImgCb.isDpofSet);
	if (gpbUiParam.selDpof==PB_UI_DPOF_PRINT_SINGLE) 
	{
		appPLayOsd_LeftRightArrow(TRUE);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : DPOF_ConfirmDlgUpdate
 *------------------------------------------------------------------------*/
static
void
DPOF_ConfirmDlgUpdate(
	void
)
{
	PB_VERBOSE("%s: isDpofSet(%d) \n", __FUNCTION__, gpbImgCb.isDpofSet);

	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	DPOF_DpofOsdIconDraw(gpbImgCb.isDpofSet);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDpoftValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
appPbDpofValidityCheckup(void)
{
	if (gpbDiskCb.isCardLock)
	{
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
		return FAIL;
	}
	else if (gpbImgCb.isFileErr)
	{
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}
	else if (!IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType))
	{
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDpofShift
 *------------------------------------------------------------------------*/
static
void
appPbDpofShift(
	UINT8 shiftCtrl
)
{

	PB_VERBOSE("%s: (%d) \n", __FUNCTION__, shiftCtrl);

	if (gpbMainCb.subStat!=PB_DPOF_STAT_CONFIRM_MENU &&
		gpbUiParam.selDpof!=PB_UI_DPOF_PRINT_SINGLE)
	{
		PB_VERBOSE("%s: (%d) busy !!\n", __FUNCTION__, shiftCtrl);
		return;
	}

	PB_BEEP;
	switch (shiftCtrl) {
	case PB_SHIFT_PRE:
	case PB_SHIFT_NEXT:
		if (FAIL==appPbImageShift_PreCheck()) {
			PB_DBG("Shift_PreCheck failed !!\n");
			break;
		}

		if (FAIL==appPbJpgImageShift(shiftCtrl, TRUE/*isSkipFileErr*/)) {
			/*-- For DPOF, the FILE_ERROR case should be skipped !!
			 * So, this is an unexpected error. Go Alert !!
			 */
			PB_BLACK_BKGND;
			PB_LOG("\n\n!!FATAL!! @ %s, %d, \n\n", __FUNCTION__, __LINE__);
			goto lbl_dpof_shift_exit;
		}

		/*-- Update DlgBox */
		DPOF_ConfirmDlgUpdate();
		/*-- Emulate Key Repeat */
		appPbImageShiftRepeatCheck(0);
		break;

	default:
		PB_VERBOSE("%s: undefined navCtrl(%d) \n", __FUNCTION__, shiftCtrl);
		break;
	}

lbl_dpof_shift_exit:
	return;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbDpof_ResetAll
 *------------------------------------------------------------------------*/
static
void
appPbDpof_ResetAll(
	void
)
{
	PB_VERBOSE("PbDpof: Reset ALL !!\n");

	PB_OSD_CLR;
	appPbMsg_BusyWait();

	/*-- Go on */
	gpbMainCb.lastImgIdx = gcurrDcfIdx;
	gpbMainCb.subStat = PB_DPOF_STAT_ALL_BUSY;

	gpbImgCb.isDpofSet = FALSE;
	gpbImgCb.dpofPrintNum = 0;
	gpbImgCb.isDpofStamp = FALSE;

	/*-- Do DPOF DeleteAll by SP5K */
	appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_ALL);

#if 0 /***** DEBUG ONLY *****/
{
	int i, total;

	sp5kDcfFsFileTypeSet(SP5K_DCF_FILETYPE_JPG);
	total = appPbTotalFileCountGet();
	for (i=1; i<=total; i++) {
		sp5kDcfFsCurFileIdxSet(i);
		gpbImgCb.isDpofSet = appDpofJobExistQuery(APP_DPOF_MODE_STD);
		gpbImgCb.dpofPrintNum = appDpofPrintQuantityGet(APP_DPOF_MODE_STD);
		printf("\n\n~~~~~~~~~~~[%d] DPOF Param : isSet(%d), prnQty(%d), stamp(%d) ...\n\n",
				i, gpbImgCb.isDpofSet, gpbImgCb.dpofPrintNum, gpbImgCb.isDpofStamp);
	}
}
#endif
}

#if 0
/*-------------------------------------------------------------------------
 *  File Name : MSG_JobMenuReturn_Service
 *------------------------------------------------------------------------*/
 
static
void
MSG_JobMenuReturn_Service(
	UINT32 param
)
{
	PB_VERBOSE("%s: param(0x%08x) \n", __FUNCTION__, param);

	#if HOST_DBG
	PB_VERBOSE("\n\n[DPOF] Param (PB) : isSet(%d), prnQty(%d), stamp(%d) ...\n\n",
			gpbImgCb.isDpofSet, gpbImgCb.dpofPrintNum, gpbImgCb.isDpofStamp);
	#endif

	if (param==STATE_PARAM_PB_DPOF_CANCEL) 
	{
		if (gpbUiParam.selDpof==PB_UI_DPOF_PRINT_ALL)
		{
			appPbDpofStExit(APP_STATE_PLAY_SUBMENU, STATE_PARAM_NORMAL_INIT);
		} 
		else
		{
			goto lbl_dpof_menuret_cancel;
		}
	}

	if (gpbUiParam.selDpof==PB_UI_DPOF_PRINT_SINGLE)
	{
		/*-- DPOF SINGLE
		 */
		if (gpbImgCb.dpofPrintNum)
		{
			/*- DPOF set/change */
			if (gpbImgCb.isDpofStamp)
			{
				appPbImgExifDateTimeGet(szCommonBuf);
				//dpofExifDateTimeTextFormat(&gpbImgCb.dcfAttr.time, szCommonBuf);
				appDpofStdJobAdd((UINT32)gpbImgCb.dpofPrintNum, szCommonBuf);
			} 
			else
			{
				appDpofStdJobAdd((UINT32)gpbImgCb.dpofPrintNum, "");
			}
			gpbImgCb.isDpofSet = TRUE;
			dpofDebugDump(1/*isQryDpof*/);
		}
		else 
		{
			/*- DPOF has been clear */
			if (gpbImgCb.isDpofSet)
			{
				appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);
				gpbImgCb.isDpofSet=FALSE;
				gpbImgCb.dpofPrintNum = 0;
				gpbImgCb.isDpofStamp = FALSE;
				dpofDebugDump(1/*isQryDpof*/);
			}
		}
	}
	else if (gpbUiParam.selDpof==PB_UI_DPOF_PRINT_ALL) 
	{
		/*-- DPOF ALL */
		if (gpbImgCb.dpofPrintNum==0)
		{
			/*-- DO RESET ALL !! */
			//appPbDpof_ResetAll();
			PB_OSD_CLR;
			appPbDpofStExit(APP_STATE_PLAY_SUBMENU, STATE_PARAM_NORMAL_INIT);
		}
		else
		{
			appPbDpof_SetAll();
			return;
		}
	} 
	else 
	{
		/*-- Supposedly, it wont happen here !! */
		return;
	}

lbl_dpof_menuret_cancel:
	PB_OSD_CLR;

	DPOF_ConfirmDlgShow();
	gpbMainCb.subStat = PB_DPOF_STAT_CONFIRM_MENU;
}
#endif
/*-------------------------------------------------------------------------
 *  File Name : Key_SET_Service
 *------------------------------------------------------------------------*/
static
void
Key_SET_Service(
	void
)
{
	switch (gpbUiParam.selDpof)
	{
	default:
	case PB_UI_DPOF_PRINT_ALL:
	case PB_UI_DPOF_PRINT_SINGLE:
		if (gpbMainCb.subStat != PB_DPOF_STAT_CONFIRM_MENU)
		{
			PB_VERBOSE("Ignore SET Key, Wrong state !! \n");
			break;
		}
		PB_BEEP;
		if (0==appPlayOsd_DlgBoxSelectGet())/*-- YES */
		{
			gpbMainCb.subStat = PB_DPOF_STAT_JOB_MENU;
			gpbMainCb.lastImgIdx = gcurrDcfIdx;
			appStateChange(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		} 
		else/*-- NO */
		{
			PB_OSD_CLR;
			appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);
			gpbImgCb.isDpofSet=FALSE;
			gpbImgCb.dpofPrintNum = 0;
			gpbImgCb.isDpofStamp = FALSE;
			appPbDpofStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			break;
		}
		break;

	case PB_UI_DPOF_PRINT_RESET:
		if (gpbMainCb.subStat != PB_DPOF_STAT_CONFIRM_MENU)
		{
			PB_VERBOSE("Ignore SET Key, Wrong state !! \n");
			break;
		}
		PB_BEEP;
		if (0==appPlayOsd_DlgBoxSelectGet())/*-- YES */
		{
			appPbDpof_ResetAll();
			sp5kHostMsgSend(APP_UI_MSG_PB_DPOF_EXEC_DONE, 1/*RESET_DPOF*/);
		}
		else/*-- NO */
		{
			PB_OSD_CLR;
			appPbDpofStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPbDpofCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbDpofCreate(
	void
)
{
	/*-- Validity check	 */
	if (FAIL==appPbDpofValidityCheckup())
	{
		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}

	/*-- Do Confirmation, before execute operation */
	switch (gpbUiParam.selDpof)
	{
		case PB_UI_DPOF_PRINT_SINGLE:
			DPOF_ConfirmDlgConfig(ID_STR_SET_THIS_PHOTO_);
			DPOF_ConfirmDlgShow();
			gpbMainCb.subStat = PB_DPOF_STAT_CONFIRM_MENU;
			break;
		case PB_UI_DPOF_PRINT_ALL:
			DPOF_ConfirmDlgConfig(STRTK_PB_SETALLPHOTO);
			DPOF_ConfirmDlgShow();
			gpbMainCb.subStat = PB_DPOF_STAT_CONFIRM_MENU;
			break;
		case PB_UI_DPOF_PRINT_RESET:
			DPOF_ConfirmDlgConfig(STRTK_PB_RESETDPOF);
			DPOF_ConfirmDlgShow();
			gpbMainCb.subStat = PB_DPOF_STAT_CONFIRM_MENU;
			break;
		default:
			break;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDpofStClose
 *------------------------------------------------------------------------*/
static
void
appPbDpofStClose(
	void
)
{
	PB_DBG("PbDPOF: State closing \n");
	gpbMainCb.subStat = PB_DPOF_STAT_NULL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDpofStExit
 *------------------------------------------------------------------------*/
void
appPbDpofStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("PbDpof: State exiting \n");
	gpbMainCb.mainStat = PB_MAIN_STAT_DPOF_SUSPEND;
	appPbDpofStClose();
	appDpofClose(TRUE/*write2File*/);
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDpofStInit
 *------------------------------------------------------------------------*/
static
void
appPbDpofStInit(
	UINT32 param
)
{
	PB_VERBOSE("%s: selDpof(%d) \n", __FUNCTION__, gpbUiParam.selDpof);
	gpbMainCb.subStat = PB_DPOF_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_DPOF_CREATE, 0);
	return;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbDpofStOnMsg
 *------------------------------------------------------------------------*/
void
appPbDpofStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("_OnMsg: 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg) {
	case SP5K_MSG_STILLPLAY_DONE:
		PB_VERBOSE("SP5K_MSG_STILLPLAY_DONE(0x%08x)\n", param);
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR) 
		{
			PB_LOG("!! WARNING !!, PB_DONE ERROR (0x%x) !!\n", tmpU32);
			if (isStExiting)
			{
				PB_OSD_CLR; isStExiting = FALSE;
			}
			/*-- In DPOF case, FILE_ERROR shall be skipped !!
			 */
			PB_BLACK_BKGND;
			break;
		}

		if (isStExiting)
		{
			/*-- leaving DPOF state, because of DPOF_ALL or DPOF_RESET */
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			if (gpbUiParam.selDpof==PB_UI_DPOF_PRINT_ALL)
			{
				appPbDpofStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			} 
			else if (gpbUiParam.selDpof==PB_UI_DPOF_PRINT_RESET)
			{
				appPbDpofStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			}
			return;
		}

		if ( (IS_PB_QV_PLAYING && tmpU32==PB_DONE_QV_FINISH)
			|| (IS_PB_THUMB_PLAYING && tmpU32==PB_DONE_THUMB_FINISH) )
		{
			PB_DBG("THUMB/QV Done !!\n");
			gpbImgCb.isThumbImgOK = TRUE;
			gpbMainCb.isPlayDone = TRUE;
			if (IS_PB_THUMB_PLAYING) 
			{
				appPbJpgMainPlayStart();
			}
		}
		else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH)
		{
			PB_DBG("MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
		}
		break;

	/*-- IMG Shift */
	case APP_UI_MSG_PB_SHIFT_NEXT:
		PB_DBG("APP_UI_MSG_PB_SHIFT_NEXT \n");
		appPbDpofShift(PB_SHIFT_NEXT);
		break;
	case APP_UI_MSG_PB_SHIFT_PRE:
		PB_DBG("APP_UI_MSG_PB_SHIFT_PRE \n");
		appPbDpofShift(PB_SHIFT_PRE);
		break;

	case APP_UI_MSG_PB_DPOF_EXEC_DONE:
		PB_DBG("APP_UI_MSG_PB_DPOF_EXEC_DONE (%d) \n", param);
		/*-- Reset DCF */
		sp5kDcfFsFileTypeSet(APP_PB_FILETYPE_ALL);
		gtotalImgCount = appPbTotalFileCountGet();
		gcurrDcfIdx = gpbMainCb.lastImgIdx;
		/*-- Replay Image */
		if (FAIL==appPbImagePlay(gcurrDcfIdx))
		{
			gpbMainCb.err = PB_ERR_FILE_ERROR;
			gpbImgCb.isFileErr = TRUE;
			//appPbErrorProcess(PB_ERR_FILE_ERROR);
			PB_LOG("!! WARNING !!, FILE ERROR !!\n");
			PB_OSD_CLR; PB_BLACK_BKGND;
			appPbDpofStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		isStExiting=TRUE;
			/*-- Exit till msg : STILL_PLAYBACK_DONE */
		break;
	case APP_UI_MSG_PB_DPOF_CREATE:
		if (FAIL==appPbDpofCreate())
		{
			appPbDpofStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbDpofStOnKey
 *------------------------------------------------------------------------*/
void
appPbDpofStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("_OnKey : [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_DPOF_STAT_CONFIRM_MENU) 
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		}
		break;
	case APP_KEY_PRESS_SET:
		Key_SET_Service();
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		if (gpbUiParam.selDpof==PB_UI_DPOF_PRINT_SINGLE) 
		{
			appPbDpofShift(((msg==APP_KEY_PRESS_RIGHT) ? PB_SHIFT_NEXT : PB_SHIFT_PRE));
		}
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbDpofState
 *------------------------------------------------------------------------*/

#endif

void
appPbDpofState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbDpofState");
 		break;
	case APP_STATE_MSG_CLOSE:
 		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
 		}
		else
		{
 		}
		break;
	}

}

