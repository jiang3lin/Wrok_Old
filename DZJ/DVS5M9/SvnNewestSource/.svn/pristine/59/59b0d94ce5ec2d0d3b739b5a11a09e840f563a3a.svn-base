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
#include "app_osd.h"
#include "app_osd_api.h"

#if FUNC_OPT_TP
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define GUI_CORE_DBG_EN   0
#define GUI_EVT_DBG_EN   0
#define GUI_DBG_LV	2

#if GUI_EVT_DBG_EN
#define guiEvtPrintf printf
#else
#define guiEvtPrintf 
#endif

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/
static guiObj_t guiObj[GUI_OBJ_CTRL_MAX];
static guiObj_t *pObjAct;
static guiObj_t *pObjFree;

static UINT8 sGuiOpMode[GUI_OP_MODE_MAX];
static UINT32 sGuiOpPara[GUI_OP_PARA_MAX];

static UINT32 guiDbgEn; 

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

#if 1
/**
 * @fn        void guiObjRst(guiObj_t *pObj)
 * @brief     guiObjRst
 * @param     [in] pObj
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-27
 * @todo      N/A
 * @bug       N/A
*/
static void
guiObjRst(
	guiObj_t *pObj
)
{
	if (!pObj) return;
	
	pObj->objSts = GUI_OBJ_STS_UNKNOWN;
	pObj->penSts = GUI_PEN_STS_UNKNOWN;
	pObj->fcsSts = GUI_FCS_STS_UNKNOWN;

	memset(&pObj->cusObj, sizeof(guiObjCusCtrl_t), 0);
}

/**
 * @fn        UINT32 guiObjInit(void)
 * @brief     guiObjInit
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-24
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjInit(
	void
)
{
	UINT32 i;
	guiObjEvtCtrl_t *pPenUp, *pAbort;
	
	memset(&guiObj[0], 0, sizeof(guiObj));

	for(i = 0; i < GUI_OBJ_CTRL_MAX - 1; i++)
	{
		guiObj[i].idx = i;
		guiObj[i].pNext = &guiObj[i + 1];
		
		guiObjRst(&guiObj[i]);
	}

	guiObj[GUI_OBJ_CTRL_MAX - 1].idx = GUI_OBJ_CTRL_MAX - 1;
	guiObj[GUI_OBJ_CTRL_MAX - 1].pNext = NULL;
	guiObjRst(&guiObj[GUI_OBJ_CTRL_MAX - 1]);

	pObjAct = NULL;
	pObjFree = &guiObj[0];

	pPenUp = &pObjFree->cusObj.penUp;
	pAbort = &pObjFree->cusObj.abort;
	if ((pAbort - pPenUp + 1) != GUI_OBJ_HDL_FUNC_TOTAL) {
		printf("ERR: gui hdl func cnt overflow, pls chk GUI_OBJ_HDL_FUNC_TOTAL\n");
		while(1);
	}
	
	return SUCCESS;
}

/**
 * @fn        UINT32 guiObjNew(guiObj_t **pObj)
 * @brief     guiObjNew
 * @param     [out] ppObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-30
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjNew(
	guiObj_t **ppObj
)
{
	guiObj_t *pAct;
	
	if (!pObjFree) {
		*ppObj = NULL;
		return FAIL;
	}

	if (!pObjAct) {
		pObjAct = pObjFree;
		pObjFree = pObjFree->pNext;
		pObjAct->pNext = NULL;
		
		*ppObj = pObjAct;
	} else {
		pAct = pObjAct;
		while(pAct->pNext) {
			pAct = pAct->pNext;
		}

		pAct->pNext = pObjFree;
		pAct = pAct->pNext;
		pObjFree = pObjFree->pNext;
		pAct->pNext = NULL;

		*ppObj = pAct;
	}

	return SUCCESS;
}

/**
 * @fn        UINT32 guiObjFree(guiObj_t *pObj)
 * @brief     guiObjFree
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-25
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjFree(
	guiObj_t *pObj
)
{
	guiObj_t *pAct = pObjAct;
	guiObj_t *pActPrev = pObjAct;

	if (!pObj) 
	{
		return SUCCESS;
	}

	if (pAct == pObj)
	{
		pObjAct = pAct->pNext;

		pAct->pNext = pObjFree;
		pObjFree = pAct;
	}
	else
	{
		while(pAct)
		{
			if (pAct == pObj)
			{
				break;
			}

			pActPrev = pAct;
			pAct = pAct->pNext;
		}

		if (!pAct) return FAIL;
			
		pActPrev->pNext = pAct->pNext;
		pAct->pNext = pObjFree;
		pObjFree = pAct;
	}

	/* pObjFree->attr = -1; */
	
	return SUCCESS;
}

