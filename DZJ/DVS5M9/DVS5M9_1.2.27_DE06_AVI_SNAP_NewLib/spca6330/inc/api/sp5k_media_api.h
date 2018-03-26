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
#ifndef _SP5K_MEDIA_API_H_
#define _SP5K_MEDIA_API_H_

/**************************************************************************
 *                         H E A D E R   F I L E S
 **************************************************************************/
#include "middleware/common_types.h"
#include "middleware/media_def.h"

/**************************************************************************
 *                           C O N S T A N T S
 **************************************************************************/
/* general */
#define SP5K_MEDIA_DISABLE  0
#define SP5K_MEDIA_ENABLE   1

#define SP5K_MEDIA_MAX_FILE_NAME  MEDIA_MAX_FILE_NAME

/* media attribute */
/** ++++++++++++++++++++ mediaAttr_e ++++++++++++++++++++ */
#define SP5K_MEDIA_ATTR_FILE_NAME              MEDIA_ATTR_FILE_NAME
#define SP5K_MEDIA_ATTR_FILE_TYPE              MEDIA_ATTR_FILE_TYPE /*MEDIA_ATTR_FILE_NAME_BUF*/ /* modified by Edward 20070521 */
#define SP5K_MEDIA_ATTR_WIDTH                  MEDIA_ATTR_WIDTH
#define SP5K_MEDIA_ATTR_HEIGHT                 MEDIA_ATTR_HEIGHT
#define SP5K_MEDIA_ATTR_DURATION               MEDIA_ATTR_DURATION
#define SP5K_MEDIA_ATTR_ELAPSED_TIME           MEDIA_ATTR_ELAPSED_TIME
#define SP5K_MEDIA_ATTR_VIDEO_CODEC            MEDIA_ATTR_VIDEO_CODEC
#define SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE         MEDIA_ATTR_VIDEO_BRC_TYPE
#define SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE      MEDIA_ATTR_VIDEO_AVG_BITRATE
#define SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE       MEDIA_ATTR_VIDEO_FRAME_RATE
#define SP5K_MEDIA_ATTR_AUDIO_CODEC            MEDIA_ATTR_AUDIO_CODEC
#define SP5K_MEDIA_ATTR_AUDIO_BRC_TYPE         MEDIA_ATTR_AUDIO_BRC_TYPE
#define SP5K_MEDIA_ATTR_AUDIO_AVG_BITRATE      MEDIA_ATTR_AUDIO_AVG_BITRATE
#define SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE      MEDIA_ATTR_AUDIO_SAMPLE_RATE
#define SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS      MEDIA_ATTR_AUDIO_SAMPLE_BITS
#define SP5K_MEDIA_ATTR_AUDIO_CHANNELS         MEDIA_ATTR_AUDIO_CHANNELS
#define SP5K_MEDIA_ATTR_AUDIO_AAC_PROFILE      MEDIA_ATTR_AUDIO_AAC_PROFILE
#define SP5K_MEDIA_ATTR_SEEKABLE               MEDIA_ATTR_SEEKABLE
#define SP5K_MEDIA_ATTR_AUDIO_ALC_HB           MEDIA_ATTR_AUDIO_ALC_HB	/*!> not use */
#define SP5K_MEDIA_ATTR_AUDIO_ALC_LB           MEDIA_ATTR_AUDIO_ALC_LB /*!> not use */
#define SP5K_MEDIA_ATTR_AUDIO_ALC_MAX_VOL      MEDIA_ATTR_AUDIO_ALC_MAX_VOL /*!> not use */
#define SP5K_MEDIA_ATTR_AUDIO_EQ_MODE          MEDIA_ATTR_AUDIO_EQ_MODE
#define SP5K_MEDIA_ATTR_AUDIO_EQ_BAND          MEDIA_ATTR_AUDIO_EQ_BAND
#define SP5K_MEDIA_ATTR_AUDIO_EQ_VALUE         MEDIA_ATTR_AUDIO_EQ_VALUE
#define SP5K_MEDIA_ATTR_PLAY_START_TIME        MEDIA_ATTR_PLAY_START_TIME
#define SP5K_MEDIA_ATTR_PLAY_END_TIME          MEDIA_ATTR_PLAY_END_TIME
#define SP5K_MEDIA_ATTR_SEAMLESS_TIME_SLOT     MEDIA_ATTR_SEAMLESS_TIME_SLOT
#define SP5K_MEDIA_ATTR_STAMP_OPTION           MEDIA_ATTR_STAMP_OPTION
#define SP5K_MEDIA_ATTR_STAMP_PAUSE            MEDIA_ATTR_STAMP_PAUSE
#define SP5K_MEDIA_ATTR_STAMP_PAGE_PARAM       MEDIA_ATTR_STAMP_PAGE_PARAM
#define SP5K_MEDIA_ATTR_STAMP_PAGE_CUR         MEDIA_ATTR_STAMP_PAGE_CUR

