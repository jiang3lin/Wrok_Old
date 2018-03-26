/**************************************************************************
 *
 *       Copyright (c) 2006-2013 by iCatch Technology, Inc.
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
 **************************************************************************/
/**
 * @author Matt Wang
 */
#ifndef _DISP_DEF_H_
#define _DISP_DEF_H_

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/* dispDevSwitch() */
typedef enum _dispDevId_e {
	DISP_DEV_LCD  = 1,
	DISP_DEV_NTSC = 2,
	DISP_DEV_PAL  = 3,
	DISP_DEV_HDMI = 4,
	DISP_DEV_LCD2 = 5,
} dispDevId_e;

/* dispDevAttrSet() */
typedef enum _dispAttrPara_e {
	DISP_BG_COLOR            = 0x00,
	DISP_SIGNAL_ACTIVE       = 0x01,
	DISP_BRIGHT              = 0x10,
	DISP_CONTRAST            = 0x11,
	DISP_HUE                 = 0x12,
	DISP_SATURATION          = 0x13,
	DISP_COLOR_MATRIX        = 0x14,
	DISP_EDGE_ENHANCE        = 0x15,
	DISP_GLOB_ACTIVE         = 0x20,
	DISP_IMG_ACTIVE          = 0x21,
	DISP_OSD_ACTIVE          = 0x22,
	DISP_PIP_ACTIVE          = 0x23,
	DISP_IMG_FLIP            = 0x31,
	DISP_OSD_FLIP            = 0x32,
	DISP_PIP_FLIP            = 0x33,
	DISP_IMG_WINDOW          = 0x40,
	DISP_OSD_WINDOW          = 0x41,
	DISP_IMG_FRAME_ROI       = 0x42,
	DISP_PV_IMG_WINDOW_SYNC  = 0x43,
	DISP_VIRTUAL_WINDOW_SIZE = 0x48,
	DISP_PREVIEW_FREEZE      = 0x50,
	DISP_ALPHA_SCALE         = 0x60,
	DISP_TV_HUE              = 0x70,
	DISP_TV_SATURATION       = 0x71,
	DISP_TV_SYNC_LEVEL       = 0x72,
	DISP_TV_BLANK_LEVEL      = 0x73,
	DISP_TV_BLACK_LEVEL      = 0x74,
	DISP_DEV_CMD,
} dispAttrPara_e;

typedef enum {
	DISP_DEV_CMD_POWERON,
	DISP_DEV_CMD_SUSPEND,
	DISP_DEV_CMD_RESUME,
	DISP_DEV_CMD_POWEROFF,
	DISP_DEV_CMD_MAX,
} dispDevCmd_e;

typedef enum _dispClkSrc_e {
	DISP_CLK_SRC_DRAMPLL = 0,
	DISP_CLK_SRC_XTAL27M = 1,
} dispClkSrc_e;  /* plz keep sync to halPllDispClkSrc_e. */

typedef enum _dispTvType_e {
	DISP_TV_TYPE_DIGITAL = (0 << 4),
	DISP_TV_TYPE_ANALOG  = (1 << 4),
} dispTvType_e; /* plz keep sync to halPllDispTvType_e. */

typedef enum _dispEvent_e {
	DISP_EVENT_PANEL_VSYNC_RISE,
	DISP_EVENT_PANEL_VSYNC_FALL,
	DISP_EVENT_PANEL_VVALID_RISE,
	DISP_EVENT_PANEL_VVALID_FALL,
	DISP_EVENT_LCM_VSYNC_RISE,
	DISP_EVENT_LCM_VSYNC_FALL,
	DISP_EVENT_TV_VSYNC_RISE,
	DISP_EVENT_TV_VSYNC_FALL,
	DISP_EVENT_TV_VVALID_RISE,
	DISP_EVENT_TV_VVALID_FALL,
	#if SPCA6330
	DISP_EVENT_HDMI_HPD_RISE,
	DISP_EVENT_HDMI_HPD_FALL,
	#else
	DISP_EVENT_HDMI_HPD_RISE = -1,
	DISP_EVENT_HDMI_HPD_FALL = -1,
	#endif
} dispEvent_e;  /* plz keep sync to halDispInt_e. */

typedef enum _dispYuv422ToYuv444Mode_e {
	DISP_UV_EXTEND_DUP    = (0 << 0),
	DISP_UV_EXTEND_INTRPL = (1 << 0),
} dispUvExtendMode_e;  /* plz keep sync to halDispUvExtendMode_e. */

