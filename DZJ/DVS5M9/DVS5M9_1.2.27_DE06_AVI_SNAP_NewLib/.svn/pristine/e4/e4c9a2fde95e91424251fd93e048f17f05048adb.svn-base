/*******************************************************************************
*
*       Copyright (c) 2010-2012 by iCatch Technology, Inc.
*
*  This software is copyrighted by and is the property of iCatch Technology,
*  Inc.. All rights are reserved by iCatch Technology, Inc..
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
*  File Description:
*
*  Author: 
*
*******************************************************************************/

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"
#include "debug_mod/dbg_api.h"
#include "middleware/disp_def.h"
#include "api/Sp5k_global_api.h"
#include "customization/solution_def.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
    DISP_LCM_H=1 : use lcm horizontal timing.
    DISP_LCM_V=1 : use lcm vertical timing.
*/
#define LCM_DATA_FMT  1  /* 0: rgb565, 1: rgb666. */
#define VSYNC_MODE    1  /* 0: chip master, 1: panel master. */
/*#define DISP_LCM_H   1 */
/*#define DISP_LCM_V   1 */

#define WAIT_MS	200
#define LCM_DELAY 0

#define DEV_PID  0

/* render dimension. */
#if DISP_LCM_H == 1
#define DEV_NAME     "PANEL - TXDT240TQ LCM-H"
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_LCM
#define WIDTH        320
#define HEIGHT       240
#define HSYNCOS      0
#define VSYNCOS      0
#define HSYNCW       2
#define VSYNCW       2
#define HPIXEL       360/*300*/
#if VSYNC_MODE == 1
#define VLINE        4095
#else
#define VLINE        262/*160*/
#endif
#define PNLH0        11/*8*/
#define PNLV0        3
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define SCL_CLK      148500
#if VSYNC_MODE == 1
#define CLK_NX       29700
#else
#define CLK_NX       15000
#endif
#define CLK_1X       CLK_NX/3 /* LCM_DATA_FMT=1,rgb666 */
#endif

#if DISP_LCM_V == 1
#define DEV_NAME     "PANEL - TXDT240TQ LCM-V"
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_LCM
#define WIDTH        240
#define HEIGHT       320
#define HSYNCOS      0
#define VSYNCOS      0
#define HSYNCW       2
#define VSYNCW       2
#define HPIXEL       262
#if VSYNC_MODE == 1
#define VLINE        4095
#else
#define VLINE        360
#endif
#define PNLH0        3
#define PNLV0        11
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define SCL_CLK      148500 /* 148.5MHz */

#if VSYNC_MODE == 1
#define CLK_NX       17470
#else
#define CLK_NX       15000
#endif
#define CLK_1X       CLK_NX/3 /* LCM_DATA_FMT=1,rgb666 */
#endif

/* lcm interface. */
#define BUS_TYPE     DISP_LCM_I80
#define BUS_WIDTH    DISP_LCM_8_BIT
#define CS_ACT       0
#define RS_CMD       0
#if LCM_DATA_FMT == 0  /* rgb565. */
#define DATA_FMT     DISP_LCM_RGB565
#define DATA_LEN     2
#define SWITCH_RB    0/*1*/
#define SWITCH_G     1/*1*/
#define SWITCH_PACK  1/*1*/
#define DATA_ALIGN   DISP_LCM_ALIGN_NONE
#else  /* rgb666. */
#define DATA_FMT     DISP_LCM_RGB666
#define DATA_LEN     3
#define SWITCH_RB    1
#define SWITCH_G     0
#define SWITCH_PACK  1
#define DATA_ALIGN   DISP_LCM_ALIGN_BIG
#endif

/* extra control pin. */
#define RST  (1 << 13)   /*DGPIO 13*/
#define BL   (1 << 15)   /*DGPIO 15*/
#define CS   (1 << 14)   /*DGPIO 14*/

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/
#define	RST_LO  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RST, 0, 0, 0, 0)
#define	RST_HI  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RST, RST, 0, 0, 0)
#define	BL_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, BL, 0, 0, 0, 0)
#define	BL_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, BL, BL, 0, 0, 0)
#define	CS_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, 0, 0, 0, 0)
#define	CS_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, CS, 0, 0, 0)

#define CMD(inst)  (DISP_LCM_CMD_INST | inst)

#define DAT(para)  (DISP_LCM_CMD_PARA | para)

#define CMD_WAIT(x)  (DISP_LCM_CMD_WAIT | (x))

#define CMD_STOP  DISP_LCM_CMD_STOP

#define READ8(_reg_)            (*((volatile UINT8  *)(_reg_)))
#define WRITE8(_reg_, _value_)  (*((volatile UINT8  *)(_reg_)) = (_value_))

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#if DISP_LCM_H == 1 
#define dispOpen        dispTxdt240LcmHOpen 
#define dispOpPowerOn   dispTxdt240LcmHOpPowerOn
#define dispOpPowerOff  dispTxdt240LcmHOpPowerOff
#endif 

