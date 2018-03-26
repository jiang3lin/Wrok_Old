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
#include "app_res_def.h"
#include "app_user_setting.h"
#include "sp5k_utility_api.h"
#include "app_menu_tab.h"
#include "app_menu.h"
#include "app_menu_disp.h"
#include "app_ui_para.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define SIMG_KEY_REPEAT_INTERVAL			200 /* ms */

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define SIMG_ConfirmDialogConfig() appPlayOsd_DlgConfirmYesNoConfig(ID_STR_SET_THIS_PHOTO_, 0)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	SIMG_SXN_START,
	SIMG_SXN_END,
} protUIKeyCtrl_t;

typedef enum {
	PB_SIMG_RIGHT,
	PB_SIMG_LEFT,
	//PB_SIMG_UP,
	//PB_SIMG_DOWN,
} pbProtShiftDir_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbStartImgStExit(UINT32 nextSt,	UINT32 stMsg);


/*-------------------------------------------------------------------------
 *  File Name : PROTECT_ConfirmDialogShow
 *------------------------------------------------------------------------*/
static
void
SIMG_ConfirmDialogShow(
	void
)
{
	PB_VERBOSE("%s \n", __FUNCTION__);
	appPlayOsd_DlgBoxShow();
	appPbOsd_AuxNote_OKSelect(TRUE);
	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ModeIconDraw(TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
appPbStartImgValidityCheckup(
	void
)
{
	if (gpbImgCb.isFileErr) 
	{
		PB_VERBOSE("%s: !!ERROR!!, File ERROR !!\n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		return FAIL;
	}

	if (gpbImgCb.fileType != PB_FT_JPG &&
		gpbImgCb.fileType != PB_FT_JPG_MEMO)
	{
		PB_VERBOSE("%s: !!ERROR!!, Not JPG image !!\n", __FUNCTION__);
		gpbMainCb.err = PB_ERR_FILETYPE_ERROR;
		return FAIL;
	}

	gpbMainCb.err = PB_ERR_NONE;
	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgExecute
 *------------------------------------------------------------------------*/
static
UINT32
appPbStartImgExecute(
	void
)
{
	UINT8 *addr;
	UINT32 ret, w, h, lineByte;

	PB_OSD_CLR;
	PB_PIP_CLR;
	appPbMsg_Completed();

	gpbMainCb.asyncOpTmrId = appTimerSet(1000, "PB SIMG");
		/*-- Install a timer to monitor the case which the following
		 * IMGEDIT_SAVE operation got timed out !!*/
		 
	sp5kStillPlayAbort();
	sp5kUtilPbBufGet(&addr,&w,&h,&lineByte);
	ret = sp5kResourceJpgSave(addr,w,h,w,h,RESB_JPG_STARTUPCUSTOM);
	printf("res jpg save(0x%x,%d,%d):%s\n",(UINT32)addr,w,h,RESB_JPG_STARTUPCUSTOM);
	if(ret != SUCCESS){
		printf("[ERR] %s, ret:0x%x\n", __FUNCTION__, ret);
		return FAIL;
	}

	gpbMainCb.subStat = PB_SIMG_STAT_BUSY;
	appPbMsg_Clear();

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbStartImgCreate(
	void
)
{

	/*-- Validity check	 */
	if (FAIL==appPbStartImgValidityCheckup()) 
	{
		appPbErrorProcess(PB_ERR_FILETYPE_ERROR);
		return FAIL;
	}
	
	/*-- Do Confirmation, before execute operation
	 */
	PB_OSD_CLR;
	SIMG_ConfirmDialogConfig();
	SIMG_ConfirmDialogShow();

	gpbMainCb.subStat = PB_SIMG_STAT_CONFIRM_MENU;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgStClose
 *------------------------------------------------------------------------*/
static
void
appPbStartImgStClose(
	void
)
{
	PB_DBG("SIMG: State closing \n");
	gpbMainCb.subStat = PB_SIMG_STAT_NULL;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgStExit
 *------------------------------------------------------------------------*/
static
void
appPbStartImgStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	PB_DBG("SIMG: State exiting \n");
	gpbMainCb.mainStat = PB_MAIN_STAT_START_IMG_SUSPEND;
	appPbStartImgStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbStartImgStInit(
	void
)
{
	PB_VERBOSE("%s ... \n", __FUNCTION__);

	gpbMainCb.subStat = PB_SIMG_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_SIMG_CREATE, 0);
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgStOnMsg
 *------------------------------------------------------------------------*/
void
appPbStartImgStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	PB_VERBOSE("SIMG: (OnMsg) 0x%08x, 0x%08x ... \n", msg, param);

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
			PB_DBG("%s: !! WARNING !!, PB_DONE error(0x%x) !!\n", __FUNCTION__, tmpU32);
			PB_BLACK_BKGND;
				/*-- DLG menu is presented, FILE_ERROR wont show center message!!
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
		} 
		else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH) 
		{
			PB_DBG("MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
		}
		break;

	case SP5K_MSG_IMGEDIT_DONE:
		PB_VERBOSE("SP5K_MSG_IMGEDIT_DONE(0x%08x)\n", param);

		if (param!=SUCCESS)
		{
			printf("SIMG: SAVE IMG FAILED !! \n");
			gpbMainCb.err = PB_ERR_SAVE_IMG_ERROR;
		}
		appTimerClear(&gpbMainCb.asyncOpTmrId); /*-- Clear the TIMEOUT timer for IMGEDIT_SAVE */
		gpbMainCb.err = PB_ERR_NONE;
		appPbStartImgStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		break;

	case SP5K_MSG_TIMER:
		PB_VERBOSE("SP5K_MSG_TIMER(0x%08x)\n", param);

		if (param==gpbMainCb.asyncOpTmrId)
		{
			appTimerClear(&gpbMainCb.asyncOpTmrId); /*-- Clear the TIMEOUT timer for IMGEDIT_SAVE */
			gpbMainCb.err = PB_ERR_SAVE_IMG_ERROR;
			appPbStartImgStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;

	case APP_UI_MSG_PB_SIMG_CREATE:
		if (FAIL==appPbStartImgCreate())
		{
			appPbStartImgStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;
	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgStOnKey
 *------------------------------------------------------------------------*/
void
appPbStartImgStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	uiPara_t* puiPara = appUiParaGet();

	PB_VERBOSE("SIMG : (OnKey) [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_SIMG_STAT_CONFIRM_MENU)
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		}
		break;
	case APP_KEY_PRESS_SET:
		if (gpbMainCb.subStat != PB_SIMG_STAT_CONFIRM_MENU) 
		{
			PB_VERBOSE("SIMG: Ignore SET Key, Wrong state !! \n");
			break;
		}
		PB_BEEP;
		PB_PIP_0_CLR;
		appPlayOsd_DlgBoxClear();
		#if KIT_WITH_HDMI   /*Add by Lation@20130717*/
		if(IS_HDMI_IN)
		{
		#if HDMI_MENU_BY_JPG
			menuHDMIClean();
		#endif
		}
		#endif
		if (0==appPlayOsd_DlgBoxSelectGet()) 
		{
			/*-- YES */
			__FUNC_TRACK__;
			if (FAIL==appPbStartImgExecute()) 
			{
				__FUNC_TRACK__;
				gpbMainCb.err = ERR_MSG_COPY_ERROR; /*for mantis bug 0048009*/
				appPbErrorProcess(gpbMainCb.err);   
				puiPara->OpenPic = POWERON_LOGO_SYS;
				appPbStartImgStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			}
			__FUNC_TRACK__;
		}
		else
		{
			/*-- NO */
			gpbMainCb.err = PB_ERR_NONE;
			appPbStartImgStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
		}
		break;

	case APP_KEY_PRESS_MENU:
		break;

	case APP_KEY_PRESS_LEFT:
		//appPbStartImgShift(PB_SHIFT_PRE);
		break;
	case APP_KEY_PRESS_RIGHT:
		//appPbStartImgShift(PB_SHIFT_NEXT);
		break;

	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbStartImgState
 *------------------------------------------------------------------------*/
void
appPbStartImgState(
	UINT32 msg,
	UINT32 param
)
{
	PB_VERBOSE("%s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) 
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appPbStartImgState");
		appPbStartImgStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appPbStartImgStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg)) 
		{
			appPbStartImgStOnKey(msg,param);
		}
		else
		{
			appPbStartImgStOnMsg(msg,param);
		}
		break;
	}

}