typedef enum _dispParaId_e {
	DISP_PARA_RESET,
	DISP_PARA_GLOB_ENABLE,
	DISP_PARA_DEVICE_TYPE,
	DISP_PARA_DEVICE_TOTAL_REGION,  /* please use DISP_PARA_DEVICE_TOTAL_RESOLUTION and DISP_PARA_DEVICE_SYNC_TIME to replace. */
	DISP_PARA_DEVICE_TOTAL_RESOLUTION,
	DISP_PARA_DEVICE_SYNC_TIME,
	DISP_PARA_DEVICE_SYNC_TIME_POINT,
	DISP_PARA_DEVICE_ACTIVE_REGION,
	DISP_PARA_CHNL_SRC,
	DISP_PARA_CLK_ENABLE,
	DISP_PARA_CLK_DIVISOR,
	DISP_PARA_CLK_PHASE,
	DISP_PARA_BANDWIDTH_BUFFER_MODE,
	DISP_PARA_BG_COLOR,
	DISP_PARA_INTERLACE_MODE,
	DISP_PARA_EDGE_ENHANCE,
	DISP_PARA_IMG_LAYER_ENABLE,
	DISP_PARA_IMG_LAYER_FLIP,
	DISP_PARA_IMG_NONSCALE_INTERLACE_MODE,
	DISP_PARA_IMG_COLOR_KEY_ENABLE,
	DISP_PARA_IMG_COLOR_KEY,
	DISP_PARA_IMG_FORMAT,
	DISP_PARA_IMG_MONOCHROME_ENABLE,
	DISP_PARA_IMG_UV_RECONSTRUCT_MODE,
	DISP_PARA_IMG_SCALE_BUFFER_SIZE,
	DISP_PARA_IMG_SRC,
	DISP_PARA_IMG_PV_REQUEST_PERIOD,
	DISP_PARA_IMG_PV_FRAME_BUFFER,
	DISP_PARA_IMG_PV_2ND_PATH_FRAME_BUFFER,
	DISP_PARA_IMG_PV_RENDER_REGION,
	DISP_PARA_IMG_PV_CROP_REGION,
	DISP_PARA_IMG_PV_AUTO_SCALE,
	DISP_PARA_IMG_PB_REQUEST_PERIOD,
	DISP_PARA_IMG_PB_FRAME_BUFFER,
	DISP_PARA_IMG_PB_RENDER_REGION,
	DISP_PARA_IMG_PB_CROP_REGION,
	DISP_PARA_IMG_PB_AUTO_SCALE,
	DISP_PARA_OSD_LAYER_ENABLE,
	DISP_PARA_OSD_LAYER_FLIP,
	DISP_PARA_OSD_COLOR_KEY_ENABLE,
	DISP_PARA_OSD_COLOR_KEY,
	DISP_PARA_OSD_BG_COLOR_KEY_ENABLE,
	DISP_PARA_OSD_BG_COLOR_KEY,
	DISP_PARA_OSD_FORMAT,
	DISP_PARA_OSD_GEN_ALPHA,
	DISP_PARA_OSD_ALPHA_PLANE_MODE,
	DISP_PARA_OSD_REQUEST_PERIOD,
	DISP_PARA_OSD_FRAME_BUFFER,
	DISP_PARA_OSD_ALPHA_PLANE_BUFFER,
	DISP_PARA_OSD_RENDER_REGION,
	DISP_PARA_OSD_CROP_REGION,
	DISP_PARA_OSD_ALPHA_PLANE_CROP_REGION,
	DISP_PARA_OSD_AUTO_SCALE,
	DISP_PARA_BRIGHTNESS,
	DISP_PARA_CONTRAST,
	DISP_PARA_HUE,
	DISP_PARA_SATURATION,
	DISP_PARA_WHITE_BALANCE,
	DISP_PARA_COLOR_MATRIX_ENABLE,
	DISP_PARA_COLOR_MATRIX,
	DISP_PARA_COLOR_CLAMP,
	DISP_PARA_GAMMA_TABLE_ENABLE,
	DISP_PARA_GAMMA_TABLE,
	DISP_PARA_COLOR_GAMMA_ORDER,
	DISP_PARA_VIDEO_DAC_POWER,
	DISP_PARA_TV_ENCODER_FILTER,
	DISP_PARA_TV_HUE,
	DISP_PARA_TV_SATURATION,
	DISP_PARA_TV_SYNC_LEVEL,
	DISP_PARA_TV_BLANK_LEVEL,
	DISP_PARA_TV_BLACK_LEVEL,
	DISP_PARA_PIXEL_COMBINE_MODE,
	DISP_PARA_DOT_AVERAGE_MODE,
	DISP_PARA_RGB888_ROUND_MODE,
	DISP_PARA_UPS_HSD_MODE,
	DISP_PARA_UPS_LAYOUT_MODE,
	DISP_PARA_UPS_COLOR_FILTER,
	DISP_PARA_UPS051_RGB888_ENABLE,
	DISP_PARA_UPS052_RGB_ONLY_ENABLE,
	DISP_PARA_CCIR_OUTPUT_SEQUENCE,
	DISP_PARA_SIGNAL_POLARITY,
	DISP_PARA_PIN_MUX,
	DISP_PARA_PIN_SWAP,
	DISP_PARA_PIN_SELECT,
	DISP_PARA_LCM_BUS_CFG,
	DISP_PARA_LCM_DATA_FORMAT,
	DISP_PARA_LCM_DATA_PACKET,
	DISP_PARA_LCM_SIGNAL_UPDATE_MODE,
	DISP_PARA_LCM_CMD_DELAY,
	DISP_PARA_HDMI_PHY_POWER,
	DISP_PARA_HDMI_SIGNAL_POLARITY,
	DISP_PARA_HDMI_HPD_CFG,
	DISP_PARA_HDMI_DATA_ISLAND,
	DISP_PARA_HDMI_PACKET_ENABLE,

	DISP_PARA_CLK_SRC,
	DISP_PARA_CLK_FREQ,
	DISP_PARA_SCALE_CLK_FREQ,
	DISP_PARA_GPIO_A_FUNC_CFG,
	DISP_PARA_GPIO_A_DIR_CFG,
	DISP_PARA_GPIO_A_PULL_CFG,
	DISP_PARA_GPIO_A_DATA,
	DISP_PARA_GPIO_B_FUNC_CFG,
	DISP_PARA_GPIO_B_DIR_CFG,
	DISP_PARA_GPIO_B_PULL_CFG,
	DISP_PARA_GPIO_B_DATA,

	DISP_PARA_MAX_ID,

	/* backward compatible. */
	DISP_PARA_GPIO_FUNC_CFG = DISP_PARA_GPIO_A_FUNC_CFG,
	DISP_PARA_GPIO_DIR_CFG  = DISP_PARA_GPIO_A_DIR_CFG,
	DISP_PARA_GPIO_PULL_CFG = DISP_PARA_GPIO_A_PULL_CFG,
	DISP_PARA_GPIO_DATA     = DISP_PARA_GPIO_A_DATA,

	/* invalid id. */
	DISP_PARA_INVALID_ID = 1000000,
	DISP_PARA_IMG_COLOR_KEY_BG_SRC,
	DISP_PARA_IMG_UV_EXTEND_MODE,
	DISP_PARA_IMG_SCALE_DEVICE,
	DISP_PARA_IMG_PV_SCALE_FACTOR,
	DISP_PARA_IMG_PV_SCALE_INIT_FACTOR,
	DISP_PARA_IMG_PV_SCALE_MODE,
	DISP_PARA_IMG_PV_SCALE_ORDER,
	DISP_PARA_IMG_PV_SCALE_INTERLACE_DROP_MODE_ENABLE,
	DISP_PARA_IMG_PB_SCALE_FACTOR,
	DISP_PARA_IMG_PB_SCALE_INIT_FACTOR,
	DISP_PARA_IMG_PB_SCALE_MODE,
	DISP_PARA_IMG_PB_SCALE_ORDER,
	DISP_PARA_IMG_PB_SCALE_INTERLACE_DROP_MODE_ENABLE,
	DISP_PARA_OSD_ARGB_ALPHA,
	DISP_PARA_OSD_BOUNDARY_AVERAGE_ENABLE,
	DISP_PARA_OSD_SCALE_FACTOR,
	DISP_PARA_OSD_SCALE_INITFACTOR,
	DISP_PARA_OSD_SCALE_MODE,
	DISP_PARA_UPS051_LIGHTVIEW_ENABLE,
	DISP_PARA_CCIR_CLAMP_MODE,
	DISP_PARA_CCIR_SIGNAL_POLARITY,
} dispParaId_e;  /* plz keep sync to halDispParaId_e. */

