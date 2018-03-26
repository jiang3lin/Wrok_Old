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
#include "app_ui_para.h"


#define PB_C2C_ALL_DO_ONE_BY_ONE	0

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
	C2C_SXN_START,
	C2C_SXN_END,
	C2C_SXN_DO_ALL_START,
	C2C_SXN_DO_ALL_END,
} c2cUIKeyCtrl_t;

typedef enum {
	PB_C2C_RIGHT,
	PB_C2C_LEFT,
} pbC2CShiftDir_t;

typedef struct {
	UINT32	totalImgCnt;
	UINT32	currDcfIdx;
} pbC2CSt_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/*
static UINT32 gc2cKeyIntvl = C2C_KEY_REPEAT_INTERVAL;
static UINT32 gc2cKeyRptCnt = 0;
*/
static pbC2CSt_t gc2cNand;
static pbC2CSt_t gc2cCard;
static UINT32 gc2cPlayScanCnt = 0; /*-- FileError scan count */

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbC2CStExit(UINT32 nextSt,	UINT32 stMsg);
static UINT32 appPbC2CAll(void);

/*-------------------------------------------------------------------------
 *  File Name : c2cUIKeyControl
 *------------------------------------------------------------------------*/
static
void
c2cUIKeyControl(
	UINT8 ctrl
)
{
	PB_VERBOSE("%s: ctrl(%d) \n", __FUNCTION__, ctrl);
	switch (ctrl){
	case C2C_SXN_START:
	case C2C_SXN_DO_ALL_START:
		appBtnDisable(BTN_ALL);
		break;
	case C2C_SXN_END:
	case C2C_SXN_DO_ALL_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
		PB_VERBOSE("MPLAY: undefined key session !!\n");
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : C2C_ConfirmDialogConfig
 *------------------------------------------------------------------------*/
static
void
C2C_ConfirmDialogConfig(
	void
)
{
	UINT32 strId;
	uiPara_t* puiPara = appUiParaGet();

	strId = (puiPara->selCopy2Card==PB_UI_COPY2CARD_SINGLE) ?  ID_STR_COPY_THIS_FILE_ : ID_STR_COPY_ALL_FILES_;

	appPlayOsd_DlgConfirmYesNoConfig(strId, 1);
}

/*-------------------------------------------------------------------------
 *  File Name : C2C_ConfirmDialogShow
 *------------------------------------------------------------------------*/
static
void
C2C_ConfirmDialogShow(
	void
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	PB_VERBOSE("%s \n", __FUNCTION__);
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(TRUE);
	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ModeIconDraw(TRUE);
	if (puiPara->selCopy2Card==PB_UI_COPY2CARD_SINGLE) {
		appPLayOsd_LeftRightArrow(TRUE);
	}

}


/*-------------------------------------------------------------------------
 *  File Name : c2cValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
c2cValidityCheckup(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);

	/*-- My opinion, the following shall be checked
	 * before entering PB_C2C state !!
	 */
	if (!IS_CARD_EXIST) {
		gpbMainCb.err = PB_ERR_CARD_NOT_READY;
		return FAIL;
	} else if (gpbDiskCb.isDiskErr) {
		gpbMainCb.err = PB_ERR_CARD_ERROR;
		return FAIL;
	} else if (gpbDiskCb.isCardLock) {
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
		return FAIL;
	}
	#if 0
	else if (gpbDiskCb.isDcfFull) {
		gpbMainCb.err = PB_ERR_DCF_KEY_FULL;
		return FAIL;
	} else if (gpbDiskCb.isDiskFull) {
		gpbMainCb.err = PB_ERR_DISK_FULL;
		return FAIL;
	}
	#endif
	if(!SP5K_DISK_NAND)
	{
		gpbMainCb.err = PB_ERR_NOSUPPORT_C2C;
		return FAIL;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : c2cAllAsyncPostProc
 *------------------------------------------------------------------------*/
static
void
c2cAllAsyncPostProc(
	BOOL isSuccess
)
{
	PB_DBG("C2C: (all) Async PostProc (%d) \n", isSuccess);

	if (gpbMainCb.subStat != PB_C2C_STAT_ALL_BUSY) {
		/* Not Aysnc C2C_ALL operation, ignore */
		PB_DBG("C2C: (all) not Async OP !!\n");
		return;
	}

	gpbMainCb.subStat = PB_C2C_STAT_STANDBY;

	if (isSuccess) {
		PB_DBG("C2C: (all) OK !!\n");
		gpbMainCb.err = PB_ERR_NONE;
	} else {
		PB_DBG("C2C: (all) NG !!\n");
		gpbMainCb.err = PB_ERR_COPY_ERROR;
		appPbErrorProcess(gpbMainCb.err);
	}

	/*-- Enable Key Control */
	c2cUIKeyControl(C2C_SXN_DO_ALL_END);

}


/*-------------------------------------------------------------------------
 *  File Name : c2cCopySingleCardCheckup
 *------------------------------------------------------------------------*/
UINT32
c2cCopySingleCardCheckup(
	pbImgCb_t *pimgCb
)
{
	sp5kDcfAttrElm_t *pdcfAttr;
	UINT32 freeClust, fileClust;

	PB_VERBOSE("%s: checking %s \n", __FUNCTION__, &pimgCb->szDcfFileName[0]);

	pdcfAttr = &pimgCb->dcfAttr;

	/*-- Check remaining disk (SD) size for new file !!
	 * Dont foget, we are copying from NAND to DISK,
	 * so, we have to check SD instead of active disk (NAND).
	 */
	/*-- Disk (card) remaining clusters */
	freeClust = appPbDiskFreeClusterGet(PB_DISK_CARD);
	PB_DBG("%s: Card free clust (%d) \n", __FUNCTION__, freeClust);

	/*-- Calculate clusters of current image
	 */
	fileClust = appPbBytes2ClusterCalculate(pdcfAttr->filesize);

	if (fileClust > freeClust) return FAIL;

	return SUCCESS;
}
#if 0
/*-------------------------------------------------------------------------
 *  File Name : c2cErrorProcess
 *------------------------------------------------------------------------*/
static
void
c2cErrorProcess(
	UINT32 err
)
{
	PB_VERBOSE("%s: err(%d) \n", __FUNCTION__, err);

	switch (err) {
	default:
		break;
	case PB_ERR_CARD_ERROR:
	case PB_ERR_DISK_FULL:
		appPbErrorProcess(err);
		break;
	}
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CShift
 *------------------------------------------------------------------------*/
static
void
appPbC2CShift(
	UINT8 shiftCtrl
)
{
	UINT32 ret;

	PB_VERBOSE("C2C: SHIFT(%d) \n", shiftCtrl);

	switch (shiftCtrl) {
	case PB_SHIFT_NEXT:
	case PB_SHIFT_PRE:
		if (FAIL==appPbImageShift_PreCheck()) {
			PB_LOG("Shift_PreChk failed \n");
			break;
		}

		do {
			/*-- Skip Error File and play next */
			gc2cPlayScanCnt++;
			ret = appPbImageShift(shiftCtrl);
			if (ret==SUCCESS) {
				PB_BEEP;
				break;
			}
			PB_DBG("%s: File ERROR !!, shift next !! \n", __FUNCTION__);
		} while(gc2cPlayScanCnt<gc2cNand.totalImgCnt);

		PB_VERBOSE("%s: gc2cPlayScanCnt(%d), totalImgCnt(%d) \n", __FUNCTION__, gc2cPlayScanCnt, gc2cNand.totalImgCnt);
		if (gc2cPlayScanCnt>=gc2cNand.totalImgCnt) {
			PB_DBG("%s@%d: Can not find good IMG !!\n", __FUNCTION__, __LINE__);
		}

		gc2cPlayScanCnt=0;
		gc2cNand.currDcfIdx = gcurrDcfIdx;

		/*-- Emulate Key Repeat */
		appPbImageShiftRepeatCheck(0);
		break;

	default:
		PB_VERBOSE("C2C: undefined shiftCtrl(%d) \n", shiftCtrl);
		break;
	}


}

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CSingle
 *------------------------------------------------------------------------*/
static
UINT32
appPbC2CSingle(
	void
)
{
	UINT32 ret;

	PB_VERBOSE("C2C: (single) enter \n");

	/*-- Check remaining disk space !! */
	if (FAIL==c2cCopySingleCardCheckup(&gpbImgCb)) {
		PB_VERBOSE("C2C: (single) Disk Full !!\n");
		gpbMainCb.err = PB_ERR_DISK_FULL;
		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}

	/*-- Abort current IMG Playback !?
	 * DO NOT NEED to abort, because the request will be ignored
	 * at OnKey service.
	 */

	/*-- Go on */
	PB_DBG("\n%s: Copying image !! \n\n", __FUNCTION__);

	/*-- UI : Busy Wait */
	appPbMsg_BusyWait();

	ret = sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_COPY, SP5K_DRIVE_SD, PB_OP_SYNC, 0, 0);
	if (ret == FAIL) {
		PB_DBG("C2C: (Single) Failed !!\n");
		gpbMainCb.err = PB_ERR_COPY_ERROR;
		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}

	sp5kDcfFsCurFileIdxSet(gcurrDcfIdx);

	return SUCCESS;
}


#if PB_C2C_ALL_DO_ONE_BY_ONE
/*-------------------------------------------------------------------------
 *  File Name : appPbC2CAll_DoOneByOne
 *------------------------------------------------------------------------*/
static
UINT32
appPbC2CAll_DoOneByOne(
	void
)
{
	PB_DBG("C2C: (ALL-OneByOne) !!\n");

	return SUCCESS;
}
#endif

#if (!PB_C2C_ALL_DO_ONE_BY_ONE)
/*-------------------------------------------------------------------------
 *  File Name : appPbC2CAll_DoAsyncBatch
 *------------------------------------------------------------------------*/
static
UINT32
appPbC2CAll_DoAsyncBatch(
	void
)
{
	PB_DBG("C2C: (ALL-AsyncBatch) !!\n");

	c2cUIKeyControl(C2C_SXN_DO_ALL_START);

	/*-- Check remaining disk space !?
	 * If using DoAsyncBatch, Host will rely on the SP5KFW to
	 * perform DiskFull checkup during COPYALL operation !!
	 * So, we dont check this situation here.
	 */

	appPbMsg_BusyWait();

	/*-- Go on */
	if (FAIL==sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_COPYALL, SP5K_DRIVE_SD, PB_OP_ASYNC, 0, 0)) {
		PB_LOG("C2C: (all) FILEOP failed !!\n");
		gpbMainCb.err = PB_ERR_DISK_FULL;
		appPbErrorProcess(gpbMainCb.err);
		c2cUIKeyControl(C2C_SXN_DO_ALL_END);

		#if PB_FILEOP_ASYNC_WORKAROUND /* MANTIS BUG : 20302 workaround */
		c2cAllAsyncPostProc(FALSE);
		#endif

		return FAIL;
	}

	gpbMainCb.subStat = PB_C2C_STAT_ALL_BUSY;

	#if PB_FILEOP_ASYNC_WORKAROUND /* MANTIS BUG : 20302 workaround */
	PB_LOG("C2C: (all) OK !!\n");
	c2cAllAsyncPostProc(TRUE);
	#endif

	/* !! For C2C_ALL with Async operation,
	 *    leave the Key disabled till
	 *    FILEOP_COMPLETE msg received.
	 */

	PB_VERBOSE("C2C: (ALL-AsyncBatch) exit !!\n");

	return SUCCESS;
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CAll
 *------------------------------------------------------------------------*/
static
UINT32
appPbC2CAll(
	void
)
{
#if PB_C2C_ALL_DO_ONE_BY_ONE
	return appPbC2CAll_DoOneByOne();
#else
	return appPbC2CAll_DoAsyncBatch();
#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CExecute
 *------------------------------------------------------------------------*/
static
UINT32
appPbC2CExecute(
	void
)
{
	UINT32 ret;
	uiPara_t* puiPara = appUiParaGet();

	PB_VERBOSE("%s: selCopy2Card(%d) \n", __FUNCTION__, puiPara->selCopy2Card);

	ret = FAIL;
	switch (puiPara->selCopy2Card) {
	default:
		break;
	case PB_UI_COPY2CARD_SINGLE:
		PB_DBG("C2C: Single\n");
		c2cUIKeyControl(C2C_SXN_START);
		ret = appPbC2CSingle();
		c2cUIKeyControl(C2C_SXN_END);
		break;
	case PB_UI_COPY2CARD_ALL:
		PB_DBG("C2C: ALL \n");
		ret = appPbC2CAll();
		break;
	}

	if (FAIL==ret) {
		PB_DBG("%s: Failed !!\n", __FUNCTION__);
		return FAIL;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CStCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbC2CStCreate(
	void
)
{
	UINT32 ret;

	PB_VERBOSE("C2C: Create !! \n");

	/*-- Validity check :
	 */
	if (FAIL==c2cValidityCheckup()) {
		#if HOST_DBG
		if (gpbMainCb.err==PB_ERR_CARD_NOT_READY) {
			PB_VERBOSE("%s: Card Not Exist !!\n", __FUNCTION__);
		} else if (gpbMainCb.err==PB_ERR_CARD_ERROR) {
			PB_VERBOSE("%s: Card Error !!\n", __FUNCTION__);
		} else if (gpbMainCb.err==PB_ERR_CARD_LOCKED) {
			PB_VERBOSE("%s: CARD Locked !!\n", __FUNCTION__);
		}
		#endif

		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}

	if (!gpbDiskCb.isNandMounted) {
		PB_DBG("C2C: Mounting NAND !!\n");
		gpbMainCb.err = PB_ERR_NAND_NOT_READY;
		gpbMainCb.subStat = PB_C2C_STAT_NAND_MOUNT_BUSY;
		appPbDiskMount(PB_DISK_NAND);
		return FAIL;
	}

	/*-- Backup current Card info
	 */
	gc2cCard.totalImgCnt = gtotalImgCount;
	gc2cCard.currDcfIdx = gcurrDcfIdx;

	/*-- Swap Active storage to NAND disk
	 */
	if (FAIL==appPbActiveDiskSet(PB_DISK_NAND)) {
		PB_DBG("C2C: !!ERROR!!, can not activate NAND !!");
		gpbMainCb.err = PB_ERR_NAND_ERROR;
		gpbMainCb.subStat = PB_C2C_STAT_STANDBY;
		appPbActiveDiskSet(PB_DISK_CARD);
		return FAIL;
	}

	/*-- Update NAND Info
	 */
	gtotalImgCount = gc2cNand.totalImgCnt = appPbTotalFileCountGet();
	if (gc2cNand.totalImgCnt==0) {
		PB_DBG("C2C: NAND Disk empty !!\n");
		appPbErrorProcess(PB_ERR_NO_IMAGE_SPLASH);
		//APP_OSD_REFRESH_OFF;
		PB_OSD_CLR;
		//PB_BLACK_BKGND;
		//APP_OSD_REFRESH_ON;
		appPbActiveDiskSet(PB_DISK_CARD);
		gcurrDcfIdx = gc2cCard.currDcfIdx;
		gtotalImgCount = gc2cCard.totalImgCnt;
		return FAIL;
	}
	gcurrDcfIdx = gc2cNand.currDcfIdx = gc2cNand.totalImgCnt;

	/*-- Playback current image in NAND (source) disk
	 */
	gc2cPlayScanCnt = 0;
	do {
		/*-- Skip Error File and play next */
		/*s Mask by Aries 09/08/18*/
		/*gc2cPlayScanCnt++;*/
		/*e Mask by Aries 09/08/18*/
		ret = appPbImagePlay(gcurrDcfIdx);
		if (ret==SUCCESS) {
			break;
		}
		/*s Modify by Aries 09/08/18*/
		gc2cPlayScanCnt++;
		/*e Modify by Aries 09/08/18*/
		PB_INDEX_INC(gcurrDcfIdx, 1, gtotalImgCount);
		PB_DBG("%s: File ERROR !!, play next !! \n", __FUNCTION__);
	} while(gc2cPlayScanCnt<gc2cNand.totalImgCnt);

	PB_VERBOSE("%s: gc2cPlayScanCnt(%d), totalImgCnt(%d) \n", __FUNCTION__, gc2cPlayScanCnt, gc2cNand.totalImgCnt);
	if (gc2cPlayScanCnt>=gc2cNand.totalImgCnt) {
		PB_LOG("%s@%d: Can not find good IMG !!\n", __FUNCTION__, __LINE__);
		//gpbMainCb.err = PB_ERR_FILE_ERROR;
		//appPbErr_FileError();
		appPbErrorProcess(PB_ERR_FILE_ERROR);
		return FAIL;
	}

	gc2cPlayScanCnt=0;
	gc2cNand.currDcfIdx = gcurrDcfIdx;

	/*-- Configure Confirm Dialog Box
	 */
	C2C_ConfirmDialogConfig();

	if (FALSE==IS_FILETYPE_JPG(gpbImgCb.dcfAttr.fileType)) {
		/*-- Assume AVI & WAV playback always OK at this moment !!
		 */
		C2C_ConfirmDialogShow();
		gpbMainCb.subStat = PB_C2C_STAT_CONFIRM_MENU;
	}
	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CStClose
 *------------------------------------------------------------------------*/
static
void
appPbC2CStClose(
	void
)
{
	PB_DBG("C2C: State closing \n");
	/*-- Swap Disk to CARD !! */
	if (IS_CARD_EXIST) {
		appPbActiveDiskSet(PB_DISK_CARD);
	}
	gpbMainCb.lastImgIdx = appPbTotalFileCountGet();
	if(gpbMainCb.lastImgIdx!=0)
	{
		gpbMainCb.isNoImage = FALSE;
	}
	if (gc2cNand.totalImgCnt==0)/*yn for mantis0047174 */
	{
		gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
		gpbMainCb.err = PB_ERR_NONE;
	}
	gpbMainCb.subStat = PB_C2C_STAT_NULL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CStExit
 *------------------------------------------------------------------------*/
static
void
appPbC2CStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("C2C: State exiting \n");
	appPbC2CStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbC2CStInit(
	void
)
{
	PB_VERBOSE("C2C: State initing \n");

	gpbMainCb.mainStat = PB_MAIN_STAT_C2C_SUSPEND;
	gpbMainCb.subStat = PB_C2C_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_C2C_CREATE, 0);
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbC2CStOnMsg
 *------------------------------------------------------------------------*/
void
appPbC2CStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("C2C: OnMsg [0x%08x], [0x%08x] ... \n", msg, param);

	switch (msg) {
	#if 0
	case SP5K_MSG_MODE_READY:
		PB_VERBOSE("SP5K_MSG_MODE_READY(0x%08x)\n", param);
		if (param==SP5K_MODE_VIDEO_PLAYBACK &&
			gpbMainCb.playStat==PB_PLAY_STAT_VIDEO_TITLE_PLAYING)
		{
			PB_DBG("C2C: Video TITLE ready !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbImgCb.isThumbImgOK = TRUE;
			gpbMainCb.isPlayDone = TRUE;

			/*-- Draw File Index/Number information */
			appPlayOsd_FileNumDraw(TRUE, gcurrDcfIdx, gtotalImgCount);
		}
		break;
	#endif

	case APP_UI_MSG_PB_VIDEO_TITLE_DONE:
		PB_VERBOSE("APP_UI_MSG_PB_VIDEO_TITLE_DONE(0x%08x)\n", param);
		/*-- Draw File Index/Number information */
		appPlayOsd_FileNumDraw(TRUE, gcurrDcfIdx, gtotalImgCount);
		break;

	case APP_UI_MSG_PB_AUDIO_TITLE_DONE:
		/*-- Draw File Index/Number information */
		appPlayOsd_FileNumDraw(TRUE, gcurrDcfIdx, gtotalImgCount);
		break;

	case SP5K_MSG_STILLPLAY_DONE:
		PB_VERBOSE("SP5K_MSG_STILLPLAY_DONE(0x%08x)\n", param);
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
		/*-- For C2C : We dont copy broken FILE.
		 */
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR) {
			PB_DBG("%s: !! WARNING !!, PB_DONE error(0x%x) !!\n", __FUNCTION__, tmpU32);
			gc2cPlayScanCnt++;
			if (gc2cPlayScanCnt >= gc2cNand.totalImgCnt) {
				//gpbMainCb.err = PB_ERR_FILE_ERROR;
				//appPbErr_FileError();
				appPbErrorProcess(PB_ERR_FILE_ERROR);
				appPbC2CStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}

			PB_INDEX_INC(gc2cNand.currDcfIdx, 1, gc2cNand.totalImgCnt);
			gcurrDcfIdx = gc2cNand.currDcfIdx;
			appPbImagePlay(gcurrDcfIdx);
			break;
		}

		/*-- Draw File Index/Number information */
		appPlayOsd_FileNumDraw(TRUE, gcurrDcfIdx, gtotalImgCount);

		if ( (IS_PB_QV_PLAYING && tmpU32==PB_DONE_QV_FINISH) ||
			 (IS_PB_THUMB_PLAYING && tmpU32==PB_DONE_THUMB_FINISH) )
		{
			PB_VERBOSE("THUMB/QV Done !!\n");
			gpbImgCb.isThumbImgOK = TRUE;
			gpbMainCb.isPlayDone = TRUE;
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;

			/*-- C2C special process !! */
			if (tmpU32==PB_DONE_QV_FINISH) {
				/*-- for c2c, we dont play main if QV done */
				gc2cPlayScanCnt=0;
				C2C_ConfirmDialogShow(); //c2cConfirmDialogShow();
				gpbMainCb.subStat = PB_C2C_STAT_CONFIRM_MENU;
			} else {
				appPbJpgMainPlayStart();
			}
		} else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH) {
			PB_DBG("MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;

			gc2cPlayScanCnt=0;
			C2C_ConfirmDialogShow(); //c2cConfirmDialogShow();
			gpbMainCb.subStat = PB_C2C_STAT_CONFIRM_MENU;
		}
		break;

	case SP5K_MSG_DCF_FILEOP_COMPLETE:
		PB_VERBOSE("C2C: (all) FILEOP OK \n");
		c2cAllAsyncPostProc(TRUE);
		/*jj move to appPbC2CStOnKey for mantis BUG 0046315*/
		/*-- UI : Completed */
		appPbMsg_Completed(); APP_SLEEP_MS(500);
		PB_OSD_CLR;
		appPbC2CStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		#if 0
		APP_OSD_REFRESH_OFF; /*-- Refresh OFF */
		C2C_ConfirmDialogShow();
		APP_OSD_REFRESH_ON; /*-- Refresh ON */
		gpbMainCb.subStat = PB_C2C_STAT_CONFIRM_MENU;
		#endif
		/*jj move to appPbC2CStOnKey for mantis BUG 0046315*/
		break;
	case SP5K_MSG_DCF_FILEOP_FAIL:
		PB_VERBOSE("C2C: (all) FILEOP NG \n");
		c2cAllAsyncPostProc(FALSE);
		if(param == SP5K_ERR_DCF_FSOP_NOSPACE)
		{
			appPbErr_DiskFull();
		}
		PB_OSD_CLR;
		appPbC2CStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);		
		break;

	case APP_UI_MSG_PB_SHIFT_NEXT:
		PB_VERBOSE("C2C: APP_UI_MSG_PB_SHIFT_NEXT \n");
		appPbC2CShift(PB_C2C_RIGHT);
		break;
	case APP_UI_MSG_PB_SHIFT_PRE:
		PB_VERBOSE("C2C: APP_UI_MSG_PB_SHIFT_PRE \n");
		appPbC2CShift(PB_C2C_LEFT);
		break;

	case APP_UI_MSG_PB_C2C_CREATE:
		if (FAIL==appPbC2CStCreate()) {

			if (gpbMainCb.err==PB_ERR_NAND_NOT_READY) {
				return;
			} else if (gpbMainCb.err==PB_ERR_NO_IMAGE) {
				/*  shall playback current IMG in Card to restore IMG layer !! */
				appPbImagePlay(gc2cCard.currDcfIdx);
			}
			else if(gpbMainCb.err==PB_ERR_NOSUPPORT_C2C)
			{
				appPbImagePlay(gcurrDcfIdx);
			}

			PB_DBG("C2C: State creation FAILED !!\n");
			appPbC2CStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;

	case SP5K_MSG_DISK_MOUNT_COMPLETE:
		PB_VERBOSE("C2C: SP5K_MSG_DISK_MOUNT_COMPLETE(0x%08x)\n", param);
		if (gpbMainCb.subStat!=PB_C2C_STAT_NAND_MOUNT_BUSY) {
			return;
		}

		if (param==PB_DISK_NAND) {
			gpbDiskCb.isNandMounted = TRUE;
			gpbMainCb.subStat = PB_C2C_STAT_NULL;

			sp5kHostMsgSend(APP_UI_MSG_PB_C2C_CREATE, 0);
			return;
		}
		break;
	case SP5K_MSG_DISK_MOUNT_FAIL:
		PB_VERBOSE("C2C: SP5K_MSG_DISK_MOUNT_FAIL(0x%08x)\n", param);
		if (gpbMainCb.subStat!=PB_C2C_STAT_NAND_MOUNT_BUSY) {
			return;
		}

		if (param==PB_DISK_NAND) {
			PB_DBG("%s: !!ERROR!!, NAND Disk mount failed !!\n", __FUNCTION__);
			gpbDiskCb.isNandMounted = FAIL;
			gpbMainCb.err = PB_ERR_NAND_NOT_READY;
			appPbC2CStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;

	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbC2CStOnKey
 *------------------------------------------------------------------------*/
void
appPbC2CStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();
	
	PB_VERBOSE("C2C: OnKey [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_C2C_STAT_CONFIRM_MENU) {
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		}
		break;
	case APP_KEY_PRESS_SET:
		PB_VERBOSE("C2C: APP_KEY_PRESS_SET \n");

		if (gpbMainCb.playStat!=PB_PLAY_STAT_STANDBY) {
			PB_VERBOSE("C2C: Ignore SET Key, Playing busy !! \n");
			break;
		}

		if (gpbMainCb.subStat != PB_C2C_STAT_CONFIRM_MENU) {
			PB_VERBOSE("PROT: Ignore SET Key, Wrong state !! \n");
			break;
		}

		//appPlayOsd_DlgBoxClear();
		PB_BEEP;
		if (0==appPlayOsd_DlgBoxSelectGet()) {
			/*-- YES */
			if (FAIL==appPbC2CExecute()) {
				PB_LOG("%s: C2C excute failed !! \n", __FUNCTION__);
				APP_OSD_REFRESH_OFF;/* -- Refresh OFF */
				PB_OSD_CLR;
				C2C_ConfirmDialogShow();
				APP_OSD_REFRESH_ON; /*-- Refresh ON */
				gpbMainCb.subStat = PB_C2C_STAT_CONFIRM_MENU;				//sp5kHostMsgSend(APP_UI_MSG_PB_C2C_ERROR, gpbMainCb.err);
			}
			#if 1/*jj move to appPbC2CStOnKey for mantis BUG 0046315*/
			else {
				/*-- UI : Completed */
				if(puiPara->selCopy2Card==PB_UI_COPY2CARD_SINGLE)
				{
					appPbMsg_Completed(); APP_SLEEP_MS(500);
					PB_OSD_CLR;
					appPbC2CStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
					#if 0
					APP_OSD_REFRESH_OFF;/* -- Refresh OFF */
					C2C_ConfirmDialogShow();
					APP_OSD_REFRESH_ON; /*-- Refresh ON */
					gpbMainCb.subStat = PB_C2C_STAT_CONFIRM_MENU;				//sp5kHostMsgSend(APP_UI_MSG_PB_C2C_ERROR, gpbMainCb.err);
					#endif
				}
			}
			#endif
		} else {
			/*-- NO */
			appPbActiveDiskSet(PB_DISK_CARD);
			gcurrDcfIdx = gc2cCard.currDcfIdx;
			gtotalImgCount = gc2cCard.totalImgCnt;
			PB_OSD_CLR;
			appPbImagePlay(gcurrDcfIdx);
			appPbC2CStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;

	case APP_KEY_PRESS_LEFT:
		PB_VERBOSE("C2C: APP_KEY_PRESS_LEFT \n");
		if (puiPara->selCopy2Card==PB_UI_COPY2CARD_SINGLE &&
			gpbMainCb.subStat==PB_C2C_STAT_CONFIRM_MENU)
		{
			c2cUIKeyControl(C2C_SXN_START);
			appPbC2CShift(PB_SHIFT_PRE);
			c2cUIKeyControl(C2C_SXN_END);
		}
		break;
	case APP_KEY_PRESS_RIGHT:
		PB_VERBOSE("C2C: APP_KEY_PRESS_RIGHT \n");
		if (puiPara->selCopy2Card==PB_UI_COPY2CARD_SINGLE &&
			gpbMainCb.subStat==PB_C2C_STAT_CONFIRM_MENU)
		{
			c2cUIKeyControl(C2C_SXN_START);
			appPbC2CShift(PB_SHIFT_NEXT);
			c2cUIKeyControl(C2C_SXN_END);
		}
		break;

	case APP_KEY_PRESS_MENU:
		if (gpbMainCb.subStat==PB_C2C_STAT_CONFIRM_MENU) {
			/*-- Same as answer NO to ConfirmDialog box !!*/
			PB_BEEP;
			appPlayOsd_DlgBoxClear();
			appPbC2CStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbC2CState
 *------------------------------------------------------------------------*/
void
appPbC2CState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s : [0x%x] [0x%x]\n", __FUNCTION__, msg, param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbC2CState");
		PB_VERBOSE("C2C : APP_STATE_MSG_INIT \n");
		appPbC2CStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		PB_VERBOSE("C2C : APP_STATE_MSG_CLOSE \n");
		appPbC2CStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) {
			appPbC2CStOnKey(msg,param);
		}
		else{
			appPbC2CStOnMsg(msg,param);
		}
		break;
	}

}