#define SP5K_MEDIA_ATTR_STAMP_PAGE_ID          MEDIA_ATTR_STAMP_PAGE_ID
#define SP5K_MEDIA_ATTR_STAMP_PAGE_NUM         MEDIA_ATTR_STAMP_PAGE_NUM
#define SP5K_MEDIA_ATTR_STAMP_PAGE_SWITCH      MEDIA_ATTR_STAMP_PAGE_SWITCH
#define SP5K_MEDIA_ATTR_SUBTITLE_OPTION        MEDIA_ATTR_WITH_SUBTITLE
#define SP5K_MEDIA_ATTR_MJPG_INITIAL_Q_FACTOR  MEDIA_ATTR_MJPG_INITIAL_Q_FACTOR
#define SP5K_MEDIA_ATTR_REC_TAGS               MEDIA_ATTR_REC_TAGS
#define SP5K_MEDIA_ATTR_IDENTIFY_TAGS          MEDIA_ATTR_IDENTIFY_TAGS
#define SP5K_MEDIA_ATTR_H264_IDR               MEDIA_ATTR_H264_IDR
#define SP5K_MEDIA_ATTR_H264_VUI_TAG           MEDIA_ATTR_H264_VUI_TAG
#define SP5K_MEDIA_ATTR_DUALSTREAM_WIDTH       MEDIA_ATTR_DUALSTREAM_WIDTH
#define SP5K_MEDIA_ATTR_DUALSTREAM_HEIGHT      MEDIA_ATTR_DUALSTREAM_HEIGHT
#define SP5K_MEDIA_ATTR_DUALSTREAM_Q_FACTOR    MEDIA_ATTR_DUALSTREAM_Q_FACTOR
#define SP5K_MEDIA_ATTR_DUALSTREAM_TARGETVLCSIZE MEDIA_ATTR_DUALSTREAM_TARGETVLCSIZE
#define SP5K_MEDIA_ATTR_DUALSTREAM_MAXQ_FACTOR  MEDIA_ATTR_DUALSTREAM_MAXQ_FACTOR

#define SP5K_MEDIA_ATTR_2ND_FILE_NAME          MEDIA_ATTR_2ND_FILE_NAME
#define SP5K_MEDIA_ATTR_2ND_FILE_TYPE          MEDIA_ATTR_2ND_FILE_TYPE
#define SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC        MEDIA_ATTR_2ND_VIDEO_CODEC

#define SP5K_MEDIA_ATTR_FAST_MERGE_DST_FILE    MEDIA_ATTR_FAST_MERGE_DST_FILE
#define SP5K_MEDIA_ATTR_SW_TRIGGER_DST_FILE    MEDIA_ATTR_SW_TRIGGER_DST_FILE
#define SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE     MEDIA_ATTR_H264_GOP_STRUCTURE

#define SP5K_MEDIA_ATTR_HEADER_EXTRA_SIZE      MEDIA_ATTR_HEADER_EXTRA_SIZE

/** -------------------- mediaAttr_e -------------------- */

#define SP5K_MEDIA_UBR  MEDIA_UBR
#define SP5K_MEDIA_CBR  MEDIA_CBR
#define SP5K_MEDIA_VBR  MEDIA_VBR

#define SP5K_MEDIA_GOP_IBBP  GOP_IBBP
#define SP5K_MEDIA_GOP_IPPP  GOP_IPPP
#define SP5K_MEDIA_GOP_IIII  GOP_IIII

#define SP5K_MEDIA_WAV  SP5K_DCF_FILETYPE_WAV
#define SP5K_MEDIA_MP3  SP5K_DCF_FILETYPE_MP3
#define SP5K_MEDIA_AVI  SP5K_DCF_FILETYPE_AVI
#define SP5K_MEDIA_ASF  SP5K_DCF_FILETYPE_ASF
#define SP5K_MEDIA_WMA  SP5K_DCF_FILETYPE_WMA
#define SP5K_MEDIA_AAC  SP5K_DCF_FILETYPE_AAC
#define SP5K_MEDIA_MOV	SP5K_DCF_FILETYPE_MOV
#define SP5K_MEDIA_MP4	SP5K_DCF_FILETYPE_MP4


#define SP5K_MEDIA_AUDIO_UNKNOWN     MEDIA_AUDIO_UNKNOWN
#define SP5K_MEDIA_AUDIO_PCM         MEDIA_AUDIO_PCM
/*#define SP5K_MEDIA_AUDIO_ADPCM       MEDIA_AUDIO_ADPCM*/
#define SP5K_MEDIA_AUDIO_ALAW        MEDIA_AUDIO_ALAW
#define SP5K_MEDIA_AUDIO_MULAW       MEDIA_AUDIO_MULAW
#define SP5K_MEDIA_AUDIO_IMA_ADPCM   MEDIA_AUDIO_IMA_ADPCM
#define SP5K_MEDIA_AUDIO_MPEGLAYER3  MEDIA_AUDIO_MPEGLAYER3
#define SP5K_MEDIA_AUDIO_WMA1        MEDIA_AUDIO_WMA1
#define SP5K_MEDIA_AUDIO_AAC         MEDIA_AUDIO_AAC