/* dispChnlGet(), dispChnlSrcSet() */
typedef enum _dispChnlId_e {
	DISP_CHNL_PANEL = 0,
	DISP_CHNL_TV    = 1,
} dispChnlId_e;  /* plz keep sync to halDispChnlId_e. */

/* dispChnlSrcSet() */
typedef enum _dispSrcId_e {
	DISP_SRC_PRIMARY   = 0,  /* src0. */
	DISP_SRC_SECONDARY = 1,  /* src1. */
} dispSrcId_e;

typedef enum _dispChnlSrc_e {
	DISP_PANEL_SRC0_TV_SRC1 = (0 << 0),
	DISP_PANEL_SRC1_TV_SRC0 = (1 << 0),
} dispChnlSrc_e;  /* plz keep sync to halDispChnlSrc_e. */

typedef enum _dispBandwidthBufferMode_e {
	DISP_BANDWIDTH_BALANCE     = (0 << 0),  /**< Panel 256(spca5330)/1920(spca6330) pixels, tv 384(spca5330)/2880(spca6330) pixels. */
	DISP_BANDWIDTH_PANEL_MAJOR = (1 << 0),  /**< Panel 640(spca5330)/4800(spca6330) pixels, tv 0 pixels. */
	DISP_BANDWIDTH_TV_MAJOR    = (2 << 0),  /**< Panel 0 pixels, tv 640(spca5330)/4800(spca6330) pixels. */
} dispBandwidthBufferMode_e;  /* plz keep sync to halDispBandwidthBufferMode_e. */

typedef enum _dispNonscaleIntrlcMode_e {
	DISP_NONSCALE_INTRLC_DISABLE = (0 << 0),
	DISP_NONSCALE_INTRLC_NOSWAP  = ((0 << 1) | (1 << 0)),
	DISP_NONSCALE_INTRLC_SWAP    = ((1 << 1) | (1 << 0)),
} dispNonscaleIntrlcMode_e;  /* plz keep sync to halDispNonscaleIntrlcMode_e. */

