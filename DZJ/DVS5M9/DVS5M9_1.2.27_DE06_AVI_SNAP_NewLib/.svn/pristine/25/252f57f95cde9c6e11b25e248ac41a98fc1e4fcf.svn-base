/**************************************************************************
 *         Copyright(c) 2007 by Sunplus mMedia  Technology Co., Ltd.       *
 *                                                                         *
 *  This software is copyrighted by and is the property of Sunplus mMedia  *
 *  Technology Co., Ltd. All rights are reserved by Sunplus mMedia Techno- *
 *  logy Co., Ltd. This software may only be used in accordance with the   *
 *  corresponding license agreement. Any unauthorized use, duplication,    *
 *  distribution, or disclosure of this software is expressly forbidden.   *
 *                                                                         *
 *  This Copyright notice "M U S T" not be removed or modified without     *
 *  prior written consent of Sunplus mMedia Technology  Co., Ltd.          *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd. reserves the right to modify this  *
 *  software without notice.                                               *
 *                                                                         *
 *  Sunplus mMedia Technology Co., Ltd.                                    *
 *  19-1, Innovation First Road, Science-Based Industrial Park,            *
 *  Hsin-Chu, Taiwan, R.O.C.                                               *
 *                                                                         *
 **************************************************************************/
#ifndef __APP_MENU_H__
#define __APP_MENU_H__

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#define	MENU_TAB_GET_FOCUS		0
#define	MENU_TAB_LOST_TFOCUS	1
#define	MENU_TAB_DISABLE		2
	
#define	MENU_BODY_GET_FOCUS		3
#define	MENU_BODY_LOST_FOCUS	4
#define	MENU_BODY_DISABLE		5
	
#define	MENU_STR_GET_FOCUS		6
#define	MENU_STR_LOST_FOCUS		7
#define	MENU_STR_DISABLE		8
	
#define	MENU_ICON_GET_FOCUS		9
#define	MENU_ICON_LOST_FOCUS	10
#define	MENU_ICON_DISABLE		11
	
#define	MENU_PROMPT_GET_FOCUS	12
#define	MENU_PROMPT_LOST_FOCUS	13
#define	MENU_PROMPT_DISABLE		14
	
#define	MENU_BAR_GET_FOCUS		15
#define	MENU_BAR_LOST_TFOCUS	16
#define	MENU_BAR_DISABLE		17
	
#define	MENU_DLG_GET_FOCUS		18
	
#define	OSD_COLOR_WHITE		19
#define	OSD_COLOR_BLACK		20
#define	OSD_COLOR_RED		21
#define	OSD_COLOR_YELLOW		22
#define	OSD_COLOR_GREEN		23
#define	OSD_COLOR_BLUE		24
#define	OSD_COLOR_GREY		25
#define	OSD_COLOR_PURPLE		26
#define	OSD_COLOR_PINK		27
#if FUNC_HOST_TOUCH_PANEL
#define menuGuiDraw(x, y, w, h, idx,pFuncPenDn,pFuncPenUp, pFuncFocGet, pFuncFocLost,pParaPenDn,pParaPenUp, pParaPenFocGet, pParaFocLost, message,penRepeateEn) \
{ \
	guiObj_t obj; \
	obj.x0 = (x); \
	obj.y0 = (y); \
	obj.x1 = (x) + (w); \
	obj.y1 = (y) + (h); \
	obj.pPenUp = (pFuncPenUp);\
	appGuiObjDraw(&obj, (idx)); \
	obj.pObjExt->msg = (message);\
	obj.pObjExt->pPenUpPara = (void *)(pParaPenUp);\
	obj.pObjExt->pPenDn = (pFuncPenDn);\
	obj.pObjExt->pFocusGet = pFuncFocGet;\
	obj.pObjExt->pFocusLost = pFuncFocLost;\
	obj.pObjExt->pPenDnPara= (void *)(pParaPenDn);\
	obj.pObjExt->pFocusGetPara = (void *)(pParaPenFocGet);\
	obj.pObjExt->pFocusLostPara = (void *)(pParaFocLost);\
	obj.pObjExt->penRepeateEn = (void *)(penRepeateEn);\
}

#define menuGuiClr(sIdx, eIdx) \
{ \
	UINT8 i; \
	for (i = sIdx; i < eIdx; i++) { \
		appGuiObjClr(i); \
	} \
}
#endif

#if 0 /* TODO */
typedef struct ITEM_SEQUENCE_S{
	UINT8 imageSize[PRV_SIZE_MAX];
	UINT8 videoSize[MOV_SIZE_MAX];
	UINT8 language[LANGUAGE_MAX];

} ITEM_SEQUENCE;
#endif

void appMenu(UINT16 msg);
void appMenu_PosPreset(UINT32 pos);
void appMenu_PosInit(void);
void appMenu_MenuExit(UINT32 param);
/*void menuExit(void);*/
UINT32 shtMenuChk(void);
void appOsdColorSet(UINT8 attr);
void menuDispParaInit(void);
void menuPromptDisp(UINT16 attr);
void menuProcButIconDisp(UINT16 attr,UINT16 x0,UINT16 y0,UINT16 w,UINT16 h,UINT8 objIdx,UINT16 iconIdx,UINT8 funcIdx,UINT16 cnt,UINT16 msg,UINT8 penRepeateEn);

#if HDMI_MENU_BY_JPG
void menuHDMIRsvPreload(void);	
#endif

#endif /*__APP_MENU_H__*/

