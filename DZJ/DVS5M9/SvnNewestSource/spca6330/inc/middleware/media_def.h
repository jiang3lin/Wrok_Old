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
 *  Author:
 *
 **************************************************************************/
#ifndef _MEDIA_DEF_H_
#define _MEDIA_DEF_H_

#include "common.h"

#define MEDIA_DEF_SVN_REVISION	"$Rev: 17730 $"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
#define CUSTOMER_TAG_SUPPORT 1

typedef enum _mediaCallbackId_e {
	MEDIA_CALLBACK_REC_START = 0,
	MEDIA_CALLBACK_REC_STOP  = 1,
	MEDIA_CALLBACK_PLAY_STOP = 2,
	MEDIA_CALLBACK_SEAMLESS  = 3,
	MEDIA_CALLBACK_TOT,
} mediaCallbackId_e;

typedef enum _mediaControl_e {
	MEDIA_PLAY_START         = 0,
	MEDIA_PLAY_ABORT         = 1,
	MEDIA_PLAY_PAUSE         = 2,
	MEDIA_PLAY_RESUME        = 3,
	MEDIA_PLAY_SEEK          = 4,
	MEDIA_PLAY_FORWARD       = 5,
	MEDIA_PLAY_REWIND        = 6,
	MEDIA_PLAY_SLOW_MOTION   = 7,
	MEDIA_PLAY_FRAME_CAPTURE = 8,
	MEDIA_PLAY_TRIM_START    = 9,
	MEDIA_PLAY_RESOURCE_AVI  = 10,/* @startAV */
	MEDIA_PLAY_FAST_TRIM     = 11,
	MEDIA_PLAY_FAST_MERGE    = 12,
	MEDIA_PLAY_SEAMLESS_EN   = 13,
	MEDIA_PLAY_SEAMLESS_LOAD = 14,
	MEDIA_PLAY_VIDEO_FRM_APPEND = 15,
} mediaControl_e;

typedef enum _mediaEdit_e {
	MEDIA_EDIT_TRIM = 0,
} mediaEdit_e;

typedef enum _mediaType_e {
    MEDIA_WAV = 0x00,
    MEDIA_MP3 = 0x01,
    MEDIA_AVI = 0x02,
    MEDIA_ASF = 0x03,
    MEDIA_WMA = 0x04,
    MEDIA_AAC = 0x05,
    MEDIA_MOV = 0x06,
    MEDIA_MP4 = 0x07,
    MEDIA_AMR = 0x08
} mediaType_e;

typedef enum _entropyType_e {
    ENTROPY_CABAC = 0x0,
    ENTROPY_CAVLC = 0x1
} entropyType_e;

typedef enum _gopStructure_e {
    GOP_IBBP  = 0x00,
    GOP_IPPP  = 0x10,
    GOP_IIII  = 0x20,
} gopStructure_e;