typedef enum _dispDeviceType_e {
	DISP_DEVICE_TYPE_NTSC_M                = 0x000,  /**< For TV interface only. */
	DISP_DEVICE_TYPE_NTSC_J                = 0x040,  /**< For TV interface only. */
	DISP_DEVICE_TYPE_NTSC_443              = 0x080,  /**< For TV interface only. */
	DISP_DEVICE_TYPE_PAL                   = 0x001,  /**< For TV interface only. */
	DISP_DEVICE_TYPE_PAL_M                 = 0x020,  /**< For TV interface only. */
	DISP_DEVICE_TYPE_CCIR656_NTSC          = 0x002,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_CCIR656_PAL           = 0x003,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_CCIR601_NTSC_8        = 0x004,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_CCIR601_NTSC_16       = 0x104,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_CCIR601_PAL_8         = 0x005,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_CCIR601_PAL_16        = 0x105,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_UPS051                = 0x008,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_UPS052                = 0x009,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_LCM                   = 0x00A,  /**< For both panel and TV interfaces. */
	#if SPCA6330
	DISP_DEVICE_TYPE_USB                   = 0x00B,  /**< For TV interface only. */
	#endif
	DISP_DEVICE_TYPE_INTERLACE_CCIR656     = 0x010,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_INTERLACE_CCIR601     = 0x013,  /**< For both panel and TV interfaces. */
	#if SPCA6330
	DISP_DEVICE_TYPE_STD_INTERLACE_CCIR656 = 0x014,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_STD_INTERLACE_CCIR601 = 0x017,  /**< For both panel and TV interfaces. */
	#endif
	DISP_DEVICE_TYPE_PROGRESSIVE_CCIR656   = 0x018,  /**< For both panel and TV interfaces. */
	DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601   = 0x01B,  /**< For both panel and TV interfaces. */
	/* backward compatible. */
	DISP_PANEL_DEVICE_TYPE_CCIR656_NTSC          = DISP_DEVICE_TYPE_CCIR656_NTSC,
	DISP_PANEL_DEVICE_TYPE_CCIR656_PAL           = DISP_DEVICE_TYPE_CCIR656_PAL,
	DISP_PANEL_DEVICE_TYPE_CCIR601_NTSC_8        = DISP_DEVICE_TYPE_CCIR601_NTSC_8,
	DISP_PANEL_DEVICE_TYPE_CCIR601_NTSC_16       = DISP_DEVICE_TYPE_CCIR601_NTSC_16,
	DISP_PANEL_DEVICE_TYPE_CCIR601_PAL_8         = DISP_DEVICE_TYPE_CCIR601_PAL_8,
	DISP_PANEL_DEVICE_TYPE_CCIR601_PAL_16        = DISP_DEVICE_TYPE_CCIR601_PAL_16,
	DISP_PANEL_DEVICE_TYPE_UPS051                = DISP_DEVICE_TYPE_UPS051,
	DISP_PANEL_DEVICE_TYPE_UPS052                = DISP_DEVICE_TYPE_UPS052,
	DISP_PANEL_DEVICE_TYPE_LCM                   = DISP_DEVICE_TYPE_LCM,
	DISP_PANEL_DEVICE_TYPE_INTERLACE_CCIR656     = DISP_DEVICE_TYPE_INTERLACE_CCIR656,
	DISP_PANEL_DEVICE_TYPE_INTERLACE_CCIR601     = DISP_DEVICE_TYPE_INTERLACE_CCIR601,
	#if SPCA6330
	DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR656 = DISP_DEVICE_TYPE_STD_INTERLACE_CCIR656,
	DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601 = DISP_DEVICE_TYPE_STD_INTERLACE_CCIR601,
	#endif
	DISP_PANEL_DEVICE_TYPE_PROGRESSIVE_CCIR656   = DISP_DEVICE_TYPE_PROGRESSIVE_CCIR656,
	DISP_PANEL_DEVICE_TYPE_PROGRESSIVE_CCIR601   = DISP_DEVICE_TYPE_PROGRESSIVE_CCIR601,
	DISP_TV_DEVICE_TYPE_NTSC_M                   = DISP_DEVICE_TYPE_NTSC_M,
	DISP_TV_DEVICE_TYPE_NTSC_J                   = DISP_DEVICE_TYPE_NTSC_J,
	DISP_TV_DEVICE_TYPE_NTSC_443                 = DISP_DEVICE_TYPE_NTSC_443,
	DISP_TV_DEVICE_TYPE_PAL                      = DISP_DEVICE_TYPE_PAL,
	DISP_TV_DEVICE_TYPE_PAL_M                    = DISP_DEVICE_TYPE_PAL_M,
	#if SPCA6330
	DISP_TV_DEVICE_TYPE_USB                      = DISP_DEVICE_TYPE_USB,
	#endif

	#if !SPCA6330
	DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR656 = -1,
	DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601 = -1,
	DISP_TV_DEVICE_TYPE_USB                      = -1,
	#endif
} dispDeviceType_e;  /* plz keep sync to halDispDeviceType_e. */

typedef enum _dispImgFmt_e {  /* spca6330. */
	DISP_IMG_FMT_YUV422  = ((0 << 1) | (0 << 0)),
	DISP_IMG_FMT_YUV420  = ((1 << 1) | (0 << 0)),
	DISP_IMG_FMT_YUYV422 = ((0 << 1) | (1 << 0)),
	DISP_IMG_FMT_YUYV420 = ((1 << 1) | (1 << 0)),
} dispImgFmt_e;  /* plz keep sync to halDispImgFmt_e. */

typedef enum _dispOsdFmt_e {
	DISP_OSD_FMT_RGB565  = 0,
	DISP_OSD_FMT_YUYV422 = 1,
} dispOsdFmt_e;  /* plz keep sync to halDispOsdFmt_e. */

typedef enum _dispImgSrc_e {
	DISP_IMG_SRC_PV = (1 << 1),
	DISP_IMG_SRC_PB = (0 << 1),
} dispImgSrc_e;  /* plz keep sync to halDispImgSrc_e. */

typedef enum _dispImgUvReconstuctMode_e {
	DISP_IMG_UV_DUP         = (0 << 0),
	DISP_IMG_UV_INTRPL      = (1 << 0),
	DISP_IMG_UV_REFINE      = (2 << 0),

	DISP_IMG_UV_DROP_JOINT  = (0 << 4),
	DISP_IMG_UV_DROP_SEPR   = (1 << 4),
	DISP_IMG_UV_DROP_INTRPL = (2 << 4),
	DISP_IMG_UV_DROP_SATUR  = (3 << 4),

	DISP_IMG_UV_HSCL_DUP    = (0 << 8),
	DISP_IMG_UV_HSCL_INTRPL = (1 << 8),

	DISP_IMG_UV_BLD_DUP     = (0 << 12),
	DISP_IMG_UV_BLD_JOINT   = (1 << 12),
	DISP_IMG_UV_BLD_SEPR    = (2 << 12),
	DISP_IMG_UV_BLD_REFINE  = (3 << 12),
} dispImgUvReconstuctMode_e;  /* plz keep sync to halDispImgUvReconstuctMode_e. */

