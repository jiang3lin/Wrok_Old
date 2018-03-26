/**************************************************************************
 *
 *        Copyright (c) 2006-2011 by Sunplus mMedia Inc., Ltd.
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
#ifndef APP_ICATCH_AWB_H
#define APP_ICATCH_AWB_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_3aalg.h"
#include "app_aaa.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define EVENT_DEFAULT_LV	0x0000
#define EVENT_DAY_LV		0x0001
#define EVENT_DUSK_LV		0x0002
#define EVENT_NIGHT_LV		0x0004
#define EVENT_SWITCH_LV		(EVENT_DAY_LV | EVENT_DUSK_LV | EVENT_NIGHT_LV) 

#define EVENT_IR_OFF		0x0000
#define EVENT_IR_ON			0x0001
#define EVENT_IR_ERROR		0x0002

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
//IR
typedef void (*fp_aeIRCallback_t)(UINT8);

//AE / Dual AE
typedef SINT16 (*fp_aeAPEXCal_t )(void);
typedef appAeCb_t* (*fp_aeappAeCbGet_t )(void);
typedef UINT32 (*fp_PreviewAdjustCb_t)(UINT32);

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
//AE
void appAEALGInit(void);
UINT8 appAEALG_AE_Install(fp_aeappAeCbGet_t paecbfunc, fp_PreviewAdjustCb_t padfunc, fp_aeAPEXCal_t papexfunc);
UINT32 appAEALGLib_WriteAppn(UINT32 addr, UINT32 size);

//IR
void appAEALG_IR_Cbset(fp_aeIRCallback_t pfunc);
void appAEALG_IR_BoundarySet(UINT32 hbd, UINT32 lbd);

//AE Motion Detect
aeInfo_list * appAEALG_Info_Get(void);

//Dual AE
UINT8 appAEALG_DualAE_Install(fp_aeappAeCbGet_t paecbfunc, fp_PreviewAdjustCb_t padfunc, fp_aeAPEXCal_t papexfunc);


/* Function declarations in app_aealg_lib.c */
extern SINT32 appAEALGLib_ResultGet(UINT32 param, aeResult_t *presult);

#endif  /* APP_ICATCH_AWB_H */

