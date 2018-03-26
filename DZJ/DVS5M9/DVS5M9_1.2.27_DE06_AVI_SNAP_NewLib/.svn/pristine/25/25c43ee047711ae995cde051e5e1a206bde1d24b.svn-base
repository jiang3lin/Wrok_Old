/**************************************************************************
 *
 *       Copyright (c) 2008-2012 by iCatch Technology, Inc.
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
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _SP5K_FTRACK_API_H_
#define _SP5K_FTRACK_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"
#include "middleware/ftrack_def.h"


/**< INTERFACE TO SP5K HOST */

/**
 * called in app_init.c and switch on/off in app_init.h
 * ftrack FW module will not be linked in without calling
 */
/**< calling this if host wants HW face track support for preview */
/*void ftrackPvInit(void);*/
/**< calling this if host wants HW face track support for snap (TBD) */
/*void ftrackSnapInit(void);*/
/**< calling this if host wants HW face track support for playback (TBD) */
/*void ftrackPbInit(void);*/

/**
 * \param mode disable, enable, pause/resume
 * UINT32 sp5kFaceTrackModeSet(UINT32 mode);
 */
typedef enum {
	SP5K_FACE_TRACK_MODE_OFF           = FTRACK_MODE_OFF,
	SP5K_FACE_TRACK_MODE_PV_ON         = FTRACK_MODE_PV_ON,
	SP5K_FACE_TRACK_MODE_PV_SUSPEND    = FTRACK_MODE_PV_SUSPEND,
	SP5K_FACE_TRACK_MODE_PV_RESUME     = FTRACK_MODE_PV_RESUME,
	SP5K_FACE_TRACK_MODE_SNAP_QV_ON    = FTRACK_MODE_SNAP_QV_ON,
	SP5K_FACE_TRACK_MODE_PV_SNAP_QV_ON = FTRACK_MODE_PV_SNAP_QV_ON,
	SP5K_FACE_TRACK_MODE_PV_DELAY_OFF  = FTRACK_MODE_PV_DELAY_OFF,
} sp5kFaceTrackModeSel_t;

UINT32 sp5kFaceTrackModeSet(UINT32 mode);

/**
 * \param param, user specified, for example, image id.
 * user will later receive the corresponding host message attached
 * with this param and pass to sp5kFaceTrackResultGet(param, ...);
 * \param pbuf, a structure specifies the yuv frame buffer to do face tracking
 * pbuf.yuvaddr must be 16-byte aligned and .yuvw must be multiple of 8 pixels
 * .yuvw, .yuvh define the width and height in pixels of the yuv frame buffer
 * in DRAM; .dispx, y, w, h define the cropping window displayed on screen
 * if pbuf.dispw,h are 0, cropping window fits the full yuv frame buffer.
 * \param wratio,
 * \param hratio, specifies the appearance geometry aspect ratio on screen
 * for the cropping window defined by pbuf.dispw, .disph.
 * if wratio, hratio == 0, the cropping window is squared pixel.
 */
typedef ftrackFrameBuf_t sp5kFaceTrackYuvBuf_t;

UINT32 sp5kFaceTrackYuvBufStart(UINT32 param,
	const sp5kFaceTrackYuvBuf_t *pbuf, UINT32 wratio, UINT32 hratio);

/**
 * \brief host calls this when receiving face tracking messages and
 * sp5kFaceTrackResultGet() finished
 * \param param, as used in sp5kFaceTrackYuvBufStart();
 */
UINT32 sp5kFaceTrackYuvBufEnd(UINT32 param);

/**
 * \param cfg in
 * UINT32 sp5kFaceTrackCfgSet(UINT32 cfg,UINT32 val);
 */
