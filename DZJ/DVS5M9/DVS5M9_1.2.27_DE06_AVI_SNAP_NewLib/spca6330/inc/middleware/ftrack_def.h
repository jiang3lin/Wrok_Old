/**************************************************************************
 *
 *        Copyright (c) 2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice may not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _FTRACK_DEF_H_
#define _FTRACK_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/modesw_def.h"


/**
 * \param mode in
 * UINT32 sp5kFaceTrackModeSet(UINT32 mode);
 */
typedef enum {
	FTRACK_MODE_OFF,
	FTRACK_MODE_PV_ON,
	FTRACK_MODE_PV_SUSPEND,
	FTRACK_MODE_PV_RESUME,
	FTRACK_MODE_SNAP_QV_ON,
	FTRACK_MODE_PV_SNAP_QV_ON,
	FTRACK_MODE_PV_DELAY_OFF = 0x80,
} ftrackModeSel_t;


/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
typedef modeswFrameBuf_t ftrackFrameBuf_t;


/**< face result. Host should prepare enough buffer or crash might happen */
typedef struct {
	UINT32 x:10;
	UINT32 y:10;
	UINT32 size:10;
	UINT32 attr;
} ftrackFaceRes_t;

typedef struct {
	UINT32 vehicleDistance;
	UINT32 vehicleWarn;
	UINT32 vehicleLabelNum;
} vehicleDetectGetInfo_t;


typedef struct ftrackEdgeWinAttr_s {
	/* for FW calculation */
	UINT32	zfacH;
	UINT32	zfacV;
	UINT32	muxHsize;
	UINT32	muxVsize;
	/* for HW assignment */
	UINT16	winThrH;
	UINT16	winThrV;
	UINT16	hOffset;
	UINT16	vOffset;
	UINT8	hSize;
	UINT8	vSize;
	UINT8	winCntH;
	UINT8	winCntV;
} ftrackEdgeWinAttr_t;

typedef struct ftrackSkinParam_s {
	UINT8 uh; /**< U high realm */
	UINT8 ul; /**< U low  threshold */
	UINT8 vh; /**< V high realm */
	UINT8 vl; /**< V low  threshold */
	UINT8 yh; /**< Y high realm */
	UINT8 yl; /**< Y low  threshold */
} ftrackSkinParam_t;

/**
 * \param cfg in
 * UINT32 sp5kFaceTrackCfgSet(UINT32 cfg,UINT32 val);
 */
typedef enum {
	FTRACK_CFG_PROC_MAX_FACE, /*max count of result faces*/	/*[0]*/
	FTRACK_CFG_MIN_FACE_SIZE, /*min size of faces*/
	FTRACK_CFG_TIMEOUT,
	FTRACK_CFG_PROC_SMILE_DETECT,
	FTRACK_CFG_PROC_BLINK_DETECT,
	FTRACK_CFG_PROC_BLINK_NUM, /* 1:one eye blink as blink, 2: two eyes */
	FTRACK_CFG_PROC_EACH_FACE_CB,
	FTRACK_CFG_PROC_ORIENTATION,
	FTRACK_CFG_PROC_SIDEWAYS,
	FTRACK_CFG_FD_OUT_MAX_RECT, /*max count of raw faces*/
	/* (TBD) working buf num */
	FTRACK_CFG_II_BUF_NUM, /*at least 2, max 8*/			/*[10]*/
	FTRACK_CFG_IISCALE_BUF_NUM, /*at least 1, max 8*/
	FTRACK_CFG_PROC_FACE_BUF_NUM, /*at least 2, max 8*/
	/* (TBD) turn, rotate, etc */
	FTRACK_CFG_ACCUM_PERIOD,
	FTRACK_CFG_PROC_KEY_FACE, /*0:off,1:on,2:smile detect key face only*/
	FTRACK_CFG_PROC_CHECK_COLOR,
	FTRACK_CFG_YUVBUF_BLOCK,
	FTRACK_CFG_FD_DONE_CB,
	FTRACK_CFG_PROC_RESTART,
	FTRACK_CFG_PROC_CHECK_NOSE,
	FTRACK_CFG_PROC_RES_USE_RAWBUF,							/*[20]*/
	FTRACK_CFG_PROC_REDEYE_DETECTING,
	FTRACK_CFG_LOW_POWER,
	FTRACK_CFG_COPY_FRAME_BUF,
	FTRACK_CFG_PV_SNAP_OFF, /*1:fd close under snap,0:not*/
	FTRACK_CFG_PROC_COLOR_TABLE_SET, /*0:real,1:whiter*/
	FTRACK_CFG_PROC_LOVE_PORTRAIT_MODE,
	FTRACK_CFG_PROC_COLOR_TABLE_LOAD,
	FTRACK_CFG_PROC_WHITE_TABLE_SET,
	FTRACK_CFG_PROC_WHITE_TABLE_LOAD,
	FTRACK_CFG_PROC_LUMA_TABLE_SET,							/*[30]*/
	FTRACK_CFG_PROC_LUMA_TABLE_LOAD,
	FTRACK_CFG_DRAM_PAUSE_PERIOD, /* [0:7] FD, [8:15] GPE, [16:31] rsvd */
	FTRACK_CFG_PROC_SEARCH_LEVEL, /* 0~15, internal use only */
	FTRACK_CFG_PROC_REF_BUF_INFO,
	FTRACK_CFG_PROC_DETECT_LIST_SEL,
	FTRACK_CFG_FRAME_FLESH_MODE, /* 0:off (default), o/w:on, TODO */
	FTRACK_CFG_FRAME_FLESH_CNT_THRS,
	FTRACK_CFG_FRAME_FLESH_SCORE_THRS,
	FTRACK_CFG_MIN_FACE_II_LEVEL,
	/** hostFW can choice detect list, but baseFW can apply when it need. */
	FTRACK_CFG_PROC_PB_DETECT_LIST_SEL, 					/*[40]*/
	FTRACK_CFG_PROC_COMPACT_BUF_MODE, /*1:enable, 0:disable, set before FD start*/
	FTRACK_CFG_PROC_FACE_CAR_SWITCH,
	FTRACK_CFG_PROC_CUSTOM_DB_PATH,
	FTRACK_CFG_TOTAL,
} ftrackCfgSel_t;