/**
 * @fn        UINT32 guiObjLocate(UINT16 x, UINT16 y, guiObj_t **ppObj)
 * @brief     guiObjLocate
 * @param     [in] x
 * @param     [in] y
 * @param     [in] ppObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-28
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjLocate(
	UINT16 x,
	UINT16 y,
	guiObj_t **ppObj
)
{
	guiObj_t *pAct = pObjAct;
	
	if (pAct) 
	{
		do 
		{
			if (x >= pAct->cusObj.x0 && x <= pAct->cusObj.x1 &&\
				y >= pAct->cusObj.y0 && y <= pAct->cusObj.y1 \
			) 
			{
				*ppObj = pAct;
#if GUI_CORE_DBG_EN
				printf("L:%d,%d->%d,%d,%d,%d,%d\n", \
					x, y, pAct->cusObj.x0, pAct->cusObj.y0, pAct->cusObj.x1, pAct->cusObj.y1, pAct->idx);
#endif
				return SUCCESS;
			}

			pAct = pAct->pNext;
		} while(pAct);
	}

	*ppObj = NULL;
	
	return FAIL;
}
#endif
#if 1
/**
 * @fn        UINT32 guiObjCreate(guiObjCusCtrl_t *pObjAttr, guiObj_t **ppObjHdl)
 * @brief     guiObjCreate
 * @param     [in] pObjAttr
 * @param     [in] ppObjHdl
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-27
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjCreate(
	guiObjCusCtrl_t *pObjAttr,
	guiObj_t **ppObjHdl
)
{
	UINT32 ret = SUCCESS;

	ret |= guiObjNew(ppObjHdl);
	if (ret != SUCCESS) {
		return ret;
	}

	(*ppObjHdl)->objSts = GUI_OBJ_STS_ENABLE;
	(*ppObjHdl)->penSts = GUI_PEN_STS_UP;
	(*ppObjHdl)->fcsSts = GUI_FCS_STS_LOST;

	memcpy(&(*ppObjHdl)->cusObj, pObjAttr, sizeof(guiObjCusCtrl_t));
	
	ret |= guiObjDraw(pObjAttr, PAL_YELLOW);
	
	return ret;
}

/**
 * @fn        UINT32 guiObjDestory(guiObj_t *pObj)
 * @brief     guiObjDestory
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjDestory(
	guiObj_t *pObj
)
{
	UINT32 ret = SUCCESS;

	guiObjRst(pObj);
	
	ret |= guiObjFree(pObj);
	if (ret != SUCCESS) 
	{
		return ret;
	}

	ret |= guiObjClr(&pObj->cusObj);

	return ret;
}


/**
 * @fn        UINT32 guiObjOpModeSet(UINT32 modeIdx, UINT32 modeVal)
 * @brief     guiObjOpModeSet
 * @param     [in] modeIdx
 * @param     [in] modeVal
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-7
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjOpModeSet(
	UINT32 modeIdx,
	UINT32 modeVal
)
{
	if (modeIdx < GUI_OP_MODE_MAX) {
		sGuiOpMode[modeIdx] = modeVal;
		return SUCCESS;
	} else {
		return FAIL;;
	}
}
/**
 * @fn        UINT32 guiObjOpModeGet(UINT32 modeIdx)
 * @brief     guiObjOpModeGet
 * @param     [in] modeIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-7
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjOpModeGet(
	UINT32 modeIdx
)
{
	UINT32 modeVal = -1;
	
	if (modeIdx < GUI_OP_MODE_MAX) {
		modeVal = sGuiOpMode[modeIdx];
	}
		
	return modeVal;
}

/**
 * @fn        UINT32 guiObjOpParaSet(UINT32 paraIdx, UINT32 paraVal)
 * @brief     guiObjOpParaSet
 * @param     [in] paraIdx
 * @param     [in] paraVal
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-31
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjOpParaSet(
	UINT32 paraIdx,
	UINT32 paraVal
)
{
	if (paraIdx < GUI_OP_PARA_MAX) {
		sGuiOpPara[paraIdx] = paraVal;
		return SUCCESS;
	} else {
		return FAIL;;
	}
}

/**
 * @fn        UINT32 guiObjOpParaGet(UINT32 paraIdx)
 * @brief     guiObjOpParaGet
 * @param     [in] paraIdx
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-31
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjOpParaGet(
	UINT32 paraIdx
)
{
	UINT32 paraVal = -1;
	
	if (paraIdx < GUI_OP_PARA_MAX) {
		paraVal = sGuiOpPara[paraIdx];
	}

	return paraVal;
}
#endif

#if 0
/**
 * @fn        UINT32 guiObjEvtHdl(SINT16 x, SINT16 y)
 * @brief     guiObjEvtHdl
 * @param     [in] x
 * @param     [in] y
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-2
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjEvtHdl(
	SINT16 x,
	SINT16 y
)
{
	guiObj_t *pObj;
	UINT16 msg;
	guiObj_t *pAct = pGuiObjAct;

#if 0	
	if(guiDbgEn) {
		if (x != -1 && y != -1) {
			osdIndexSave();
			osdPenCfg(0x40 | 0x3c, 2);
			osdRectDraw(x>2?x-2:x, y>2?y-2:y, 4, 4);
			osdIndexRestore();
		}
	}
#endif

	//guiEvtPrintf("X:%i, Y:%i\n", x, y);
#if 0
	if (x != -1 && y != -1) 
	{
		if (!guiPenDnSts)
		 {
			guiPenDnSts = 1;
			guiPenDnFirst=1;
			//printf("D:%i,%i\n", x, y);
			guiSlidePixPdSet(x, y);
			guiObjLocate(x, y, &pGuiObjPenDn);
		}
	}

	if (guiObjLocate(x, y, &pObj) == SUCCESS) 
	{
		/* pen down (idle->active) */
		if (!pAct) 
		{

			/* record the active object */
			pAct = pObj;
			//pGuiObjPenDn = pAct; /* save the pen down object */
			//intf("DN:(%d,%d,)=>(%d,%d),Sts=%d\n",pGuiObjPenDn->x0,pGuiObjPenDn->y0,pGuiObjPenDn->x1,pGuiObjPenDn->y1,guiPenDnSts);
			if (guiPenDnFirst) 
			{
				guiEvtPrintf("pd->");
				//guiPenDnSts = 1;
 				
				/* pen down event */
				if (pAct->pObjExt && pAct->pObjExt->pPenDn) 
				{
					pAct->pObjExt->pPenDn(pAct);
				} 
				else 
				{
					guiEvtPrintf("N");
				}
				guiPenDnFirst=0;
				tpScanProcHdl(TP_PROC_PD);
			} 
			else
			{
				/* pen move the active */
				guiEvtPrintf("G2A->");
			}
			
			/* focus get */
			guiEvtPrintf("act\n");
			if (pAct->pObjExt && pAct->pObjExt->pFocusGet) 
			{
				if (!pAct->attr.focus)
				{	
					pAct->pObjExt->pFocusGet(pAct);
				} 
				else
				{
					guiEvtPrintf("FAG\n");//Focus already get
				}
			}
			else 
			{
				if(guiDbgEn) 
				{
					guiObjDraw(pAct, 0xc0 | 0x10);
				}
				guiEvtPrintf("N");
			}
			pAct->attr.focus = 1;
		}
		else 
		{
			if (pAct == pObj) 
			{
				guiEvtPrintf("r\n");
				/* pen repeate (active->active) */
				/* start the background repeate parsing in time0 ISR */
			}
			else
			{
				guiEvtPrintf("A2A->");		
				/* pen move to another object */
				/* foces lost event handle */
				guiEvtPrintf("lost\n");				
				if (pAct->pObjExt && pAct->pObjExt->pFocusLost)
				{
					if (pAct->attr.focus) 
					{
						pAct->pObjExt->pFocusLost(pAct);
					} 
					else 
					{
						guiEvtPrintf("FNF\n");//focus not found
					}
					tpScanProcHdl(TP_PROC_PD);
				}
				else 
				{
					if(guiDbgEn) 
					{
						guiObjDraw(pAct, 0x40 | 0x3c);
					}
					guiEvtPrintf("N");
				}
				pAct->attr.focus = 0;

				guiEvtPrintf("act\n");
				/* get new obj handle */
				pAct = pObj;
				
				//printf("NEW:(%d,%d,)=>(%d,%d)\n",pAct->x0,pAct->y0,pAct->x1,pAct->y1);
				if (pAct->pObjExt && pAct->pObjExt->pFocusGet) 
				{
					if (!pAct->attr.focus) 
					{	
						pAct->pObjExt->pFocusGet(pAct);
					}
					else
					{
						guiEvtPrintf("FAG\n");//Focus already get
					}
				} 
				else 
				{
					if(guiDbgEn) 
					{
						guiObjDraw(pAct, 0xc0 | 0x10);
					}
					guiEvtPrintf("N");
				}
				pAct->attr.focus = 1;
			}
		}
		
	}
	else
	{
		//printf(" @#$ %d\n",tpScanProcGet());


		/* pen up (Active->idle) */
		if (pAct) 
		{
			guiEvtPrintf("lst\n");
			/* foces lost event handle */
			if (pAct->pObjExt && pAct->pObjExt->pFocusLost)
			{
				if (pAct->attr.focus)
				{
					pAct->pObjExt->pFocusLost(pAct);
				} 
				else
				{
					guiEvtPrintf("FNF\n");//focus not found
				}
			} 
			else 
			{
				if(guiDbgEn)
				{
					guiObjDraw(pAct, 0x40 | 0x3c);
				}
				guiEvtPrintf("N");
			}
			pAct->attr.focus = 0;
			if (tpScanProcGet() & TP_PROC_PU) 
			{
				/* pen up event handle */
				if (guiPenDnSts) 
				{
					guiPenDnSts = 0;

					guiEvtPrintf("pu->");
					if (guiObjOpModeGet(GUI_OP_MODE_PENUP) == MODE_PENUP_ALWAYS \
						|| (pGuiObjPenDn == pAct)) 
					{
						if (pAct->pObjExt && pAct->pObjExt->msg)
						{
							msg = pAct->pObjExt->msg;//avoid pObj->pPenUp modifying msg
						}
						
						if (pAct->pPenUp)
						{
							pAct->pPenUp(pAct); 	
						}
						else
						{
							guiEvtPrintf("N");
						}
						
						if (msg != SP1K_MSG_NULL) 
						{
							//printf("msg=0x%x\n",msg);
							osMsgPost(msg);
						}
					}
				pAct = NULL;
					pGuiObjPenDn = NULL;
					tpScanProcHdl(TP_PROC_PU);
				}		
			} 

		
		}
		else
		{
			/* idle to idle */		
			//guiPenDnSts = 0;
			pGuiObjPenDn= NULL;
			pAct = NULL;

			
		}
	}

	pGuiObjAct = pAct;

	if (x != -1 && y != -1)
	{
		guiSlidePixPuSet(x, y);
		guiObjLocate(x, y, &pGuiObjAct);
	}
	else
	{
		tpScanProcHdl(TP_PROC_PD|TP_PROC_PU);
			guiSlidePdPosClr(-1,-1);
			guiSlideNewPosClr(-1,-1);
		guiPenDnSts = 0;
	}
