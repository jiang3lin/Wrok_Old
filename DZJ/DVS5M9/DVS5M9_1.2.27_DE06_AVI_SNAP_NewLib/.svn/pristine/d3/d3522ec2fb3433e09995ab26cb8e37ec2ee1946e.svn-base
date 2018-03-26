/**************************************************************************
 *
 *       Copyright (c) 2005-2012 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Author: Matt Wang
 *
 **************************************************************************/
#ifndef _SP5K_DISP_API_H_
#define _SP5K_DISP_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/disp_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/** @brief Disp source ID, for sp5kDispMultiDevSwitch(). */
typedef enum _sp5kDispSrcId_e {
	SP5K_DISP_SRC_PRIMARY   = DISP_SRC_PRIMARY,
	SP5K_DISP_SRC_SECONDARY = DISP_SRC_SECONDARY,
} sp5kDispSrcId_e;

/* sp5kDispDevSwicth(), sp5kDispMultiDevSwitch(). */
typedef enum _sp5kDispDevId_e {
	SP5K_DISP_DEV_LCD  = DISP_DEV_LCD,
	SP5K_DISP_DEV_NTSC = DISP_DEV_NTSC,
	SP5K_DISP_DEV_PAL  = DISP_DEV_PAL,
	SP5K_DISP_DEV_HDMI = DISP_DEV_HDMI,
	#if SPCA5330
	SP5K_DISP_DEV_LCD2 = DISP_DEV_LCD2,
	#endif
	SP5K_DISP_DEV_TOT  = DISP_DEV_TOT,
} sp5kDispDevId_e;

/* sp5kDispDevAttrSet(). */
typedef enum _sp5kDispPara_e {
	SP5K_DISP_BG_COLOR            = DISP_BG_COLOR,
	SP5K_DISP_SIGNAL_ACTIVE       = DISP_SIGNAL_ACTIVE,
	SP5K_DISP_BRIGHT              = DISP_BRIGHT,
	SP5K_DISP_CONTRAST            = DISP_CONTRAST,
	SP5K_DISP_HUE                 = DISP_HUE,
	SP5K_DISP_SATURATION          = DISP_SATURATION,
	SP5K_DISP_COLOR_MATRIX        = DISP_COLOR_MATRIX,
	SP5K_DISP_EDGE_ENHANCE        = DISP_EDGE_ENHANCE,
	SP5K_DISP_GLOB_ACTIVE         = DISP_GLOB_ACTIVE,
	SP5K_DISP_IMG_ACTIVE          = DISP_IMG_ACTIVE,
	SP5K_DISP_OSD_ACTIVE          = DISP_OSD_ACTIVE,
	SP5K_DISP_PIP_ACTIVE          = DISP_PIP_ACTIVE,
	SP5K_DISP_IMG_FLIP            = DISP_IMG_FLIP,
	SP5K_DISP_OSD_FLIP            = DISP_OSD_FLIP,
	SP5K_DISP_PIP_FLIP            = DISP_PIP_FLIP,
	SP5K_DISP_IMG_WINDOW          = DISP_IMG_WINDOW,
	SP5K_DISP_OSD_WINDOW          = DISP_OSD_WINDOW,
	SP5K_DISP_PV_IMG_WINDOW_SYNC  = DISP_PV_IMG_WINDOW_SYNC,
	SP5K_DISP_IMG_FRAME_ROI       = DISP_IMG_FRAME_ROI,
	SP5K_DISP_VIRTUAL_WINDOW_SIZE = DISP_VIRTUAL_WINDOW_SIZE,
	SP5K_DISP_PREVIEW_FREEZE      = DISP_PREVIEW_FREEZE,
	SP5K_DISP_ALPHA_SCALE         = DISP_ALPHA_SCALE,
	SP5K_DISP_TV_HUE              = DISP_TV_HUE,
	SP5K_DISP_TV_SATURATION       = DISP_TV_SATURATION,
	SP5K_DISP_TV_SYNC_LEVEL       = DISP_TV_SYNC_LEVEL,
	SP5K_DISP_TV_BLANK_LEVEL      = DISP_TV_BLANK_LEVEL,
	SP5K_DISP_TV_BLACK_LEVEL      = DISP_TV_BLACK_LEVEL,
	SP5K_DISP_DEV_CMD             = DISP_DEV_CMD,
} sp5kDispPara_e;