#define SP5K_MEDIA_VIDEO_UNKNOWN  MEDIA_VIDEO_UNKNOWN
#define SP5K_MEDIA_VIDEO_MPEG4    MEDIA_VIDEO_MPEG4
#define SP5K_MEDIA_VIDEO_MJPG     MEDIA_VIDEO_MJPG
#define SP5K_MEDIA_VIDEO_H264     MEDIA_VIDEO_H264

/* media record configuration */
/** ++++++++++++++++++++ mediaRecCfg_e ++++++++++++++++++++ */
#define SP5K_MEDIA_REC_NO_VIDEO           MEDIA_REC_NO_VIDEO
#define SP5K_MEDIA_REC_NO_AUDIO           MEDIA_REC_NO_AUDIO
#define SP5K_MEDIA_REC_MUTE_PERIOD        MEDIA_REC_MUTE_PERIOD
#define SP5K_MEDIA_REC_FLIP_ROTATE        MEDIA_REC_FLIP_ROTATE
#define SP5K_MEDIA_REC_ALC                MEDIA_REC_ALC
#define SP5K_MEDIA_REC_ALC_MODE           MEDIA_REC_ALC_MODE
#define SP5K_MEDIA_REC_ALC_MUTE           MEDIA_REC_ALC_MUTE /*!> not use */
#define SP5K_MEDIA_REC_DIS_LEVEL          MEDIA_REC_DIS_LEVEL
#define SP5K_MEDIA_REC_PAUSE_CALLBACK     MEDIA_REC_PAUSE_CALLBACK
#define SP5K_MEDIA_REC_DISK_THR_KB        MEDIA_REC_DISK_THR_KB
#define SP5K_MEDIA_REC_PREREC_MS          MEDIA_REC_PREREC_MS
#define SP5K_MEDIA_REC_YUV_FORMAT         MEDIA_REC_YUV_FORMAT
#define SP5K_MEDIA_REC_APPEND_NEXT_VIDEO  MEDIA_REC_APPEND_NEXT_VIDEO
#define SP5K_MEDIA_REC_SEAMLESS_CALLBACK  MEDIA_REC_SEAMLESS_CALLBACK
#define SP5K_MEDIA_REC_USRDATA_CALLBACK   MEDIA_REC_USRDATA_CALLBACK
#define SP5K_MEDIA_REC_VLC_BUF_SIZE       MEDIA_REC_VLC_BUF_SIZE
#define SP5K_MEDIA_REC_VLC_BUF_CNT        MEDIA_REC_VLC_BUF_CNT
#define SP5K_MEDIA_REC_MAX_QVALUE         MEDIA_REC_MAX_QVALUE
#define SP5K_MEDIA_REC_DUP_L_TO_R_ENABLE  MEDIA_REC_DUP_L_TO_R_ENABLE
#define SP5K_MEDIA_REC_LR_SWAP_ENABLE     MEDIA_REC_LR_SWAP_ENABLE
#define SP5K_MEDIA_DIS_COMP_BYPASS        MEDIA_DIS_COMP_BYPASS /**< Bypass DIS compenstation: 0=bypass, 1=normal flow (default) */
#define SP5K_MEDIA_DIS_COMP_TYPE          MEDIA_DIS_COMP_TYPE /**< DIS compenstation Type: 0=fast(default), 1=smooth */
#define SP5K_MEDIA_DIS_COMP_FAST          0x0
#define SP5K_MEDIA_DIS_COMP_SMOOTH        0x1
#define SP5K_MEDIA_DIS_COMP_SMOOTH_RSC    0x2

#define SP5K_MEDIA_REC_H264_PFRAME_SKIP   MEDIA_REC_H264_PFRAME_SKIP