#if DISP_LCM_V == 1 
#define dispOpen        dispTxdt240LcmVOpen 
#define dispOpPowerOn   dispTxdt240LcmVOpPowerOn
#define dispOpPowerOff  dispTxdt240LcmVOpPowerOff
#endif 

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);

/*******************************************************************************
*                            G L O B A L   D A T A
*******************************************************************************/
/* device name. */
static UINT8 dispDevName[] = DEV_NAME;

/* device capability. */
static dispCapabDesc_t dispCapabDesc = {
	.width        = WIDTH,
	.height       = HEIGHT,
	.xoff         = PNLH0,
	.yoff         = PNLV0,
	#if DISP_LCM_H == 1
	.aspectRatioX = 4,
	.aspectRatioY = 3,
	#endif
	#if DISP_LCM_V == 1
	.aspectRatioX = 3,
	.aspectRatioY = 4,
	#endif
	.logiEn       = 0,
	.frameRate    = 70000,
};

/* parameter table. */
static dispPara_t dispPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0},
	{DISP_PARA_CLK_PHASE, 0, 0, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0},
	{DISP_PARA_EDGE_ENHANCE, 0, 8, 1, 2, 0},

	/* set lcm interface. */
	{DISP_PARA_LCM_BUS_CFG, BUS_TYPE, BUS_WIDTH, CS_ACT, RS_CMD, 0},
	{DISP_PARA_LCM_DATA_FORMAT, DATA_FMT, DATA_LEN, 0, 0, 0},
	{DISP_PARA_LCM_DATA_PACKET, SWITCH_G, SWITCH_G, SWITCH_PACK, DATA_ALIGN, 0}, 
	{DISP_PARA_LCM_CMD_DELAY, LCM_DELAY, 0, 0, 0, 0},

	/*{DISP_PARA_LCM_SIGNAL_UPDATE_MODE, 1, 0, 0, 0, 0},*/

	/*{DISP_PARA_LCM_DATA_PACKET, SWITCH_RB, SWITCH_G, SWITCH_PACK, DATA_ALIGN, 0},*/ /* Eric 110723 */

	{DISP_PARA_GPIO_FUNC_CFG, RST | CS | BL , RST | CS | BL, 0, 0, 0},
	{DISP_PARA_GPIO_DIR_CFG, RST | CS | BL , RST | CS | BL, 0, 0, 0},
	#if VSYNC_MODE == 1
	{DISP_PARA_GPIO_FUNC_CFG, 0x200, 0x200, 0, 0, 0}, /* panel vsync GPIO singal sel. */
	{DISP_PARA_GPIO_DIR_CFG, 0x200, 0x000, 0, 0, 0},  /* panel vsync GPIO sel input. */
	/*{DISP_PARA_GPIO_DIR_CFG, 0x200, 0x000, 0, 0, 0},*/
	{DISP_PARA_SIGNAL_POLARITY, 0x01, 0x01, 0, 0, 0}, /* panel vsync active high. */
	{DISP_PARA_LCM_SIGNAL_UPDATE_MODE, 0x01, 0x01, 0, 0, 0}, /* enable panel vsync mode. */
	#endif

	/* set vh timing and format. */
	/*{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0}, */
	/*{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0}, */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL, VLINE, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS, VSYNCOS, HSYNCW, VSYNCW, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0},

	{DISP_PARA_INTERLACE_MODE, DISP_PROGRESSIVE, 0, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

/* lcm command table. */
static UINT32 dispPwrOnCmdTbl[] = {
	#if 0
	CMD(0xef),CMD_WAIT(WAIT_MS),
	DAT(0x03),CMD_WAIT(WAIT_MS),
	DAT(0x80),CMD_WAIT(WAIT_MS),
	DAT(0x02),CMD_WAIT(WAIT_MS),
	#endif
	CMD(0xCF),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x81),CMD_WAIT(WAIT_MS),
	DAT(0X30),CMD_WAIT(WAIT_MS),
	CMD(0xED),CMD_WAIT(WAIT_MS),
	DAT(0x64),CMD_WAIT(WAIT_MS),
	DAT(0x03),CMD_WAIT(WAIT_MS),
	DAT(0X12),CMD_WAIT(WAIT_MS),
	DAT(0X81),CMD_WAIT(WAIT_MS),
	CMD(0xE8),CMD_WAIT(WAIT_MS),
	DAT(0x85),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x79),CMD_WAIT(WAIT_MS),
	CMD(0xCB),CMD_WAIT(WAIT_MS),
	DAT(0x39),CMD_WAIT(WAIT_MS),
	DAT(0x2C),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x34),CMD_WAIT(WAIT_MS),
	DAT(0x02),CMD_WAIT(WAIT_MS),
	CMD(0xF7),CMD_WAIT(WAIT_MS),
	DAT(0x20),CMD_WAIT(WAIT_MS),
	CMD(0xEA),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	CMD(0xC0),CMD_WAIT(WAIT_MS), /* Power Control 1 */
	DAT(0x1D),CMD_WAIT(WAIT_MS), /* Set the GVDD level:0x1D:1.30v, 0x21:4.5v */
	CMD(0xC1),CMD_WAIT(WAIT_MS), /* Power Control 2 */
	DAT(0x12),CMD_WAIT(WAIT_MS), /* Sets the factor used in the step-up circuits */
	CMD(0xC5),CMD_WAIT(WAIT_MS), /* VCOM Control 1,Set the VCOMH/VCOML voltage. */
	DAT(0x30),CMD_WAIT(WAIT_MS), /* VCOMH: 3.9v */
	DAT(0x3F),CMD_WAIT(WAIT_MS), /* VCOML: -0.925v */
	CMD(0xC7),CMD_WAIT(WAIT_MS), /* VCOM Control 2,Set the VCOM offset voltage. */
	DAT(0X90),CMD_WAIT(WAIT_MS), /* VCOMH:VMH-48; VCOML:VML-48 */
	CMD(0xB5),CMD_WAIT(WAIT_MS), /* Blanking Porch Control */
	DAT(0X7F),CMD_WAIT(WAIT_MS), /* the line number of vertical front porch period. */
	DAT(0X40),CMD_WAIT(WAIT_MS), /* the line number of vertical back porch period. */
	DAT(0X0A),CMD_WAIT(WAIT_MS), /* the line number of horizontal front porch period.(define value) */
	DAT(0X14),CMD_WAIT(WAIT_MS), /* the line number of horizontal back porch period.(define value) */
	CMD(0x36),CMD_WAIT(WAIT_MS), /* 0x36, Memory Access Control */
	#if DISP_LCM_V
	DAT(0x08),CMD_WAIT(WAIT_MS),
	#endif
	#if DISP_LCM_H
	DAT(0x68),CMD_WAIT(WAIT_MS),
	#endif
	CMD(0x3A),CMD_WAIT(WAIT_MS), /* Pixel Format Set */
	DAT(0x66),CMD_WAIT(WAIT_MS), /* 0x66 -> RGB666, 0x55 -> RGB565 */
	#if VSYNC_MODE == 1
	CMD(0x44),CMD_WAIT(WAIT_MS), /* Set Tear Scanline */
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS), 
	CMD(0x35),CMD_WAIT(WAIT_MS), /* Tearing Effect Line ON */
	DAT(0x00),CMD_WAIT(WAIT_MS),
	#else
	CMD(0x34),CMD_WAIT(WAIT_MS), /* Tearing Effect Line OFF */
	#endif
	#if DISP_LCM_V
	CMD(0x2B),CMD_WAIT(WAIT_MS), /* Page Address Set */
	#endif
	#if DISP_LCM_H
	CMD(0x2A),CMD_WAIT(WAIT_MS), /* Column Address Set */
	#endif
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x01),CMD_WAIT(WAIT_MS),
	DAT(0x3F),CMD_WAIT(WAIT_MS),
	#if DISP_LCM_V
	CMD(0x2A),CMD_WAIT(WAIT_MS),
	#endif
	#if DISP_LCM_H
	CMD(0x2B),CMD_WAIT(WAIT_MS),
	#endif
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0xEF),CMD_WAIT(WAIT_MS),
	CMD(0xB1),CMD_WAIT(WAIT_MS), /* Frame Control(In Normal Mode) */
	DAT(0x00),CMD_WAIT(WAIT_MS), /* Division Ratio:fosc/1 */
	DAT(0x1F),CMD_WAIT(WAIT_MS), /* Frame Rate (Hz):0x1F:61Hz */
	CMD(0xB6),CMD_WAIT(WAIT_MS), /* Display Function Control */
	DAT(0x0A),CMD_WAIT(WAIT_MS),
	DAT(0xA2),CMD_WAIT(WAIT_MS),
	CMD(0xF2),CMD_WAIT(WAIT_MS), /*  3Gamma Function Disable */
	DAT(0x00),CMD_WAIT(WAIT_MS),
	CMD(0x26),CMD_WAIT(WAIT_MS), /* Gamma Set,Gamma curve selected */
	DAT(0x01),CMD_WAIT(WAIT_MS),
	CMD(0xE0),CMD_WAIT(WAIT_MS), /* Positive Gamma Correction */
	DAT(0x0F),CMD_WAIT(WAIT_MS),
	DAT(0x22),CMD_WAIT(WAIT_MS),
	DAT(0x1C),CMD_WAIT(WAIT_MS),
	DAT(0x1B),CMD_WAIT(WAIT_MS),
	DAT(0x08),CMD_WAIT(WAIT_MS),
	DAT(0x0F),CMD_WAIT(WAIT_MS),
	DAT(0x49),CMD_WAIT(WAIT_MS),
	DAT(0XB8),CMD_WAIT(WAIT_MS),
	DAT(0x34),CMD_WAIT(WAIT_MS),
	DAT(0x05),CMD_WAIT(WAIT_MS),
	DAT(0x0C),CMD_WAIT(WAIT_MS),
	DAT(0x09),CMD_WAIT(WAIT_MS),
	DAT(0x0F),CMD_WAIT(WAIT_MS),
	DAT(0x07),CMD_WAIT(WAIT_MS),
	DAT(0x00),CMD_WAIT(WAIT_MS),
	CMD(0XE1),CMD_WAIT(WAIT_MS), /* Negative Gamma Correction */
	DAT(0x00),CMD_WAIT(WAIT_MS),
	DAT(0x23),CMD_WAIT(WAIT_MS),
	DAT(0x24),CMD_WAIT(WAIT_MS),
	DAT(0x07),CMD_WAIT(WAIT_MS),
	DAT(0x10),CMD_WAIT(WAIT_MS),
	DAT(0x07),CMD_WAIT(WAIT_MS),
	DAT(0x38),CMD_WAIT(WAIT_MS),
	DAT(0x47),CMD_WAIT(WAIT_MS),
	DAT(0x4B),CMD_WAIT(WAIT_MS),
	DAT(0x0A),CMD_WAIT(WAIT_MS),
	DAT(0x13),CMD_WAIT(WAIT_MS),
	DAT(0x06),CMD_WAIT(WAIT_MS),
	DAT(0x30),CMD_WAIT(WAIT_MS),
	DAT(0x38),CMD_WAIT(WAIT_MS),
	DAT(0x0F),CMD_WAIT(WAIT_MS),
	CMD(0x11),CMD_WAIT(WAIT_MS*120), /* Sleep OUT,necessary to wait 120msec */
	CMD(0x29),CMD_WAIT(WAIT_MS), /* Display ON */
	CMD(0x2C),CMD_WAIT(WAIT_MS), /* Memory Write */
	CMD_STOP,
};

