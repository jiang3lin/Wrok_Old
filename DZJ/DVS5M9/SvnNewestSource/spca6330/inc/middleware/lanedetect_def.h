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
#ifndef _LANEDETECT_DEF_H_
#define _LANEDETECT_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/modesw_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/

/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
 #if 0
typedef struct redeyeFaceInfo_s {
	UINT32 faceX;		/* ROI start x */
	UINT32 faceY;		/* ROI start y */
	UINT32 faceSize;	/* ROI width/height */
	UINT32 faceAngle;   /* The angle of this face */
} redeyeFaceInfo_t;
#endif

typedef struct{
	int thita_L[5];
	int thita_R[5];
	int r_L[5];
	int r_R[5];
}LD_HOUGH_DATA;
typedef struct {
	UINT32 dw0,dw1,dw2,dw3,dw4,dw5,dw6,dw7,dw8,dw9,dw10,dw11
		#if SPCA5330
		,
		dw12,
		dw13,
		dw14,
		dw15
		#endif
		;
}moeQue_t;

typedef struct {
	UINT32 ftx1;
	UINT32 ftx2;
	UINT32 ftx3;
	UINT32 ftx4;
	UINT32 fty1;
	UINT32 fty2;
	UINT32 fty3;
	UINT32 fty4;
	UINT32 ROI_h_end;
}lanePosotion;

typedef struct {
	UINT8 leftLaneSignal;
	UINT8 rightLaneSignal;
}departureWarning;

 typedef struct {
	SINT32 min;	/*!< 2's complement */
	SINT32 max; /*!< 2's complement */
} MoeClipCfg_t;

typedef struct {
	SINT16 para0;	/*!< 1S10bit */
	SINT16 para1;	/*!< 1S10bit */
	SINT16 para2;	/*!< 1S10bit */
	SINT16 para3;	/*!< 1S10bit */
	SINT16 para4;	/*!< 1S10bit */
	SINT16 para5;	/*!< 1S10bit */
	SINT16 para6;	/*!< 1S10bit */
	SINT16 para7;	/*!< 1S10bit */
	SINT16 para8;	/*!< 1S10bit */
	SINT16 para9;	/*!< 1S10bit */
} MoeFilterObj_t;
typedef struct {
	UINT8 topMrrPixel;/*!< 0 ~ 3 */
	UINT8 topMrrEn;/*!< 0 or 1 */
	UINT8 botMrrPixel;/*!< 0 ~ 3 */
	UINT8 botMrrEn;/*!< 0 or 1 */
	UINT8 leftMrrPixel;/*!< 0 ~ 3 */
	UINT8 leftMrrEn;/*!< 0 or 1 */
	UINT8 rightMrrPixel;/*!< 0 ~ 3 */
	UINT8 rightMrrEn;/*!< 0 or 1 */
	UINT8 reserved;/*!< should be 0 */
} MoeMirrorCfg_t;

typedef struct {
	void *pbuf;
	UINT16 width;	/*!< max 8184, multiple of 8 */
	UINT16 height;	/*!< max 4095 */
	UINT16 x;		/*!< max width-1 */
	UINT16 y;		/*!< max height-1 */
	UINT8 fmt;		/*!< MoeDataFmt_t */
	UINT8 sign;		/*!< MoeDataType_t */
	UINT8 shift;	/*!< max 16, left shift for src, right shift for dst */
	UINT8 abs;		/*!< see MoeDataAbs_t */
} MoeObj_t;

typedef struct {
	UINT16 common;
	UINT8 sign;		/*!< MoeDataType_t */
	UINT8 shift;	/*!< left shift */
} MoeComObj_t;

/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
typedef modeswFrameBuf_t laneDetectFrameBuf_t;

#endif
