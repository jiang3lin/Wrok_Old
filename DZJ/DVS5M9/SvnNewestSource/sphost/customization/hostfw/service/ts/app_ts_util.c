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

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "app_com_def.h"
#include "customization/app_init.h"
#include "app_ts_gui.h"
#include "app_ts_util.h"
#include "app_ts_api.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
 
/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static guiMsg_t guiMsgBuff[GUI_OBJ_CTRL_MAX];
static UINT32 menuGuiItem[6]={0,1,2,3,4,5};

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/
extern UINT16 guiObjIdx;
/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

static UINT32
appGuiEvtHdl_sp5kHostMsgSend(
	void *ptr
)
{
	guiMsg_t *pMsg = (guiMsg_t *)ptr;

	return sp5kHostMsgSend(pMsg->msg, pMsg->para);
}

/**
 * @fn        UINT32 appGuiObjNew_EvtMsg(guiObjRect_t *pRect, UINT32 msg, UINT32 para)
 * @brief     appGuiObjNew_EvtMsg
 * @param     [in] pRect
 * @param     [in] msg
 * @param     [in] para
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-29
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appGuiObjNew_EvtMsg(
	guiObjRect_t *pRect,
	UINT32 msg,
	UINT32 para
)
{
	guiObjCusCtrl_t obj = {0};
	guiMsg_t *pMsg = &guiMsgBuff[guiObjIdx];

	if (!pRect||guiObjIdx>GUI_OBJ_CTRL_MAX) return FAIL;
	
	obj.x0 = pRect->x0;
	obj.y0 = pRect->y0;
	obj.x1 = pRect->x1;
	obj.y1 = pRect->y1;
	pMsg->msg = msg;
	pMsg->para = para;
	
	obj.penUp.pf = appGuiEvtHdl_sp5kHostMsgSend;
	obj.penUp.pfPara = pMsg;
/*	HLPrintf1("guiObjIdx:%d", guiObjIdx);	*/
	appGuiObjDraw(&obj, guiObjIdx++);
	
	return SUCCESS;
}

/**
 * @fn        UINT32 appGuiObjNew_EvtMsgExt(guiObjRect_t *pRect, guiObjHdlMsg_t *pMsg)
 * @brief     appGuiObjNew_EvtMsgExt
 * @param     [in] pRect
 * @param     [in] pMsg: MUST point to global or static local variables
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-4
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appGuiObjNew_EvtMsgExt(
	guiObjRect_t *pRect,
	guiObjHdlMsg_t *pMsg
)
{
	guiObjCusCtrl_t obj = {0};
	UINT32 i;
	guiMsg_t *pMsgAct;
	guiObjEvtCtrl_t *pObj;
	
	
	if (!pRect || !pMsg) return FAIL;

	obj.x0 = pRect->x0;
	obj.y0 = pRect->y0;
	obj.x1 = pRect->x1;
	obj.y1 = pRect->y1;

	pMsgAct = &pMsg->msgPenUp;
	pObj = &obj.penUp;
	for (i = 0; i < GUI_OBJ_HDL_FUNC_TOTAL; i++) {
		if (pMsgAct->msg) {
			pObj->pf = appGuiEvtHdl_sp5kHostMsgSend;
			pObj->pfPara = pMsgAct;
		}
		pObj++;
		pMsgAct++;
	}
	
	appGuiObjDraw(&obj, guiObjIdx++);

	return SUCCESS;
}


UINT32
appGuiObjNew_EvtMsgReg(	
	SINT16 x0,
	SINT16 y0,
	SINT16 w,
	SINT16 h,
	UINT32 msg,
	UINT32 para	
)
{
	guiObjRect_t rect = {
		.x0 = x0,
		.y0 = y0,
		.x1 = x0+ w,
		.y1 = y0 + h,
	};
	
	return appGuiObjNew_EvtMsg(&rect, msg, para);
}

UINT32
appGuiObjNew_EvtMsgRegExt(	
	SINT16 x0,
	SINT16 y0,
	SINT16 w,
	SINT16 h,
	guiObjHdlMsg_t *viewMsg
)
{
	guiObjRect_t rect = {
		.x0 = x0,
		.y0 = y0,
		.x1 = x0 + w,
		.y1 = y0 + h,
	};
	/*
	static guiObjHdlMsg_t viewMsg = {
		.msgFcsKeep = {0x00, 0x00}, 
	};
	
	viewMsg.msgFcsKeep.msg = msg;
	viewMsg.msgFcsKeep.para= para;
	*/

	return appGuiObjNew_EvtMsgExt(&rect, viewMsg);
}

UINT32
appGuiObjNew_EvtCallBackReg( 
  SINT16 x0,
  SINT16 y0,
  SINT16 w,
  SINT16 h,
  pFunc  evtFunc,
  UINT32 para 
)
{
  guiObjCusCtrl_t obj = {
    .x0 = x0,
    .y0 = y0,
    .x1 = x0 + w,
    .y1 = y0 + h,
    .penUp.pf = evtFunc,
    .penUp.pfPara = &menuGuiItem[para%(sizeof(menuGuiItem))], /* menu item index 0 */
  };
  return appGuiObjAdd(&obj);
}