typedef enum _mediaAttr_e {
	MEDIA_ATTR_FILE_NAME         = 0,
	MEDIA_ATTR_FILE_TYPE         = 1, /* modified by Edward 20070521 */
	MEDIA_ATTR_REC_STATE         = 2,
	MEDIA_ATTR_WIDTH             = 3,
	MEDIA_ATTR_HEIGHT            = 4,
	MEDIA_ATTR_DURATION          = 5,
	MEDIA_ATTR_ELAPSED_TIME      = 6,
	MEDIA_ATTR_SEEKABLE          = 7,
	MEDIA_ATTR_VIDEO_CODEC       = 8,
	MEDIA_ATTR_VIDEO_BRC_TYPE    = 9,
	MEDIA_ATTR_VIDEO_AVG_BITRATE = 10,
	MEDIA_ATTR_VIDEO_FRAME_RATE  = 11,
	MEDIA_ATTR_AUDIO_CODEC       = 12,
	MEDIA_ATTR_AUDIO_BRC_TYPE    = 13,
	MEDIA_ATTR_AUDIO_AVG_BITRATE = 14,
	MEDIA_ATTR_AUDIO_SAMPLE_RATE = 15,
	MEDIA_ATTR_AUDIO_SAMPLE_BITS = 16,
	MEDIA_ATTR_AUDIO_CHANNELS    = 17,
	MEDIA_ATTR_AUDIO_ALC_HB      = 18,
	MEDIA_ATTR_AUDIO_ALC_LB      = 19,
	MEDIA_ATTR_AUDIO_ALC_MAX_VOL = 20,
	MEDIA_ATTR_AUDIO_AAC_PLUS    = 21,/*@aac plus support*/
	MEDIA_ATTR_AUDIO_AAC_PROFILE = 22,/*@aac profile setting*/
	MEDIA_ATTR_VIDEO_OPEN_DML,
	MEDIA_ATTR_VIDEO_DIVX,
	MEDIA_ATTR_FILE_NAME_BUF,
	MEDIA_ATTR_FILE_NAME_RSV = MEDIA_ATTR_FILE_NAME_BUF + (256/4), /* 256 bytes - reserve for file name */
	MEDIA_ATTR_PLAY_START_TIME, /* @mediaTrim */
	MEDIA_ATTR_PLAY_END_TIME, /* @mediaTrim */
	MEDIA_ATTR_SEAMLESS_TIME_SLOT, /*seamless */
	MEDIA_ATTR_STAMP_OPTION,
	MEDIA_ATTR_STAMP_PAUSE,
	MEDIA_ATTR_STAMP_PAGE_PARAM,
	MEDIA_ATTR_STAMP_PAGE_CUR,
	MEDIA_ATTR_STAMP_PAGE_ID,
	MEDIA_ATTR_STAMP_PAGE_NUM,
	MEDIA_ATTR_STAMP_PAGE_SWITCH,

	MEDIA_ATTR_H264_ENTROPY_MODE, /* 0 for CABAC, 1 for CAVLC */
	MEDIA_ATTR_H264_GOP_STRUCTURE,/* 0x00 for IBBP, 0x10 for IPPP, 0x20 for IIII */
	MEDIA_ATTR_H264_GOP_NO,       /* # of gop */
	MEDIA_ATTR_H264_QP,           /* fix qp mode & qp #, -1 for RC mode */
	MEDIA_ATTR_H264_IDR,		/* I frame with IDR/SPS/PPS or not? 0: only first frame; 1: every beginning I of GOP */
	MEDIA_ATTR_H264_VUI_TAG,	  /* b[0]: video_signal_type_present_flag */

	MEDIA_ATTR_MJPG_INITIAL_Q_FACTOR,

	MEDIA_ATTR_WITH_SUBTITLE,	/* 1 for subtitle embedded, 0 for no subtitle.  */
	MEDIA_ATTR_VIDEO_PTS_RATE,
	#if CUSTOMER_TAG_SUPPORT
	MEDIA_ATTR_REC_TAGS,
	MEDIA_ATTR_IDENTIFY_TAGS,
	#endif

	MEDIA_ATTR_DUALSTREAM_WIDTH,
	MEDIA_ATTR_DUALSTREAM_HEIGHT,
	MEDIA_ATTR_DUALSTREAM_Q_FACTOR,
	MEDIA_ATTR_DUALSTREAM_TARGETVLCSIZE,
	MEDIA_ATTR_DUALSTREAM_MAXQ_FACTOR,

	MEDIA_ATTR_FAST_MERGE_DST_FILE,
	MEDIA_ATTR_SW_TRIGGER_DST_FILE,

	MEDIA_ATTR_HEADER_EXTRA_SIZE,

	/* Secondary Record Attributes */
	MEDIA_ATTR_2ND_FILE_NAME,
	MEDIA_ATTR_2ND_FILE_TYPE,
	MEDIA_ATTR_2ND_FILE_NAME_BUF,
	MEDIA_ATTR_2ND_FILE_NAME_RSV = MEDIA_ATTR_2ND_FILE_NAME_BUF + (256/4), /* 256 bytes - reserve for file name */
	MEDIA_ATTR_2ND_VIDEO_CODEC,

	MEDIA_ATTR_MAX
} mediaAttr_e;

typedef enum {
	MEDIA_PLAY_THUMB_SCALE    = 0,
	MEDIA_PLAY_THUMB_BGCOLOR  = 1,
	MEDIA_PLAY_THUMB_BGFRAME  = 2,
	MEDIA_PLAY_THUMB_COLORKEY = 3,
	MEDIA_PLAY_THUMB_SKIP     = 4,
	MEDIA_PLAY_THUMB_CFG_MAX
} mediaPlayThumbCfg_e;

