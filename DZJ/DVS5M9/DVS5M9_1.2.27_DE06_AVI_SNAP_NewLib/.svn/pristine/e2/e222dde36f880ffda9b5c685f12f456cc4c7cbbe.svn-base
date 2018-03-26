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
#include "app_errorosd.h"
#include "app_sound.h"
#include "app_error_msg.h"
#include "App_playback_osd.h"
#include "App_view_osd.h"

#if SPCA6330
#include "app_ptp.h"
#endif
#include "app_video.h"

BOOL isClrMsg=0;
BOOL isErrorMsgShowing=0;
ushort gErrMsg;
UINT32 timrHdlr = TIMER_NULL;
extern UINT32 memoryFullEvtFlg;
extern UINT32 appStillRemainNumGet(void);

void
_errorMsgDraw(
	appErrMsgCaseOption_e errMsg,
	UINT32 show
)
{
	switch(errMsg) 
	{
		case ERR_MSG_CLEAR:
			appOsdMessageBoxShow(FALSE,STRTK_NULL);
			break;
		case ERR_MSG_CARD_LOCKED:
			appOsdMessageBoxShow(show,ID_STR_CARD_LOCKED_);
			break;
		case ERR_MSG_FORMAT_ERRPR:
			appOsdMessageBoxShow(show,ID_STR_FORMAT_FAIL_);
			break;	
		case ERR_MSG_COMPLETED:
			appOsdMessageBoxShow(show,ID_STR_COMPLETED);
			break;
		case ERR_MSG_FILE_ERROR:
			appOsdMessageBoxShow(show,ID_STR_FILE_ERROR_);
			break;
		case ERR_MSG_OUT_OF_BATTERY:
			appViewOsd_AllClean();
			sp5kDispDevAttrSet(SP5K_DISP_PIP_ACTIVE, 0, 0, 0, 0);
			appOsdMessageBoxShow(show,ID_STR_IMAGE_SAVED);
			break;
		case ERR_MSG_CARD_ERROR:
			#if ICAT_WIFI && SPCA6330
			appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SDCARD_ERROR, 0, 0);
			#endif
			appOsdMessageBoxShow(show,ID_STR_CARD_ERROR);
			break;
		case ERR_MSG_NO_MEMORY_CARD:
			#if ICAT_WIFI  && SPCA6330
			appStill_PIMA_Send_iCatch_Event(PTP_ICAT_EVENT_SDCARD_OUT, 0, 0);
			#endif
			appOsdMessageBoxShow(show,ID_STR_NO_CARD_);
			break;
		case ERR_MSG_MEMORY_FULL:
			#if ICAT_WIFI  && SPCA6330
			if(IS_CARD_EXIST){
				if(app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_ON || app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_OFF){
					if(appVideoRemainSecGet()==0){
						memoryFullEvtFlg=1;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 0, 0);
					}else if(memoryFullEvtFlg){
						memoryFullEvtFlg=0;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 1, 0);
					}
				}else if(app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_CAMERA){
					if(appStillRemainNumGet()==0){
						memoryFullEvtFlg=1;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 0, 0);
					}else if(memoryFullEvtFlg){
						memoryFullEvtFlg=0;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_SD, 1, 0);
					}				
				}
			}else{
				if(app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_ON || app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_VIDEO_OFF){
					if(appVideoRemainSecGet()==0){
						memoryFullEvtFlg=1;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_NAND, 0, 0);
					}else if(memoryFullEvtFlg){
						memoryFullEvtFlg=0;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_NAND, 1, 0);
					}
				}else if(app_PTP_Get_DscOpMode(MODE_ACTIVE)==PTP_DSCOP_MODE_CAMERA){
					if(appStillRemainNumGet()==0){
						memoryFullEvtFlg=1;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_NAND, 0, 0);
					}else if(memoryFullEvtFlg){
						memoryFullEvtFlg=0;
						appStill_PIMA_Send_Event(PTP_EC_STORE_FULL, SP5K_DRIVE_NAND, 1, 0);
					}				
				}				
			}
			#endif
			appOsdMessageBoxShow(show,ID_STR_MEMORY_FULL);
			break;
		case ERR_MSG_MEMORY_NOT_ENOUGH:  /*add for mantis bug 0047510*/
			appOsdMessageBoxShow(show,ID_STR_MEMORY_NOT_ENOUGH);
			break;	
		case ERR_MSG_NO_IMG:
			appOsdMessageBoxShow(show,ID_STR_NO_FILE);
			break;
		case ERR_MSG_FILETYPE_ERROR:
			appOsdMessageBoxShow(show,ID_STR_NOT_AVAILABLE_FOR_THIS_FILE);
			break;
		case ERR_MSG_FILE_LOCKED:
			appOsdMessageBoxShow(show,ID_STR_FILE_LOCKED_);
			break;
		case ERR_MSG_BUSY_WAIT:
			appOsdMessageBoxShow(show,ID_STR_BUSY___PLEASE_WAIT);
			break;
		case ERR_MSG_IMG_SAVED:
			appOsdMessageBoxShow(show,ID_STR_IMAGE_SAVED);
			break;
		case ERR_MSG_COPY_ERROR:
			appOsdMessageBoxShow(show,ID_STR_COPY_ERROR);
			break;
		case ERR_MSG_NOTSUPPORT_C2C:
			appOsdMessageBoxShow(show,ID_STR_NOT_SUPPORT_C2C);
			break;	
		case ERR_MSG_CARD_REMAIN_SIZE:		/* SC.LIU */
			appOsdMessageBoxShow(show,ID_STR_CAPTURE_NOT_ENOUGH_SIZE);
			break;
		case ERR_MSG_CAPTURE_DONE:
			appOsdMessageBoxShow(show,ID_STR_CAPTURE_DONE);
			break;
		default:
			break;
	}
}