#define SP5K_MEDIA_DIS_COMP_RANGE         MEDIA_DIS_COMP_RANGE       /**< DIS compensation maximum range in percentage, value: 0~100 */
#define SP5K_MEDIA_DIS_COMP_RANGE_MAX_W   MEDIA_DIS_COMP_RANGE_MAX_W /* DIS compensation range max in pixel*/
#define SP5K_MEDIA_DIS_COMP_RANGE_MAX_H   MEDIA_DIS_COMP_RANGE_MAX_H /* DIS compensation range max in pixel*/
#define SP5K_MEDIA_DIS_COMP_RANGE_WIDTH   MEDIA_DIS_COMP_RANGE_WIDTH /**< DIS compensation range in pixel*/
#define SP5K_MEDIA_DIS_COMP_RANGE_HEIGHT  MEDIA_DIS_COMP_RANGE_HEIGHT /**< DIS compensation range in pixel*/
#define SP5K_MEDIA_DIS_COMP_POS_X         MEDIA_DIS_COMP_POS_X		 /* DIS compensation result X in half pixel, host get only */
#define SP5K_MEDIA_DIS_COMP_POS_Y         MEDIA_DIS_COMP_POS_Y		 /* DIS compensation result Y in half pixel, host get only */
#define SP5K_MEDIA_DIS_COMP_INIT_POS_X	  MEDIA_DIS_COMP_INIT_POS_X	 /* DIS compensation init X in half pixel */
#define SP5K_MEDIA_DIS_COMP_INIT_POS_Y	  MEDIA_DIS_COMP_INIT_POS_Y	 /* DIS compensation init Y in half pixel */
#define SP5K_MEDIA_REC_SYNC_THRESHOLD_US  MEDIA_REC_SYNC_THRESHOLD_US	/* internal check threshold for encode timing */
#define SP5K_MEDIA_REC_RECOVERY_EN		  MEDIA_REC_RECOVERY_EN
#define SP5K_MEDIA_REC_RECOVERY_OVER4G_EN	  MEDIA_REC_RECOVERY_OVER4G_EN
#define SP5K_MEDIA_REC_MUTIL_MUXER	  	  MEDIA_REC_MUTIL_MUXER
#define SP5K_MEDIA_REC_MOV_CONSTANT_DURATION MEDIA_REC_MOV_CONSTANT_DURATION
#define SP5K_MEDIA_REC_MOV_DUMMY_FRAME 	  MEDIA_REC_MOV_DUMMY_FRAME
#define SP5K_MEDIA_REC_RECOVERY_FLUSH_INTERVAL MEDIA_REC_RECOVERY_FLUSH_INTERVAL


#define SP5K_MEDIA_DIS_EDGE_THRESHOLD     MEDIA_DIS_EDGE_THRESHOLD
#define SP5K_MEDIA_REC_CAPTURE_FRAMES     MEDIA_REC_CAPTURE_FRAMES
/* UINT32 frmCb(UINT32 id, UINT32 addr, sp5kYuvCbImgSize_t *p, void *rsv) */
#define SP5K_MEDIA_REC_CAPTURE_FRAME_CB   MEDIA_REC_CAPTURE_FRAME_CB
/*
 * UINT32 procCb(UINT32 id, UINT32 addr, sp5kYuvCbImgSize_t *p, void *rsv)
 * return true to save file
 */
#define SP5K_MEDIA_REC_CAPTURE_PROC_CB    MEDIA_REC_CAPTURE_PROC_CB
#define SP5K_MEDIA_REC_SAVED_FRAME_RATE   MEDIA_REC_SAVED_FRAME_RATE
#define SP5K_MEDIA_REC_FRAME_RATE_CLIP    MEDIA_REC_FRAME_RATE_CLIP
#define SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO  MEDIA_REC_VIDEO_FIFO_BUF_NO
#define SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO  MEDIA_REC_AUDIO_FIFO_BUF_NO
#define SP5K_MEDIA_REC_VIDEO_MSGQ_NO      MEDIA_REC_VIDEO_MSGQ_NO
#define SP5K_MEDIA_REC_AUDIO_MSGQ_NO      MEDIA_REC_AUDIO_MSGQ_NO

/*
 * UINT32 swtrigCb(UINT8 *pbuf, sp5kYuvCbImgSize_t *p, UINT32 rsvd0, UINT32 rsvd1)
 * return SUCCESS to trigger the codec.
 */
#define SP5K_MEDIA_REC_SWTRIG_CB          MEDIA_REC_SWTRIG_CB
#define SP5K_MEDIA_REC_SWTRIG_DUMMY_CB		MEDIA_REC_SWTRIG_DUMMY_CB
#define SP5K_MEDIA_REC_SWTRIG_CFG_PTR_CB  MEDIA_REC_SWTRIG_CFG_PTR_CB

/** Defined for time lapse */
#define SP5K_MEDIA_REC_TIMELAPSE_FILE     MEDIA_REC_TIMELAPSE_FILE

/** Dual stream for JFIF as 2nd stream. 0: disable, 1: enable (MUST en before record start)  */
#define SP5K_MEDIA_REC_DUALSTREAM_JFIF_EN MEDIA_REC_DUALSTREAM_EN

/* Enable RTP Muxer for RTP streaming */
#define SP5K_MEDIA_REC_RTP_STREAMING_EN   MEDIA_REC_RTP_RECORD_EN

/* Enable 2nd record  */
#define SP5K_MEDIA_REC_2ND_RECORD_EN      MEDIA_REC_2ND_RECORD_EN