typedef enum {
	MEDIA_AVI_CHUNK_STRD = 0,
	MEDIA_AVI_CHUNK_STRN = 1,
	MEDIA_AVI_CHUNK_ISFT = 2,
	MEDIA_AVI_CHUNK_JUNK = 3,
} mediaAviChunk_e;

typedef enum _audAlcParam_e {
	ALC_PARAM_ALC_OPT = 0,
	ALC_PARAM_ALC_WIN_SIZE,
	ALC_PARAM_ALC_MAX_VOL,
	ALC_PARAM_ALC_MIN_VOL,
	ALC_PARAM_ALC_MAX_GAIN_IDX,
	ALC_PARAM_ALC_MIN_GAIN_IDX,
	ALC_PARAM_ALC_DEFAULT_GAIN_IDX,
	ALC_PARAM_ALC_GAIN_UP_SPEED,
	ALC_PARAM_ALC_GAIN_DOWN_SPEED,
	ALC_PARAM_ALC_MAX_VOL_GAP,
	ALC_PARAM_SILENCE_GAIN_DOWN_SPEED,
	ALC_PARAM_ALC_GAIN_UP_STEP,
	ALC_PARAM_ALC_GAIN_DOWN_STEP,
	ALC_PARAM_ALC_CUR_ENGENY,
	ALC_PARAM_ALC_MAX
} audAlcParam_e;

/*@videoStamp*/
typedef enum _videoStampCbIdx_e {
	MEDIA_STAMP_NONE = 0,
	MEDIA_STAMP_RGB565,
	MEDIA_STAMP_YUV,
	MEDIA_STAMP_CFG_MAX
} videoStampCbIdx_e;

/*@videoStamp*/
typedef enum _videoStampCbStage_e {
	MEDIA_STAMP_START = 0,
	MEDIA_STAMP_BLEND,
	MEDIA_STAMP_STOP,
	STAMP_STAGE_MAX
} videoStampCbStage_e;

