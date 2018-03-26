/**************************************************************************
 *
 *      Copyright (c) 2008-2009 by Sunplus mMedia Inc., Ltd.
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
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 6955 $
$Date:: 2011-02-25 23:32:16#$
 */
#ifndef _OTRACK_DEF_H_
#define _OTRACK_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "middleware/modesw_def.h"


/**
 * objid, size, used in otrackTargetStart/Stop etc,
 */
#define OTRACK_TARGET_ID_ALL      (-1)
#define OTRACK_TARGET_ID_INVALID  0

#define OTRACK_TARGET_SIZE_AUTO   0

/**
 * \param cfg in
 * UINT32 sp5kObjTrackCfgSet(UINT32 cfg,UINT32 val);
 */
typedef enum {
	OTRACK_CFG_ACCUM_PERIOD,
	OTRACK_CFG_TARGET_CNT_MAX, /*max count of result obj*/
	OTRACK_CFG_MIN_OBJ_SIZE, /*min size of obj*/
	OTRACK_CFG_TIMEOUT,
	OTRACK_CFG_PROC_REF_YPOP_THRS,
	OTRACK_CFG_MANU_MODE_DEV,
	OTRACK_CFG_PROC_EACH_OBJ_CB,
	OTRACK_CFG_PROC_DONE_CB,
	OTRACK_CFG_SEARCH_RANGE, /*in times of target size*/
	OTRACK_CFG_POS_HIS_DEPTH,
	OTRACK_CFG_SAD_COST_ARRAY_DBG,
	OTRACK_CFG_PROC_OUTBUF_NUM, /*at least 1, max 8*/
	OTRACK_CFG_RES_SMOOTH_NUM, /*0:no smooth; n:smooth from n results*/
	OTRACK_CFG_TOTAL,
} otrackCfgSel_t;


#define OTRACK_OBJ_MODE_IDLE_BMAP    0
#define OTRACK_OBJ_MODE_START_BMAP   0xc0000000
#define OTRACK_OBJ_MODE_FOUND_BMAP   0x80000000
#define OTRACK_OBJ_MODE_MISS_BMAP    0x40000000
#define OTRACK_OBJ_MODE_IDLE         0
#define OTRACK_OBJ_MODE_START        3
#define OTRACK_OBJ_MODE_FOUND        2
#define OTRACK_OBJ_MODE_MISS         1


typedef union {
	struct otrackPos_s {
		UINT32 x: 10;
		UINT32 y: 10;
		UINT32 sz:10;
		UINT32 status:2;
	} elem;
	UINT32 l;
} otrackPos_t;

typedef struct {
	UINT32 objid;
	otrackPos_t pos;
} otrackResult_t;

/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
typedef modeswFrameBuf_t otrackFrameBuf_t;


#ifdef __cplusplus
}
#endif

#endif /* _OTRACK_DEF_H_ */
