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

#ifndef _APP_TS_GUI_H_
#define _APP_TS_GUI_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "app_pal.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/*
	Define the max number of touch obj
*/ 
#define GUI_OBJ_CTRL_MAX    32

#define GUI_OBJ_STS_DISABLE 0x00
#define GUI_OBJ_STS_ENABLE  0x01
#define GUI_OBJ_STS_UNKNOWN 0xff

#define GUI_PEN_STS_UP      0x00
#define GUI_PEN_STS_DN      0x01
#define GUI_PEN_STS_UNKNOWN 0xff

#define GUI_FCS_STS_LOST    0x00
#define GUI_FCS_STS_GET     0x01
#define GUI_FCS_STS_UNKNOWN 0xff

/*
	NOTE:
	Function developer MUST mind this definition, 
	It define the callback function number in guiObjCusCtrl_t
*/
#define GUI_OBJ_HDL_FUNC_TOTAL 6   /* pen up/down, focus get/lost/keep, abort */

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/

typedef struct guiObjCtrl_s {
	UINT8 en; /* default value is enabled */

	UINT32 (*pf)(void *para); /* call back function */
	void *pfPara; /* function para pass to "pf" */
} guiObjEvtCtrl_t, guiTPCtrl_t;

typedef struct guiObjCusCtrl_s {
/*	UINT8 name[8];*/

	/* Object Position */
	SINT16 x0;
	SINT16 y0;
	SINT16 x1;
	SINT16 y1;
	
	/* Object Event */
	guiObjEvtCtrl_t penUp;   /* Pen up */
	guiObjEvtCtrl_t penDn;   /* Pen down */
	guiObjEvtCtrl_t fcsGet;  /* focus get */
	guiObjEvtCtrl_t fcsLost; /* focus lost */
	guiObjEvtCtrl_t fcsKeep; /* focus keep */
	guiObjEvtCtrl_t abort;   /* abort current */

} guiObjCusCtrl_t;

typedef struct guiObj_s {
	/*
		private control
	*/
	/* Object Attribution */
	UINT16 idx;
	UINT8 objSts; /* object enable / disable */
	UINT8 penSts; /* pen down / object idle */
	UINT8 fcsSts; /* focus get / focus lost */
	
	/* Misc Control */
	struct guiObj_s *pNext;
	void *pExt;

	/*
		public control
	*/
	/* Customized Object */
	guiObjCusCtrl_t cusObj;
	
} guiObj_t;

typedef enum guiSwpDir_e {
	GUI_SWP_DIR_UP,
	GUI_SWP_DIR_DN,
	GUI_SWP_DIR_LEFT,
	GUI_SWP_DIR_RIGHT,
} guiSwpDir_t;

typedef struct guiTPSwipeEvt_s {
	UINT8 en;
	UINT8 bSwpRun;
	
	UINT32 (*pSwpBf)(void *para);   /* before swipe start callback function */
	UINT32 (*pSwpAft)(void *para);  /* after swipe stop callback function */
	UINT32 (*pSwpProc)(void *para); /* swipe process function */
	UINT32 (*pSwpGst)(guiSwpDir_t dir, void *para); /* swipe gesture detection handle function */

	void *pSwpBfPara;    /* function para pass to "pSwpBf" */
	void *pSwpAftPara;   /* function para pass to "pSwpAft" */
	void *pSwpProcPara;  /* function para pass to "pSwpProc" */
	void *pSwpGstPara;   /* function para pass to "pSwpGst" */
} guiTPSwipeEvt_t;

typedef struct guiPix_s {
	UINT16 bPrs;
	UINT16 x;
	UINT16 y;
	UINT16 time;
} guiPix_t;

typedef struct guiTouchPnl_s {
	UINT8 en;  /* touch panel enable */
	UINT8 sts; /* idle / active */
	UINT8 bLongPrsRun;

	guiPix_t pixPrev; /* previous pixel */
	guiObjCusCtrl_t *pObjPrev; /* previous object */

	guiTPCtrl_t penUp;      /* for the whole touch panel pen up */
	guiTPCtrl_t penDn;      /* for the whole touch panel pen down */
	guiTPSwipeEvt_t pnlSwp; /* for panel swipe gesture handle */
} guiTouchPnl_t;


typedef enum guiOpMode_e {
	GUI_OP_MODE_PENDN_ONLY = (unsigned char)0,
	GUI_OP_MODE_PENUP = (unsigned char)1,
	GUI_OP_MODE_SLIDE_EN = (unsigned char)2,
	GUI_OP_MODE_MAX,
} guiOpMode_t;

typedef enum guiOpModePenUp_e {
	MODE_PENUP_BY_PENDN = (unsigned char)0,
	MODE_PENUP_ALWAYS = (unsigned char)1,
} guiOpModePenUp_t;

typedef enum guiOpPara_e {
	GUI_OP_PARA_SWIPE_THD_X = (unsigned char)0,
	GUI_OP_PARA_SWIPE_THD_Y = (unsigned char)1,
	GUI_OP_PARA_MAX,
} guiOpPara_t;

 
/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
UINT32 guiObjInit(void);
UINT32 guiObjNew(guiObj_t **ppObj);
UINT32 guiObjFree(guiObj_t *pObj);
UINT32 guiObjLocate(UINT16 x, UINT16 y, guiObj_t **ppObj);

UINT32 guiObjCreate(guiObjCusCtrl_t *pObjAttr, guiObj_t **ppObjHdl);
UINT32 guiObjDestory(guiObj_t *pObj);
UINT32 guiObjOpModeSet(UINT32 modeIdx, UINT32 modeVal);
UINT32 guiObjOpModeGet(UINT32 modeIdx);
UINT32 guiObjOpParaSet(UINT32 paraIdx, UINT32 paraVal);
UINT32 guiObjOpParaGet(UINT32 paraIdx);

UINT32 guiObjEvtHdl(SINT16 x, SINT16 y);
void guiObjActHdlGet(guiObj_t **pObj);
void guiObjFisrtPdStsClr(void);
UINT32 guiObjSlideHdl(SINT16 x,SINT16 y);
UINT32 guiSlidePixGet(guiPix_t *pixS, guiPix_t *pixE);
UINT32 guiCurrenPosGet(guiPix_t *pixS,guiPix_t *pixE);
UINT32 guiMoveLengthGet(guiPix_t *pos);

void guiDbgEnSet(UINT32 en);
UINT32 guiObjDraw(guiObjCusCtrl_t *pObj, COLOR_DEF color);
UINT32 guiObjClr(guiObjCusCtrl_t *pObj);
void guiObjDump(void);

#ifdef __cplusplus
}
#endif

#endif /* _APP_TS_GUI_H_ */

