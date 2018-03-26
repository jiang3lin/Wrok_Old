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
#ifndef APP_PLAYBACK_UIPARAM_H
#define APP_PLAYBACK_UIPARAM_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

#if 0
#include "sp5k_global_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_exif_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_pb_api.h"
#include "sp5k_slide_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"
#include "gpio_custom.h"
#include "app_btn_def.h"
#include "app_dcf.h"
#include "app_disk.h"
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define PB_PROTECT_DEFAULT          PB_UI_PROTECT_SINGLE
#define PB_DELETE_DEFAULT           PB_UI_DELETE_SINGLE
#define PB_SLIDE_DEFAULT            PB_UI_SLIDE_START
#define PB_COPY2CARD_DEFAULT        PB_UI_COPY2CARD_SINGLE
#define PB_EFFECT_DEFAULT           PB_UI_EFFECT_RED_EYE
#define PB_ROTATE_SELECT_DEFAULT    PB_UI_ROTATE_SELECT_SET
#define PB_RESIZE_DEFAULT           PB_UI_RESIZE_9M
#define PB_VOICE_MEMO_DEFAULT       PB_UI_VOICE_MEMO_SET
#define PB_FRAME_DEFAULT            PB_UI_FRAME_1
#define PB_START_IMAGE_DEFAULT      PB_UI_FRAME_1
#define PB_SLSHOW_INTERVAL_DEFAULT  SLSHOW_1SEC
#define PB_SLSHOW_MUSIC_DEFAULT     SLSHOW_NONE
#define PB_SLSHOW_TRANSITION_DEFAULT    SLSHOW_TRANSITION_TYPE1
#define PB_SLSHOW_REPEAT_DEFAULT    SLSHOW_REPEAT_OFF

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*-- Playback UI param
 */
typedef struct {
	UINT8	selImgProtect;
	UINT8	selImgDelete;
	UINT8	selSlideShow;
	UINT8	selDpof;

	UINT8	selSlideInteval;
	UINT8	selSlideMusic;
	UINT8	selSlideTrans;
	UINT8	selSlideRepeat;

	UINT8	selCopy2Card;
	UINT8	selImgEffect;
	UINT8	selImgRotate;
	UINT8	selImgResize;

	UINT8	selVoiceMemo;
	UINT8	selStartImage;
	UINT8	selPhotoframe;
	UINT8	r1;
} pbUIParam_t;


typedef enum {
	PB_UI_PROTECT_SINGLE,
	PB_UI_PROTECT_ALL,
	PB_UI_PROTECT_UNLOCK_ALL,
	PB_UI_PROTECT_TOTAL,
} pbUiProtectSel_t;

typedef enum {
	PB_UI_DELETE_SINGLE,
	PB_UI_DELETE_ALL,
	PB_UI_DELETE_TOTAL,
} pbUiDeleteSel;

typedef enum {
	PB_UI_SLIDE_START,
	PB_UI_SLIDE_INTERVAL,
	PB_UI_SLIDE_MUSIC,
	PB_UI_SLIDE_TRANSITION,
	PB_UI_SLIDE_REPECT,
	PB_UI_SLIDE_TOTAL,
} pbUiSlideShowCfg_t;

typedef enum {
	PB_UI_DPOF_PRINTNUM			= 0x00,
	PB_UI_DPOF_DATESTAMP,			
	PB_UI_DPOF_RETURN,			
}pbUiDpofSel;

typedef enum {
	PB_UI_DPOF_PRINT_SINGLE,
	PB_UI_DPOF_PRINT_ALL,
	PB_UI_DPOF_PRINT_RESET,
	PB_UI_DPOF_PRINT_TOTAL,
} pbUiDpofPrintSel;

typedef enum {
	PB_UI_COPY2CARD_SINGLE,
	PB_UI_COPY2CARD_ALL,
	PB_UI_COPY2CARD_TOTAL,
} pbUiCopy2CardSel_t;

