/*******************************************************************************
*
*       Copyright (c) 2011-2012 by iCatch Technology, Inc.
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
*******************************************************************************/

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#if SPCA6330
#include "common.h"
#include "debug_mod/dbg_api.h"
#include "middleware/disp_def.h"
#include "api/sp5k_cec_api.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
    DISP_ALL=1 : 720X480I60HZ, 720X576I50HZ, 720X480P60HZ, 720X576P50HZ,
                 1280X720P60HZ, 1280X720P50HZ, 1920X1080I60HZ, 1920X1080I50HZ.
*/

/* render dimension. */
#define DEV_PID  1

/* EIA-CEA-861-D section 4.6 720(1440)x480i @59.94/60 Hz (CEA-861-D Figure 5) */
#define DEV_NAME_FMT6    "HDMI - 720X480I60HZ"
#define DEV_TYPE_FMT6    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT6       DISP_INTERLACE_NOSOF
#define FIELD_FMT6       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT6       1440
#define HEIGHT_FMT6      (240+240)
#define HSYNCOS_FMT6     (38-1)
#define VSYNCOS_FMT6     ((4-1)+(4-1))
#define HSYNCW_FMT6      (38+124-1)
#define VSYNCW_FMT6      ((4+3-1)+(4+3-1))
#define HPIXEL_FMT6      (1440+276)
#define VLINE_FMT6       ((240+22)+(240+23))
#define PNLH0_FMT6       (38+124+114-1)
#define PNLV0_FMT6       ((4+3+15-1)+(4+3+16-1))
#define PNLH1_FMT6       (PNLH0_FMT6 + WIDTH_FMT6 - 1)
#define PNLV1_FMT6       (PNLV0_FMT6 + HEIGHT_FMT6 - 1)
#ifndef FPGA
#define CLK_NX_FMT6      CLK_1X_FMT6
#define CLK_1X_FMT6      27000
#define SCL_CLK_FMT6     200000
#else
#define CLK_NX_DIV_FMT6  11
#define CLK_1X_DIV_FMT6  1
#endif

/* EIA-CEA-861-D section 4.10 720(1440)x576i @50 Hz (CEA-861-D Figure 9) */
#define DEV_NAME_FMT21    "HDMI - 720X576I50HZ"
#define DEV_TYPE_FMT21    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT21       DISP_INTERLACE_NOSOF
#define FIELD_FMT21       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT21       1440
#define HEIGHT_FMT21      (288+288)
#define HSYNCOS_FMT21     (24-1)
#define VSYNCOS_FMT21     ((2-1)+(2-1))
#define HSYNCW_FMT21      (24+126-1)
#define VSYNCW_FMT21      ((2+3-1)+(2+3-1))
#define HPIXEL_FMT21      (1440+288)
#define VLINE_FMT21       ((288+24)+(288+25))
#define PNLH0_FMT21       (24+126+138-1)
#define PNLV0_FMT21       ((2+3+19-1)+(2+3+20-1))
#define PNLH1_FMT21       (PNLH0_FMT21 + WIDTH_FMT21 - 1)
#define PNLV1_FMT21       (PNLV0_FMT21 + HEIGHT_FMT21 - 1)
#ifndef FPGA
#define CLK_NX_FMT21      CLK_1X_FMT21
#define CLK_1X_FMT21      27000
#define SCL_CLK_FMT21     200000
#else
#define CLK_NX_DIV_FMT21  11
#define CLK_1X_DIV_FMT21  1
#endif

/* EIA-CEA-861-D section 4.5 720x480p @59.94/60 Hz (CEA-861-D Figure 4) */
#define DEV_NAME_FMT2    "HDMI - 720X480P60HZ"
#define DEV_TYPE_FMT2    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT2       DISP_PROGRESSIVE
#define FIELD_FMT2       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT2       720
#define HEIGHT_FMT2      480
#define HSYNCOS_FMT2     (16-1)
#define VSYNCOS_FMT2     (9-1)
#define HSYNCW_FMT2      (16+62-1)
#define VSYNCW_FMT2      (9+6-1)
#define HPIXEL_FMT2      (720+138)
#define VLINE_FMT2       (480+45)
#define PNLH0_FMT2       (16+62+60-1)
#define PNLV0_FMT2       (9+6+30-1)
#define PNLH1_FMT2       (PNLH0_FMT2 + WIDTH_FMT2 - 1)
#define PNLV1_FMT2       (PNLV0_FMT2 + HEIGHT_FMT2 - 1)
#ifndef FPGA
#define CLK_NX_FMT2      CLK_1X_FMT2
#define CLK_1X_FMT2      27000
#define SCL_CLK_FMT2     200000
#else
#define CLK_NX_DIV_FMT2  11
#define CLK_1X_DIV_FMT2  1
#endif

