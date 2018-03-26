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
#ifndef APP_PLAY_PRIVATE_H
#define APP_PLAY_PRIVATE_H

/* global control for playback private debug at development phase */
#define PB_PRIVATE_DBG_MASTER		1

/* global control for playback LOG at RELEASE phase */
#define PB_PRIVATE_LOG_MASTER		1

/* other dbg control for playback at development phase */
#define PB_PRIVATE_DBG_FUNC_TRACE	0
#define PB_PRIVATE_DBG_VERBOSE		0

#define PB_FILEOP_ASYNC_WORKAROUND	1
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "api/sp5k_os_api.h"
#include "gpio_custom.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_stringid.h"
#include "app_errorosd.h"
#include "app_menu_tab.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define PB_WAIT_DISK_INTERVAL		10 	/* unit: ms */
#define PB_DISK_DPOF_RESERVED		512  /* unit :KByte */

#define PB_LCD_RESOLUTION_X			320
#define PB_LCD_RESOLUTION_Y			240

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	PB_JPG_EDIT_RESIZE,
	PB_JPG_EDIT_EFFECT,
	PB_JPG_EDIT_PHOTOFRAME,
} pbJPGEditSel_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define PB_SLEEP(ms)	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, ms)



typedef enum {
	TMP_UI_CANCEL,
	TMP_UI_RIGHT,
	TMP_UI_LEFT,
	TMP_UI_DOWN,
	TMP_UI_UP,
	TMP_UI_SET,
	TMP_UI_SAVE,
} pbTmpUIKeySelDef_t;

#if PB_PRIVATE_DBG_MASTER
#define PB_TMP_OSD_STR_SIZE		24
extern UINT8 szOsdStr[PB_TMP_OSD_STR_SIZE];
#define PB_OSD_STR(x, y, str, arg...)	\
		do { memset(&szOsdStr[0], 0, PB_TMP_OSD_STR_SIZE); \
			 sprintf(szOsdStr, str, ##arg); \
			 playOsd_TextCenterPrint(x, y, szOsdStr); \
		} while (0)
#define PB_OSD_BLINK(x, y, str, arg...)	\
		do { memset(&szOsdStr[0], 0, PB_TMP_OSD_STR_SIZE); CLEAR_OSD_SRC; \
			 sprintf(szOsdStr, str, ##arg); \
			 /* show */ \
			 appOsd_GfxStringDraw(x, y, SP5K_GFX_ALIGN_CENTER, szOsdStr); \
			 PB_SLEEP(300); \
			 /* clear */ \
			 CLEAR_OSD_SRC; PB_SLEEP(300); \
			 /* show */ \
			 appOsd_GfxStringDraw(x, y, SP5K_GFX_ALIGN_CENTER, szOsdStr); \
			 PB_SLEEP(300); \
			 /* clear */ \
			 CLEAR_OSD_SRC; \
		} while (0)
#else
#define PB_TMP_OSD_STR_SIZE		24
#define PB_OSD_STR(x, y, str, arg...)		PB_DBG_NULL_FUNC
#define PB_OSD_BLINK(x, y, str, arg...)	PB_DBG_NULL_FUNC
#endif

/*----------
 * Playback Debug macros
 */
#define PB_DBG_NULL_FUNC	do {} while(0)

#if PB_PRIVATE_DBG_MASTER
#define playback_printf(str, arg...)	printf(str, ##arg)
#else
#define playback_printf(str, arg...)	PB_DBG_NULL_FUNC
#endif

#if PB_PRIVATE_LOG_MASTER
#define playback_log(str, arg...)	printf(str, ##arg)
#else
#define playback_log(str, arg...)	PB_DBG_NULL_FUNC
#endif

#if PB_PRIVATE_DBG_FUNC_TRACE
#define playback_func_trace(str, arg...)	playback_printf(str, ##arg)
#else
#define playback_func_trace(str, arg...)	PB_DBG_NULL_FUNC
#endif

#if PB_PRIVATE_DBG_VERBOSE
#define playback_verbose(str, arg...)		playback_printf(str, ##arg)
#else
#define playback_verbose(str, arg...)		PB_DBG_NULL_FUNC
#endif

#define PB_DBG(str, arg...)		playback_printf(str, ##arg)
#define PB_LOG(str, arg...)		playback_log(str, ##arg)
#define PB_VERBOSE(str, arg...)	playback_verbose(str, ##arg)
#define PB_NOT_READY	printf("\n@@@@@@@@@@@ %s, %d : not implemented yet !! \n\n", __FUNCTION__, __LINE__)

#define PB_FIN()				playback_func_trace(">>%s\n",__FUNCTION__)
#define PB_FINA(str, arg...)	do { \
										playback_func_trace(">>%s: ",__FUNCTION__); \
										playback_func_trace(str, ##arg); \
										playback_func_trace("\n");	\
									} while(0)
#define PB_FOUT()				playback_func_trace("\t<<%s",__FUNCTION__)

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* APP_PLAY_PRIVATE_H */


