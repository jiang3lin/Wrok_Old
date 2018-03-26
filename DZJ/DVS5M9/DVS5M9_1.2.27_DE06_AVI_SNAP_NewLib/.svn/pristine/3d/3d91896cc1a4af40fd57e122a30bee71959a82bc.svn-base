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
 *  Author: Matt Wang
 *
 **************************************************************************/
#ifndef _SP5K_GFX_API_H_
#define _SP5K_GFX_API_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/gfx_def.h"
#include "middleware/fnt_def.h"
#include "middleware/str_def.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
#define GFX_PRINTF_LEVEL  2

#define SP5K_GFX_VECTOR_LAYER

/* sp5kGfxInitCfgSet(). */
typedef enum _sp5kGfxInitCfg_e {
	SP5K_GFX_INIT_DISP_SRC                       = GFX_INIT_DISP_SRC,
	SP5K_GFX_INIT_LAYER_TOT                      = GFX_INIT_LAYER_TOT,
	SP5K_GFX_INIT_FRAME_SIZE                     = GFX_INIT_FRAME_SIZE,
	SP5K_GFX_INIT_VIRTUAL_FRAME_SIZE             = GFX_INIT_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_INIT_LAYER_WINDOW_OFFSET            = GFX_INIT_LAYER_WINDOW_OFFSET,
	SP5K_GFX_INIT_LAYER_WINDOW_SIZE              = GFX_INIT_LAYER_WINDOW_SIZE,
	SP5K_GFX_INIT_LAYER_VIRTUAL_WINDOW_OFFSET    = GFX_INIT_LAYER_VIRTUAL_WINDOW_OFFSET,
	SP5K_GFX_INIT_LAYER_VIRTUAL_WINDOW_SIZE      = GFX_INIT_LAYER_VIRTUAL_WINDOW_SIZE,
	SP5K_GFX_INIT_LAYER_FRAME_SIZE               = GFX_INIT_LAYER_FRAME_SIZE,
	SP5K_GFX_INIT_LAYER_FRAME_ROI_OFFSET         = GFX_INIT_LAYER_FRAME_ROI_OFFSET,
	SP5K_GFX_INIT_LAYER_FRAME_ROI_SIZE           = GFX_INIT_LAYER_FRAME_ROI_SIZE,
	SP5K_GFX_INIT_LAYER_VIRTUAL_FRAME_SIZE       = GFX_INIT_LAYER_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_OFFSET = GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_OFFSET,
	SP5K_GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_SIZE   = GFX_INIT_LAYER_VIRTUAL_FRAME_ROI_SIZE,
	SP5K_GFX_INIT_LAYER_TYPE                     = GFX_INIT_LAYER_TYPE,
	SP5K_GFX_INIT_LAYER_FORMAT                   = GFX_INIT_LAYER_FORMAT,
	SP5K_GFX_INIT_LAYER_CMD_TOT                  = GFX_INIT_LAYER_CMD_TOT,
	SP5K_GFX_INIT_LAYER_ACTIVE                   = GFX_INIT_LAYER_SHOWN,
	SP5K_GFX_INIT_LAYER_WITH_ALPHA_PLANE         = GFX_INIT_LAYER_WITH_ALPHA_PLANE,
	SP5K_GFX_INIT_LAYER_WIDTH_3X                 = GFX_INIT_LAYER_WIDTH_3X,
	SP5K_GFX_INIT_CFG_NEW_TOT,

	SP5K_GFX_AB_FRAME_SIZE                       = SP5K_GFX_INIT_CFG_NEW_TOT,
	SP5K_GFX_AB_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_USR_PAGE_ID,
	SP5K_GFX_USR_FMT,
	SP5K_GFX_USR_LAYER_DEFAULT_TYPE,
	SP5K_GFX_USR_FRAME_SIZE,
	SP5K_GFX_USR_FRAME_ROI,
	SP5K_GFX_USR_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_USR_VIRTUAL_FRAME_ROI,
	SP5K_GFX_USR_WINDOW,
	SP5K_GFX_USR_OBJ_TOT,
	SP5K_GFX_USR_POOL_SIZE,
	SP5K_GFX_USR_WITH_ALPHA_PLANE,
	SP5K_GFX_USR_WIDTH_3X,
	SP5K_GFX_OSD_PAGE_TOT,
	SP5K_GFX_OSD_FMT,
	SP5K_GFX_OSD_LAYER_DEFAULT_TYPE,
	SP5K_GFX_OSD_FRAME_SIZE,
	SP5K_GFX_OSD_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_OSD_WINDOW,
	SP5K_GFX_OSD_OBJ_TOT,
	SP5K_GFX_OSD_POOL_SIZE,
	SP5K_GFX_OSD_WITH_ALPHA_PLANE,
	SP5K_GFX_OSD_WIDTH_3X,
	SP5K_GFX_PIP_PAGE_TOT,
	SP5K_GFX_PIP_FMT,
	SP5K_GFX_PIP_LAYER_DEFAULT_TYPE,
	SP5K_GFX_PIP_FRAME_SIZE,
	SP5K_GFX_PIP_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_PIP_WINDOW,
	SP5K_GFX_PIP_OBJ_TOT,
	SP5K_GFX_PIP_POOL_SIZE,
	SP5K_GFX_PIP_WITH_ALPHA_PLANE,
	SP5K_GFX_PIP_WIDTH_3X,
	SP5K_GFX_PF_PAGE_TOT,
	SP5K_GFX_PF_FMT,
	SP5K_GFX_PF_LAYER_DEFAULT_TYPE,
	SP5K_GFX_PF_FRAME_SIZE,
	SP5K_GFX_PF_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_PF_WINDOW,
	SP5K_GFX_PF_OBJ_TOT,
	SP5K_GFX_PF_POOL_SIZE,
	SP5K_GFX_PF_WITH_ALPHA_PLANE,
	SP5K_GFX_PF_WIDTH_3X,
	SP5K_GFX_VCT_PAGE_TOT,
	SP5K_GFX_VCT_FMT,
	SP5K_GFX_VCT_LAYER_DEFAULT_TYPE,
	SP5K_GFX_VCT_FRAME_SIZE,
	SP5K_GFX_VCT_VIRTUAL_FRAME_SIZE,
	SP5K_GFX_VCT_WINDOW,
	SP5K_GFX_VCT_OBJ_TOT,
	SP5K_GFX_VCT_POOL_SIZE,
	SP5K_GFX_INIT_CFG_TOT,
} sp5kGfxInitCfg_e;