void appErrMsgIsMessageClear(void){
	
	if( isClrMsg) {
		_errorMsgDraw(gErrMsg,FALSE);
		isClrMsg =0;
		isErrorMsgShowing=FALSE;		
	}
}	


void
appErrMsgTimeISRWithSound(
	appErrMsgCaseOption_e errMsg,
	appErrMsgSoundOption_e errSound,
	UINT32 showTimeInMs
)
{
	
	
	if (errMsg==ERR_MSG_CLEAR) {
		_errorMsgDraw(errMsg, FALSE);
		return;
	}

	//draw error msg
	_errorMsgDraw(errMsg,TRUE);
	gErrMsg=errMsg ;
	//play warning sound
	isClrMsg = (showTimeInMs & ERR_MSG_SHOW_FOREVER) ? FALSE : TRUE;
	isErrorMsgShowing=TRUE;
	showTimeInMs &= (~ERR_MSG_SHOW_FOREVER);
	switch(errSound) {
		case ERR_MSG_ONLY:
			timrHdlr=sp5kTimerIsrOneShotReg(0, showTimeInMs,appErrMsgIsMessageClear);
			break;
		case ERR_MSG_WITH_WARNING_SOUND:
			appPreloadSoundPlay(SOUND_ID_WARNING);
			timrHdlr=sp5kTimerIsrOneShotReg(0, showTimeInMs,appErrMsgIsMessageClear);
			break;	
	}

}


void
appErrMsgWithSound(
	appErrMsgCaseOption_e errMsg,
	appErrMsgSoundOption_e errSound,
	UINT32 showTimeInMs
)
{
	BOOL isClrMsg;

	if (errMsg==ERR_MSG_CLEAR)
	{
		_errorMsgDraw(errMsg, FALSE);
		return;
	}

	//draw error msg
	_errorMsgDraw(errMsg,TRUE);
	//play warning sound
	isClrMsg = (showTimeInMs & ERR_MSG_SHOW_FOREVER) ? FALSE : TRUE;
	showTimeInMs &= (~ERR_MSG_SHOW_FOREVER);
	switch(errSound) 
	{
		case ERR_MSG_ONLY:
			appTimeDelayMs(showTimeInMs);
			break;
		case ERR_MSG_WITH_WARNING_SOUND:
			appPreloadSoundPlay(SOUND_ID_WARNING);
			appTimeDelayMs( showTimeInMs);
			break;	
	}
	if(showTimeInMs && isClrMsg) 
	{
		_errorMsgDraw(errMsg,FALSE);
	}
}
	
void appErrMsgShow(
	appErrMsgCaseOption_e errMsg,
	UINT32 showTimeInMs
)
{
	appErrMsgWithSound(errMsg, ERR_MSG_ONLY, showTimeInMs);
}