/* Disable save a/v stream to file include main and secondary records. */
#define SP5K_MEDIA_REC_DISABLE_STORAGE    MEDIA_REC_NO_STORAGE

/** Total size of dual stream buf = (buf size)*(buf cnt) */
#define SP5K_MEDIA_REC_DUALSTREAM_BUF_SIZE MEDIA_REC_DUALSTREAM_BUF_SIZE
#define SP5K_MEDIA_REC_DUALSTREAM_BUF_CNT  MEDIA_REC_DUALSTREAM_BUF_CNT

/* UINT32 (*gmvSetCb)(UINT32 shrng, UINT32 addr1, UINT32 addr2, void *rsv0); */
#define SP5K_MEDIA_DIS_GMV_UPDATE_CB      MEDIA_DIS_GMV_UPDATE_CB
#define SP5K_MEDIA_DIS_FRAME_INTERVAL     MEDIA_DIS_FRAME_INTERVAL

#define SP5K_MEDIA_REC_EXTENSION_TYPE_EN     MEDIA_REC_EXTENSION_TYPE_EN

#define SP5K_MEDIA_REC_TRIGGER_TIME_SLOT     MEDIA_REC_TRIGGER_TIME_SLOT

#define SP5K_MEDIA_REC_TRIGGER_FILE_NAME	 MEDIA_REC_TRIGGER_FILE_NAME

/** -------------------- mediaRecCfg_e -------------------- */


#define SP5K_MEDIA_REC_OFF  0
#define SP5K_MEDIA_REC_ON   1

#define SP5K_MEDIA_REC_ROTATE_0         MEDIA_REC_ROTATE_0
#define SP5K_MEDIA_REC_ROTATE_180       MEDIA_REC_ROTATE_180
#define SP5K_MEDIA_REC_FLIP_VERTICAL    MEDIA_REC_FLIP_VERTICAL
#define SP5K_MEDIA_REC_FLIP_HORIZONTAL  MEDIA_REC_FLIP_HORIZONTAL

#define SP5K_MEDIA_REC_YUV422           MEDIA_REC_YUV422
#define SP5K_MEDIA_REC_YUV420           MEDIA_REC_YUV420

#define SP5K_MEDIA_REC_IS_TYPE          MEDIA_REC_DIS_LEVEL
#define SP5K_MEDIA_REC_IS_OFF           MEDIA_REC_DIS_OFF
#define SP5K_MEDIA_REC_IS_DIS           MEDIA_REC_DIS_MID
#define SP5K_MEDIA_REC_IS_RSC           MEDIA_REC_RSC_MID
#define SP5K_MEDIA_REC_IS_DIS_RSC       MEDIA_REC_RSC_DIS_MID

/** The following definition are obsolete and will be removed sometime later */
#define SP5K_MEDIA_REC_DIS_OFF          MEDIA_REC_DIS_OFF
#define SP5K_MEDIA_REC_DIS_LOW          MEDIA_REC_DIS_LOW
#define SP5K_MEDIA_REC_DIS_MID          MEDIA_REC_DIS_MID
#define SP5K_MEDIA_REC_DIS_HIGH         MEDIA_REC_DIS_HIGH
#define SP5K_MEDIA_REC_RSC_OFF          MEDIA_REC_RSC_OFF
#define SP5K_MEDIA_REC_RSC_LOW          MEDIA_REC_RSC_LOW
#define SP5K_MEDIA_REC_RSC_MID          MEDIA_REC_RSC_MID
#define SP5K_MEDIA_REC_RSC_HIGH         MEDIA_REC_RSC_HIGH
#define SP5K_MEDIA_REC_RSC_DIS_OFF      MEDIA_REC_RSC_DIS_OFF
#define SP5K_MEDIA_REC_RSC_DIS_LOW      MEDIA_REC_RSC_DIS_LOW
#define SP5K_MEDIA_REC_RSC_DIS_MID      MEDIA_REC_RSC_DIS_MID
#define SP5K_MEDIA_REC_RSC_DIS_HIGH     MEDIA_REC_RSC_DIS_HIGH

/* media record control */
#define SP5K_MEDIA_REC_ABORT            MEDIA_REC_ABORT
#define SP5K_MEDIA_REC_PAUSE            MEDIA_REC_PAUSE
#define SP5K_MEDIA_REC_RESUME           MEDIA_REC_RESUME
#define SP5K_MEDIA_REC_FRAME_CAPTURE    MEDIA_REC_FRAME_CAPTURE
#define SP5K_MEDIA_REC_JFIF_CAPTURE     MEDIA_REC_JFIF_CAPTURE
#define SP5K_MEDIA_REC_EXIF_CAPTURE     MEDIA_REC_EXIF_CAPTURE

#define SP5K_MEDIA_REC_SWTRIG_START     MEDIA_REC_SWTRIG_START
#define SP5K_MEDIA_REC_SWTRIG_STOP      MEDIA_REC_SWTRIG_STOP