typedef enum {
	SP5K_FACE_TRACK_CFG_PROC_MAX_FACE           = FTRACK_CFG_PROC_MAX_FACE,
	SP5K_FACE_TRACK_CFG_MIN_FACE_SIZE           = FTRACK_CFG_MIN_FACE_SIZE,
	SP5K_FACE_TRACK_CFG_TIMEOUT                 = FTRACK_CFG_TIMEOUT,
	SP5K_FACE_TRACK_CFG_SMILE_DETECT            = FTRACK_CFG_PROC_SMILE_DETECT,
	SP5K_FACE_TRACK_CFG_BLINK_DETECT            = FTRACK_CFG_PROC_BLINK_DETECT,
	SP5K_FACE_TRACK_CFG_BLINK_NUM               = FTRACK_CFG_PROC_BLINK_NUM,  /* 1: one eye blink as blink, 2: two eyes blink as blink */
	SP5K_FACE_TRACK_CFG_EACH_FACE_CB            = FTRACK_CFG_PROC_EACH_FACE_CB,
	SP5K_FACE_TRACK_CFG_ORIENTATION             = FTRACK_CFG_PROC_ORIENTATION,
	SP5K_FACE_TRACK_CFG_SIDEWAYS                = FTRACK_CFG_PROC_SIDEWAYS,
	SP5K_FACE_TRACK_CFG_FD_OUT_MAX_RECT         = FTRACK_CFG_FD_OUT_MAX_RECT,
	SP5K_FACE_TRACK_CFG_II_BUF_NUM              = FTRACK_CFG_II_BUF_NUM,
	SP5K_FACE_TRACK_CFG_IISCALE_BUF_NUM         = FTRACK_CFG_IISCALE_BUF_NUM,
	SP5K_FACE_TRACK_CFG_PROC_FACE_BUF_NUM       = FTRACK_CFG_PROC_FACE_BUF_NUM,
	SP5K_FACE_TRACK_CFG_ACCUM_PERIOD            = FTRACK_CFG_ACCUM_PERIOD,
	SP5K_FACE_TRACK_CFG_KEY_FACE                = FTRACK_CFG_PROC_KEY_FACE,
	SP5K_FACE_TRACK_CFG_CHECK_COLOR             = FTRACK_CFG_PROC_CHECK_COLOR,
	SP5K_FACE_TRACK_CFG_YUVBUF_BLOCK            = FTRACK_CFG_YUVBUF_BLOCK,
	SP5K_FACE_TRACK_CFG_FD_DONE_CB              = FTRACK_CFG_FD_DONE_CB,
	SP5K_FACE_TRACK_CFG_PROC_RESTART            = FTRACK_CFG_PROC_RESTART,
	SP5K_FACE_TRACK_CFG_CHECK_NOSE              = FTRACK_CFG_PROC_CHECK_NOSE,
	SP5K_FACE_TRACK_CFG_LOW_POWER               = FTRACK_CFG_LOW_POWER,
	SP5K_FACE_TRACK_CFG_COPY_FRAME_BUF          = FTRACK_CFG_COPY_FRAME_BUF,
	SP5K_FACE_TRACK_CFG_PV_SNAP_OFF             = FTRACK_CFG_PV_SNAP_OFF,
	SP5K_FACE_TRACK_CFG_COLOR_TABLE_SET         = FTRACK_CFG_PROC_COLOR_TABLE_SET,
	SP5K_FACE_TRACK_CFG_LOVE_PORTRAIT_MODE      = FTRACK_CFG_PROC_LOVE_PORTRAIT_MODE,
	SP5K_FACE_TRACK_CFG_DRAM_PAUSE_PERIOD       = FTRACK_CFG_DRAM_PAUSE_PERIOD,
	SP5K_FACE_TRACK_CFG_PROC_REF_BUF_INFO       = FTRACK_CFG_PROC_REF_BUF_INFO,
	SP5K_FACE_TRACK_CFG_PROC_DET_LIST_SEL       = FTRACK_CFG_PROC_DETECT_LIST_SEL,
	SP5K_FACE_TRACK_CFG_FRAME_FLESH_MODE        = FTRACK_CFG_FRAME_FLESH_MODE,
	SP5K_FACE_TRACK_CFG_FRAME_FLESH_CNT_THRS    = FTRACK_CFG_FRAME_FLESH_CNT_THRS,
	SP5K_FACE_TRACK_CFG_FRAME_FLESH_SCORE_THRS  = FTRACK_CFG_FRAME_FLESH_SCORE_THRS,
	SP5K_FACE_TRACK_CFG_PROC_PB_DETECT_LIST_SEL = FTRACK_CFG_PROC_PB_DETECT_LIST_SEL,
	SP5K_FACE_TRACK_CFG_PROC_COMPACT_BUF_MODE   = FTRACK_CFG_PROC_COMPACT_BUF_MODE,
	SP5K_FACE_TRACK_CFG_PROC_CUSTOM_DB_PATH     = FTRACK_CFG_PROC_CUSTOM_DB_PATH,
	SP5K_FACE_TRACK_CFG_TOTAL                   = FTRACK_CFG_TOTAL,
} sp5kFaceTrackCfgSel_t;

