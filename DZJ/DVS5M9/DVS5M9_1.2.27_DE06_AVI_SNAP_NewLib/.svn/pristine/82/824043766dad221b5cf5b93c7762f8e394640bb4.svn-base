/*******************************************************************************
*
*       Copyright (c) 2009 by Sunplus mMedia Inc., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus mMedia
*  Inc., Ltd. All rights are reserved by Sunplus mMedia Inc., Ltd.
*  This software may only be used in accordance with the corresponding
*  license agreement. Any unauthorized use, duplication, distribution,
*  or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus mMedia Inc., Ltd.
*
*  Sunplus mMedia Inc., Ltd. reserves the right to modify this software
*  without notice.
*
*  Sunplus mMedia Inc., Ltd.
*  19-1, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*  File Description:
*
*  Author: Matt Wang
*
*******************************************************************************/
#ifndef _SP5K_PIP_API_H_
#define _SP5K_PIP_API_H_

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"
#include "middleware/pip_def.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/* sp5kPipInitCfgSet(). */
typedef enum {
	SP5K_PIP_INIT_MAIN_FRAME_SIZE = PIP_INIT_CFG_MAIN_FRAME_SIZE,
} sp5kPipInitCfg_e;

/*******************************************************************************
*                              D A T A   T Y P E S
*******************************************************************************/
typedef pipLayerCfg_t  sp5kPipLayerCfg_t;

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* sp5k_pip_init.c */
UINT32 sp5kPipInitCfgSet(UINT32 id, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);
UINT32 sp5kPipInit(void);
UINT32 sp5kPipTerm(void);

/* sp5k_pip_layer.c */
UINT32 sp5kPipLayerCreate(UINT32 layerId, pipLayerCfg_t *pcfg);
UINT32 sp5kPipLayerDelete(UINT32 layerId);

#endif  /* _SP5K_PIP_API_H_ */

