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
#ifndef APP_ZOOM_API_H
#define APP_ZOOM_API_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_dzoom_api.h"
#include "sp5k_modesw_api.h"
#include "app_lens_api.h"
#include "app_sys_def.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define DZOOM_SEG_MARKER	0xFFFF0000

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*-- Key control during DZOOM & Lens session */
typedef enum appZoomKeyCtrl_s {
	APP_ZOOM_SESSION_START,
	APP_ZOOM_SESSION_END
} appZoomKeyCtrl_t;

/*-- ZOOM direction */
typedef enum appZoomDir_s {
	APP_ZOOM_TELE,
	APP_ZOOM_WIDE
} appZoomDir_t;

/*-- UI mode for DZOOM */
typedef enum appDZoomUiMode_s {
	APP_DZOOM_UI_MODE_STILL_VIEW = (APP_STATE_GROUP_VIEW|SP5K_MODE_STILL_PREVIEW),
	APP_DZOOM_UI_MODE_STILL_SNAP = (APP_STATE_GROUP_VIEW|SP5K_MODE_STILL_SNAP),
	#ifdef NO_VIDEO_PREVIEW_MODE
	APP_DZOOM_UI_MODE_VIDEO_VIEW = (APP_STATE_GROUP_VIDEO|SP5K_MODE_STILL_PREVIEW),
	#else
	APP_DZOOM_UI_MODE_VIDEO_VIEW = (APP_STATE_GROUP_VIDEO|SP5K_MODE_VIDEO_PREVIEW),
	#endif
	APP_DZOOM_UI_MODE_VIDEO_REC  = (APP_STATE_GROUP_VIDEO|SP5K_MODE_VIDEO_RECORD)
} appDZoomUiMode_t;

/*-- DZOOM primary control param block */
typedef struct appDZoomCtrl_s {
	BOOL		isEnabled;
	UINT8		curSeg;
	UINT8		ratioIdx;
	UINT8		revd_u8;
} appDZoomCtrl_t;

/*-- ZOOM Next Mode query */
typedef enum {
	APP_ZOOM_NEXT_LENS,
	APP_ZOOM_NEXT_DZOOM,
	APP_ZOOM_NEXT_UNKNOWN,
} appZoomNextMode_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void appStillDZoomTele(void);
void appStillDZoomWide(void);
void appStillZoomTele(void);
void appStillZoomWide(void);

void appVideoDZoomTele(void);
void appVideoDZoomWide(void);
void appVideoZoomTele(void);
void appVideoZoomWide(void);

void appDZoomReset(UINT32 uiMode);
void appDZoomEnableCtrl(BOOL isEnable);
void appZoomStop(void);
UINT32 appDZoomGetRation(void);
UINT32 appZoomNextModeQuery(UINT8 zoomDir);

#endif  /* APP_ZOOM_API_H */