#define SP5K_MEDIA_REC_TRIGGER_MODE		MEDIA_REC_TRIGGER_MODE

#define sp5kMediaVideoTagUserData_t	mediaVideoTagUserData_t
#define sp5kMediaVideoTags_t            mediaVideoTags_t

/* media playback configuration */
#define SP5K_MEDIA_PLAY_NO_VIDEO            MEDIA_PLAY_NO_VIDEO
#define SP5K_MEDIA_PLAY_NO_AUDIO            MEDIA_PLAY_NO_AUDIO
#define SP5K_MEDIA_PLAY_MUTE_PERIOD         MEDIA_PLAY_MUTE_PERIOD
#define SP5K_MEDIA_PLAY_FLIP_ROTATE         MEDIA_PLAY_FLIP_ROTATE
#define SP5K_MEDIA_PLAY_VIDEO_FF_END_PAUSE  MEDIA_PLAY_VIDEO_FF_END_PAUSE
#define SP5K_MEDIA_PLAY_TARGET_FRAME_RATE   MEDIA_PLAY_TARGET_FRAME_RATE
#define SP5K_MEDIA_PLAY_VIDEO_FRAME_CB      MEDIA_PLAY_VIDEO_FRAME_CB
#define SP5K_MEDIA_PLAY_AUDIO_DATA_CB       MEDIA_PLAY_AUDIO_DATA_CB
#define SP5K_MEDIA_PLAY_HEADER_BUF_SIZE     MEDIA_PLAY_HEADER_BUF_SIZE
#define SP5K_MEDIA_PLAY_MAX_DEC_BUF_NUM     MEDIA_PLAY_MAX_DEC_BUF_NUM


#define SP5K_MEDIA_PLAY_START              0
#define SP5K_MEDIA_PLAY_ABORT              1
#define SP5K_MEDIA_PLAY_PAUSE              2
#define SP5K_MEDIA_PLAY_RESUME             3
#define SP5K_MEDIA_PLAY_SEEK               4
#define SP5K_MEDIA_PLAY_FORWARD            5
#define SP5K_MEDIA_PLAY_REWIND             6
#define SP5K_MEDIA_PLAY_SLOW_MOTION        7
#define SP5K_MEDIA_PLAY_FRAME_CAPTURE      8
#define SP5K_MEDIA_PLAY_TRIM_START     	   MEDIA_PLAY_TRIM_START
#define SP5K_MEDIA_PLAY_RESOUCE_AVI    	   MEDIA_PLAY_RESOURCE_AVI/*@startAV*/
#define SP5K_MEDIA_PLAY_FAST_TRIM      	   MEDIA_PLAY_FAST_TRIM
#define SP5K_MEDIA_PLAY_FAST_MERGE     	   MEDIA_PLAY_FAST_MERGE
#define SP5K_MEDIA_PLAY_VIDEO_FRM_APPEND   MEDIA_PLAY_VIDEO_FRM_APPEND
#define SP5K_MEDIA_PLAY_SEAMLESS_EN	   MEDIA_PLAY_SEAMLESS_EN
#define SP5K_MEDIA_PLAY_SEAMLESS_LOAD      MEDIA_PLAY_SEAMLESS_LOAD
#define SP5K_MEDIA_PLAY_SEEK_FORWARD       16
#define SP5K_MEDIA_PLAY_SEEK_BACKWARD      17

/* voice memo */
#define SP5K_MEDIA_MEMO_RECORD  ( 0x00 )
#define SP5K_MEDIA_MEMO_PLAY    ( 0x01 )
#define SP5K_MEDIA_MEMO_ABORT   ( 0x02 )
#define SP5K_MEDIA_MEMO_PAUSE   ( 0x03 )
#define SP5K_MEDIA_MEMO_RESUME  ( 0x04 )
#define SP5K_MEDIA_MEMO_SEEK    ( 0x05 )

#define SP5K_MSG_MEDIA_PARAM_OKAY  MEDIA_MSG_PARAM_OKAY
#define SP5K_MSG_MEDIA_PARAM_FAIL  MEDIA_MSG_PARAM_FAIL
#define SP5K_MSG_MEDIA_PARAM_NOHANDLE  MEDIA_MSG_PARAM_NOHANDLE


/* Selectors for setting the extra information for memo operation */
typedef enum {
	SP5K_MEDIA_MEMO_CFG_SEEK_TIME,
	SP5K_MEDIA_MEMO_CFG_FORWARD_SPEED,
	SP5K_MEDIA_MEMO_CFG_BACKWARD_SPEED,
} sp5kMediaMemoCfg_e;

