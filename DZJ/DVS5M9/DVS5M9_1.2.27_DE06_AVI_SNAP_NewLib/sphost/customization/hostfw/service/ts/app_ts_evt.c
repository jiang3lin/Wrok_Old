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
#include "app_ts_evt.h"
#include "app_osd.h"

#include "sp5k_gfx_api.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
#define GUI_TP_STS_IDLE 0
#define GUI_TP_STS_ACT  1

#define GUI_TP_PEN_UP       0
#define GUI_TP_PEN_DN       1
#define GUI_TP_PEN_UNKNOWN -1

#define GUI_TPEVT_DEBUG  1
/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/
#define ABS(x,y)  ((x)>(y)) ? ((x)-(y)) : ((y)-(x))
#define MIN(x,y)  ((x)<(y)) ? (x) : (y)
#define MAX(x,y)  ((x)>(y)) ? (x) : (y)

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *                            G L O B A L   D A T A
 **************************************************************************/

static guiTouchPnl_t guiPnl;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
#if 1
/**
 * @fn        void guiTpInit(void)
 * @brief     guiTpInit
 * @param     NONE
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-28
 * @todo      N/A
 * @bug       N/A
*/
void
guiTpInit(
	void
)
{
	guiTouchPnl_t *pPnl = &guiPnl;

	memset(pPnl, 0, sizeof(guiTouchPnl_t));
	
	pPnl->en = 0;
	pPnl->sts = GUI_TP_STS_IDLE;
	pPnl->pixPrev.bPrs = GUI_TP_PEN_UNKNOWN;
}
	
/**
 * @fn        UINT32 guiTpEnGet(void)
 * @brief     guiTpEnGet
 * @param     NONE
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-28
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiTpEnGet(
	void
)
{
	guiTouchPnl_t *pPnl = &guiPnl;

	return pPnl->en;
}

/**
 * @fn        void guiTpEnSet(UINT32 en)
 * @brief     guiTpEnSet
 * @param     [in] en
 * @retval    NONE
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-28
 * @todo      N/A
 * @bug       N/A
*/
void
guiTpEnSet(
	UINT32 en
)
{
	guiTouchPnl_t *pPnl = &guiPnl;

	pPnl->en = en?1:0;
}

/**
 * @fn        UINT32 guiTpSwpChk(guiPix_t *pPixCurr)
 * @brief     guiTpSwpChk
 * @param     [in] pPixCurr
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-29
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
guiTpSwpChk(
	guiPix_t *pPixCurr
)
{
	guiTouchPnl_t *pPnl = &guiPnl;

	if (!pPnl->pnlSwp.en) {
		return FALSE;
	}

	if (pPnl->pnlSwp.bSwpRun) {
		return TRUE;
	} else {
		if (ABS(pPixCurr->x, pPnl->pixPrev.x) >= (UINT16)guiObjOpParaGet(GUI_OP_PARA_SWIPE_THD_X) || \
			ABS(pPixCurr->y, pPnl->pixPrev.y) >= (UINT16)guiObjOpParaGet(GUI_OP_PARA_SWIPE_THD_Y)) {
			return TRUE;
		} else {
			return FALSE;
		}
	}

	return TRUE;
}

/**
 * @fn        UINT32 guiTpLongPrsChk(guiPix_t *pPix)
 * @brief     guiTpLongPrsChk
 * @param     [in] pPix
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-29
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
guiTpLongPrsChk(
	guiPix_t *pPix
)
{
	guiTouchPnl_t *pPnl = &guiPnl;
	
	if (pPnl->bLongPrsRun) {
		return TRUE;
	} else {
		/* long press check */
	}

	return TRUE;
}