typedef enum _dispInterlaceMode_e {
	DISP_PROGRESSIVE      = ((0 << 2) | (0 << 0)),
	DISP_INTERLACE_NOSWAP = ((1 << 2) | (0 << 1) | (1 << 0)),
	DISP_INTERLACE_SWAP   = ((1 << 2) | (1 << 1) | (1 << 0)),
	DISP_INTERLACE_NOSOF  = ((0 << 2) | (0 << 1) | (1 << 0)),
} dispInterlaceMode_e;  /* plz keep sync to halDispInterlaceMode_e. */

typedef enum _dispSrcFieldMode_e {  /* spca6330. */
	DISP_SRC_FIELD_NOSWAP = (0 << 4),
	DISP_SRC_FIELD_SWAP   = (1 << 4),
} dispSrcFieldMode_e;  /* plz keep sync to halDispSrcFieldMode_e. */

typedef enum _dispColorClampType_e {
	DISP_COLOR_CLAMP_RGB_0_255    = (0 << 4),
	DISP_COLOR_CLAMP_YCBCR_16_235 = (1 << 4),
	DISP_COLOR_CLAMP_YUV_0_255    = (2 << 4),
	#if SPCA6330
	DISP_COLOR_CLAMP_RGB_16_235   = (5 << 4),
	#endif
} dispColorClampType_e;  /* plz keep sync to halDispColorClampType_e. */

typedef enum _dispColorGammaOrder_e {  /* spca6330. */
	DISP_GAMMA_AFTER_COLOR  = (0 << 2),
	DISP_GAMMA_BEFORE_COLOR = (1 << 2),
} dispColorGammaOrder_e;  /* plz keep sync to halDispColorGammaOrder_e. */

typedef enum _dispPixelCombineMode_e {
	DISP_PIXEL_COMBINE_DISABLE = ((0 << 4) | (0 << 2) | (0 << 0)),
	DISP_PIXEL_COMBINE_RGB     = ((2 << 4) | (1 << 2) | (0 << 0)),
	DISP_PIXEL_COMBINE_GBR     = ((1 << 4) | (0 << 2) | (2 << 0)),
	DISP_PIXEL_COMBINE_BRG     = ((0 << 4) | (2 << 2) | (1 << 0)),
	DISP_PIXEL_COMBINE_BGR     = ((0 << 4) | (1 << 2) | (2 << 0)),
	DISP_PIXEL_COMBINE_RBG     = ((1 << 4) | (2 << 2) | (0 << 0)),
	DISP_PIXEL_COMBINE_GRB     = ((2 << 4) | (0 << 2) | (1 << 0)),
} dispPixelCombineMode_e;  /* plz keep sync to halDispPixelCombineMode_e. */

typedef enum _dispDotAverageMode_e {
	DISP_DOT_EVEN_R100 = (0 << 0),
	DISP_DOT_EVEN_R75  = (1 << 0),
	DISP_DOT_EVEN_R50  = (2 << 0),
	DISP_DOT_EVEN_R25  = (3 << 0),
	DISP_DOT_EVEN_R0   = (4 << 0),

	DISP_DOT_ODD_R100  = (0 << 4),
	DISP_DOT_ODD_R75   = (1 << 4),
	DISP_DOT_ODD_R50   = (2 << 4),
	DISP_DOT_ODD_R25   = (3 << 4),
	DISP_DOT_ODD_R0    = (4 << 4),

	DISP_DOT_EVEN_G100 = (0 << 8),
	DISP_DOT_EVEN_G75  = (1 << 8),
	DISP_DOT_EVEN_G50  = (2 << 8),
	DISP_DOT_EVEN_G25  = (3 << 8),
	DISP_DOT_EVEN_G0   = (4 << 8),

	DISP_DOT_ODD_G100  = (0 << 12),
	DISP_DOT_ODD_G75   = (1 << 12),
	DISP_DOT_ODD_G50   = (2 << 12),
	DISP_DOT_ODD_G25   = (3 << 12),
	DISP_DOT_ODD_G0    = (4 << 12),

	DISP_DOT_EVEN_B100 = (0 << 16),
	DISP_DOT_EVEN_B75  = (1 << 16),
	DISP_DOT_EVEN_B50  = (2 << 16),
	DISP_DOT_EVEN_B25  = (3 << 16),
	DISP_DOT_EVEN_B0   = (4 << 16),

	DISP_DOT_ODD_B100  = (0 << 20),
	DISP_DOT_ODD_B75   = (1 << 20),
	DISP_DOT_ODD_B50   = (2 << 20),
	DISP_DOT_ODD_B25   = (3 << 20),
	DISP_DOT_ODD_B0    = (4 << 20),

	DISP_DOT_ALL_FULL  = 0x000000,
	DISP_DOT_ALL_HALF  = 0x222222,
} dispDotAverageMode_e;  /* plz keep sync to halDispDotAverageMode_e. */