#define MEDIA_REC_SELECTOR_OFFSET 0x1000
typedef enum _mediaRecCfg_e {
	MEDIA_REC_NO_VIDEO = MEDIA_REC_SELECTOR_OFFSET,
	MEDIA_REC_NO_AUDIO,
	MEDIA_REC_MUTE_PERIOD,
	MEDIA_REC_FLIP_ROTATE,
	MEDIA_REC_ALC,
	MEDIA_REC_ALC_MUTE,
	MEDIA_REC_DIS_LEVEL,
	MEDIA_REC_PAUSE_CALLBACK,
	MEDIA_REC_DISK_THR_KB,
	MEDIA_REC_PREREC_MS,
	MEDIA_REC_YUV_FORMAT,
	MEDIA_REC_APPEND_NEXT_VIDEO,
	MEDIA_REC_DIS_GMV_FILTER,
	MEDIA_REC_SEAMLESS_CALLBACK,
	MEDIA_REC_USRDATA_CALLBACK,
	MEDIA_REC_ALC_MODE,
	MEDIA_REC_VLC_BUF_SIZE,
	MEDIA_REC_VLC_BUF_CNT,
	MEDIA_REC_MAX_QVALUE,
	MEDIA_DIS_COMP_BYPASS, /* Bypass DIS compenstation */
	MEDIA_DIS_COMP_TYPE, /* DIS compenstation Type: fast, smooth */
	MEDIA_DIS_COMP_RANGE_MAX_W, /* DIS compensation range max in pixel*/
	MEDIA_DIS_COMP_RANGE_MAX_H, /* DIS compensation range max in pixel*/
	MEDIA_DIS_COMP_RANGE_WIDTH, /* DIS compensation range in pixel*/
	MEDIA_DIS_COMP_RANGE_HEIGHT, /* DIS compensation range in pixel*/
	MEDIA_DIS_EDGE_THRESHOLD, /*DIS edgewin threshold*/
	MEDIA_REC_CAPTURE_FRAMES,
	MEDIA_REC_CAPTURE_FRAME_CB,
	MEDIA_REC_CAPTURE_PROC_CB,
	MEDIA_REC_SAVED_FRAME_RATE,
	MEDIA_REC_FRAME_RATE_CLIP,
	MEDIA_REC_VIDEO_FIFO_BUF_NO,
	MEDIA_REC_AUDIO_FIFO_BUF_NO,
	MEDIA_REC_VIDEO_MSGQ_NO,
	MEDIA_REC_AUDIO_MSGQ_NO,
	MEDIA_REC_SWTRIG_CB,
	MEDIA_REC_SWTRIG_DUMMY_CB,
	MEDIA_REC_SWTRIG_CFG_PTR_CB,
	MEDIA_REC_DUP_L_TO_R_ENABLE,
	MEDIA_REC_LR_SWAP_ENABLE,
	MEDIA_REC_TIMELAPSE_FILE,
	MEDIA_REC_TRIGGER_FILE_NAME,

	MEDIA_REC_DUALSTREAM_EN,  /**< Dual stream for JFIF as 2nd stream. 0: disable, 1: enable (MUST en before record start)  */
	MEDIA_REC_DUALSTREAM_BUF_SIZE, /**< Total size of dual stream buf = (buf size)*(buf cnt) */
	MEDIA_REC_DUALSTREAM_BUF_CNT,  /**< Total size of dual stream buf = (buf size)*(buf cnt) */

	MEDIA_REC_NO_STORAGE,  /* Disable save a/v stream to file. */
	MEDIA_REC_RTP_RECORD_EN,
	MEDIA_REC_2ND_RECORD_EN,  /* Enable secondary record or not. */

	MEDIA_DIS_GMV_UPDATE_CB,
	MEDIA_DIS_FRAME_INTERVAL,
	MEDIA_DIS_COMP_RANGE,       /* DIS compensation max range % */
	MEDIA_DIS_COMP_POS_X,		/* DIS compensation result X, host get only */
	MEDIA_DIS_COMP_POS_Y,		/* DIS compensation result Y, host get only */
	MEDIA_REC_SYNC_THRESHOLD_US,	/* internal check threshold for encode timing, default 200 */
	MEDIA_DIS_COMP_INIT_POS_X,	/* DIS compensation init X */
	MEDIA_DIS_COMP_INIT_POS_Y,	/* DIS compensation init Y */
	MEDIA_REC_SOUT_RDWR_SIZE_MAX,	/* stream_out segment reading/writing size.*/
	MEDIA_REC_EXTENSION_TYPE_EN, /*Enable record extension type file, support file size over 4GB */
	MEDIA_REC_H264_PFRAME_SKIP,
	MEDIA_REC_TRIGGER_TIME_SLOT,  /*trigger mode*/
	MEDIA_REC_RECOVERY_EN, /*video file recovery*/
	MEDIA_REC_RECOVERY_OVER4G_EN, /*video file recovery*/
	MEDIA_REC_MUTIL_MUXER,
	MEDIA_REC_MOV_CONSTANT_DURATION,
	MEDIA_REC_MOV_DUMMY_FRAME,
	MEDIA_REC_RECOVERY_FLUSH_INTERVAL,
	MEDIA_REC_CFG_MAX,
} mediaRecCfg_e;

#define MEDIA_REC_YUV422              0x00
#define MEDIA_REC_YUV420              0x01

#define MEDIA_REC_ROTATE_0            0x00
#define MEDIA_REC_ROTATE_180          0x01
#define MEDIA_REC_FLIP_VERTICAL       0x02
#define MEDIA_REC_FLIP_HORIZONTAL     0x03

#define MEDIA_UBR                     0x00
#define MEDIA_CBR                     0x01
#define MEDIA_VBR                     0x02

#define MEDIA_AUDIO_UNKNOWN           0x0000
#define MEDIA_AUDIO_PCM               0x0001
/*#define MEDIA_AUDIO_ADPCM             0x0002*/
#define MEDIA_AUDIO_ALAW              0x0006
#define MEDIA_AUDIO_MULAW             0x0007
#define MEDIA_AUDIO_IMA_ADPCM         0x0011
#define MEDIA_AUDIO_G726              0x0045
#define MEDIA_AUDIO_MPEGLAYER3        0x0055
#define MEDIA_AUDIO_WMA1              0x0160
#define MEDIA_AUDIO_AAC               0x00FF