typedef enum {
	SP5K_FORWARD_COLLISION_WARN_DISTANCE_SET		= FORWARD_COLLISION_WARN_DISTANCE_SET,
	SP5K_FORWARD_COLLISION_ROI_CROP_FACTOR_SET	    = FORWARD_COLLISION_ROI_CROP_FACTOR_SET,
	SP5K_FORWARD_COLLISION_ROI_CROP_OFFSET_SET	    = FORWARD_COLLISION_ROI_CROP_OFFSET_SET,
	SP5K_FORWARD_COLLISION_NIGHT_YUV_LUT_TABLE_SET  = FORWARD_COLLISION_NIGHT_YUV_LUT_TABLE_SET,
	SP5K_FORWARD_COLLISION_SENSOR_PIXEL_UNIT_SET	= FORWARD_COLLISION_SENSOR_PIXEL_UNIT_SET,
	SP5K_FORWARD_COLLISION_SENSOR_FOCAL_SET		    = FORWARD_COLLISION_SENSOR_FOCAL_SET,
	SP5K_FORWARD_COLLISION_DAYNIGHT_SET			    = FORWARD_COLLISION_DAYNIGHT_SET,
	SP5K_FORWARD_COLLISION_WARN_DISTANCE_GET		= FORWARD_COLLISION_WARN_DISTANCE_GET,
	SP5K_FORWARD_COLLISION_ROI_CROP_FACTOR_GET		= FORWARD_COLLISION_ROI_CROP_FACTOR_GET,
	SP5K_FORWARD_COLLISION_ROI_CROP_OFFSET_GET		= FORWARD_COLLISION_ROI_CROP_OFFSET_GET,
	SP5K_FORWARD_COLLISION_SENSOR_PIXEL_UNIT_GET	= FORWARD_COLLISION_SENSOR_PIXEL_UNIT_GET,
	SP5K_FORWARD_COLLISION_SENSOR_FOCAL_GET		    = FORWARD_COLLISION_SENSOR_FOCAL_GET,
	SP5K_FORWARD_COLLISION_DISTANCE_GET 			= FORWARD_COLLISION_DISTANCE_GET,
	SP5K_FORWARD_COLLISION_WARNING_FLAG_GET 		= FORWARD_COLLISION_WARNING_FLAG_GET,
	SP5K_FORWARD_COLLISION_DAYNIGHT_GET		        = FORWARD_COLLISION_DAYNIGHT_GET,
	SP5K_FORWARD_COLLISION_CFG_TOTAL				= FORWARD_COLLISION_CFG_TOTAL,
} sp5kVehicleDetectCfgSel_t;

/**
 * \param cfg in ftrackCfgSel_t
 */
UINT32 sp5kFaceTrackCfgSet(UINT32 cfg, UINT32 val);
UINT32 sp5kFaceTrackCfgGet(UINT32 cfg, UINT32 *pval);

/**< face types bitwise-OR bitmap */
#define SP5K_FACE_TRACK_FACE_TYPE_SMILE   FTRACK_FACE_TYPE_SMILE
#define SP5K_FACE_TRACK_FACE_TYPE_BLINK   FTRACK_FACE_TYPE_BLINK
#define SP5K_FACE_TRACK_FACE_TYPE_KEYFACE FTRACK_FACE_TYPE_KEYFACE

/**< Rotation axis is normal line on guy's nose, while viewer see the guy in picture:
     it's 0 degree when guy's forehead is up in picture,
     it's 90 degrees when guy's forhead is left in picture (counter-clockwise 90 degrees),
     it's 270 degrees when guy's forhead is right in picture (clockwise 90 degrees).
 */