/* obsolete */
#if 0
#define SP5K_MEDIA_AUDIO_EQ_NORMAL_MODE   ( 0x00 )
#define SP5K_MEDIA_AUDIO_EQ_CLASSIC_MODE  ( 0x01 )
#define SP5K_MEDIA_AUDIO_EQ_POP_MODE      ( 0x02 )
#define SP5K_MEDIA_AUDIO_EQ_BASS_MODE     ( 0x03 )
#define SP5K_MEDIA_AUDIO_EQ_ROCK_MODE     ( 0x04 )
#define SP5K_MEDIA_AUDIO_EQ_CUSTOM_MODE   ( 0x05 )
#endif

/* ALC */
typedef enum _sp5kMediaAlcParam_e {
	/*!> 0: disable, 1: enable ALC */
	SP5K_MEDIA_ALC_OPT                 = ALC_PARAM_ALC_OPT,
	/*!> not used */
	SP5K_MEDIA_ALC_WIN_SIZE            = ALC_PARAM_ALC_WIN_SIZE,
	/*!> max gain index, 0 ~ 43, +1.5dB each step ; 0 ~ 50, 3dB each step for 5330 */
	SP5K_MEDIA_ALC_MAX_GAIN_IDX        = ALC_PARAM_ALC_MAX_GAIN_IDX,
	/*!> min gain index, 0 ~ 43, +1.5dB each step ; 0 ~ 50, 3dB each step for 5330 */
	SP5K_MEDIA_ALC_MIN_GAIN_IDX        = ALC_PARAM_ALC_MIN_GAIN_IDX,
	/*!> max rms in one frame */
	SP5K_MEDIA_ALC_MAX_VOL             = ALC_PARAM_ALC_MAX_VOL,
	/*!> min rms in one frame */
	SP5K_MEDIA_ALC_MIN_VOL             = ALC_PARAM_ALC_MIN_VOL,
	/*!> default gain index, 0 ~ 43, +1.5dB each step ; 0 ~ 50, 3dB each step for 5330 */
	SP5K_MEDIA_ALC_DEFAULT_GAIN_IDX    = ALC_PARAM_ALC_DEFAULT_GAIN_IDX,
	/*!> frame count to tune up gain */
	SP5K_MEDIA_ALC_GAIN_UP_SPEED       = ALC_PARAM_ALC_GAIN_UP_SPEED,
	/*!> frame count to tune down gain */
	SP5K_MEDIA_ALC_GAIN_DOWN_SPEED     = ALC_PARAM_ALC_GAIN_DOWN_SPEED,
	/*!> [31]: slience, [27:0]: value */
	SP5K_MEDIA_ALC_CUR_ENERGY          = ALC_PARAM_ALC_CUR_ENGENY,
	/*!> max audio volume gap */
	SP5K_MEDIA_ALC_MAX_VOL_GAP         = ALC_PARAM_ALC_MAX_VOL_GAP,
	/*!> frame count to tune down gain in silence state */
	SP5K_MEDIA_SILENCE_GAIN_DOWN_SPEED = ALC_PARAM_SILENCE_GAIN_DOWN_SPEED,
	/*!> The increase value of audio gain */
	SP5K_MEDIA_ALC_GAIN_UP_STEP        = ALC_PARAM_ALC_GAIN_UP_STEP,
	/*!> The decrease value of audio gain */
	SP5K_MEDIA_ALC_GAIN_DOWN_STEP      = ALC_PARAM_ALC_GAIN_DOWN_STEP,
	SP5K_MEDIA_ALC_MAX                 = ALC_PARAM_ALC_MAX,
} sp5kMediaAlcParam_e;

typedef enum _sp5kMediaStampFlow_e {
	SP5K_MEDIA_STAMP_NONE   = MEDIA_STAMP_NONE,
	SP5K_MEDIA_STAMP_RGB565 = MEDIA_STAMP_RGB565,
	SP5K_MEDIA_STAMP_YUV    = MEDIA_STAMP_YUV,
} sp5kMediaStampFlow_e;

#define sp5kMediaVideoStampParam_t mediaVideoStampPara_t
/**
 * \note
 * scale down media thumbnail with the specified factor
 * para: scale down factor, range from 1000 (1x) to 100 (0.1x)
 */
#define SP5K_MEDIA_PLAY_THUMB_SCALE		MEDIA_PLAY_THUMB_SCALE
/**
 * \note
 * set background color if scaling down media thumbnail is set
 * para: color in RGB565 format
 */
#define SP5K_MEDIA_PLAY_THUMB_BGCOLOR	MEDIA_PLAY_THUMB_BGCOLOR
/**
 * \note
 * enable/disable background frame blending to media thumbnail
 * para: 0:disable background frame, 1:enable
 */
#define SP5K_MEDIA_PLAY_THUMB_BGFRAME	MEDIA_PLAY_THUMB_BGFRAME
/**
 * \note
 * for background frame blending, if y value in background frame is
 * less than the input threshold, the pixel will be treated as
 * color key
 * para: y threahold (0~255)
 */