typedef enum {
	FORWARD_COLLISION_WARN_DISTANCE_SET,
	FORWARD_COLLISION_ROI_CROP_FACTOR_SET,
	FORWARD_COLLISION_ROI_CROP_OFFSET_SET,
	FORWARD_COLLISION_NIGHT_YUV_LUT_TABLE_SET,
	FORWARD_COLLISION_SENSOR_PIXEL_UNIT_SET,
	FORWARD_COLLISION_SENSOR_FOCAL_SET,
	FORWARD_COLLISION_DAYNIGHT_SET,
	FORWARD_COLLISION_WARN_DISTANCE_GET,
	FORWARD_COLLISION_ROI_CROP_FACTOR_GET,
	FORWARD_COLLISION_ROI_CROP_OFFSET_GET,
	FORWARD_COLLISION_SENSOR_PIXEL_UNIT_GET,
	FORWARD_COLLISION_SENSOR_FOCAL_GET,
	FORWARD_COLLISION_DISTANCE_GET,
	FORWARD_COLLISION_WARNING_FLAG_GET,
	FORWARD_COLLISION_DAYNIGHT_GET,
	FORWARD_COLLISION_CFG_TOTAL,
} VEHICLE_DETECTCfgSel_t;

#define FTRACK_FACE_TYPE_SMILE   0x80000000
#define FTRACK_FACE_TYPE_BLINK   0x40000000
#define FTRACK_FACE_TYPE_KEYFACE 0x10000000
#define FTRACK_FACE_TYPE_FACE    0
#define FTRACK_FACE_TYPE_RIGHT   0x00000040
#define FTRACK_FACE_TYPE_LEFT    0x00000080
#define FTRACK_FACE_TYPE_NO_NOSE 0x00000100

/*20130806 MJ*/
#define FTRACK_FACE_TYPE_VEHICLE_SEDAN 0x00000000
#define FTRACK_FACE_TYPE_VEHICLE_HB 0x01000000
#define FTRACK_FACE_TYPE_VEHICLE_TRUCK 0x04000000
#define FTRACK_FACE_TYPE_VEHICLE_BUS 0x08000000

/* clockwise */
typedef enum {
	FTRACK_FACE_ANGLE_0 = 0x00,
	FTRACK_FACE_ANGLE_30,
	FTRACK_FACE_ANGLE_60,
	FTRACK_FACE_ANGLE_90,
	FTRACK_FACE_ANGLE_120,
	FTRACK_FACE_ANGLE_150,
	FTRACK_FACE_ANGLE_180,
	FTRACK_FACE_ANGLE_210,
	FTRACK_FACE_ANGLE_240,
	FTRACK_FACE_ANGLE_270,
	FTRACK_FACE_ANGLE_300,
	FTRACK_FACE_ANGLE_330,
} ftrackFaceAngle_e;

#ifdef __cplusplus
}
#endif

#endif /* _FTRACK_DEF_H_ */