typedef enum _dispRgb888RoundMode_e {
	DISP_RGB888_ROUND_DISABLE   = (0 << 4),
	DISP_RGB888_ROUND_TO_RGB666 = (1 << 4),
	DISP_RGB888_ROUND_TO_RGB565 = (2 << 4),
	DISP_RGB888_ROUND_TO_RGB555 = (3 << 4),
} dispRgb888RoundMode_e;  /* plz keep sync to halDispRgb888RoundMode_e. */

typedef enum _dispUpsHsdMode_e {
	DISP_UPS_HSD_NO_DELAY = (1 << 0),
	DISP_UPS_HSD_DELAY_3T = (0 << 0),
	DISP_UPS_HSD_DELAY_2T = (4 << 0),
} dispUpsHsdMode_e;  /* plz keep sync to halDispUpsHsdMode_e. */

typedef enum _dispUpsLayoutMode_e {
	DISP_UPS_LAYOUT_DISABLE       = (0 << 0),
	DISP_UPS_LAYOUT_ODD_DELAY_1T  = (1 << 0),
	DISP_UPS_LAYOUT_EVEN_DELAY_1T = (3 << 0),
} dispUpsLayoutMode_e;  /* plz keep sync to halDispUpsLayoutMode_e. */

typedef enum _dispUpsEvenColorFilter_e {
	DISP_UPS051_EVEN_RGB = (0 << 0),
	DISP_UPS051_EVEN_GBR = (1 << 0),
	DISP_UPS051_EVEN_BRG = (2 << 0),
	DISP_UPS051_EVEN_BGR = (3 << 0),
	DISP_UPS051_EVEN_RBG = (4 << 0),
	DISP_UPS051_EVEN_GRB = (5 << 0),

	DISP_UPS052_EVEN_RGB = (0 << 0),
	DISP_UPS052_EVEN_GBR = (1 << 0),
	DISP_UPS052_EVEN_BRG = (2 << 0),
	DISP_UPS052_EVEN_BGR = (3 << 0),
	DISP_UPS052_EVEN_RBG = (4 << 0),
	DISP_UPS052_EVEN_GRB = (5 << 0),
} dispUpsEvenColorFilter_e;  /* plz keep sync to halDispUpsEvenColorFilter_e. */

typedef enum _dispUpsOddColorFilter_e {
	DISP_UPS051_ODD_RGB = (0 << 4),
	DISP_UPS051_ODD_GBR = (1 << 4),
	DISP_UPS051_ODD_BRG = (2 << 4),
	DISP_UPS051_ODD_BGR = (3 << 4),
	DISP_UPS051_ODD_RBG = (4 << 4),
	DISP_UPS051_ODD_GRB = (5 << 4),

	DISP_UPS052_ODD_RGB = (0 << 4),
	DISP_UPS052_ODD_GBR = (1 << 4),
	DISP_UPS052_ODD_BRG = (2 << 4),
	DISP_UPS052_ODD_BGR = (3 << 4),
	DISP_UPS052_ODD_RBG = (4 << 4),
	DISP_UPS052_ODD_GRB = (5 << 4),
} dispUpsOddColorFilter_e;  /* plz keep sync to halDispUpsOddColorFilter_e. */

typedef enum _dispCcirOutputSeq_e {
	DISP_CCIR_YCRYCB = (0 << 0),
	DISP_CCIR_CRYCBY = (1 << 0),
	DISP_CCIR_YCBYCR = (2 << 0),
	DISP_CCIR_CBYCRY = (3 << 0),
} dispCcirOutputSeq_e;  /* plz keep sync to halDispCcirOutputSeq_e. */

typedef enum _dispCcirClampMode_e {
	DISP_CCIR_CLAMP_DISABLE = (0 << 4),
	DISP_CCIR_CLAMP_YCBCR   = (1 << 4),
	DISP_CCIR_CLAMP_YUV     = (2 << 4),
} dispCcirClampMode_e;  /* plz keep sync to halDispCcirClampMode_e. */

typedef enum _dispLcmBusType_e {
	DISP_LCM_I80 = (0 << 0),
	DISP_LCM_M68 = (1 << 0),
} dispLcmBusType_e;  /* plz keep sync to halDispLcmBusType_e. */

typedef enum _dispLcmDataBusWidth_e {
	DISP_LCM_8_BIT  = (0 << 4),
	DISP_LCM_9_BIT  = (1 << 4),
	DISP_LCM_16_BIT = (2 << 4),
	DISP_LCM_18_BIT = (3 << 4),
	DISP_LCM_24_BIT = (4 << 4),
} dispLcmDataBusWidth_e;  /* plz keep sync to halDispLcmDataBusWidth_e. */

typedef enum _dispLcmDataFmt_e {
	DISP_LCM_RGB565 = (0 << 4),
	DISP_LCM_RGB666 = (1 << 4),
} dispLcmDataFmt_e;  /* plz keep sync to halDispLcmDataFmt_e. */

typedef enum _dispLcmDataAlign_e {
	DISP_LCM_ALIGN_NONE = (0 << 5),
	DISP_LCM_ALIGN_BIG  = (1 << 5),
	DISP_LCM_ALIGN_MID  = (2 << 5),
	DISP_LCM_ALIGN_BOTH = (3 << 5),
} dispLcmDataAlign_e;  /* plz keep sync to halDispLcmDataAlign_e. */