#endif
	return SUCCESS;
}


void guiObjFisrtPdStsClr(void)
{
	guiFirstPenDn=0;
}
/**
 * @fn        void guiObjActHdlGet(guiObj_t **pObj)
 * @brief     guiObjActHdlGet
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjSlideHdl(
		SINT16 x,
		SINT16 y
)
{

	if (x != -1 && y != -1) 
	{

		if(!guiFirstPenDn){
			guiFirstPenDn=1;
			guiSlidePdPosSet(x, y);
			//printf("D(%d,%d)->\n",x,y);
			guiOldPos.x = guiNewPos.x = x;
			guiOldPos.y = guiNewPos.y = y;
		}
		else
		{
			if ((ABS(x ,  guiNewPos.x)) >= 1 || (ABS(y, guiNewPos.y))>= 1) 
			{
				guiNewPos.x = x;
				guiNewPos.y = y;
				//printf("Nx=%d,y=%d\n",x,y);
			}

			if ((ABS(guiOldPos.x ,	guiNewPos.x)) >= guiObjOpParaGet(GUI_OP_PARA_SWIPE_THD_X) || \
				(ABS(guiOldPos.y, guiNewPos.y))>=guiObjOpParaGet(GUI_OP_PARA_SWIPE_THD_Y)
			) 
			{
//				osMsgPost(SP1K_MSG_GUI_PEN_SLIDE);
				//printf("N(%d,%d)\n",x,y);
				guiLength.x = guiNewPos.x - guiOldPos.x;
				guiLength.y = guiNewPos.y - guiOldPos.y;
				guiOldPos.x = guiNewPos.x = x;
				guiOldPos.y = guiNewPos.y = y;
			}
		
		}
	}
	return SUCCESS;
}
/**
 * @fn        void guiObjActHdlGet(guiObj_t **pObj)
 * @brief     guiObjActHdlGet
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-12-23
 * @todo      N/A
 * @bug       N/A
*/
void
guiObjActHdlGet(
	guiObj_t **pObj
)
{
	*pObj = pGuiObjAct;
}