/* display source. */
typedef enum _sp5kGfxDispSrc_e {
	SP5K_GFX_DISP_SRC_PRIMARY   = GFX_DISP_SRC_PRIMARY,
	SP5K_GFX_DISP_SRC_SECONDARY = GFX_DISP_SRC_SECONDARY,
} sp5kGfxDispSrc_e;

/* sp5kGfxSysCfgSet */
typedef enum _sp5kGfxSysCfgId_e {
	SP5K_GFX_SYS_CFG_GLOBAL_FNT_ID_ERR_MSG,
	SP5K_GFX_SYS_CFG_PER_PIXEL_BLEND_MODE=0x1000, /* 0:MOE blend, 1:GPE blend, default=0 */
} sp5kGfxSysCfgId_e; /* plz keep sync to gfxBaseGlobalCfgId_e. */

/* layer type. */
typedef enum _sp5kGfxLayerType_e {
	SP5K_GFX_LAYER_TYPE_BITMAP  = 0,  /* default. */
	SP5K_GFX_LAYER_TYPE_VIRTUAL = 1,
} sp5kGfxLayerType_e;

/* data format. */
typedef enum _sp5kGfxFmt_e {
	SP5K_GFX_FMT_YUV422 = 0,
	SP5K_GFX_FMT_RGB565 = 1,
	SP5K_GFX_FMT_IDX8   = 3,
	#if SPCA6330
	SP5K_GFX_FMT_YUV420 = 8,
	#endif
	SP5K_GFX_FMT_JFIF   = 0x1000,
	SP5K_GFX_FMT_JPG    = 0x1001,
} sp5kGfxFmt_e;  /* plz keep sync to gfxDataFmt_e. */

/* page id base, SP5K_GFX_PAGE_USR_0 is the bottomest page. */
typedef enum _sp5kGfxPageNum_e {
	SP5K_GFX_PAGE_USR_0 = 0,   /* 8 pages. */
	SP5K_GFX_PAGE_OSD_0 = 8,   /* 2 pages. */
	SP5K_GFX_PAGE_PIP_0 = 10,  /* 2 pages. */
	SP5K_GFX_PAGE_VCT_0 = 12,  /* 2 page. */
	SP5K_GFX_PAGE_PF_0  = 14,  /* 2 page. */
	SP5K_GFX_PAGE_TOT   = 16,
} sp5kGfxPageNum_e;