typedef enum _dispPinSel_e {
	DISP_PIN_SEL_SIF2          = 4,  /**< 0: gpio[15:13], 1: sif2sdo/sif2sck/sif2sen. */
	DISP_PIN_SEL_TVGPIO_15TO0  = 5,  /**< 0: gpio[30:17, 12:11], 1: tvgpio[15:0]. */
	DISP_PIN_SEL_TVGPIO_27TO16 = 6,  /**< 0: gpio[42:31, 15:13], 1: tvgpio[27:16, 27:25]. */
	DISP_PIN_SEL_TV_LCM        = 7,  /**< TV LCM interface multiplex selection 0: pinloc 1, 1: pinloc 2. */
} dispPinSel_e;  /* plz keep sync to halDispPinSel_e. */

typedef enum _dispHdmiPktType_e {  /* spca6330. */
	DISP_HDMI_PKT_ACR      = 0x01,  /**< Audio Clock Regeneration Packet, refer to spec HDMI 1.4 section 5.3.3 and 7.2. */
	DISP_HDMI_PKT_GC       = 0x03,  /**< General Control Packet, refer to spec HDMI 1.4 section 5.3.6. */
	DISP_HDMI_PKT_ACP      = 0x04,  /**< Audio Content Protection Packet, refer to spec HDMI 1.4 section 5.3.7. */
	DISP_HDMI_PKT_ISRC1    = 0x05,  /**< ISRC1 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	DISP_HDMI_PKT_ISRC2    = 0x06,  /**< ISRC2 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	DISP_HDMI_PKT_GM       = 0x0A,  /**< Gamut Metadata Packet, refer to spec HDMI 1.4 section 5.3.12. */
	DISP_HDMI_PKT_VS_IF    = 0x81,  /**< Vendor Specific InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.3. */
	DISP_HDMI_PKT_AVI_IF   = 0x82,  /**< AVI InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.1. */
	DISP_HDMI_PKT_SPD_IF   = 0x83,  /**< Source Product Descriptor InfoFrame Packet. */
	DISP_HDMI_PKT_AUDIO_IF = 0x84,  /**< Audio InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.2. */
	DISP_HDMI_PKT_MPEG_IF  = 0x85,  /**< MPEG Source InfoFrame Packet. */
} dispHdmiPktType_e;  /* plz keep sync to halDispHdmiPktType_e. */

typedef enum _dispHdmiPktEn_e {  /* spca6330. */
	DISP_HDMI_PKT_GC_EN       = (1 <<  0),  /**< General Control Packet, refer to spec HDMI 1.4 section 5.3.6. */
	DISP_HDMI_PKT_GM_EN       = (1 <<  1),  /**< Gamut Metadata Packet, refer to spec HDMI 1.4 section 5.3.12. */
	DISP_HDMI_PKT_ACR_EN      = (1 <<  2),  /**< Audio Clock Regeneration Packet, refer to spec HDMI 1.4 section 5.3.3 and 7.2. */
	DISP_HDMI_PKT_AS_EN       = (1 <<  3),  /**< Audio Sample Packet, refer to spec HDMI 1.4 section 5.3.4. */
	DISP_HDMI_PKT_ACP_EN      = (1 <<  4),  /**< Audio Content Protection Packet, refer to spec HDMI 1.4 section 5.3.7. */
	DISP_HDMI_PKT_ISRC1_EN    = (1 <<  5),  /**< ISRC1 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	DISP_HDMI_PKT_ISRC2_EN    = (1 <<  6),  /**< ISRC2 Packet, refer to spec HDMI 1.4 section 5.3.8. */
	DISP_HDMI_PKT_VS_IF_EN    = (1 <<  7),  /**< Vendor Specific InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.3. */
	DISP_HDMI_PKT_AVI_IF_EN   = (1 <<  8),  /**< AVI InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.1. */
	DISP_HDMI_PKT_SPD_IF_EN   = (1 <<  9),  /**< Source Product Descriptor InfoFrame Packet. */
	DISP_HDMI_PKT_AUDIO_IF_EN = (1 << 10),  /**< Audio InfoFrame Packet, refer to spec HDMI 1.4 section 8.2.2. */
	DISP_HDMI_PKT_MPEG_IF_EN  = (1 << 11),  /**< MPEG Source InfoFrame Packet. */
} dispHdmiPktEn_e;  /* plz keep sync to halDispHdmiPktEn_e. */

typedef enum _dispLcmCmdHeader_e {
	DISP_LCM_CMD_INST = 0x01000000L,
	DISP_LCM_CMD_PARA = 0x02000000L,
	DISP_LCM_CMD_WAIT = 0x03000000L,
	DISP_LCM_CMD_STOP = 0x04000000L,
} dispLcmCmdHeader_e;  /* plz keep sync to halDispLcmCmdHeader_e. */

typedef enum _dispDzoomFitMethod_e {
	DISP_DZOOM_FIT_METHOD_DST_INSIDE,
	DISP_DZOOM_FIT_METHOD_DST_OUTSIDE,
	DISP_DZOOM_FIT_METHOD_SRC_VARY_WINDOW,
	DISP_DZOOM_FIT_METHOD_SRC_FIX_WINDOW,
	DISP_DZOOM_FIT_METHOD_SRC_STRETCH,
} dispDzoomFitMethod_e;  /* plz keep sync to halCanvasFitMethod_e. */

typedef enum {
	DISP_SYS_CFG_TOTAL,
} dispSysCfg_t;

#define DISP_DEV_TOT              5
#define DISP_SRC_TOT              2
#define DISP_ALPHA_MAX            128
#define DISP_BLEND_TBL_ENTRY_TOT  4
#define DISP_PALETTE_ENTRY_TOT    256
#define DISP_HDMI_CHANNEL_TOT     1

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef struct _dispCapabDesc_t {
	UINT32 width;
	UINT32 height;
	UINT32 xoff;
	UINT32 yoff;
	UINT32 aspectRatioX;
	UINT32 aspectRatioY;
	UINT32 logiEn;        /*!< logical size is in aspectRatioX and aspectRatioY or not */
	UINT32 frameRate;     /* frame rate in 1000X. */
} dispCapabDesc_t;