/**
 * @fn        UINT32 guiSlidePixGet(guiPix_t *pixS, guiPix_t *pixE)
 * @brief     guiSlidePixGet
 * @param     [in] pixS
 * @param     [in] pixE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-1-11
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiSlidePixGet(
	guiPix_t *pixS,
	guiPix_t *pixE
)	
{
	if (pixS) {
		pixS->x = guiPixPd.x;
		pixS->y = guiPixPd.y;
	}
	if (pixE) {
		pixE->x = guiPixPu.x;
		pixE->y = guiPixPu.y;
	}
	return SUCCESS;
}

UINT32 guiCurrenPosGet(guiPix_t *pixS,guiPix_t *pixE)
{
	if (pixS) 
	{
		pixS->x = guiSlidePdPos.x;
		pixS->y = guiSlidePdPos.y;
	}
	
	if (pixE) 
	{
		pixE->x = guiNewPos.x;
		pixE->y = guiNewPos.y;
	}
	return SUCCESS;
}


UINT32 guiMoveLengthGet(guiPix_t *pos)
{
	if(pos)
	{
		pos->x = guiLength.x;
		pos->y = guiLength.y;
	}
	return SUCCESS;
	
}
#endif

#if 1

/**
 * @fn        void guiDbgEnSet(UINT32 en)
 * @brief     guiDbgEnSet
 * @param     [in] en
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-5
 * @todo      N/A
 * @bug       N/A
*/
void
guiDbgEnSet(
	UINT32 en
)
{
	if (en) {
		guiDbgEn = 1;
	} else {
		guiDbgEn = 0;
	}
}