/**
 * @fn        UINT32 guiTpEvtHdl_Dn(guiPix_t *pPix)
 * @brief     guiTpEvtHdl_Dn
 * @param     [in] pPix
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-29
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
guiTpEvtHdl_Dn(
	guiPix_t *pPix
)
{
	guiTouchPnl_t *pPnl = &guiPnl;
	guiObjCusCtrl_t *pObj = NULL;
	guiObj_t *pObjGot;
	
	#if	GUI_TPEVT_DEBUG
	printf("hdl_dn\n");/**/
	#endif
	/*
		panel event handle
	*/
	pPnl->sts = GUI_TP_STS_ACT;

	/*
		obj event handle
	*/
	if (guiObjLocate(pPix->x, pPix->y, &pObjGot) != FAIL) {
		pObj = &pObjGot->cusObj;
		/* object pen down call back handle */
		if (pObj->penDn.pf) {
			pObj->penDn.pf(pObj->penDn.pfPara);
		}
		/* object focus get call back handle */
		if (pObj->fcsGet.pf) {
			#if	GUI_TPEVT_DEBUG
			printf("focus get\n");
			#endif
			pObj->fcsGet.pf(pObj->fcsGet.pfPara);
		}
	} else {
		/* pen down to the gap area */
		/* panel pen down event handle */
		if (pPnl->penDn.pf) {
			pPnl->penDn.pf(pPnl->penDn.pfPara);
		}
	}

	/* save current object info include NULL */
	#if	GUI_TPEVT_DEBUG
	printf("dn end\n");	
	#endif
	pPnl->pObjPrev = pObj;
	
	return SUCCESS;
}

/**
 * @fn        UINT32 guiTpEvtHdl_Up(guiPix_t *pPix)
 * @brief     guiTpEvtHdl_Up
 * @param     [in] pPix
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-29
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
guiTpEvtHdl_Up(
	guiPix_t *pPix
)
{
	guiTouchPnl_t *pPnl = &guiPnl;
	guiObjCusCtrl_t *pObj = NULL;
	guiObj_t *pObjGot;

/*printf("hdl_up\n");*/

	/*
		panel event handle
	*/
	pPnl->sts = GUI_TP_STS_IDLE;

	/*
		obj event handle
	*/
	if (guiObjLocate(pPix->x, pPix->y, &pObjGot) != FAIL) {
		pObj = &pObjGot->cusObj;
		if (pObj == pPnl->pObjPrev) {
			/* object focus lost call back handle */
			if (pObj->fcsLost.pf) {
				#if	GUI_TPEVT_DEBUG
				printf("object focus lost\n");
				#endif
				pObj->fcsLost.pf(pObj->fcsLost.pfPara);
			}
			/* object pen up call back handle */
			if (pObj->penUp.pf) {
				#if	GUI_TPEVT_DEBUG
				printf("pen up\n");
				#endif
				pObj->penUp.pf(pObj->penUp.pfPara);
			}
		} else {
			if (pPnl->pObjPrev != NULL) {
				/* object move to another then pen up */
				if(pPnl->pObjPrev->fcsLost.pf) {
					#if	GUI_TPEVT_DEBUG
					printf("object move to another focus lost\n");
					#endif
					pPnl->pObjPrev->fcsLost.pf(pPnl->pObjPrev->fcsLost.pfPara);
				}
			} else {
				/* GAP to object then pen up*/
			}
		}
	} else {
		if (pPnl->pObjPrev != NULL) {
			/* object move to GAP then pen up */
			if(pPnl->pObjPrev->fcsLost.pf) {
				#if	GUI_TPEVT_DEBUG
				printf("object move to GAP focus lost\n");
				#endif
				pPnl->pObjPrev->fcsLost.pf(pPnl->pObjPrev->fcsLost.pfPara);
			}
		} else {
			/* GAP to GAP then pen up */
			/* panel pen up event handle */
			if (pPnl->penUp.pf) {
				#if	GUI_TPEVT_DEBUG
				printf("GAP to GAP pen up\n");
				#endif
				pPnl->penUp.pf(pPnl->penUp.pfPara);
			}
		}
	}

	/* set backup object info to NULL */
	#if	GUI_TPEVT_DEBUG
	printf("set backup object info to NULL\n");
	#endif
	pPnl->pObjPrev = NULL;
	
	return SUCCESS;
}