static UINT32 dispPwrOffCmdTbl[] = {
	CMD(0x28),CMD_WAIT(WAIT_MS*120), /* Display off */
	#if 0
	CMD(0x10),CMD_WAIT(WAIT_MS), /* Enter Sleep  */
	#endif

	CMD_STOP,
};

/*******************************************************************************
*
* Function Name: dispOpen
*
* Descriptions:
*
* Arguments:
*
* Returns:
*   SUCCESS or FAIL.
*
* See also:
*
*******************************************************************************/
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
	ret = dispDevOpen(&dispOpFunc, sizeof(dispOperatingFunc_t));

	DBG_FUNC_RETURN(ret, NULL);
}

/*******************************************************************************
*
* Function Name: dispOpDevNameGet
*
* Descriptions:
*
* Arguments:
*
* Returns:
*   return = device name string.
*
* See also:
*
*******************************************************************************/
static UINT8 *
dispOpDevNameGet(
	void
)
{
	return dispDevName;
}

/*******************************************************************************
*
* Function Name: dispOpCapabilityGet
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
static void
dispOpCapabilityGet(
	dispCapabDesc_t* pcapabDesc,
	UINT32 descSize
)
{
	*pcapabDesc = dispCapabDesc;
}

/*******************************************************************************
*
* Function Name: dispOpPowerOn
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
static UINT32
dispOpPowerOn(
	void
)
{
	/*UINT32 i;*/

	DBG_FUNC_ENTER(NULL);

	/* set backlight off. */

	dispParaTableSet(DEV_PID, dispPwrOnParaTbl);

	/* send lcm reset. */
	RST_HI;
	tmrUsWait(1000);
	RST_LO;
	tmrUsWait(10000);
	RST_HI;
	tmrUsWait(120000);

	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOnCmdTbl);

	dispParaSet(DEV_PID, DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2);

	/* set backlight on. */

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/*******************************************************************************
*
* Function Name: dispOpPowerOff
*
* Descriptions:
*
* Arguments:
*
* Returns:
*
* See also:
*
*******************************************************************************/
static UINT32
dispOpPowerOff(
	void
)
{
	DBG_FUNC_ENTER(NULL);

	/* set backlight off. */

	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOffCmdTbl);
	/*WRITE8(0xB000A142, READ8(0xB000A142) & ~0x10); *//*disable DMA*/

	tmrUsWait(30000);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