#define MEDIA_VIDEO_UNKNOWN           0x0000
#define MEDIA_VIDEO_MPEG4             0x5634504d /* LE "MP4V" */
#define MEDIA_VIDEO_MJPG              0x47504a4d /* LE "MJPG" */
#define MEDIA_VIDEO_H264              0x34363248 /* LE "H264" */

#define MEDIA_REC_ABORT               0x00
#define MEDIA_REC_PAUSE               0x01
#define MEDIA_REC_RESUME              0x02
#define MEDIA_REC_FRAME_CAPTURE       0x03
#define MEDIA_REC_JFIF_CAPTURE        0x04
#define MEDIA_REC_EXIF_CAPTURE		  0x05
#define MEDIA_REC_SWTRIG_START        0x06
#define MEDIA_REC_SWTRIG_STOP         0x07
#define MEDIA_REC_TRIGGER_MODE		  0x08


#define MEDIA_REC_DIS_OFF             0x00
#define MEDIA_REC_DIS_LOW             0x01
#define MEDIA_REC_DIS_MID             0x02
#define MEDIA_REC_DIS_HIGH            0x03
#define MEDIA_REC_RSC_OFF             0x10
#define MEDIA_REC_RSC_LOW             0x11
#define MEDIA_REC_RSC_MID             0x12
#define MEDIA_REC_RSC_HIGH            0x13
#define MEDIA_REC_RSC_DIS_OFF         0x20
#define MEDIA_REC_RSC_DIS_LOW         0x21
#define MEDIA_REC_RSC_DIS_MID         0x22
#define MEDIA_REC_RSC_DIS_HIGH        0x23

typedef enum _mediaPlayCfg_e {
	MEDIA_PLAY_NO_VIDEO,
	MEDIA_PLAY_NO_AUDIO,
	MEDIA_PLAY_MUTE_PERIOD,
	MEDIA_PLAY_FLIP_ROTATE,
	MEDIA_PLAY_VIDEO_FF_END_PAUSE,
	MEDIA_PLAY_TARGET_FRAME_RATE,
	MEDIA_PLAY_VIDEO_FRAME_CB,
	MEDIA_PLAY_AUDIO_DATA_CB,
	MEDIA_PLAY_HEADER_BUF_SIZE,
	MEDIA_PLAY_MAX_DEC_BUF_NUM,
	MEDIA_PLAY_CFG_MAX,
} mediaPlayCfg_e;

#define MEDIA_MSG_PARAM_OKAY          0x00
#define MEDIA_MSG_PARAM_FAIL          0x01
#define MEDIA_MSG_PARAM_NOHANDLE      0x02	/*add for mux open fail*/
#define MEDIA_MSG_PLAY_START          0x500
#define MEDIA_MSG_PLAY_STOP           0x501
#define MEDIA_MSG_PLAY_END            0x502
#define MEDIA_MSG_REC_START           0x503
#define MEDIA_MSG_REC_STOP            0x504
#define MEDIA_MSG_REC_DISK_FULL       0x505
#define MEDIA_MSG_FF_TO_END           0x506
#define MEDIA_MSG_RW_TO_END           0x507
#define MEDIA_MSG_SOUND_FINISH        0x508
#define MEDIA_MSG_PLAY_FILE_ERROR     0x50b
#define MEDIA_MSG_REC_PAUSE_FINISH    0x50c
#define MEDIA_MSG_FILE_SIZE_NOTIFY 	  0x50d
#define MEDIA_MSG_REC_DROP_FRAME      0x50e
#define MEDIA_MSG_AVI_REC_FPS_LOW     0x50f
#define MEDIA_MSG_REC_AUD_HW_STOP     0x510
#define MEDIA_MSG_TRIM_END            0x511/* @mediaTrim */
#define MEDIA_MSG_SEMALESS_CNT_NOTIFY 0x512/* @seamless */
#define MEDIA_MSG_URGENT_CB_STOP      0x513/* @urgentCB */
#define MEDIA_MSG_SEMALESS_DELETE     0x514/* @seamless */
#define MEDIA_MSG_REC_FRAME_CAPUTRE_END 0x515
#define MEDIA_MSG_REC_RESUME_FINISH    0x516
#define MEDIA_MSG_SEAMLESS_CHANGE_FILE 0x517/* @seamless video tag*/
#define MEDIA_MSG_PLAY_SEAMLESS_LOAD_NOTIFY  0x520
#define MEDIA_MSG_PLAY_SEAMLESS_LOAD_FINISH  0x521
#define MEDIA_MSG_PLAY_SEAMLESS_LOAD_FAILED  0x522
#define MEDIA_MSG_REC_TRIGGER_FINISH		 0x523 /* @trigger mode*/
#define MEDIA_MSG_REC_FPS_CHECK        0x530