typedef enum _sp5kGfxMiscPageNum_e {
	SP5K_GFX_PAGE_SHOWN      = 16,  /* 1 page. */
	SP5K_GFX_PAGE_HIDDEN     = 17,  /* 1 page. */
	SP5K_GFX_PAGE_IMAGE      = 18,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_SHOWN  = 19,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_HIDDEN = 20,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_MAIN   = 21,  /* 1 page. */
	SP5K_GFX_PAGE_PIP_BASE   = 22,  /* 8 pages. */
	SP5K_GFX_PAGE_APLANE     = 32,  /* alpha plane of SP5K_GFX_PAGE_XXX in sp5kGfxPageNum_e is (SP5K_GFX_PAGE_ALPHA + SP5K_GFX_PAGE_XXX). */
} sp5kGfxMiscPageNum_e;

/* sp5kGfxAttrSet(). */
typedef enum _sp5kGfxAttr_e {
	SP5K_GFX_TRANS_COLOR,       /* equals to set both SP5K_GFX_IDX8_TRANS_COLOR + SP5K_GFX_RGB565_TRANS_COLOR. */
	SP5K_GFX_PEN_ENABLE,        /* equals to set both SP5K_GFX_IDX8_PEN_ENABLE + SP5K_GFX_RGB565_PEN_ENABLE. */
	SP5K_GFX_PEN_COLOR,         /* equals to set both SP5K_GFX_IDX8_PEN_COLOR + SP5K_GFX_RGB565_PEN_COLOR. */
	SP5K_GFX_PEN_WIDTH,         /* equals to set both SP5K_GFX_IDX8_PEN_WIDTH + SP5K_GFX_RGB565_PEN_WIDTH. */
	SP5K_GFX_FILL_ENABLE,       /* equals to set both SP5K_GFX_IDX8_FILL_ENABLE + SP5K_GFX_RGB565_FILL_ENABLE. */
	SP5K_GFX_FILL_COLOR,        /* equals to set both SP5K_GFX_IDX8_FILL_COLOR + SP5K_GFX_RGB565_FILL_COLOR. */
	SP5K_GFX_TEXT_PITCH,        /* equals to set both SP5K_GFX_IDX8_TEXT_PITCH + SP5K_GFX_RGB565_TEXT_PITCH. */
	SP5K_GFX_PIC_SCALE_FACTOR,  /* equals to set both SP5K_GFX_IDX8_PIC_SCALE_FACTOR + SP5K_GFX_RGB565_PIC_SCALE_FACTOR. */
	SP5K_GFX_PIC_SCALE_INIT,    /* equals to set both SP5K_GFX_IDX8_PIC_SCALE_INIT + SP5K_GFX_RGB565_PIC_SCALE_INIT. */
	SP5K_GFX_BLEND_FACTOR,      /* equals to set both SP5K_GFX_IDX8_BLEND_FACTOR + SP5K_GFX_RGB565_BLEND_FACTOR. */
	SP5K_GFX_PIC_SCALE_ANTIALIAS, /* equals to set both SP5K_GFX_IDX8_SCALE_ANTIALIAS + SP5K_GFX_RGB565_SCALE_ANTIALIAS. */
	SP5K_GFX_PIC_ROTATE,
	SP5K_GFX_ATTR_TOT,

	SP5K_GFX_IDX8_TRANS_COLOR = SP5K_GFX_ATTR_TOT,
	SP5K_GFX_IDX8_PEN_ENABLE,
	SP5K_GFX_IDX8_PEN_COLOR,
	SP5K_GFX_IDX8_PEN_WIDTH,
	SP5K_GFX_IDX8_FILL_ENABLE,
	SP5K_GFX_IDX8_FILL_COLOR,
	SP5K_GFX_IDX8_TEXT_PITCH,
	SP5K_GFX_IDX8_PIC_SCALE_FACTOR,
	SP5K_GFX_IDX8_PIC_SCALE_INIT,
	SP5K_GFX_IDX8_BLEND_FACTOR,
	SP5K_GFX_IDX8_SCALE_ANTIALIAS,
	SP5K_GFX_IDX8_ROTATE,
	SP5K_GFX_IDX8_ATTR_TOT,

	SP5K_GFX_RGB565_TRANS_COLOR = SP5K_GFX_IDX8_ATTR_TOT,
	SP5K_GFX_RGB565_PEN_ENABLE,
	SP5K_GFX_RGB565_PEN_COLOR,
	SP5K_GFX_RGB565_PEN_WIDTH,
	SP5K_GFX_RGB565_FILL_ENABLE,
	SP5K_GFX_RGB565_FILL_COLOR,
	SP5K_GFX_RGB565_TEXT_PITCH,
	SP5K_GFX_RGB565_PIC_SCALE_FACTOR,
	SP5K_GFX_RGB565_PIC_SCALE_INIT,
	SP5K_GFX_RGB565_BLEND_FACTOR,
	SP5K_GFX_RGB565_SCALE_ANTIALIAS,
	SP5K_GFX_RGB565_ROTATE,
	SP5K_GFX_RGB565_ATTR_TOT,

	SP5K_GFX_ICON_ID = SP5K_GFX_RGB565_ATTR_TOT,
	SP5K_GFX_FONT_ID,
	SP5K_GFX_YUV422_TO_RGB565_DITHER_ENABLE,
	/**
	 * \note
		  para1 = 0: disable, 1: enable.
		  para2 = 0: waiting for dirty clear, 1: no waiting.
		  para3 = 0: not, 1: disable refresh when done.
	  	  para4 = 0: not, 1: toggle a/b (shown/hidden) frame.
	              2: blend all layers to a/b frame (no clear) then toggle.
	              3: toggle a/b (show/hidden) frame but not do zfupdate.
	              4: blend all layers to a/b frame but not toggle.
	 */
	SP5K_GFX_REFRESH_ACTIVE,
	SP5K_GFX_ORDER_REFRESH, /*!< must be under refresh done and disable */
	SP5K_GFX_LAYER_ACTIVE,
	SP5K_GFX_LAYER_ALPHA,
	SP5K_GFX_LAYER_WINDOW_OFFSET,
	SP5K_GFX_LAYER_WINDOW_SIZE,
	SP5K_GFX_LAYER_FRAME_ROI_OFFSET,
	SP5K_GFX_LAYER_FRAME_ROI_SIZE,
	SP5K_GFX_LAYER_TYPE,
	SP5K_GFX_LAYER_RENDER_ORDER, /*!< the less number means the topper layer */
	SP5K_GFX_LAYER_ATTR_TOT,

	/**
	 * \note
	 * param1: 0=disable, 1=enable
	 * param2: 0=wait refresh done, 1=not
	 * param3: 0=not, 1:disable refresh after done
	 * param4: don't care
	 */
	SP5K_GFX_PIP_REFRESH_ACTIVE = SP5K_GFX_LAYER_ATTR_TOT,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: 0:hide, 1: show
	 */
	SP5K_GFX_PIP_LAYER_ACTIVE,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: alpha value, 0~32
	 */
	SP5K_GFX_PIP_LAYER_ALPHA,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: offset x of window
	 * param3: offset y of window
	 */
	SP5K_GFX_PIP_LAYER_WINDOW_OFFSET,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: layer frame roi x offset.
	 * param3: layer frame roi y offset.
	*/
	SP5K_GFX_PIP_LAYER_FRAME_ROI_OFFSET,
	/**
	 * \note
	 * param1: gfx pip page id
	 * param2: layer frame roi width
	 * param3: layer frame roi height.
	*/
	SP5K_GFX_PIP_LAYER_FRAME_ROI_SIZE,
	/**
	 * \note
	 * if y value is less than the threshold, the pixel will be treated
	 * as color key
	 * param1: gfx pip page id
	 * param2: y threshold from 0 to 255
	 * param3: 0:color key disable (default), 1:enable
	 */
	SP5K_GFX_PIP_LAYER_COLOR_KEY,
	SP5K_GFX_PIP_LAYER_ATTR_TOT,
	SP5K_GFX_MERGE_DEFAULT_COLOR,
} sp5kGfxAttr_e;

