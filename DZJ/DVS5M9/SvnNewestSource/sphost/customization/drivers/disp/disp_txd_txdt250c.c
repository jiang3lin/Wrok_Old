/*******************************************************************************
*
*       Copyright (c) 2006-2012 by iCatch Technology, Inc.
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
*  Author: cs.lin
*
*******************************************************************************/

/*******************************************************************************
*                            H E A D E R   F I L E S
*******************************************************************************/
#include "common.h"
#include "api/sp5k_global_api.h"
#include "debug_mod/dbg_api.h"
#include "middleware/disp_def.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/
/*
  compile option in Makefile:
    DISP_UPS052=1 : use ups052 timing.
*/

/* render dimension. */
#define DEV_PID  0

#if DISP_UPS052 == 1
#define DEV_NAME     "PANEL - TXD TXDT250C UPS052"
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_UPS052
#define WIDTH        320
#define HEIGHT       240
#if SPCA5330
#define HSYNCOS      0
#define VSYNCOS      0
#endif
#define HSYNCW       1
#define VSYNCW       1
#define HPIXEL       390
#define VLINE        264
#define PNLH0        58
#define PNLV0        21
#define PNLH1        (PNLH0 + WIDTH - 1 + 1)
#define PNLV1        (PNLV0 + HEIGHT - 1 + 1)  /* don't remove "+1", otherwise green horizontal line on top side. */
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define SCL_CLK      99000
#define CLK_NX       24545
#define CLK_1X       6136
#define CLK_PHASE    8
#define FILTER_EVEN  DISP_UPS052_EVEN_BRG
#define FILTER_ODD   DISP_UPS052_ODD_BRG
#endif

/* 3-wire interface. */
#ifdef FPGA
#define SCL  (1 << 12)
#define SDA  (1 << 13)
#define CS   (1 << 14)
#else
#define SCL  (1 << 14)
#define SDA  (1 << 15)
#define CS   (1 << 13)
#endif

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/
#define	SCL_LO  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SCL, 0, 0, 0, 0)
#define	SCL_HI  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SCL, SCL, 0, 0, 0)
#define	SDA_LO  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SDA, 0, 0, 0, 0)
#define	SDA_HI  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, SDA, SDA, 0, 0, 0)
#define	CS_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, 0, 0, 0, 0)
#define	CS_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, CS, 0, 0, 0)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */
#if DISP_UPS052 == 1
#define dispOpen        dispTxdTxdt025cUps052Open
#define dispOpPowerOn   dispTxdTxdt025cUps052OpPowerOn
#define dispOpPowerOff  dispTxdTxdt025cUps052OpPowerOff
#endif

/* operating function. */
static UINT8 *dispOpDevNameGet(void);
static void   dispOpCapabilityGet(dispCapabDesc_t *pcapabDesc, UINT32 descSize);
static UINT32 dispOpPowerOn(void);
static UINT32 dispOpPowerOff(void);

/* serial command. */
static void dispSpiCmdTableSend(UINT32 *cmdTbl);

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
	.aspectRatioX = 4,
	.aspectRatioY = 3,
	.logiEn       = 0,
	.frameRate    = 60000,
};

static dispOperatingFunc_t dispOpFunc = {
	.pfOpDevNameGet    = dispOpDevNameGet,
	.pfOpPowerOn       = dispOpPowerOn,
	.pfOpPowerOff      = dispOpPowerOff,
	.pfOpCapabilityGet = dispOpCapabilityGet
};

/* parameter table. */
static dispPara_t dispParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0},
	{DISP_PARA_CLK_PHASE, CLK_PHASE, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0},

	/* set vh timing and format. */
	#if SPCA5330
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL, VLINE, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS, VSYNCOS, HSYNCW, VSYNCW, 0},
	#else
	{DISP_PARA_DEVICE_TOTAL_REGION, HPIXEL, VLINE, HSYNCW, VSYNCW, 0},
	#endif
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0},
	#if DISP_UPS052 == 1
	{DISP_PARA_UPS_LAYOUT_MODE, DISP_UPS_LAYOUT_DISABLE, 0, 0, 0, 0},
	{DISP_PARA_UPS052_RGB_ONLY_ENABLE, 0, 0, 0, 0, 0},
	{DISP_PARA_UPS_HSD_MODE, 0, 0, 0, 0, 0},
	#if SPCA5330
	{DISP_PARA_UPS_COLOR_FILTER, FILTER_EVEN, FILTER_ODD, 0, 0, 0},
	#else
	{DISP_PARA_UPS_COLOR_FILTER, 0, 0, 0, 0, 0},
	#endif
	{DISP_PARA_INTERLACE_MODE, DISP_PROGRESSIVE, 0, 0, 0, 0},
	#endif
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	/* send 3-wire command. */
	{DISP_PARA_GPIO_FUNC_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0},
	{DISP_PARA_GPIO_DIR_CFG, SCL | SDA | CS, SCL | SDA | CS, 0, 0, 0},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