#define MEDIA_MAX_FILE_NAME    ( 64 )

#define MEDIA_ATTR_AUDIO_EQ_MODE      0x15
#define MEDIA_ATTR_AUDIO_EQ_BAND      0x16
#define MEDIA_ATTR_AUDIO_EQ_VALUE     0x17

#define MEDIA_REC_ALC_CST_MODE  0x00  /* hw alc: constant level mode. */
#define MEDIA_REC_ALC_DRC_MODE  0x01  /* fw alc: customized gain mapping (dynamic range) mode. */

#define MEDIA_REC_ENABLE_STORAGE()    (!mediaRecCfgGet(MEDIA_REC_RTP_RECORD_EN) || !mediaRecCfgGet(MEDIA_REC_NO_STORAGE))

/**************************************************************************
 *                Media Buffer for Media Source
 **************************************************************************/
typedef struct mediaBufferObject_s {} *mediaBufferObject_t;

/* Buffer Flags*/
enum {
	/* bit[0:3]: Elementary Stream type */
	MEDIA_BUF_ES_UNKNOW   = 0,

	MEDIA_BUF_ES_VIDEO    = 1,
	MEDIA_BUF_ES_AUDIO    = 2,
	/* MEDIA_SRC_ES_TEXT    = 3, Not used now. */

	MEDIA_BUF_ES_NR       = 2,
	MEDIA_BUF_ES_BITMASK  = 0x0F,

	/* bit[4]: Key-frame flag */
	MEDIA_BUF_KEY_FRAME   = 0x01 << 4,
	MEDIA_BUF_PARAM_FRAME = 0x01 << 5,
};

typedef enum {
	MEDIA_BUF_CODEC_UNKNOW = 0,
	MEDIA_BUF_CODEC_H264   = MEDIA_VIDEO_H264,
	MEDIA_BUF_CODEC_JPEG   = MEDIA_VIDEO_MJPG,
	MEDIA_BUF_CODEC_PCM    = MEDIA_AUDIO_PCM,

	MEDIA_BUF_CODEC_NR     = 3
} mediaBufCodecType_e;

#define MEDIA_BUF_ES_TYPE(flags)        ((flags)&MEDIA_BUF_ES_BITMASK)
#define MEDIA_BUF_ES_IDX(es)            (((es)&MEDIA_BUF_ES_BITMASK) - 1)  /* Start from 0. */
#define MEDIA_BUF_CODEC_IS_AUDIO(c)     ((c) == MEDIA_BUF_CODEC_PCM)
#define MEDIA_BUF_CODEC_IS_VIDEO(c)     ((c) == MEDIA_BUF_CODEC_H264 || (c) == MEDIA_BUF_CODEC_JPEG)

/* Deprecated */
#define MEDIA_SRC_ES_UNKNOW   MEDIA_BUF_ES_UNKNOW
#define MEDIA_SRC_ES_VIDEO    MEDIA_BUF_ES_VIDEO
#define MEDIA_SRC_ES_AUDIO    MEDIA_BUF_ES_AUDIO
#define MEDIA_SRC_ES_NR       MEDIA_BUF_ES_NR
#define MEDIA_SRC_ES_BITMASK  MEDIA_BUF_ES_BITMASK
#define MEDIA_SRC_KEY_FRAME   MEDIA_BUF_KEY_FRAME
#define MEDIA_SRC_PARAM_FRAME MEDIA_BUF_PARAM_FRAME

#define MEDIA_SRC_CODEC_UNKNOW MEDIA_BUF_CODEC_UNKNOW
#define MEDIA_SRC_CODEC_H264   MEDIA_BUF_CODEC_H264
#define MEDIA_SRC_CODEC_JPEG   MEDIA_BUF_CODEC_JPEG
#define MEDIA_SRC_CODEC_PCM    MEDIA_BUF_CODEC_PCM
#define MEDIA_SRC_CODEC_NR     MEDIA_BUF_CODEC_NR