/**
 * @fn        UINT32 guiObjDraw(guiObjCusCtrl_t *pObj, COLOR_DEF color)
 * @brief     guiObjDraw
 * @param     [in] pObj
 * @param     [in] color
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-5
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjDraw(
	guiObjCusCtrl_t *pObj,
	COLOR_DEF color
)
{
	UINT32 ret = SUCCESS;
	
	if (!pObj || !guiDbgEn) return ret;
	
	appOsd_ColorDrawShapeColorSet(color, PAL_BLEND_100, color, PAL_BLEND_100);
	appOsdLib_OsdLayerFrameDraw(pObj->x0, pObj->y0, \
		pObj->x1 - pObj->x0, pObj->y1 - pObj->y0, 1);
	
	return ret;
}

/**
 * @fn        UINT32 guiObjClr(guiObjCusCtrl_t *pObj)
 * @brief     guiObjClr
 * @param     [in] pObj
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-9-1
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiObjClr(
	guiObjCusCtrl_t *pObj
)
{
	return guiObjDraw(pObj, PAL_NOCOLOR);
}

#if GUI_DBG_LV == 2
/**
 * @fn        void guiObjDump(void)
 * @brief     guiObjDump
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2011-8-29
 * @todo      N/A
 * @bug       N/A
*/
void
guiObjDump(
	void
)
{
	UINT32 i;
	guiObj_t *ptr = NULL;

#if 0
	printf("All\n");
	i = 0;
	while(i < APP_GUI_OBJ_MAX) {
		printf("[%d]:%bu\t", i, guiObj[i++]);
		if (!(i&3)) putchar('\n');
	}
#endif

	ptr = pObjAct;
	printf("\nAct\n");
	i = 0;
	while(ptr) {
		printf("[%d]:%d(%d,%d,%d,%d)\t", i++, ptr->idx, \
		ptr->cusObj.x0, ptr->cusObj.y0, ptr->cusObj.x1, ptr->cusObj.y1);
		ptr = ptr->pNext;
		if (!(i&1)) putchar('\n');
	}

	ptr = pObjFree;
	printf("\nFree\n");
	i = 0;
	while(ptr) {
		printf("[%d]:%d\t", i++, ptr->idx);
		ptr = ptr->pNext;
		if (!(i&3)) putchar('\n');
	}
}
#endif /* GUI_DBG_LV == 2 */
#endif

#endif /* FUNC_OPT_TP */