typedef enum {
	PB_UI_EFFECT_RED_EYE,
	PB_UI_EFFECT_BLACK_WHITE,
	PB_UI_EFFECT_SEPIA,
	PB_UI_EFFECT_NEGA,
	PB_UI_EFFECT_MOSAIC,
	PB_UI_EFFECT_SKIN_BEAUTY,
	PB_UI_EFFECT_RED,
	PB_UI_EFFECT_GREEN,
	PB_UI_EFFECT_BLUE,
	PB_UI_EFFECT_TOTAL,
	PB_UI_EFFECT_NONE,
	PB_UI_EFFECT_SAVE,
} pbUiEffectSel_t;


typedef enum {
	PB_UI_ROTATE_SELECT_SET,
	PB_UI_ROTATE_SELECT_SAVE,
	PB_UI_ROTATE_SELECT_BACK,
	PB_UI_ROTATE_SELECT_TOTAL,
} pbUiRotateSel_t;

typedef enum {
	PB_UI_RESIZE_9M,
	PB_UI_RESIZE_8M,
	PB_UI_RESIZE_7M,
	PB_UI_RESIZE_5M,
	PB_UI_RESIZE_3M,
	PB_UI_RESIZE_2M,
	PB_UI_RESIZE_1M,
	PB_UI_RESIZE_VGA,
	PB_UI_RESIZE_TOTAL,
	PB_UI_RESIZE_4M,
	PB_UI_RESIZE_6M,
	PB_UI_RESIZE_10M,
} pbUiResizeSel_t;

typedef enum {
	PB_UI_VOICE_MEMO_SET,
	PB_UI_VOICE_MEMO_DELETE,
	PB_UI_VOICE_MEMO_TOTAL,
} pbUiVoiceMemoSel_t;


typedef enum {
	PB_UI_FRAME_1,
	PB_UI_FRAME_2,
	PB_UI_FRAME_3,
	PB_UI_FRAME_4,
	PB_UI_FRAME_5,
	PB_UI_FRAME_TOTAL,
	PB_UI_FRAME_NONE,
	PB_UI_FRAME_SAVE,
} pbUiPhotoframeSel_t;

typedef enum {
	PB_UI_START_IMAGE_SELECT,
	PB_UI_START_IMAGE_RETURN,
	PB_UI_START_IMAGE_TOTAL,
} pbUiStartImageSel_t;

typedef enum 
{
	SLSHOW_1SEC				= 0x00,
	SLSHOW_2SEC,
	SLSHOW_3SEC,
	SLSHOW_4SEC,
	SLSHOW_5SEC,
	SLSHOW_6SEC,
	SLSHOW_7SEC,
	SLSHOW_8SEC,
	SLSHOW_9SEC,
	SLSHOW_10SEC,
	SLSHOW_INTERVAL_TOTAL,
}SLSHOW_INTERVAL_e;

typedef enum 
{
	SLSHOW_NONE				= 0x00,
	SLSHOW_MUSIC1,
	SLSHOW_MUSIC2,
	SLSHOW_MUSIC3,
	SLSHOW_MUSIC_TOTAL,
}SLSHOW_MUSIC_e;

typedef enum 
{
	SLSHOW_TRANSITION_TYPE1		= 0x00,
	SLSHOW_TRANSITION_TYPE2,
	SLSHOW_TRANSITION_TYPE3,
	SLSHOW_TRANSITION_TYPE4,
	SLSHOW_TRANSITION_TYPE5,
	SLSHOW_TRANSITION_MIX,
	SLSHOW_TRANSITION_TOTAL,
}SLSHOW_TRANSITION_e;

typedef enum 
{
	SLSHOW_REPEAT_OFF		= 0x00,
	SLSHOW_REPEAT_ON,
	SLSHOW_REPEAT_TOTAL,
}SLSHOW_REPEAT_e;

typedef enum 
{
	DPOF_PRINTNUM			= 0x00,
	DPOF_DATESTAMP,			
	DPOF_RETURN,			
}DPOF_SELEC_INDEX_e;


typedef enum 
{
	PB_SLSHOW_STOP 		= 0x00,
	PB_SLSHOW_PLAY,	
	PB_SLSHOW_PAUSE,	
	PB_SLSHOW_TOTAL,	
}PB_SLSHOWSTATES_e;
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

extern pbUIParam_t gpbUiParam;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/


#endif /* APP_PLAYBACK_UIPARAM_H */