#define MEDIA_SRC_ES_TYPE        MEDIA_BUF_ES_TYPE
#define MEDIA_SRC_ES_IDX         MEDIA_BUF_ES_IDX
#define MEDIA_SRC_CODEC_IS_AUDIO MEDIA_BUF_CODEC_IS_AUDIO
#define MEDIA_SRC_CODEC_IS_VIDEO MEDIA_BUF_CODEC_IS_VIDEO

#define mediaSrcCodecType_e mediaBufCodecType_e

/**************************************************************************
 *                Media Source
 **************************************************************************/
#include <sys/time.h>

enum {
	/* Flags for mediaSrcOpenXXX functions. */
	MEDIA_SRC_NO_AUDIO_SWAP = 1 << 0,
};

typedef enum {
	MEDIA_SRC_CTRL_PAUSE   = 0, /* No arg */
	MEDIA_SRC_CTRL_RESUME,      /* No arg */
	MEDIA_SRC_CTRL_SEEK,        /* arg: UINT32 param*/
	MEDIA_SRC_CTRL_DUMP,        /* No arg */
	/* Return all supported attribute IDs.
	 * arg: mediaAttr_e **attrs, UINT32 *num. The last element is (UINT32)-1
	 */
	MEDIA_SRC_CTRL_GET_ATTRS,

	MEDIA_SRC_CTRL_NR
} mediaSrcCtrlCode_e;

typedef void *mediaSrcHandle_t; /* 0 is invalid handle */

typedef struct mediaSrcBuffer_s
{
	mediaBufferObject_t bufobj; /* For releasing buffer */
	UINT32 bufflags;
	UINT8  *data;
	UINT32 length;
	UINT32 duration; /* in ms */
	struct timeval pts;
} mediaSrcBuffer_t;

typedef void (*mediaSrcOnGotBuffer_t)(mediaSrcBuffer_t *buf, unsigned long user_data);

/* Deprecated */
#define mediaSrcBufferObject_t mediaBufferObject_t
#define mediaSrcHandler_t mediaSrcHandle_t

/**************************************************************************
 *                End fo Media Source
 **************************************************************************/

typedef enum {
	MEDIA_SYS_CFG_H264_MB_QP_DIFF, /* &qparr, type */
	MEDIA_SYS_CFG_h264_MV_CTRL,    /* mode, termination, shift */
} mediaSystemCfg_e;

/**************************************************************************
 *                          D A T A    T Y P E S
 **************************************************************************/
typedef UINT32 (*pmediaRecPauseCB_t)(UINT32);
typedef void (*pmediaRecSeamlessCB_t)(void);

typedef UINT32 (*pmediaVideoStampCb_t)(UINT32 fid, UINT8 *pbuf, UINT32 w, UINT32 h);
typedef struct mediaVideoStampPara_s {
	UINT16 x;      /*!< offset x */
	UINT16 y;      /*!< offset y */
	UINT32 width;  /*!< stamp width */
	UINT32 height; /*!< stamp height */
	void (*fpMediaVideoStampInitCb)(UINT32 first, UINT32 second); /*!< cb after initial */
	void (*fpMediaVideoStampSwitchCb)(UINT32 draw); /*!< cb after really switched */
	struct mediaVideoStampPara_s *next;
}mediaVideoStampPara_t,*PSTAMPARA;


#if CUSTOMER_TAG_SUPPORT
/* REX: not implemented yet*/
/*
typedef enum {
	MEDIA_TAG_UPDT_TIME = 0,
	MEDIA_TAG_UPDT_GPS,
	MEDIA_TAG_UPDT_SENSOR,
	MEDIA_TAG_UPDT_EVT
} mediaVideoUsrDatUpdtType_e;
*/
typedef struct mediaVideoTagUserData_s {
	UINT32	name;			/**< Name for user data type atom, by spec it's "udta" */
	UINT32	location;   	/**< In order to meet customer's request
								 0: default value, default location by spec, under moov atom
								 1: one level up to on par with ftyp atom */
	UINT32	update_mode; 	/**< How user data is updated
								 0: write all data into atom when close
								 1: write when mux control is called
								 When setting 0 or 1 we don't care about data just write.
								 Below Not Implemened Yet:
								 2: Customzied for RDV-1000 to support 4 kinds of user atom
							 */
	UINT8*	pData;			/**< Used when update mode is 0*/
	UINT32	nDataSize;		/**< Used when update mode is 0*/
	UINT32	pCallBack;		/**< Used when there is something host need to pass to mux task when done */
} mediaVideoTagUserData_t;