/* EIA-CEA-861-D section 4.9 720x576p @50 Hz (CEA-861-D Figure 8) */
#define DEV_NAME_FMT17    "HDMI - 720X576P50HZ"
#define DEV_TYPE_FMT17    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT17       DISP_PROGRESSIVE
#define FIELD_FMT17       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT17       720
#define HEIGHT_FMT17      576
#define HSYNCOS_FMT17     (12-1)
#define VSYNCOS_FMT17     (5-1)
#define HSYNCW_FMT17      (12+64-1)
#define VSYNCW_FMT17      (5+5-1)
#define HPIXEL_FMT17      (720+144)
#define VLINE_FMT17       (576+49)
#define PNLH0_FMT17       (12+64+68-1)
#define PNLV0_FMT17       (5+5+39-1)
#define PNLH1_FMT17       (PNLH0_FMT17 + WIDTH_FMT17 - 1)
#define PNLV1_FMT17       (PNLV0_FMT17 + HEIGHT_FMT17 - 1)
#ifndef FPGA
#define CLK_NX_FMT17      CLK_1X_FMT17
#define CLK_1X_FMT17      27000
#define SCL_CLK_FMT17     200000
#else
#define CLK_NX_DIV_FMT17  11
#define CLK_1X_DIV_FMT17  1
#endif

/* EIA-CEA-861-D section 4.3 1280x720p @59.94/60 Hz (CEA-861-D Figure 2) */
#define DEV_NAME_FMT4    "HDMI - 1280X720P60HZ"
#define DEV_TYPE_FMT4    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT4       DISP_PROGRESSIVE
#define FIELD_FMT4       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT4       1280
#define HEIGHT_FMT4      720
#define HSYNCOS_FMT4     (110-1)
#define VSYNCOS_FMT4     (5-1)
#define HSYNCW_FMT4      (110+40-1)
#define VSYNCW_FMT4      (5+5-1)
#define HPIXEL_FMT4      (1280+370)
#define VLINE_FMT4       (720+30)
#define PNLH0_FMT4       (110+40+220-1)
#define PNLV0_FMT4       (5+5+20-1)
#define PNLH1_FMT4       (PNLH0_FMT4 + WIDTH_FMT4 - 1)
#define PNLV1_FMT4       (PNLV0_FMT4 + HEIGHT_FMT4 - 1)
#ifndef FPGA
#define CLK_NX_FMT4      CLK_1X_FMT4
#define CLK_1X_FMT4      74250
#define SCL_CLK_FMT4     148500
#else
#define CLK_NX_DIV_FMT4  4
#define CLK_1X_DIV_FMT4  1
#endif

/* EIA-CEA-861-D section 4.7 1280x720p @50 Hz (CEA-861-D Figure 6) */
#define DEV_NAME_FMT19    "HDMI - 1280X720P50HZ"
#define DEV_TYPE_FMT19    DISP_PANEL_DEVICE_TYPE_UPS051
#define FRAME_FMT19       DISP_PROGRESSIVE
#define FIELD_FMT19       DISP_SRC_FIELD_NOSWAP
#define WIDTH_FMT19       1280
#define HEIGHT_FMT19      720
#if 1  /* mattwang's modification. */
#define HSYNCOS_FMT19     (110-1)
#define VSYNCOS_FMT19     (5-1)
#define HSYNCW_FMT19      (110+40-1)
#define VSYNCW_FMT19      (5+5-1)
#define HPIXEL_FMT19      (1280+700)
#define VLINE_FMT19       (720+30)
#define PNLH0_FMT19       (110+40+220-1)
#define PNLV0_FMT19       (5+5+20-1)
#else  /* original spec setting. */
#define HSYNCOS_FMT19     (440-1)
#define VSYNCOS_FMT19     (5-1)
#define HSYNCW_FMT19      (440+40-1)
#define VSYNCW_FMT19      (5+5-1)
#define HPIXEL_FMT19      (1280+700)
#define VLINE_FMT19       (720+30)
#define PNLH0_FMT19       (440+40+220-1)
#define PNLV0_FMT19       (5+5+20-1)
#endif
#define PNLH1_FMT19       (PNLH0_FMT19 + WIDTH_FMT19 - 1)
#define PNLV1_FMT19       (PNLV0_FMT19 + HEIGHT_FMT19 - 1)
#ifndef FPGA
#define CLK_NX_FMT19      CLK_1X_FMT19
#define CLK_1X_FMT19      74250
#define SCL_CLK_FMT19     148500
#else
#define CLK_NX_DIV_FMT19  4
#define CLK_1X_DIV_FMT19  1
#endif