typedef enum _sp5kGfxIconId_e {
	SP5K_GFX_FONT_ID_1 = FNT_TYPE_FONT,
	SP5K_GFX_ICON_ID_1 = FNT_TYPE_ICON,
	SP5K_GFX_ICON_ID_2 = FNT_TYPE_ICON_2,
	SP5K_GFX_ICON_ID_3 = FNT_TYPE_ICON_3,
	SP5K_GFX_ICON_ID_4 = FNT_TYPE_ICON_4,
	SP5K_GFX_ICON_ID_5 = FNT_TYPE_ICON_5,
	SP5K_GFX_STAMP     = FNT_TYPE_STAMP,
	SP5K_GFX_FONT_ID_2 = 100,
} sp5kGfxIconId_e;

/* sp5kGfxIconFileDraw(), sp5kGfxStringDraw(), sp5kGfxStringIdDraw(). */
typedef enum _sp5kGfxAlign_e {
	SP5K_GFX_ALIGN_TOP_LEFT     = GFX_ALIGN_TOP_LEFT,
	SP5K_GFX_ALIGN_TOP          = GFX_ALIGN_TOP_CENTER,
	SP5K_GFX_ALIGN_TOP_RIGHT    = GFX_ALIGN_TOP_RIGHT,
	SP5K_GFX_ALIGN_LEFT         = GFX_ALIGN_LEFT,
	SP5K_GFX_ALIGN_CENTER       = GFX_ALIGN_CENTER,
	SP5K_GFX_ALIGN_RIGHT        = GFX_ALIGN_RIGHT,
	SP5K_GFX_ALIGN_BOTTOM_LEFT  = GFX_ALIGN_BOTTOM_LEFT,
	SP5K_GFX_ALIGN_BOTTOM       = GFX_ALIGN_BOTTOM_CENTER,
	SP5K_GFX_ALIGN_BOTTOM_RIGHT = GFX_ALIGN_BOTTOM_RIGHT,
} sp5kGfxAlign_e;