/**
 * @fn        UINT32 guiTpEvtHdl_Keep(guiPix_t *pPix)
 * @brief     guiTpEvtHdl_Keep
 * @param     [in] pPix
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-9-07
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
guiTpEvtHdl_Keep(
	guiPix_t *pPix
)
{
	guiTouchPnl_t *pPnl = &guiPnl;
	guiObjCusCtrl_t *pObj = NULL;
	guiObj_t *pObjGot;

	/*
		obj event handle
	*/
	if (guiObjLocate(pPix->x, pPix->y, &pObjGot) != FAIL) {
		pObj = &pObjGot->cusObj;
		if (pObj == pPnl->pObjPrev) {
			if (guiTpLongPrsChk(pPix) == TRUE) {
				/* object long press handle */
				if (pObj->fcsKeep.pf) {
					pObj->fcsKeep.pf(pObj->fcsKeep.pfPara);
				}
			} else {
				/* object keep */
			}
		} else {
			if (pPnl->pObjPrev == NULL) {
				/* GAP to Object, the object get focus */
				if (pObj->fcsGet.pf) {
					#if	GUI_TPEVT_DEBUG
					printf("GAP to Object focus get\n");
					#endif
					pObj->fcsGet.pf(pObj->fcsGet.pfPara);
					}
			} else {
				/* Object to Object, first lost then get */
				if (pPnl->pObjPrev->fcsLost.pf) {
					#if	GUI_TPEVT_DEBUG
					printf("Object to Object focus lost\n");
					#endif
					pPnl->pObjPrev->fcsLost.pf(pPnl->pObjPrev->fcsLost.pfPara);
				}
				if (pObj->fcsGet.pf) {
					#if	GUI_TPEVT_DEBUG
					printf("Object to Object focus get\n");
					#endif
					pObj->fcsGet.pf(pObj->fcsGet.pfPara);
				}
			}
		}
	} else {
		if (pPnl->pObjPrev) {
			/* object to GAP, the object lost focus */
			if (pPnl->pObjPrev->fcsLost.pf) {
				#if	GUI_TPEVT_DEBUG
				printf("Object to GAP focus lost\n");
				#endif
				pPnl->pObjPrev->fcsLost.pf(pPnl->pObjPrev->fcsLost.pfPara);
			}
		} else {
			/* pen keep in gap area */
		}
	}

	/* save current object info include NULL */
	pPnl->pObjPrev = pObj;

	return SUCCESS;
}

/**
 * @fn        UINT32 guiTpEvtHdl_Swipe(guiPix_t *pPix)
 * @brief     guiTpEvtHdl_Swipe
 * @param     [in] pPix
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-29
 * @todo      N/A
 * @bug       N/A
*/
static UINT32
guiTpEvtHdl_Swipe(
	guiPix_t *pPix
)
{
	guiTouchPnl_t *pPnl = &guiPnl;

/*printf("hdl_swp\n");*/

	if (pPnl->pObjPrev != NULL) {
		/* ready to swipe, release this object */
		if(pPnl->pObjPrev->abort.pf) {
			pPnl->pObjPrev->abort.pf(pPnl->pObjPrev->abort.pfPara);
		}
		pPnl->pObjPrev = NULL;
	}

	/* swipe process */
	
	return SUCCESS;
}

/**
 * @fn        UINT32 guiTpEvtHdl(guiPix_t *pPix)
 * @brief     guiTpEvtHdl
 * @param     [in] pPix
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-8-29
 * @todo      N/A
 * @bug       N/A
*/
UINT32
guiTpEvtHdl(
	guiPix_t *pPix
)
{
	guiTouchPnl_t *pPnl = &guiPnl;
	
	if (!pPix) return FAIL;

	if (!pPnl->en) {
		printf("TP off\n");
		return SUCCESS;
	}

#if 0
	appOsd_GfxPixelShapeDraw(SP5K_GFX_PAGE_OSD_0, pPix->x, pPix->y);
#endif

	if (pPix->bPrs == GUI_TP_PEN_DN) {
		if (pPnl->sts == GUI_TP_STS_IDLE) {
			/* TP idle -> active */
			guiTpEvtHdl_Dn(pPix);
			#if	GUI_TPEVT_DEBUG
			printf("TP idle -> active\n");
			#endif
		} else { /* pPnl->sts == GUI_TP_STS_ACT */
			/* TP active -> active */
			if (guiTpSwpChk(pPix) == TRUE) {
				/* TP swipe process */
				guiTpEvtHdl_Swipe(pPix);
			} else {
				/* TP keep pen down	*/
				guiTpEvtHdl_Keep(pPix);
			}
		}
	} else { /* pPix->bPrs == GUI_TP_PEN_UP */
		if (pPnl->sts == GUI_TP_STS_ACT) {
			/* TP active -> idle */
			if (guiTpSwpChk(pPix) == TRUE) {
				/* TP swipe process */
				guiTpEvtHdl_Swipe(pPix);
			} else {
				/* TP pen up */
				guiTpEvtHdl_Up(pPix);
			}
		} else { /* pPnl->sts == GUI_TP_STS_IDLE */
			printf("TP idle with pen up?!\n");
		}
	}

	/* save the current pixel */
	memcpy(&pPnl->pixPrev, pPix, sizeof(guiPix_t));

	return SUCCESS;
}

#endif

