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

#ifndef _APP_TS_INIT_H_
#define _APP_TS_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "customization/app_init.h"
/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef enum tsCalCfgInit_e {
	TS_CAL_CFG_AUTO, /* Load resource file firstly, then default value */
	TS_CAL_CFG_RES,  /* Load resource file */
	TS_CAL_CFG_DEF,  /* Using default value */
	TS_CAL_CFG_MAX,
} tsCalCfgInit_t;

/**
 * \note
 * Quadrant of the origin of coordinates
 */
typedef enum appTouchQuadrant_e {
	APP_TP_ORIGIN_LEFT_TOP		= 0,
	APP_TP_ORIGIN_RIGHT_TOP 	= 1,
	APP_TP_ORIGIN_LEFT_BOTTOM	= 2,
	APP_TP_ORIGIN_RIGHT_BOTTOM	= 3,
} appTouchQuadrant_t;

/**************************************************************************
 *                      E X T E R N A L   R E F E R E N C E
 **************************************************************************/

/**************************************************************************
 *                  F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
#if FUNC_OPT_TP 

UINT32 appTouchScreenInit(void);
UINT32 appTsCalCfgInit(tsCalCfgInit_t initMode, UINT16 dispWidth, UINT16 dispHeight);
void appTsTask(UINT32 para);
void appTsCoordGet(UINT16 *pPosX, UINT16 *pPosY);

#else

#define appTouchScreenInit()
#define appTsCalCfgInit(initMode, dispWidth, dispHeight)
#define appTsTask()
#endif /* FUNC_OPT_TP */

#ifdef __cplusplus
}
#endif

#endif /* _APP_TS_INIT_H_ */