typedef enum _sp5kGfxRotFlip_e {
	SP5K_GFX_ROTATE_0    = 0x00,
	SP5K_GFX_ROTATE_90,
	SP5K_GFX_ROTATE_180,
	SP5K_GFX_ROTATE_270,
	SP5K_GFX_ROTATE_0_FLIP,
	SP5K_GFX_ROTATE_90_FLIP,
	SP5K_GFX_ROTATE_180_FLIP,
	SP5K_GFX_ROTATE_270_FLIP,
	SP5K_GFX_ROTATE_TOT,
} sp5kGfxRotFlip_t;

/* sp5kGfxStringDraw(). */
typedef enum _sp5kGfxEncode_e {
	SP5K_GFX_ENCODE_ASCII   = STR_ENCODE_ASCII,
	SP5K_GFX_ENCODE_UNICODE = STR_ENCODE_UNICODE,
} sp5kGfxEncode_e;

/* sp5kGfxObjectEdit(). */
typedef enum _sp5kGfxObjEdit_e {
	SP5K_GFX_OBJECT_EDIT_CDSP,
	SP5K_GFX_OBJECT_EDIT_MOSAIC,
	SP5K_GFX_OBJECT_EDIT_TOT,
} sp5kGfxObjEdit_t;

typedef enum _sp5kGfxFt2Cfg_e {
	SP5K_GFX_FT2_CHAR_SIZE,
} sp5kGfxFt2AttrCfg_e;

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/* sp5kGfxPolygonDraw() */
typedef struct _sp5kGfxPoint_t {
	SINT32 iX;
	SINT32 iY;
} sp5kGfxPoint_t;