/* EIA-CEA-861-D section 4.4 1920x1080i @59.94/60 Hz (CEA-861-D Figure 3) */
#define DEV_NAME_FMT5    "HDMI - 1920X1080I60HZ"
#define DEV_TYPE_FMT5    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT5       DISP_INTERLACE_NOSOF
#define FIELD_FMT5       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT5       1920
#define HEIGHT_FMT5      (540+540)
#define HSYNCOS_FMT5     (88-1)
#define VSYNCOS_FMT5     ((2-1)+(2-1))
#define HSYNCW_FMT5      (88+44-1)
#define VSYNCW_FMT5      ((2+5-1)+(2+5-1))
#define HPIXEL_FMT5      (1920+280)
#define VLINE_FMT5       ((540+22)+(540+23))
#define PNLH0_FMT5       (88+44+148-1)
#define PNLV0_FMT5       ((2+5+15-1)+(2+5+16-1))
#define PNLH1_FMT5       (PNLH0_FMT5 + WIDTH_FMT5 - 1)
#define PNLV1_FMT5       (PNLV0_FMT5 + HEIGHT_FMT5 - 1)
#ifndef FPGA
#define CLK_NX_FMT5      CLK_1X_FMT5
#define CLK_1X_FMT5      74250
#define SCL_CLK_FMT5     148500
#else
#define CLK_NX_DIV_FMT5  4
#define CLK_1X_DIV_FMT5  1
#endif

/* EIA-CEA-861-D section 4.8 1920x1080i @50 Hz (CEA-861-D Figure 7) */
#define DEV_NAME_FMT20    "HDMI - 1920X1080I50HZ"
#define DEV_TYPE_FMT20    DISP_PANEL_DEVICE_TYPE_STD_INTERLACE_CCIR601
#define FRAME_FMT20       DISP_INTERLACE_NOSOF
#define FIELD_FMT20       DISP_SRC_FIELD_SWAP
#define WIDTH_FMT20       1920
#define HEIGHT_FMT20      (540+540)
#if 1  /* mattwang's modification. */
#define HSYNCOS_FMT20     (88-1)
#define VSYNCOS_FMT20     ((2-1)+(2-1))
#define HSYNCW_FMT20      (88+44-1)
#define VSYNCW_FMT20      ((2+5-1)+(2+5-1))
#define HPIXEL_FMT20      (1920+720)
#define VLINE_FMT20       ((540+22)+(540+23))
#define PNLH0_FMT20       (88+44+148-1)
#define PNLV0_FMT20       ((2+5+15-1)+(2+5+16-1))
#else  /* original spec setting. */
#define HSYNCOS_FMT20     (528-1)
#define VSYNCOS_FMT20     ((2-1)+(2-1))
#define HSYNCW_FMT20      (528+44-1)
#define VSYNCW_FMT20      ((2+5-1)+(2+5-1))
#define HPIXEL_FMT20      (1920+720)
#define VLINE_FMT20       ((540+22)+(540+23))
#define PNLH0_FMT20       (528+44+148-1)
#define PNLV0_FMT20       ((2+5+15-1)+(2+5+16-1))
#endif
#define PNLH1_FMT20       (PNLH0_FMT20 + WIDTH_FMT20 - 1)
#define PNLV1_FMT20       (PNLV0_FMT20 + HEIGHT_FMT20 - 1)
#ifndef FPGA
#define CLK_NX_FMT20      CLK_1X_FMT20
#define CLK_1X_FMT20      74250
#define SCL_CLK_FMT20     148500
#else
#define CLK_NX_DIV_FMT20  4
#define CLK_1X_DIV_FMT20  1
#endif