typedef struct mediaVideoTags_s {
	/**
	  *		MOV specific tags
	  */
	UINT32	major_brand;
	UINT32	minor_version;
	UINT32* compatible_brands;
	UINT32	compatible_brands_cnt;
	mediaVideoTagUserData_t udta_atom;
	UINT16 	pref_volume;
	UINT16	video_track_lang_id;		/** video track language ID */
	UINT32 	vendor;
	UINT32 	component_manuf;
	UINT32 	data_ref_type;
	UINT32 	tkhd_flags;

	/** Video Related Fields */
	UINT32 	time_scale;
	UINT32 	video_track_matrix[9];
	UINT16	gfx_mode;
	UINT16 	opcolor[3];
	UINT16	video_dscp_ver;
	UINT16	video_dscp_rev_lvl;
	UINT8	video_compressor_name[32];		/**< A 32-byte Pascal string */
	UINT32	temporal_quality;
	UINT32 	spatial_quality;
	UINT32 	video_sample_per_chunk;			/**< 4 bytes integer indicate how much samples stored per chunk*/
	UINT8* 	video_handler_comp_name;		/**< null terminated string*/
	UINT8*	video_handler_ref_comp_name;	/**< null terminated string*/

	/** Audio Related Fields */
	UINT16	audio_dscp_ver;
	UINT16	audio_compression_id;
	UINT32	audio_stsz_sample_size;
	UINT32 	audio_sample_per_chunk;			/**< 4 bytes integer indicate how much samples stored per chunk*/
	UINT8*  audio_handler_comp_name;		/**< null terminated string*/
	UINT8* 	audio_handler_ref_comp_name;	/**< null terminated string*/
} mediaVideoTags_t;

typedef void (*pmediaRecUsrDataCB_t)(void);
#endif

/* Dual-Stream */
typedef enum {
	MEDIA_REC_DUALSTREAM_PAUSE,
	MEDIA_REC_DUALSTREAM_RESUME,
	MEDIA_REC_DUALSTREAM_MAXQ_SET,
	MEDIA_REC_DUALSTREAM_TGTVLCSZ_SET,
} mediaRecDualStreamCtrlCode_e;

typedef enum {
	MEDIA_REC_DUALSTREAM_OFF,
	MEDIA_REC_DUALSTREAM_ON_NORMAL,
	MEDIA_REC_DUALSTREAM_ON_EXT,
} mediaRecDualStreamState_e;

typedef struct {
	UINT32 dataFmt; /* 0 : EXIF_YUV420, 1: EXIF_YUV422 */
	UINT32 width, height;
	UINT32 vlcPos;
	UINT32 vlcSize;
	UINT32 qIdx;
	UINT8  qtable[128];
} mediaRecJpegAttr_t, mediaRecDualStreamJpegAttr_t;

typedef void (*mediaRecDualStreamCbFunc)(mediaSrcBuffer_t *buf
	, mediaRecDualStreamJpegAttr_t const *jpegattr
	, UINT32 udata);

typedef struct mediaRecDualStreamClient_s {
	mediaRecDualStreamCbFunc cbfunc;
	UINT32 udata;
	/* Frame rate control. Don't modify the following members directly. */
	UINT32 frc_frmrate;   /* 0: no frame-rate control */
	UINT32 frc_shiftbits; /* 0 <==> 29 */
	UINT32 frc_maskbits;
	struct mediaRecDualStreamClient_s *next;
} mediaRecDualStreamClient_t;

/* Dual-Stream End */

typedef struct {
	SINT32 rscx,rscy;
	SINT32 curx,cury;
	SINT32 rangex,rangey;
} distGmvCbEx_t;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/

#endif /* _MEDIA_DEF_H_ */

