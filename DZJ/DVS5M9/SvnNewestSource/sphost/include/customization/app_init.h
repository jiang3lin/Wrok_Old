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
#ifndef _APP_INIT_H_
#define _APP_INIT_H_

#include "customization/solution_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* cmd task init */
#define CMD_TASK_INIT             1

#define CMD_MEM     1
#define CMD_OS      1
#define CMD_DOS     1
#define CMD_DISK    0
#define CMD_ERR     0
#define CMD_MEDIA   1
#define CMD_ISP     0
#define CMD_HW      0
#define CMD_NAND    1

/* vendor command init */
#define VENDOR_CMD_INIT           1
/* usb mass storage driver init */
#define MASS_STORAGE_INIT         1
/* usb still image cleass driver init */
#define STILL_IMAGE_CLASS_INIT    1
/* usb task init */
#define USB_DRIVER_INIT           1
/* USB API test firmware init */
#define API_TEST_INIT             1
/* USB OTG init */
#define ON_THE_GO_DRIVER_INIT     0
/* USB picture bridge init */
#define PICTURE_BRIDGE_INIT       0

/* ---- media ----------------------- */
/* support media codec type define
 *
 * A/V format
 * MJPG  : motion JPEG video
 * PCM   : linear PCM audio
 * ULAW  : mu-law audio
 * ALAW  : a-law audio
 * ADPCM : adaptive DPCM audio
 * MP3   : MPEG-1 audio layer 3
 * WMA   : Microsoft WMA audio
 *
 * container format
 * AVI   : Microsoft AVI file format
 * ASF   : Microsoft ASF file format
 * WAV   : Microsoft WAV audio file format
 */
/*decoders*/
/*decoders*/
#define MEDIA_DEC_MJPG            1
#define MEDIA_DEC_PCM             1
#define MEDIA_DEC_ULAW            1
#define MEDIA_DEC_ALAW            0
#define MEDIA_DEC_ADPCM           0
#define MEDIA_DEC_AAC             0
#define MEDIA_DEC_MP3             0
#define MEDIA_DEC_WMA             0

/*encoders*/
#define MEDIA_ENC_MJPG            1
#define MEDIA_ENC_PCM             1
#define MEDIA_ENC_ULAW            1
#define MEDIA_ENC_ALAW            0
#define MEDIA_ENC_ADPCM           0
#define MEDIA_ENC_AAC             0

/*mux recorders*/
#define MEDIA_RECORD_AVI          1
#define MEDIA_RECORD_ASF          0
#define MEDIA_RECORD_WAV          1
#define MEDIA_RECORD_MP3          0
#define MEDIA_RECORD_MOV          1
#define MEDIA_RECORD_MP4          1

/*demux players*/
#define MEDIA_PLAY_AVI            1
#define MEDIA_PLAY_ASF            0
#define MEDIA_PLAY_WAV            1
#define MEDIA_PLAY_MP3            0
#define MEDIA_PLAY_WMA            0
#define MEDIA_PLAY_MOV            1
#define MEDIA_PLAY_MP4            1


/* -- Sunplus default AE/AWB support -- */
/*if user intends to use the Sunplus default preview AE algorithm
 * at any point of time, including pre-flash AE, define as 1
 */
 #if SP5K_ALGORITHM_AE
#define AE_DEFAULT_SUPPORT 0
#else
#define AE_DEFAULT_SUPPORT 1
#endif
/*if user intends to use the Sunplus default preview AWB algorithm
 * at any point of time, define as 1
 */
#if SP5K_ALGORITHM_AWB
#define AWB_DEFAULT_SUPPORT 0
#else
#define AWB_DEFAULT_SUPPORT 1
#endif
/*s Add by Aries 10/05/12*/
/* Smear reduction support */
#define SMEAR_REDUCT_SUPPORT	0

/* -- playback flow options -- */
#define PB_MAX_COMPRESSION_RATIO    3
/*e Add by Aries 10/05/12*/

/*s video record always support*/
#if CAM_TYPE_CVR
#define VIDEO_REC_ALWAYS 0
#else
#define VIDEO_REC_ALWAYS 0
#endif
/*e video record always support*/


/* -- Sunplus DIST support -- */
#if SPCA5310
#define STILL_DIST_SUPPORT 1
#define VIDEO_DIST_SUPPORT 1
#else
#define STILL_DIST_SUPPORT 0
#define VIDEO_DIST_SUPPORT 0
#endif

