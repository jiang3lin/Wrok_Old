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
#ifndef APP_ERR_MSG_H
#define APP_ERR_MSG_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define ERR_MSG_SHOW_FOREVER ((UINT32)0x80000000)
#define ERR_MSG_SHOW_1SEC  1000
#define ERR_MSG_SHOW_2SEC  2000

 typedef enum {
	ERR_MSG_ONLY  = 0,  
	ERR_MSG_WITH_WARNING_SOUND,
} appErrMsgSoundOption_e;

 typedef enum {
	ERR_MSG_CLEAR = 0,
	ERR_MSG_CARD_LOCKED,
	ERR_MSG_FORMAT_ERRPR,
	ERR_MSG_COMPLETED,
	ERR_MSG_FILE_ERROR,
	ERR_MSG_OUT_OF_BATTERY,
	ERR_MSG_CARD_ERROR,
	ERR_MSG_NO_MEMORY_CARD,
	ERR_MSG_MEMORY_FULL,
	ERR_MSG_MEMORY_NOT_ENOUGH,
	ERR_MSG_NO_IMG,
	ERR_MSG_FILETYPE_ERROR, /* Not_Available_For_This_File */
	ERR_MSG_FILE_LOCKED,
	ERR_MSG_BUSY_WAIT,
	ERR_MSG_IMG_SAVED,
	ERR_MSG_COPY_ERROR,
	ERR_MSG_NOTSUPPORT_C2C,
	ERR_MSG_CARD_REMAIN_SIZE,	/* SC.LIU */
	ERR_MSG_CAPTURE_DONE,
} appErrMsgCaseOption_e;
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appErrMsgWithSound(appErrMsgCaseOption_e errMsg, appErrMsgSoundOption_e errSound,UINT32 showTimeInMs);
void appErrMsgShow(appErrMsgCaseOption_e errMsg, UINT32 showTimeInMs);
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
void appErrMsgTimeISRWithSound(appErrMsgCaseOption_e errMsg,appErrMsgSoundOption_e errSound,UINT32 showTimeInMs);
void _errorMsgDraw(appErrMsgCaseOption_e errMsg,UINT32 show);


#endif  /* APP_ERR_MSG_H*/