typedef UINT8 *(*pfOperatingDispDevNameGet_t)(void);
typedef UINT32 (*pfOperatingDispPowerOn_t)(void);
typedef UINT32 (*pfOperatingDispPowerOff_t)(void);
typedef void (*pfOperatingDispCapabilityGet_t)(dispCapabDesc_t* pcapabDesc, UINT32 descSize);
typedef UINT32 (*pfOperatingDispResolutionSet_t)(UINT32 resId);
typedef UINT32 (*pfOperationDispDevCmd)(UINT32 mode);

typedef struct _dispOperatingFunc_t {
	pfOperatingDispDevNameGet_t pfOpDevNameGet;
	pfOperatingDispPowerOn_t pfOpPowerOn;
	pfOperatingDispPowerOff_t pfOpPowerOff;
	pfOperatingDispCapabilityGet_t pfOpCapabilityGet;
	pfOperatingDispResolutionSet_t pfOpResolutionSet;
	pfOperationDispDevCmd pfOpDevCmd;
} dispOperatingFunc_t;

typedef void (*dispCallbackEvent_t)(UINT32 id);

typedef struct _dispCallbackFunc_t {
	dispCallbackEvent_t pfCbBeforePowerOn;
	dispCallbackEvent_t pfCbAfterPowerOn;
	dispCallbackEvent_t pfCbBeforePowerOff;
	dispCallbackEvent_t pfCbAfterPowerOff;
} dispCallbackFunc_t;

typedef struct _dispPara_t {
	UINT32 paraId;
	SINT32 para1;
	SINT32 para2;
	SINT32 para3;
	SINT32 para4;
	UINT32 upd;
} dispPara_t;

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define RGB888_TO_RGB565(rgb888) \
	(UINT16)((UINT32)(((rgb888) & 0xF80000) >> 8) | \
	         (UINT32)(((rgb888) & 0x00FC00) >> 5) | \
	         (UINT32)(((rgb888) & 0x0000F8) >> 3))

#define BGR888_TO_RGB565(bgr888) \
	(UINT16)((UINT32)(((bgr888) & 0xF80000) >> 19) | \
	         (UINT32)(((bgr888) & 0x00FC00) >> 5) | \
	         (UINT32)(((bgr888) & 0x0000F8) << 8))

#define RGB888_TO_BGR565(rgb888)  BGR888_TO_RGB565(rgb888)

#define BGR888_TO_BGR565(bgr888)  RGB888_TO_RGB565(bgr888)

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* disp_cb.c */
UINT32 dispCallbackReg(UINT32 id, void *pfunc, UINT32 waitCnt, UINT32 loopCnt);
UINT32 dispCallbackUnreg(UINT32 id);

/* disp_dev.c */
UINT32 dispDevSwitch(UINT32 devId);
UINT32 dispDevAttrSet(UINT32 paraId, SINT32 para1, SINT32 para2, SINT32 para3, SINT32 para4);

/* disp_event.c */
UINT32 dispEventWait(UINT32 id);

/* disp_lcm.c */
UINT32 dispLcmCmdTableSend(UINT32 *cmdTbl);
UINT32 dispLcmCmdRecieve(UINT32 cmd, UINT32 dataLen, UINT32 rdPin, UINT32 attr, UINT32 *pdata);

/* disp_logi.c */
UINT32 dispDevAlloc(UINT32 devTot);
UINT32 dispDevOpen(dispOperatingFunc_t *popFunc, UINT32 opFuncSize);
UINT32 dispDevCallbackReg(UINT32 devHndl, dispCallbackFunc_t *pcbFunc, UINT32 cbFuncSize);
UINT32 dispDevHandleGet(UINT8 *pname);
UINT32 dispDevActSet(UINT32 handle);
UINT32 dispDevActGet(void);
UINT32 dispDevTotalGet(void);
UINT8 *dispDevNameGet(UINT32 handle);
UINT32 dispPowerOn(UINT32 handle);
UINT32 dispPowerOff(UINT32 handle);
void   dispCapabilityGet(UINT32 handle, dispCapabDesc_t* pcapabDesc);
UINT32 dispResolutionSet(UINT32 handle, UINT32 resId);
void   dispVirtualWinSizeSet(UINT32 width, UINT32 height);
void   dispVirtualWinSizeGet(UINT32 *pwidth, UINT32 *pheight);
UINT32 dispChnlGet(UINT32 handle);

/* disp_osd.c */
void dispOsdRgb565BlendTblSet(UINT16 *pcolorTbl, UINT8 *pblendTbl);
void dispOsdPaletteSet(UINT16 *paltTbl, UINT32 begIdx, UINT32 len);
void dispOsdPaletteGet(UINT16 *paltTbl, UINT32 begIdx, UINT32 len);
void dispOsdRgb565ColorKeySet(UINT32 color, UINT32 blend);

/* disp_para.c */
UINT32 dispParaSet(UINT32 chnlId, UINT32 paraId, SINT32 para1, SINT32 para2, SINT32 para3, SINT32 para4, UINT32 upd);
UINT32 dispParaTableSet(UINT32 chnlId, dispPara_t *paraTbl);

#endif  /* _DISP_DEF_H_ */

