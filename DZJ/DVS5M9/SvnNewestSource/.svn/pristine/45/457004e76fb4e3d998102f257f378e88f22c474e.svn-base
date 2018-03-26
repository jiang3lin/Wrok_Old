/*******************************************************************************
 *
 *         Copyright (c) 2002 by Sunplus Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology
 *  Co., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus Technology Co., Ltd.
 *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Richie Lee
 *
 ******************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev$
$Date::                     $
 */

#ifndef _SNAP_DEF_H_
#define _SNAP_DEF_H_

/*******************************************************************************
 *                           C O N S T A N T S
 ******************************************************************************/

typedef enum {
	SNAP_SYS_CFG_STORE_CONFIRM_TIMEOUT,
	SNAP_SYS_MAX_COMPRESSION_RATIO, /* for VLC buffer allocation */
	SNAP_SYS_CFG_JPEG_PADDING,		/* pad black jpg to target size */
	SNAP_SYS_CFG_JPEG_CHECK_SUM,
	SNAP_SYS_CFG_AHD_BUF_LINES,
	/* 5 */
	SNAP_SYS_CFG_VLC_BUF_NUM,
	SNAP_SYS_CFG_RAW_BUF_NUM,
	SNAP_SYS_CFG_FAST_BK_PREVIEW,
	SNAP_SYS_CFG_VLC_BUF_SAVE_BYTES, /* overlap the vlc and raw buffer */
	SNAP_SYS_CFG_BURST_FAST_CDSP, 	/* disable AHD & xdenoise for burst shot performance, Zxxxx does this! */
	/* 10 */
	SNAP_SYS_CFG_FAST_BK_PREVIEW_SKIP_FRAME,
	SNAP_SYS_MAX_COMPRESSION_PRECISE_RATIO, /* for VLC buffer allocation */
	SNAP_SYS_CFG_VLC_STUFFING,
	SNAP_SYS_CFG_BURST_FAST_SENSOR_TRIG, 
	SNAP_SYS_CFG_RAW_BUF_LDC_EXTRA_LINES,
	SNAP_SYS_CFG_JPG_VLC_SIZE_OVERHEAD, /* main vlc size overhead in byte */
	SNAP_SYS_CFG_TOTAL,
} snapStsCfg_t;

#define SNAP_SYS_CFG_RAW_BUF_EXTRA_LINES SNAP_SYS_CFG_AHD_BUF_LINES

#define SNAP_FAKE_FIELD_NAUM	 6
#define JPG_UNDERFLOW_RECONDE  1 /* 0: not reencode if vlc is underflow; 1: encode again if vlc is underflow */

/* RAW callback return value */
#define SNAP_RAW_CB_RET_REDO_LSC 		( 1 << 0 )
#define SNAP_RAW_CB_RET_REDO_WB_OFFSET	( 1 << 1 )

/* YUV callback return value */
#define SNAP_YUV_CB_RET_SKIP_FLOW ( 1 << 0 ) /* the snap flow will be skipped if host return this value in yuv callback */
#define SNAP_YUV_CB_RET_LOCK_BUFF ( 1 << 1 ) /* force to not return raw buffer yuv callback */
#define SNAP_YUV_CB_RET_UNLOCK_ALL_BUFF ( 1 << 2 ) /*return all locked raw buffers */
#define SNAP_YUV_CB_RET_REDO_AHD ( 1 << 3 ) /* redo AHD again after yuv callback */
#define SNAP_YUV_CB_RET_YUV_BUF_RET ( 1 << 4 ) /* redo AHD again after yuv callback */

#define SNAP_YUV_CB_RET_SKIP_AND_LOCK_BUFF (SNAP_YUV_CB_RET_LOCK_BUFF | SNAP_YUV_CB_RET_SKIP_FLOW) /* the snap flow will be skipped and raw buffer will not be returned */
#define SNAP_YUV_CB_RET_SKIP_AND_UNLOCK_BUFF (SNAP_YUV_CB_RET_UNLOCK_ALL_BUFF | SNAP_YUV_CB_RET_SKIP_FLOW) /*  the snap flow will be skipped and all locked raw buffer will be returned */


#define IS_YUV_CB_RET_SKIP(x)  ( ( x & SNAP_YUV_CB_RET_SKIP_FLOW ) == SNAP_YUV_CB_RET_SKIP_FLOW )
#define IS_YUV_CB_RET_LOCK_BUFF(x)  ( ( x & SNAP_YUV_CB_RET_LOCK_BUFF ) == SNAP_YUV_CB_RET_LOCK_BUFF )
#define IS_YUV_CB_RET_UNLOCK_BUFF(x)  ( ( x & SNAP_YUV_CB_RET_UNLOCK_ALL_BUFF ) == SNAP_YUV_CB_RET_UNLOCK_ALL_BUFF )
#define IS_YUV_CB_RET_REDO_AHD(x)  ( ( x & SNAP_YUV_CB_RET_REDO_AHD ) == SNAP_YUV_CB_RET_REDO_AHD )
#define IS_YUV_CB_RET_YUV_BUF_RET(x)  ( ( x & SNAP_YUV_CB_RET_YUV_BUF_RET ) == SNAP_YUV_CB_RET_YUV_BUF_RET )



#define SNAP_HDR_MODE_OFF 0
#define SNAP_HDR_MODE_ACC 1
#define SNAP_HDR_MODE_NORMAL 2

#define SNAP_HDR_ACC_AVERAGE_OFF 0
#define SNAP_HDR_ACC_AVERAGE_ON 1

/*******************************************************************************
 *                              M A C R O S
 ******************************************************************************/

/*******************************************************************************
 *                          D A T A    T Y P E S
 ******************************************************************************/
typedef enum {
	SNAP_CALLBACK_DATA_FMT_RAW_PACK_8_BIT = 0,
	SNAP_CALLBACK_DATA_FMT_RAW_PACK_10_BIT,
	SNAP_CALLBACK_DATA_FMT_RAW_PACK_12_BIT,
	SNAP_CALLBACK_DATA_FMT_RAW_PACK_14_BIT,
	SNAP_CALLBACK_DATA_FMT_RAW_PACK_16_BIT,
} snapCallbackDataFmt_t;

/*******************************************************************************
 *                         G L O B A L    D A T A
 ******************************************************************************/

/*******************************************************************************
 *                  E X T E R N A L    R E F E R E N C E
 ******************************************************************************/

/*******************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 ******************************************************************************/

#endif /* _SNAP_DEF_H_ */