#define SP5K_MEDIA_PLAY_THUMB_COLORKEY	MEDIA_PLAY_THUMB_COLORKEY
/**
 * \note
 * for skip the display of video thumbnail.
 * para: 0: disable skip, 1: skip.
 */
#define SP5K_MEDIA_PLAY_THUMB_SKIP      MEDIA_PLAY_THUMB_SKIP

/* selector of channel for ALC parameter*/
typedef enum _sp5kMediaRecAlcChnl_e {
	SP5K_MEDIA_REC_ALC_CH_LEFT  = 1,
	SP5K_MEDIA_REC_ALC_CH_RIGHT = 2,
	SP5K_MEDIA_REC_ALC_CH_ALL   = 3,
} sp5kMediaRecAlcChnl_e;  /* direct map to audRecChnl_e. */

/* ALC mode selector of cfgid SP5K_MEDIA_REC_ALC_MODE in sp5kMediaRecCfgSet */
#define SP5K_MEDIA_REC_ALC_CST_MODE  MEDIA_REC_ALC_CST_MODE
#define SP5K_MEDIA_REC_ALC_DRC_MODE  MEDIA_REC_ALC_DRC_MODE

/* media edit */
#define SP5K_MEDIA_EDIT_TRIM  MEDIA_EDIT_TRIM


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

#define sp5kDistGmvCbEx_t distGmvCbEx_t

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 **************************************************************************/
/* sp5k_media_api.c */
UINT32 sp5kMediaRecCfgSet(UINT32 cfgId, UINT32 value);
UINT32 sp5kMediaRecCfgGet(UINT32 cfgId, UINT32 *pvalue);
UINT32 sp5kMediaRecControl(UINT32 option);
UINT32 sp5kMediaPlayControl(UINT32 option, UINT32 param1, UINT32 param2);
UINT32 sp5kMediaPlayAttrParse(void);
UINT32 sp5kMediaRecAttrSet(UINT32 attrId, UINT32 value);
UINT32 sp5kMediaRecAttrGet(UINT32 attrId, UINT32 *pvalue);
UINT32 sp5kMediaPlayCfgSet(UINT32 cfgId, UINT32 value);
UINT32 sp5kMediaPlayCfgGet(UINT32 cfgId, UINT32 *pvalue);
UINT32 sp5kMediaPlayAttrGet(UINT32 attrId, UINT32 *pvalue);
UINT32 sp5kMediaPlayFileNameSet(UINT8 *pfname);
UINT32 sp5kMediaMemoControl(UINT32 dcfIdx, UINT32 control);
UINT32 sp5kMediaMemoCfgSet(UINT32 selector, void *value);
UINT32 sp5kMediaPlayThumbCfgSet(UINT32 cfgId, UINT32 value);
UINT32 sp5kMediaPlayThumbCfgGet(UINT32 cfgId, UINT32 *pvalue);
UINT32 sp5kMediaRecAviChunkSet(mediaAviChunk_e chunk, UINT8 *pbuf, UINT32 size);
UINT32 sp5kMediaPlayAviChunkInfoGet(mediaAviChunk_e chunk, UINT32 *poffset, UINT32 *psize);
UINT32 sp5kMediaPlayAviChunkBufSet(mediaAviChunk_e chunk, UINT8 *pbuf, UINT32 size);
UINT32 sp5kMediaRecAviChunkUpdate(mediaAviChunk_e chunk, UINT32 offset, UINT32 size, UINT8 *pbuf);
UINT32 sp5kMediaRecWavInfoSet(UINT8 *pbuf, UINT32 size);
UINT32 sp5kMediaPlayWavInfoBufSet(UINT8 *pbuf, UINT32 size);
UINT32 sp5kMediaPlayImgWinSet(UINT32 x, UINT32 y, UINT32 w, UINT32 h);
UINT32 sp5kMediaPlayThumbFrameSet(UINT8 *pfilename);
UINT32 sp5kMediaAlcParamSet(UINT32 chnlId, UINT32 attrId, UINT32 value);
UINT32 sp5kMediaAlcParamGet(UINT32 chnlId, UINT32 attrId, UINT32 *pvalue);
UINT32 sp5kMediaEdit(UINT32 option, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);
UINT32 sp5kMediaPlayAttrSet(UINT32 attrId, UINT32 value);
UINT32 sp5kMediaAlcParamFileLoad(UINT8 *pfileName);
UINT32 sp5kMediaRecUserDataUpdate(UINT8 *psrc, UINT32 size, UINT32 offset, UINT32 append);

UINT32 sp5kMediaRecDualStreamGetStream(UINT32 *pRIdx, UINT32 *pBuf, UINT32 *pSize);

UINT32 sp5kVideoUrgentCallbackSet(UINT32 interval, void *pfunc);


#endif /* _SP5K_MEDIA_API_H_ */