/* -- Sunplus face tracking support -- */
#define FACE_TRACK_PREVIEW_SUPPORT   1
#define FACE_TRACK_SNAP_SUPPORT      0
#define FACE_TRACK_PLAYBACK_SUPPORT  1
#define FACE_TRACK_TINY_FACE_DETECT  1
#define FACE_TRACK_SMILE_DETECT      1
#define FACE_TRACK_BLINK_DETECT      1
#define FACE_TRACK_TINY_SMILE_DETECT 1
#define FACE_TRACK_TINY_BLINK_DETECT 1
#define FACE_TRACK_CHECK_FACE_COLOR  1


/* if user intends to use the Sunplus xdenoise, define as 1 */
#define XDENOISE_SUPPORT 0

/* -- custom host UI config -- */
/*if user intends to use evbSimpleJobDo() as host message handler, define as 1 */
#define SIMPLE_UI_FLOW 0


/*define how many messages at most are available for host UI queue*/
#define	HOST_MSG_MAX_SIZE 128

/*define how many messages at most are available for custom APP queue*/
#define	CUSTOMAPP_MSG_MAX_SIZE 128

/*to use separated custom heap in blocks, define as 1*/
#define CUSTOM_HEAP_BLOCK_SUPPORT 1

/*to use separated custom heap in byte pool, define request size*/
#define CUSTOM_HEAP_BYTE_POOL_SIZE (192*1024) /*0: no separated byte pool custom heap*/


/* -- Snap flow options -- */
/* Snap mode */
#if 0 /* original */
#define SNAP_CAPTURE_MODE			1 /* 1:S2S */
/* Snap heap raw and jpeg blocks */
#define SNAP_HEAP_RAW_BLOCKS		1 /* If 0, auto mode */
#define SNAP_HEAP_JPEG_BLOCKS		2 /* If 0, auto mode */
#else
#define SNAP_CAPTURE_MODE			2 /* 2:RGB */
/* Snap heap raw and jpeg blocks */
#define SNAP_HEAP_RAW_BLOCKS		1 /* If 0, auto mode */
#define SNAP_HEAP_JPEG_BLOCKS		1 /* If 0, auto mode */
#endif
/* Snap jpeg max compression ratio */
#define SNAP_MAX_COMPRESSION_RATIO	1
/* Proprietary APP Image Size */
#define APP_QVGA_WIDTH		320
#define APP_QVGA_HEIGHT		240
#define PIC_SIZE_WIDTH_MAX	3640/*3648*//*4608*/
#define PIC_SIZE_HEIGHT_MAX	2048/*2736*//*3456*/


/* slide show effects. */
#define SLIDE_EFFECT_FADE_SUPPORT            1
#define SLIDE_EFFECT_FRAME_DRAG_SUPPORT      1
#define SLIDE_EFFECT_FRAME_SCROLL_SUPPORT    1
#define SLIDE_EFFECT_FRAME_STRETCH_SUPPORT   1
#define SLIDE_EFFECT_STRIP_DRAG_SUPPORT      1
#define SLIDE_EFFECT_DRAPE_DRAG_SUPPORT      1
#define SLIDE_EFFECT_DRAPE_PROGRESS_SUPPORT  1
#define SLIDE_EFFECT_SLOPE_PROGRESS_SUPPORT  1
#define SLIDE_EFFECT_ZIPPER_DRAG_SUPPORT     1
#define SLIDE_EFFECT_ZOOM_FADE_SUPPORT       1
#define SLIDE_EFFECT_DIAGONAL_CUT_SUPPORT    0
#define SLIDE_EFFECT_STRIPE_FADE_SUPPORT     0
#define SLIDE_EFFECT_TWIST_SUPPORT           0
#define SLIDE_EFFECT_SWIRL_DRAG_SUPPORT      0
#define SLIDE_EFFECT_CROSS_SPLIT_SUPPORT     0
#define SLIDE_EFFECT_DOOR_OPEN_SUPPORT       0

/* calibration */
#define CALIB_BP_SUPPORT			1
#define CALIB_LSC_SUPPORT			1

/*
	Touch panel support
*/

#define FUNC_OPT_TP    1
#define FUNC_HOST_TOUCH_PANEL FUNC_OPT_TP
#define PANORAMA_SUPPORT	1
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct rsvSize_s{
	UINT32 size;
}rsvSize_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appRsvFileLoad(UINT32 option,UINT8 *pfilename);

#include "macro_expand.h"

#endif  /* _APP_INIT_H_ */

