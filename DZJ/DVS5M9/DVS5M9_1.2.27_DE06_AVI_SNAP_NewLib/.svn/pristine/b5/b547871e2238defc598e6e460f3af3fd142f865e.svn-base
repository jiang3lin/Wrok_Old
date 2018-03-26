/**************************************************************************
 *
 *       Copyright (c) 2006-2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Phil Lin
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

#ifndef _APP_TS_UTIL_H_
#define _APP_TS_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef UINT32 (* pFunc)(void *para);

typedef struct guiObjRect_s {
	SINT16 x0;
	SINT16 y0;
	SINT16 x1;
	SINT16 y1;
} guiObjRect_t;

typedef struct guiMsg_s {
	UINT32 msg;
	UINT32 para;
} guiMsg_t;

/* 
	Note
	The count of message handle should sync with GUI_OBJ_HDL_FUNC_TOTAL
*/
typedef struct guiObjHdlMsg_s {
	guiMsg_t msgPenUp;
	guiMsg_t msgPenDown;
	guiMsg_t msgFcsGet;
	guiMsg_t msgFcsLost;
	guiMsg_t msgFcsKeep;
	guiMsg_t msgAbort;
} guiObjHdlMsg_t;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT32 appGuiObjNew_EvtMsg(guiObjRect_t *pRect, UINT32 msg, UINT32 para);
UINT32 appGuiObjNew_EvtMsgExt(guiObjRect_t *pRect, guiObjHdlMsg_t *pMsg);
UINT32 appGuiObjNew_EvtMsgReg(	SINT16 x0,SINT16 y0,SINT16 w,SINT16 h,UINT32 msg,UINT32 para);
UINT32 appGuiObjNew_EvtMsgRegExt(	SINT16 x0,SINT16 y0,SINT16 w,SINT16 h,guiObjHdlMsg_t *viewMsg);
UINT32 appGuiObjNew_EvtCallBackReg(SINT16 x0,SINT16 y0,SINT16 w,SINT16 h,pFunc  evtFunc,UINT32 para);

#ifdef __cplusplus
}
#endif

#endif /* _APP_TS_UTIL_H_ */