/* serial command table. */
static UINT32 dispInit1Tbl[] = {
	(0x05<<8)+0x5e,
	(0x05<<8)+0x1e, /* reset */
	0xFFFFFFFF,
};

static UINT32 dispInit2Tbl[] = {
	(0x05<<8)+0x5e,
	(0x2b<<8)+0x01, /* sleep out */
	0xFFFFFFFF,
};

static UINT32 dispCmdTbl[] = {
	#if DISP_UPS052 == 1
	(0x2f<<8)+0x44,
	(0x9e<<8)+0x80,
	(0x00<<8)+0x07,
	(0x01<<8)+0xbb,
	(0xa1<<8)+0x01,
	(0x0d<<8)+0x80, /* only for this panel */
	(0x03<<8)+0x20, /* only for this panel */

	(0x04<<8)+0x1f,
/*-------------VCOMH ----------*/
	(0x80<<8)+0x01, /* V63 */
	(0x81<<8)+0x16, /* V60 */
	(0x82<<8)+0x1A, /* V57 */
	(0x83<<8)+0x0C, /* V55 */
	(0x84<<8)+0x0C, /* V52 */
	(0x85<<8)+0x07, /* V48 */
	(0x86<<8)+0x09, /* V43 */
	(0x87<<8)+0x01, /* V38 */
	(0x88<<8)+0x0A, /* V25 */
	(0x89<<8)+0x0F, /* V20 */
	(0x8A<<8)+0x15, /* V15 */
	(0x8B<<8)+0x10, /* V11 */
	(0x8C<<8)+0x0F, /* V8 */
	(0x8D<<8)+0x35, /* V6 */
	(0x8E<<8)+0x27, /* V3 */
	(0x8F<<8)+0x18, /* V0 */
/*-------------VCOML---------- */
	(0x30<<8)+0x06, /* V0 */
	(0x31<<8)+0x14, /* V3 */
	(0x32<<8)+0x22, /* V6 */
	(0x33<<8)+0x10, /* V8 */
	(0x34<<8)+0x10, /* V11 */
	(0x35<<8)+0x17, /* V15 */
	(0x36<<8)+0x0E, /* V20 */
	(0x37<<8)+0x09, /* V25 */
	(0x38<<8)+0x01, /* V38 */
	(0x39<<8)+0x09, /* V43 */
	(0x3A<<8)+0x05, /* V48 */
	(0x3B<<8)+0x0C, /* V52 */
	(0x3C<<8)+0x0E, /* V55 */
	(0x3D<<8)+0x2D, /* V57 */
	(0x3E<<8)+0x29, /* V60 */
	(0x3F<<8)+0x14, /* V63 */

	#else  /* reference code. */

	#endif

	0xFFFFFFFF,
};

static UINT32 dispCmdTblOff[] = {
	0xFFFFFFFF,
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
	DBG_FUNC_ENTER(NULL);

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
	DBG_FUNC_ENTER(NULL);
	dispParaTableSet(DEV_PID, dispParaTbl);
	#ifdef HW_PRJ_6330
	*(volatile UINT8*)(0xb000403D) = 0x28;
	*(volatile UINT8*)(0xb0004035) = 0x8;
	sp5kTimeDelay(3, 1);
	*(volatile UINT8*)(0xb0004035) = 0x0;
	sp5kTimeDelay(3, 1);
	*(volatile UINT8*)(0xb0004035) = 0x8;
	#endif

	dispSpiCmdTableSend(dispInit1Tbl);
	sp5kTimeDelay(3, 10);
	dispSpiCmdTableSend(dispInit2Tbl);
	sp5kTimeDelay(3, 10);
	dispSpiCmdTableSend(dispCmdTbl);


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

	dispSpiCmdTableSend(dispCmdTblOff);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

/*******************************************************************************
*
* Function Name: dispSpiCmdTableSend
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
dispSpiCmdTableSend(
	UINT32 *cmdTbl
)
{
	UINT32	i, j, cmd;


	DBG_FUNC_ENTER("0x%08x", cmdTbl);
	CS_HI;
	SCL_LO;
	sp5kTimeDelay(3, 1);
	while ( 1 ) {
		cmd = *cmdTbl;
		DBG_LINE_PRINT("cmd=0x%08x", cmd);
		if ( cmd == 0xFFFFFFFF ) {
			break;
		}

		CS_LO;
		/*SCL_HI;*/
		for ( i = 16; i > 0; i-- ) {
			SCL_LO;
			if ( cmd & 0x8000 ) {
				SDA_HI;
			}
			else {
				SDA_LO;
			}
			for ( j = 3; j > 0; j-- );
			SCL_HI;

			cmd <<= 1;
		}
		SCL_LO;
		SDA_LO;
		CS_HI;
		for ( j = 3; j > 0; j-- );
		cmdTbl++;
	}

	DBG_FUNC_RETURN_VOID(NULL);
}

