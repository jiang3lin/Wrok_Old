/**************************************************************************
 *
 *        Copyright (c) 2009 by Sunplus mMedia Inc., Ltd.
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
 *  Author: Yi-Ming Kao
 *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 6955 $
$Date:: 2011-02-25 23:32:16#$
 */


/**
 * @file mview_def.h
 * @brief The common definition of multiple preview function.
 * @author Yi-Ming Kao
 */
#ifndef _MVIEW_DEF_H_
#define _MVIEW_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

/** The selector of multiple preview configuration */
typedef enum {
	/** generate next frame after # of sensor frames */
	MVIEW_CFG_ACCUM_PERIOD,      
	MVIEW_CFG_WINDOW_NUM,        /*!< window # of each frame */
	MVIEW_CFG_WINDOW_MAX_WIDTH,  /*!< maximum window width */
	MVIEW_CFG_WINDOW_MAX_HEIGHT, /*!< maximum window height */
	MVIEW_CFG_WINDOW_INFO,       /*!< the window related information */
	MVIEW_CFG_WINDOW_PRE_CB,     /*!< pre-process callback of each window */
	MVIEW_CFG_WINDOW_POST_CB,    /*!< post-process callback of each window */
	MVIEW_CFG_FRAME_PRE_CB,      /*!< pre-process callback of each frame */
	MVIEW_CFG_FRAME_RAW_CB,      /*!< the raw callback for each frame */
} mviewCfgSel_t;


/**
 * @struct mviewWindow_t
 * @brief The information of a window
 *
 *   The base firmware and host passes the window related information by it.
 * There are two places to be used:
 * - Callbak: base firmware provides the generated window informaion to host.
 * - Configure: host wants to change the target size of the window.
 *
 * @note The width and height might not the same as defined through the selector
 * @a MVIEW_CFG_WINDOW_MAX_WIDTH and @a MVIEW_CFG_WINDOW_MAX_HEIGHT when it is
 * used in callback. The output width and height will be decided by the smallest
 * one of the following.
 * - Preview frame output size.
 * - The maximum value set by MVIEW_WINDOW_MAX_WIDTH, MVIEW_WINDOW_MAX_HEIGHT
 * - The configuration of each window size through MVIEW_CFG_WINDOW_INFO.
 */
typedef struct {
	UINT32 id;       /*!< the sequence identifier */
	UINT8 *addr;   /*!< logical byte address of this yuv window */
	UINT32 width;  /*!< width */
	UINT32 height; /*!< height */
} mviewWindow_t;


/**
 * @struct mviewRawInfo_t
 * @brief The information of raw information for each frame. It is same as
 * snapImgWbInfo_t and should be uniformed.
 *
 * The raw callback of mview is invoked before doing the multiple preview.
 */
typedef struct {
	UINT32 width;      /*!< the dram width */
	UINT32 height;     /*!< the dram height */
	UINT32 cropWidth;  /*!< the really raw width, start from left */
	UINT32 cropHeight; /*!< the really raw height */
	UINT8 rowOfs;      /*!< unused in mview */
	UINT8 row0;        /*!< unused in mview */
	UINT8 row1;        /*!< unused in mview */
	UINT8 bayerOrder;  /*!< the bayer pattern sequence */
} mviewRawInfo_t;

#ifdef __cplusplus
}
#endif

#endif /* _MVIEW_DEF_H_ */