/* sp5kGfxPageCapabilityGet() */
typedef struct _sp5kGfxPageCapabDesc_t {
	UINT8 *pbuf;     /* layer frame buffer. */
	UINT32 frmW;     /* frame buffer width, set by SP5K_GFX_INIT_LAYER_FRAME_SIZE. */
	UINT32 frmH;     /* frame buffer height, set by SP5K_GFX_INIT_LAYER_FRAME_SIZE. */
	SINT32 frmRoiX;  /* frame buffer roi x offset, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_OFFSET or SP5K_GFX_LAYER_FRAME_ROI_OFFSET. */
	SINT32 frmRoiY;  /* frame buffer roi y offset, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_OFFSET or SP5K_GFX_LAYER_FRAME_ROI_OFFSET. */
	UINT32 frmRoiW;  /* frame buffer roi width, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_SIZE or SP5K_GFX_LAYER_FRAME_ROI_SIZE. */
	UINT32 frmRoiH;  /* frame buffer roi height, set by SP5K_GFX_INIT_LAYER_FRAME_ROI_SIZE or SP5K_GFX_LAYER_FRAME_ROI_SIZE. */
	SINT32 winX;     /* window x offset on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_OFFSET or SP5K_GFX_LAYER_WINDOW_OFFSET. */
	SINT32 winY;     /* window y offset on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_OFFSET or SP5K_GFX_LAYER_WINDOW_OFFSET. */
	UINT32 winW;     /* window width on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_SIZE or SP5K_GFX_LAYER_WINDOW_SIZE. */
	UINT32 winH;     /* window height on a/b frame, set by SP5K_GFX_INIT_LAYER_WINDOW_SIZE or SP5K_GFX_LAYER_WINDOW_SIZE. */
	UINT32 type;     /* one of sp5kGfxLayerType_e, set by SP5K_GFX_INIT_LAYER_TYPE. */
	UINT32 fmt;      /* one of sp5kGfxFmt_e, set by SP5K_GFX_INIT_LAYER_FORMAT. */
	UINT32 isShow;   /* 0: layer hidden, 1: layer shown, set by SP5K_GFX_INIT_LAYER_ACTIVE or SP5K_GFX_LAYER_ACTIVE. */
	UINT32 alpha;    /* layer alpha, set by SP5K_GFX_LAYER_ALPHA. */
	UINT8 *pabuf;    /* alpha frame buffer */
} sp5kGfxPageCapabDesc_t;

typedef struct _sp5kGfxPicture_t {
	UINT8 *pbuf;  /* picture frame buffer. */
	UINT32 fmt;   /* one of sp5kGfxFmt_e. */
	UINT32 frmW;  /* frame buffer width. */
	UINT32 frmH;  /* frame buffer height. */
	SINT32 roiX;  /* frame buffer roi x offset. */
	SINT32 roiY;  /* frame buffer roi y offset. */
	UINT32 roiW;  /* frame buffer roi width. */
	UINT32 roiH;  /* frame buffer roi height. */
} sp5kGfxPicture_t;  /* plz keep sync to gfxPicture_t. */

typedef struct _sp5kGfxRoi_t {
	SINT32 roiX;
	SINT32 roiY;
	UINT32 roiW;
	UINT32 roiH;
} sp5kGfxRoi_t;

typedef struct _sp5kGfxObj_t {
	UINT8 *pbuf;
	UINT8 *paplaneBuf;
	UINT32 bufW;
	UINT32 bufH;
	SINT32 roiX;
	SINT32 roiY;
	UINT32 fmt;  /* one of gfxDataFmt_e. */
	UINT32 roiW;
	UINT32 roiH;
} sp5kGfxObj_t;  /* note that roiW & roiH is in sp5kGfxObj_t only. changing gfxObj_t & halXxxObj_t is too risky.
plz keep halGpeObj_t, halIpmObj_t, halRotObj_t and gfxObj_t all in same data structure except roiW & roiH. */

typedef struct _sp5kGfxLayer_t {
	UINT8 *pbuf;          /* layer frame buffer. */
	UINT8 *paplaneBuf;    /* alpha plane frame buffer. */
	UINT32 rsv1[3];
	UINT32 frmW;          /* frame buffer width. */
	UINT32 frmH;          /* frame buffer height. */
	UINT32 rsv2[4];
	SINT32 frmRoiX;       /* frame buffer roi x offset. */
	SINT32 frmRoiY;       /* frame buffer roi y offset. */
	UINT32 frmRoiW;       /* frame buffer roi width. */
	UINT32 frmRoiH;       /* frame buffer roi height. */
	UINT32 rsv3;
	SINT32 winX;          /* window x offset on a/b frame. */
	SINT32 winY;          /* window y offset on a/b frame. */
	UINT32 winW;          /* window width on a/b frame. */
	UINT32 winH;          /* window height on a/b frame. */
	UINT32 rsv4[2];
	UINT32 fmt;           /* one of gfxDataFmt_e. */
	UINT32 rsv5[4];
	UINT32 alpha;         /* layer alpha. */
	UINT32 isWithAplane;  /* 0: without, 1: with alpha plane. */
	UINT32 rsv6[3];
} sp5kGfxLayer_t;
/* plz keep sp5kGfxLayer_t compatible with gfxLayer_t */

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* sp5k_gfx_init.c */
UINT32 sp5kGfxInitCfgSet(UINT32 paraId, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);
UINT32 sp5kGfxInitCfgGet(UINT32 paraId, UINT32 *ppara1, UINT32 *ppara2, UINT32 *ppara3,	UINT32 *ppara4);
UINT32 sp5kGfxInit(void);

