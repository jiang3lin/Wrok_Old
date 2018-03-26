/*******************************************************************************
*
*       Copyright (c) 2009 by Sunplus mMedia Inc., Ltd.
*
*  This software is copyrighted by and is the property of Sunplus mMedia
*  Inc., Ltd. All rights are reserved by Sunplus mMedia Inc., Ltd.
*  This software may only be used in accordance with the corresponding
*  license agreement. Any unauthorized use, duplication, distribution,
*  or disclosure of this software is expressly forbidden.
*
*  This Copyright notice MUST not be removed or modified without prior
*  written consent of Sunplus mMedia Inc., Ltd.
*
*  Sunplus mMedia Inc., Ltd. reserves the right to modify this software
*  without notice.
*
*  Sunplus mMedia Inc., Ltd.
*  19-1, Innovation First Road, Science-Based Industrial Park,
*  Hsin-Chu, Taiwan, R.O.C.
*
*  File Description:
*
*  Author: Matt Wang
*
*******************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 6955 $
$Date:: 2011-02-25 23:32:16#$
 */

#ifndef _PIP_DEF_H_
#define _PIP_DEF_H_

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
typedef enum _pipInitCfg_e {
	PIP_INIT_CFG_MAIN_FRAME_SIZE,
	PIP_INIT_CFG_TOT,
} pipInitCfg_e;

/*******************************************************************************
*                              D A T A   T Y P E S
*******************************************************************************/
typedef struct _pipLayerCfg_t {
	UINT32 frmW;     /* frame buffer width. */
	UINT32 frmH;     /* frame buffer height. */
	UINT32 frmRoiW;  /* frame buffer roi width. */
	UINT32 frmRoiH;  /* frame buffer roi height. */
	SINT32 winX;     /* window x offset on a/b frame. */
	SINT32 winY;     /* window y offset on a/b frame. */
	UINT32 isShow;   /* 0: layer hidden, 1: layer shown. */
	UINT32 alpha;    /* layer alpha. */
} pipLayerCfg_t;

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* pip_layer.c */
UINT32 pipLayerCreate(UINT32 layerId, pipLayerCfg_t *pcfg);
UINT32 pipLayerDelete(UINT32 layerId);

#endif  /* _PIP_DEF_H_ */