#define HDMI_PKT_EN  DISP_HDMI_PKT_AUDIO_IF_EN | DISP_HDMI_PKT_AVI_IF_EN | \
                     DISP_HDMI_PKT_AS_EN | DISP_HDMI_PKT_ACR_EN | DISP_HDMI_PKT_GC_EN

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#define dispOpen             dispHdmiTvOpen
#define dispOpDevNameGet     dispHdmiTvOpDevNameGet
#define dispOpPowerOn        dispHdmiTvOpPowerOn
#define dispOpPowerOff       dispHdmiTvOpPowerOff
#define dispOpCapabilityGet  dispHdmiTvOpCapabilityGet
#define dispOpResolutionSet  dispHdmiTvOpResolutionSet

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpResolutionSet(UINT32 resId);

/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* resolution id. */
static UINT32 dispResId = SP5K_EDID_720X480I_60HZ_4T3;

/* device name. */
static UINT8 *pdispDevName[] = {
	[SP5K_EDID_720X480I_60HZ_4T3   ] = DEV_NAME_FMT6,
	[SP5K_EDID_720X576I_50HZ_4T3   ] = DEV_NAME_FMT21,
	[SP5K_EDID_720X480P_60HZ_4T3   ] = DEV_NAME_FMT2,
	[SP5K_EDID_720X576P_50HZ_4T3   ] = DEV_NAME_FMT17,
	[SP5K_EDID_1280X720P_60HZ_16T9 ] = DEV_NAME_FMT4,
	[SP5K_EDID_1280X720P_50HZ_16T9 ] = DEV_NAME_FMT19,
	[SP5K_EDID_1920X1080I_60HZ_16T9] = DEV_NAME_FMT5,
	[SP5K_EDID_1920X1080I_50HZ_16T9] = DEV_NAME_FMT20,
};