typedef enum {
	SP5K_FACE_TRACK_FACE_ANGLE_0   = FTRACK_FACE_ANGLE_0,
	SP5K_FACE_TRACK_FACE_ANGLE_30  = FTRACK_FACE_ANGLE_30,
	SP5K_FACE_TRACK_FACE_ANGLE_60  = FTRACK_FACE_ANGLE_60,
	SP5K_FACE_TRACK_FACE_ANGLE_90  = FTRACK_FACE_ANGLE_90,
	SP5K_FACE_TRACK_FACE_ANGLE_120 = FTRACK_FACE_ANGLE_120,
	SP5K_FACE_TRACK_FACE_ANGLE_150 = FTRACK_FACE_ANGLE_150,
	SP5K_FACE_TRACK_FACE_ANGLE_180 = FTRACK_FACE_ANGLE_180,
	SP5K_FACE_TRACK_FACE_ANGLE_210 = FTRACK_FACE_ANGLE_210,
	SP5K_FACE_TRACK_FACE_ANGLE_240 = FTRACK_FACE_ANGLE_240,
	SP5K_FACE_TRACK_FACE_ANGLE_270 = FTRACK_FACE_ANGLE_270,
	SP5K_FACE_TRACK_FACE_ANGLE_300 = FTRACK_FACE_ANGLE_300,
	SP5K_FACE_TRACK_FACE_ANGLE_330 = FTRACK_FACE_ANGLE_330,
} sp5kFaceTrackFaceAngle_e;

/**< upon receiving host message SP5K_MSG_FACE_TRACK_READY, host calls */
/**
 * \param param is the param attached to the received host message for checking
 * \param pnface points to a UINT32 in which ftrack system fills number
 * of faces found
 * \param presult points to
 * (TBD) user prepared space or system prepared space??
 * presult[ 0.. *pnface-1 ] are properties of those faces,
 * x, y, size, attr: type of face,
 * where type is (TBD) NORMAL_FACE, SMILE_FACE, BLINK_FACE (eye closed),
 * (TBD, bitmap) MAJOR_FACE, FRONT_FACE, SIDE_FACE, 0,90,180,270_ROTATE, etc...
 */
typedef ftrackFaceRes_t sp5kFaceTrackFaceRes_t;
typedef vehicleDetectGetInfo_t sp5kVehicleDetectGetInfo_t;

SINT32 sp5kFaceTrackResultGet(UINT32 param, UINT32 *pnface,
		sp5kFaceTrackFaceRes_t *presult);

/**
 * this API is an alternative for getting face type without using
 * sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_XXX_DETECT,1);
 * that the face type would be embedded in sp5kFaceTrackResultGet();
 * this API is discouraged to use other than in the callback registered by
 * sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_EACH_FACE_CB,fp);
 *
 * \param param passed by each face callback
 * \param faceidx passed by each face callback
 * \param checktype a bitwise-OR bitmap requesting which types to check
 * \param ptype a pointer to bitwise-OR bitmap which will contain type result
 */
SINT32 sp5kFaceTrackFaceTypeGet(UINT32 param, UINT32 faceidx,
		UINT32 checktype, UINT32 *ptype);

/**< TBD */
/**
 * to provide specific APIs for drawing boxes around faces?
 * or auto draw rectangles?
 */

/**
 * \param nrect is number of rectangles to draw
 * \param prect points to a user prepared stream of UINT32, where
 * elements prect[ 0.. nrect-1 ] are properties of those boxes,
 * LSB0: x, LSB1: y, LSB2: size, LSB3: box palette index (see next)
 */
UINT32 sp5kFaceTrackRectDraw(UINT32 nrect, UINT32 *prect);

/**
 * \param npal is number of palette items
 * \param ppal points to a user prepared stream of UINT32, where
 * elements ppal[ 0.. npal-1 ] are properties of those palette items,
 * LSB[0-2]: palette color RGB888, LSB[3]: box thinkness
 */
UINT32 sp5kFaceTrackRectPaletteSet(UINT32 npal, UINT32 *ppal);

UINT32 sp5kVehicleDetectCfgSet(UINT32 cfg,UINT32 val);
UINT32 sp5kVehicleDetectCfgGet(UINT32 cfg,UINT32 *pval);
UINT32 sp5kVehicleDetectResultGet(vehicleDetectGetInfo_t *presult);


/**< TBD */
/**
 * to provide auto capture trigger when all the faces are detected smile
 * without any face blink (eye closed)?
 */

/**< TBD */
/**
 * to provide specific APIs for auto face tracking AF?
 */

/**
 * FIXME
 * major face?
 * optical zoom?
 */

#ifdef __cplusplus
}
#endif

#endif /* _SP5K_FTRACK_API_H_ */
