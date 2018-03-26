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
#ifndef APP_ICATCH_DIQ_LIB_H
#define APP_ICATCH_DIQ_LIB_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_still_iq.h"
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum {
    DIQ_BLOCK_DEFAULT= 0,
    DIQ_BLOCK_USE,
    DIQ_BLOCK_UNUSED,
    CIQ_BLOCK_USE, //custom iq setting
} DIQ_BLOCK_PARAM_SEL;
/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void DynIqTaskInstall(UINT32 en);
UINT32 app_DynIqTask_En_Get(UINT32 sel);
void DynIqTaskInit(void);
void DynIqPauseSet(UINT32 pause);
void DynIqTaskStop(void);

UINT32 app_IQ_TblStatus_All_Set(UINT8 status);
UINT32 app_IQ_BlockSync_En(UINT32 blk5kId, UINT8 status);

void app_IQ_BlockSyn(IQ_SWH_t* piq_swh, UINT8 status);
void app_IQ_BlockSync_Debug(UINT32 imgMode);

void app_CIQ_BlockSyncAll_En(UINT8 status, UINT8 en);

void app_DIQ_DynGamma_Intpl_init(UINT16 IncBrightness, UINT16 DecBrightness, UINT16 SwitchGammaThd);
void app_DIQ_DynGamma_Set(UINT8 sel,UINT32 val);
void app_DIQ_DynWdr_LVSet(UINT8 sel);
void app_DIQ_DynIso_Set(void);
UINT32 app_DIQ_DynWdr_Pause_Get(void);

void app_DIQ_OBIntpl(UINT32 isoIdx, UINT32 IsoIdxtoSvIdx, UINT32 CurSvIdx, UINT16 *IcatchIQOB, UINT8 shift);
#endif  /* APP_ICATCH_AE_LIB_H */