/* device capability. */
static dispCapabDesc_t dispCapabDesc[] = {
	[SP5K_EDID_720X480I_60HZ_4T3] = {
		.width        = WIDTH_FMT6,
		.height       = HEIGHT_FMT6,
		.xoff         = PNLH0_FMT6,
		.yoff         = PNLV0_FMT6,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X576I_50HZ_4T3] = {
		.width        = WIDTH_FMT21,
		.height       = HEIGHT_FMT21,
		.xoff         = PNLH0_FMT21,
		.yoff         = PNLV0_FMT21,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_720X480P_60HZ_4T3] = {
		.width        = WIDTH_FMT2,
		.height       = HEIGHT_FMT2,
		.xoff         = PNLH0_FMT2,
		.yoff         = PNLV0_FMT2,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_720X576P_50HZ_4T3] = {
		.width        = WIDTH_FMT17,
		.height       = HEIGHT_FMT17,
		.xoff         = PNLH0_FMT17,
		.yoff         = PNLV0_FMT17,
		.aspectRatioX = 4,
		.aspectRatioY = 3,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1280X720P_60HZ_16T9] = {
		.width        = WIDTH_FMT4,
		.height       = HEIGHT_FMT4,
		.xoff         = PNLH0_FMT4,
		.yoff         = PNLV0_FMT4,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1280X720P_50HZ_16T9] = {
		.width        = WIDTH_FMT19,
		.height       = HEIGHT_FMT19,
		.xoff         = PNLH0_FMT19,
		.yoff         = PNLV0_FMT19,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
	[SP5K_EDID_1920X1080I_60HZ_16T9] = {
		.width        = WIDTH_FMT5,
		.height       = HEIGHT_FMT5,
		.xoff         = PNLH0_FMT5,
		.yoff         = PNLV0_FMT5,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 60000,
	},
	[SP5K_EDID_1920X1080I_50HZ_16T9] = {
		.width        = WIDTH_FMT20,
		.height       = HEIGHT_FMT20,
		.xoff         = PNLH0_FMT20,
		.yoff         = PNLV0_FMT20,
		.aspectRatioX = 16,
		.aspectRatioY = 9,
		.logiEn       = 0,
		.frameRate    = 50000,
	},
};

/* hdmi data island. */
static UINT8 dispHdmiAviInfoFramePacketHeader[] = {
	DISP_HDMI_PKT_AVI_IF,  /* Packet Type. */
	0x02,                  /* Version Number. */
	0x0D,                  /* Length. */
};

static UINT8 disp720x480i60HzHdmiAviInfoFramePacketBody[] = {
	0x00,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x06,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 disp720x576i50HzHdmiAviInfoFramePacketBody[] = {
	0x00,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x15,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x01,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 disp720x480p60HzHdmiAviInfoFramePacketBody[] = {
	0x05,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x02,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 disp720x576p50HzHdmiAviInfoFramePacketBody[] = {
	0x05,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0x58,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x11,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 disp1280x720p60HzHdmiAviInfoFramePacketBody[] = {
	0xB3,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x04,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 disp1280x720p50HzHdmiAviInfoFramePacketBody[] = {
	0xB3,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x13,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 disp1920x1080i60HzHdmiAviInfoFramePacketBody[] = {
	0xB2,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x05,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 disp1920x1080i50HzHdmiAviInfoFramePacketBody[] = {
	0xB2,  /* Checksum, but it will be figured automatically. */
	0x10,  /* Rsvd Y1   Y0   A0   | B1   B0   S1   S0   */
	0xA8,  /* C1   C0   M1   M0   | R3   R2   R1   R0   */
	0x00,  /* ITC  EC2  EC1  EC0  | Q1   Q0   SC1  SC0  */
	0x14,  /* Rsvd VIC6 VIC5 VIC4 | VIC3 VIC2 VIC1 VIC0 */
	0x00,  /* YQ1  YQ0  CN1  CN0  | PR3  PR2  PR1  PR0  */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

static UINT8 dispHdmiAudioInfoFramePacketHeader[] = {
	DISP_HDMI_PKT_AUDIO_IF,  /* Packet Type. */
	0x01,                    /* Version Number. */
	0x0A,                    /* Length. */
};

static UINT8 dispHdmiAudioInfoFramePacketBody[] = {
	0x70,  /* Checksum, but it will be figured automatically. */
	0x01,  /* CT3    CT2  CT1  CT0  | Rsvd CC2  CC1     CC0     */
	0x00,  /* Rsvd   Rsvd Rsvd SF2  | SF1  SF0  SS1     SS0     */
	0x00,  /* Format depends on coding type. */
	0x00,  /* CA7    CA6  CA5  CA4  | CA3  CA2  CA1     CA0     */
	0x00,  /* DM_INH LSV3 LSV2 LSV1 | LSV0 Rsvd LFEPBL1 LFEPBL0 */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

/* parameter table. */
static dispPara_t disp720x480i60HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT6, CLK_NX_FMT6, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT6, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT6, CLK_1X_DIV_FMT6, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT6, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT6, FIELD_FMT6, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x00, 0x00, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp720x480i60HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT6, VLINE_FMT6, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT6, VSYNCOS_FMT6, HSYNCW_FMT6, VSYNCW_FMT6, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT6, PNLV0_FMT6, PNLH1_FMT6, PNLV1_FMT6, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576i50HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT21, CLK_NX_FMT21, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT21, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT21, CLK_1X_DIV_FMT21, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT21, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT21, FIELD_FMT21, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x00, 0x00, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp720x576i50HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT21, VLINE_FMT21, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT21, VSYNCOS_FMT21, HSYNCW_FMT21, VSYNCW_FMT21, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT21, PNLV0_FMT21, PNLH1_FMT21, PNLV1_FMT21, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x480p60HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT2, CLK_NX_FMT2, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT2, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT2, CLK_1X_DIV_FMT2, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT2, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT2, FIELD_FMT2, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x00, 0x00, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp720x480p60HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT2, VLINE_FMT2, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT2, VSYNCOS_FMT2, HSYNCW_FMT2, VSYNCW_FMT2, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT2, PNLV0_FMT2, PNLH1_FMT2, PNLV1_FMT2, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp720x576p50HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT17, CLK_NX_FMT17, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT17, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT17, CLK_1X_DIV_FMT17, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT17, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT17, FIELD_FMT17, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x00, 0x00, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp720x576p50HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT17, VLINE_FMT17, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT17, VSYNCOS_FMT17, HSYNCW_FMT17, VSYNCW_FMT17, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT17, PNLV0_FMT17, PNLH1_FMT17, PNLV1_FMT17, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p60HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT4, CLK_NX_FMT4, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT4, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT4, CLK_1X_DIV_FMT4, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT4, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT4, FIELD_FMT4, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x01, 0x01, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp1280x720p60HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT4, VLINE_FMT4, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT4, VSYNCOS_FMT4, HSYNCW_FMT4, VSYNCW_FMT4, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT4, PNLV0_FMT4, PNLH1_FMT4, PNLV1_FMT4, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1280x720p50HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT19, CLK_NX_FMT19, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT19, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT19, CLK_1X_DIV_FMT19, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT19, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT19, FIELD_FMT19, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x01, 0x01, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp1280x720p50HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT19, VLINE_FMT19, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT19, VSYNCOS_FMT19, HSYNCW_FMT19, VSYNCW_FMT19, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT19, PNLV0_FMT19, PNLH1_FMT19, PNLV1_FMT19, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i60HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT5, CLK_NX_FMT5, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT5, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT5, CLK_1X_DIV_FMT5, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT5, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT5, FIELD_FMT5, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x01, 0x01, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp1920x1080i60HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT5, VLINE_FMT5, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT5, VSYNCOS_FMT5, HSYNCW_FMT5, VSYNCW_FMT5, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT5, PNLV0_FMT5, PNLH1_FMT5, PNLV1_FMT5, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t disp1920x1080i50HzPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, DISP_CLK_SRC_DRAMPLL, DISP_TV_TYPE_DIGITAL, 0, 0, 0},
	#ifndef FPGA
	{DISP_PARA_CLK_FREQ, CLK_1X_FMT20, CLK_NX_FMT20, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK_FMT20, 0, 0, 0, 0},
	#else
	{DISP_PARA_CLK_DIVISOR, CLK_NX_DIV_FMT20, CLK_1X_DIV_FMT20, 0, 0, 0},
	#endif
	{DISP_PARA_RESET, 0, 1, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE_FMT20, 0, 0, 0, 0},
	{DISP_PARA_INTERLACE_MODE, FRAME_FMT20, FIELD_FMT20, 0, 0, 0},
	{DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_NOSWAP, 0, 0, 0, 0},
	{DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_TV_MAJOR, 0, 0, 0, 0},

	/* hdmi data island. */
	{DISP_PARA_HDMI_PHY_POWER, 1, 0, 0, 0, 0},
	{DISP_PARA_HDMI_SIGNAL_POLARITY, 0x01, 0x01, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAviInfoFramePacketHeader, (UINT32)disp1920x1080i50HzHdmiAviInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_DATA_ISLAND, (UINT32)dispHdmiAudioInfoFramePacketHeader, (UINT32)dispHdmiAudioInfoFramePacketBody, 0, 0, 0},
	{DISP_PARA_HDMI_PACKET_ENABLE, HDMI_PKT_EN, HDMI_PKT_EN, 0, 0, 0},

	/* set vh timing and format */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL_FMT20, VLINE_FMT20, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS_FMT20, VSYNCOS_FMT20, HSYNCW_FMT20, VSYNCW_FMT20, 0},
	{DISP_PARA_DEVICE_SYNC_TIME_POINT, 1, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0_FMT20, PNLV0_FMT20, PNLH1_FMT20, PNLV1_FMT20, 0},

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

static dispPara_t *pdispPwrOnParaTbl[] = {
	[SP5K_EDID_720X480I_60HZ_4T3   ] = disp720x480i60HzPwrOnParaTbl,
	[SP5K_EDID_720X576I_50HZ_4T3   ] = disp720x576i50HzPwrOnParaTbl,
	[SP5K_EDID_720X480P_60HZ_4T3   ] = disp720x480p60HzPwrOnParaTbl,
	[SP5K_EDID_720X576P_50HZ_4T3   ] = disp720x576p50HzPwrOnParaTbl,
	[SP5K_EDID_1280X720P_60HZ_16T9 ] = disp1280x720p60HzPwrOnParaTbl,
	[SP5K_EDID_1280X720P_50HZ_16T9 ] = disp1280x720p50HzPwrOnParaTbl,
	[SP5K_EDID_1920X1080I_60HZ_16T9] = disp1920x1080i60HzPwrOnParaTbl,
	[SP5K_EDID_1920X1080I_50HZ_16T9] = disp1920x1080i50HzPwrOnParaTbl,
};

/**
 * @brief Open (register) a device driver.
 *
 * @param None.
 *
 * @return Device handle.
 */
UINT32
dispOpen(
	void
)
{
	UINT32 ret;
	dispOperatingFunc_t dispOpFunc;

	DBG_FUNC_ENTER(NULL);

	dispOpFunc.pfOpDevNameGet    = dispOpDevNameGet;
	dispOpFunc.pfOpPowerOn       = dispOpPowerOn;
	dispOpFunc.pfOpPowerOff      = dispOpPowerOff;
	dispOpFunc.pfOpCapabilityGet = dispOpCapabilityGet;
	dispOpFunc.pfOpResolutionSet = dispOpResolutionSet;
	ret = dispDevOpen(&dispOpFunc, sizeof(dispOperatingFunc_t));

	DBG_FUNC_RETURN(ret, NULL);
}

/**
 * @brief Get device name string in current resolution of the device.
 *
 * @param None.
 *
 * @return Device name string.
 */
static UINT8 *
dispOpDevNameGet(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	DBG_FUNC_RETURN(pdispDevName[dispResId], "%s", pdispDevName[dispResId]);
}

/**
 * @brief Power on the device.
 *
 * @param None.
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpPowerOn(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	dispParaTableSet(DEV_PID, pdispPwrOnParaTbl[dispResId]);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/**
 * @brief Power off the device.
 *
 * @param None.
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpPowerOff(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	dispParaSet(DEV_PID, DISP_PARA_HDMI_PHY_POWER, 0, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_IMG_NONSCALE_INTERLACE_MODE, DISP_NONSCALE_INTRLC_DISABLE, 0, 0, 0, 0);
	dispParaSet(DEV_PID, DISP_PARA_BANDWIDTH_BUFFER_MODE, DISP_BANDWIDTH_BALANCE, 0, 0, 0, 1);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/**
 * @brief Get capability in current resolution of the device.
 *
 * @param [out] *pcapabDesc Capability descriptor.
 * @param [in]  descSize    Data size of the descriptor.
 *
 * @return None.
 */
static void
dispOpCapabilityGet(
	dispCapabDesc_t *pcapabDesc,
	UINT32 descSize
)
{
	*pcapabDesc = dispCapabDesc[dispResId];
}

/**
 * @brief Set resolution for further operating.
 *
 * @param [in] resId Resolution ID, \n
 *                   SP5K_EDID_720X480I_60HZ_4T3    EIA-CEA-861-D section 4.6 720(1440)x480i @59.94/60 Hz, \n
 *                   SP5K_EDID_720X576I_50HZ_4T3    EIA-CEA-861-D section 4.10 720(1440)x576i @50 Hz, \n
 *                   SP5K_EDID_720X480P_60HZ_4T3    EIA-CEA-861-D section 4.5 720x480p @59.94/60 Hz, \n
 *                   SP5K_EDID_720X576P_50HZ_4T3    EIA-CEA-861-D section 4.9 720x576p @50 Hz, \n
 *                   SP5K_EDID_1280X720P_60HZ_16T9  EIA-CEA-861-D section 4.3 1280x720p @59.94/60 Hz, \n
 *                   SP5K_EDID_1280X720P_50HZ_16T9  EIA-CEA-861-D section 4.7 1280x720p @50 Hz, \n
 *                   SP5K_EDID_1920X1080I_60HZ_16T9 EIA-CEA-861-D section 4.4 1920x1080i @59.94/60 Hz, \n
 *                   SP5K_EDID_1920X1080I_50HZ_16T9 EIA-CEA-861-D section 4.8 1920x1080i @50 Hz).
 *
 * @return SUCCESS or FAIL.
 */
static UINT32
dispOpResolutionSet(
	UINT32 resId
)
{
	DBG_FUNC_ENTER("%d", resId);

	dispResId = resId;

	DBG_FUNC_RETURN(SUCCESS, "OK");
}
#endif

