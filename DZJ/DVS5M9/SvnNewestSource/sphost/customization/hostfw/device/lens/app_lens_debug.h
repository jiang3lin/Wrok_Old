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

#ifndef APP_LENS_DEBUG_H
#define APP_LENS_DEBUG_H

#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_api.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_lens_private.h"
#include "app_lens_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#if KIT_WITH_LENS
#define _PRIVATE_DEBUG_ENABLE_		1
#else
#define _PRIVATE_DEBUG_ENABLE_		0
#endif
#define _debug_cmdLine_parameters_  0
#define _MAX_DBG_PARAM_BUF_SIZE_  128

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct _privateDebugParam_s
{
  UINT8* pszCmd;
  UINT32 p1;
  UINT32 p2;
  UINT32 p3;
  UINT32 p4;
  UINT8* pszP1;
  UINT8* pszP2;
  UINT8* pszP3;
  UINT8* pszP4;
} _privateDebugParam_t;


typedef struct _rawAverageInfo_s
{
  UINT32 R;
  UINT32 Gr;
  UINT32 B;
  UINT32 Gb;
} _rawAverageInfo_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern _privateDebugParam_t cmdParam;
extern UINT32 cmdLineParam;
extern UINT8 cmdLinePauseEnable;
extern UINT8 cmdLineResume;


/*-- alex_cmd --*/
void cmdParamInit(UINT8* paramIn);
void cmdParamInit(UINT8* paramIn);
void utility_cmd_dispatch(void);

#endif /* APP_LENS_DEBUG_H */