typedef enum {
	SP5K_DISP_DEV_CMD_POWERON,
	SP5K_DISP_DEV_CMD_SUSPEND,
	SP5K_DISP_DEV_CMD_RESUME,
	SP5K_DISP_DEV_CMD_POWEROFF,
	SP5K_DISP_DEV_CMD_MAX,
} sp5kDispDevCmd_e;

/* sp5kDispCustomCallback(). */
typedef enum _sp5kDispEvent_e {
	SP5K_DISP_BEFORE_POWER_ON  = 0,
	SP5K_DISP_AFTER_POWER_ON   = 1,
	SP5K_DISP_BEFORE_POWER_OFF = 2,
	SP5K_DISP_AFTER_POWER_OFF  = 3,
} sp5kDispEvent_e;

/* sp5kDispColorKeySet(). */
typedef enum _sp5kDispColorKeyId_e {
	SP5K_DISP_COLOR_KEY_RGB565 = 0,
	SP5K_DISP_COLOR_KEY_YUV422 = 1,
} sp5kDispColorKeyId_e;

/* sp5kDispResolutionSet() for composite NTSC TV. */
typedef enum _sp5kDispNtscId_e {
	SP5K_DISP_TV_NTSC_M   = 0,
	SP5K_DISP_TV_NTSC_J   = 1,
	SP5K_DISP_TV_NTSC_443 = 2,
} sp5kDispNtscId_e;

/* sp5kDispResolutionSet() for composite PAL TV. */
typedef enum _sp5kDispPalId_e {
	SP5K_DISP_TV_PAL   = 0,
	SP5K_DISP_TV_PAL_M = 1,
} sp5kDispPalId_e;

#define SP5K_DISP_SRC_TOT  DISP_SRC_TOT
#define SP5K_DISP_HDMI_CHANNEL_TOT  DISP_HDMI_CHANNEL_TOT

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* sp5k_disp_custom.c (in app/customization) */
UINT32 sp5kDispCustomCallback(UINT32 devId, UINT32 eventId);

/* sp5k_disp_dev.c */
UINT32 sp5kDispResolutionSet(UINT32 devId, UINT32 resId);
UINT32 sp5kDispDevSwitchDirect(UINT32 devId);
UINT32 sp5kDispDevSwitch(UINT32 devId);
UINT32 sp5kDispDevAttrSet(UINT32 paraId, SINT32 para1, SINT32 para2, SINT32 para3, SINT32 para4);

/* sp5k_disp_dim.c */
UINT32 sp5kDispDimensionGet(UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kDispPvRegionGet(UINT32 *px, UINT32 *py, UINT32 *pw, UINT32 *ph);

/*sp5k_disp_hdmi.c */
UINT32 sp5kDispHdmiHotPlugDetectGet(UINT32 chnlId);

/* sp5k_disp_multi_dev.c */
UINT32 sp5kDispMultiDevSwitch(UINT32 srcId, UINT32 devId);

/* sp5k_disp_osd.c */
UINT32 sp5kDispBlendTblSet(UINT8 *blendTbl);
UINT32 sp5kDispColorKeySet(UINT32 id, UINT32 para1, UINT32 para2);
UINT32 sp5kDispOsdPaletteSet(UINT16 *paltTbl, UINT32 begIdx, UINT32 len);
UINT32 sp5kDispPipPaletteSet(UINT16 *paltTbl, UINT32 begIdx, UINT32 len);

/* sp5k_disp_spi.c */
UINT32 sp5kDispSpiWrite(UINT32 data, UINT32 bitLen, UINT32 lsbMsb);
UINT32 sp5kDispSpiRead(UINT32 addr, UINT32 bitLen, UINT32 lsbMsb);

#endif  /* _SP5K_DISP_API_H_ */