/* sp5k_gfx_cfg.c */
UINT32 sp5kGfxSysCfgSet(UINT32 paraId, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);

/* sp5k_gfx_page.c */
UINT32 sp5kGfxPageClear(UINT32 pageId, UINT32 color);
UINT32 sp5kGfxPageCopy(UINT32 srcPageId, UINT32 dstPageId);
UINT32 sp5kGfxPageSelect(UINT32 pageId);
UINT32 sp5kGfxPageSwitch(UINT32 pageId);
UINT32 sp5kGfxPageScale(UINT32 srcPageId, UINT32 dstPageId, SINT32 srcRoiX, SINT32 srcRoiY, UINT32 srcRoiW, UINT32 srcRoiH, SINT32 dstRoiX, SINT32 dstRoiY, UINT32 dstRoiW, UINT32 dstRoiH);
UINT32 sp5kGfxPageCapabilityGet(UINT32 pageId, sp5kGfxPageCapabDesc_t *pcapabDesc);

/* sp5k_gfx_attr.c */
UINT32 sp5kGfxAttrSet(UINT32 paraId, UINT32 para1, UINT32 para2, UINT32 para3, UINT32 para4);

/* sp5k_gfx_icon.c */
UINT32 sp5kGfxIconDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 iconId);
UINT32 sp5kGfxIconFileDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT8 *fileName, UINT32 iconId);
UINT32 sp5kGfxIconSizeGet(UINT32 iconId, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxPictureScale(UINT32 pageId, SINT32 roiX, SINT32 roiY, UINT32 roiW, UINT32 roiH, sp5kGfxPicture_t *ppic);
UINT32 sp5kGfxIconColorSet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);
UINT32 sp5kGfxIconColorGet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);
UINT32 sp5kGfxPictureBlend(sp5kGfxPicture_t *pdstPic, UINT16 alpha, UINT16 keyEn, UINT16 colorKey, sp5kGfxPicture_t *psrcPic);

/* sp5k_gfx_rect.c */
UINT32 sp5kGfxRectDraw(UINT32 pageId, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1);
UINT32 sp5kGfxRectFillDirect(UINT32 pageId, SINT32 x, SINT32 y, UINT32 width, UINT32 height, UINT16 color);
/**
 * \note
 * only for SP5K_GFX_LAYER_TYPE_BITMAP page
 * x:     offset x, multiple of 2 if YUV format
 * y:     offset y
 * width: roi width, multiple of 2 if YUV format
 * height: roi height
 * color: YUV422/RGB565/IDX8
 *        YUV422 => V:[23~16], Y:[15~8], U:[7~0]
 */
UINT32 sp5kGfxRectFill(UINT32 pageId, SINT32 x, SINT32 y, UINT32 width, UINT32 height, UINT32 color);
/**
 * \note
 * only for the layer whose format is SP5K_GFX_FMT_RGB565
 * ppvRoi    virtual roi of pv frame, the range is from 0 to 1024
 * ppageRoi  physical roi of the selected page
 */
UINT32 sp5kGfxPvFrameScale(UINT32 pageId,sp5kGfxRoi_t *ppvRoi, sp5kGfxRoi_t *ppageRoi);

/* sp5k_gfx_round_rect.c */
UINT32 sp5kGfxRoundRectDraw(UINT32 pageId, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1, UINT32 radiusX, UINT32 radiusY);

