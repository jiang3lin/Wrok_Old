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
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/
#ifndef _REDEYE_DEF_H_
#define _REDEYE_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct redeyeFaceInfo_s {
	UINT32 faceX;		/* ROI start x */
	UINT32 faceY;		/* ROI start y */
	UINT32 faceSize;	/* ROI width/height */
	UINT32 faceAngle;   /* The angle of this face */
} redeyeFaceInfo_t;

typedef struct redeyeEyeInfo_s {
	UINT32 eyeX;	/* ROI start x */
	UINT32 eyeY;	/* ROI start y */
	UINT32 eyeW;	/* ROI width */
	UINT32 eyeH;	/* ROI height */
} redeyeEyeInfo_t;

typedef struct redeyeEyeCircleInfo_s {
	UINT16 centerX; /* center x */
	UINT16 centerY; /* center y */
	UINT16 radius;  /* radius */
	UINT16 index;   /* face index */
} redeyeEyeCircleInfo_t;

typedef enum {
	REDEYE_FACE_ANGLE_0 = 0x00,
	REDEYE_FACE_ANGLE_30,
	REDEYE_FACE_ANGLE_60,
	REDEYE_FACE_ANGLE_90,
	REDEYE_FACE_ANGLE_120,
	REDEYE_FACE_ANGLE_150,
	REDEYE_FACE_ANGLE_180,
	REDEYE_FACE_ANGLE_210,
	REDEYE_FACE_ANGLE_240,
	REDEYE_FACE_ANGLE_270,
	REDEYE_FACE_ANGLE_300,
	REDEYE_FACE_ANGLE_330,
	REDEYE_FACE_ANGLE_NONE = 0xFF,
} redeyeFaceAngle_e;

#endif /* _REDEYE_DEF_H_ */