/* sp5k_gfx_str.c */
UINT32 sp5kGfxStringDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 encodeType, UINT8 *str);
UINT32 sp5kGfxStringIdDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 strId);
UINT32 sp5kGfxStringIdStringGet(UINT32 strId, UINT32 *plangId, UINT32 *pencodeType, UINT8 **pstr);
UINT32 sp5kGfxStringSizeGet(UINT32 encodeType, UINT8 *str, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxStringIdSizeGet(UINT32 strId, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxCharDraw(UINT32 pageId, SINT32 x, SINT32 y, UINT32 align, UINT32 chrId);
UINT32 sp5kGfxFontSizeGet(UINT32 fntId, UINT32 *pwidth, UINT32 *pheight);
UINT32 sp5kGfxTextColorSet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);
UINT32 sp5kGfxTextColorGet(UINT8 *colorTbl, UINT32 begIdx, UINT32 len);

UINT32 sp5kGfxFt2CfgSet( UINT32 paramId, ... );
UINT32 sp5kGfxFt2CacheInit( UINT32 cacheSize );
UINT32 sp5kGfxFt2CahceFont( UINT8* str, UINT32 encodeType );
UINT32 sp5kGfxFt2Done();

/* sp5k_gfx_pixel.c */
UINT32 sp5kGfxPixelDraw(UINT32 pageId, SINT32 x, SINT32 y);

/* sp5k_gfx_line.c */
UINT32 sp5kGfxLineDraw(UINT32 pageId, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1);

/* sp5k_gfx_circle.c */
UINT32 sp5kGfxCircleDraw(UINT32 pageId, SINT32 x, SINT32 y, SINT32 radius);
UINT32 sp5kGfxEllipseDraw(UINT32 pageId, SINT32 x, SINT32 y, SINT32 radiusX, SINT32 radiusY);

/* sp5k_gfx_polygon.c */
UINT32 sp5kGfxPolygonDraw(UINT32 pageId, SINT32 x0, SINT32 y0, sp5kGfxPoint_t *points, UINT32 tot);

/* sp5k_gfx_obj.c */
UINT32 sp5kGfxObjectFill(sp5kGfxObj_t *pobj, UINT32 color);
UINT32 sp5kGfxObjectCopy(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectScale(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectBilinearScale(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectPerPixelBlend(sp5kGfxObj_t *psrc1Obj, sp5kGfxObj_t *psrc2Obj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectYuvAverageGet(sp5kGfxObj_t *pobj, UINT32 *py, UINT32 *pu, UINT32 *pv);
UINT32 sp5kGfxObjectYuvAverageGetExt(sp5kGfxObj_t *pobj, UINT32 bw, UINT32 bh, UINT8 *py, UINT8 *pu, UINT8 *pv);
UINT32 sp5kGfxObjTrapeziumToRectangleTransform(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, SINT32 ax, SINT32 ay, SINT32 bx, SINT32 by, SINT32 cx, SINT32 cy, SINT32 dx, SINT32 dy);
UINT32 sp5kGfxObjRectangleToTrapeziumTransform(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, SINT32 ax, SINT32 ay, SINT32 bx, SINT32 by, SINT32 cx, SINT32 cy, SINT32 dx, SINT32 dy);
UINT32 sp5kGfxObjectConvert(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj);
UINT32 sp5kGfxObjectEdit(sp5kGfxObj_t *psrcObj, sp5kGfxObj_t *pdstObj, UINT32 editId, UINT32 rsv);
UINT32 sp5kGfxObjectSave(sp5kGfxObj_t *pobj, UINT32 rotate, UINT32 jpgW, UINT32 jpgH, UINT8 *pfname);
UINT32 sp5kGfxObjectDraw(UINT32 pageId, sp5kGfxRoi_t *roi, UINT32 rotate, sp5kGfxObj_t *obj);

/* sp5k_gfx_obj_rot.c */
/*gfx rotate engine can only handle < 1024 images. Use Sp5krsvYuvScaleRoiRotate if >= 1024*/
UINT32 sp5kGfxObjectRotate(sp5kGfxObj_t *pobjSrc, sp5kGfxObj_t *pobjDst, UINT32 orientation);
UINT32 sp5kGfxObjectFreeRotate(sp5kGfxObj_t *pobjSrc, sp5kGfxObj_t *pobjDst, int xc, int yc, int angle, double scale); /* (xc,yc): rotate center, angle unit=degree */

/* sp5k_gfx_compatible.c : note that APIs moved here are kept for compatible only, should be removed later */
UINT32 sp5kGfxObjectColorFill(sp5kGfxObj_t *pobj, UINT32 roiW, UINT32 roiH, UINT32 color);
UINT32 sp5kGfxColorConvert(sp5kGfxPicture_t *psrcPic, sp5kGfxPicture_t *pdstPic);
UINT32 sp5kGfxYuvLineDraw(UINT8 *pyuvBuf, UINT32 yuvWidth, UINT32 yuvHeight, SINT32 x0, SINT32 y0, SINT32 x1, SINT32 y1, UINT32 yuvColor, UINT16 penWidth);

#endif  /* _SP5K_GFX_API_H_ */

