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
*  Author: Matt Wang
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
#define DEV_NAME     "PANEL - TP TD025THEEA UPS052"
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
#define PNLH0        59
#define PNLV0        21
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1 + 1)  /* don't remove "+1", otherwise green horizontal line on top side. */
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define CLK_NX       24545
#define CLK_1X       6136
#define CLK_PHASE    8
#define SCL_CLK      90000
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
#define dispOpen        dispTpTd025theeaUps052Open
#define dispOpPowerOn   dispTpTd025theeaUps052OpPowerOn
#define dispOpPowerOff  dispTpTd025theeaUps052OpPowerOff
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
	{DISP_PARA_UPS_HSD_MODE, DISP_UPS_HSD_NO_DELAY, 0, 0, 0, 0},
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
static UINT32 dispCmdTbl[] = {
	#if DISP_UPS052 == 1
	(0x02<<10)+0x00,
	(0x03<<10)+0x01,
	(0x04<<10)+0x0F,
	(0x05<<10)+0x2B,
	(0x07<<10)+0x08,
	(0x0c<<10)+0x00,
	#else  /* reference code. */
	(0x02<<10)+0x02,  /* through mode (rgb only mode), 1280. */
	(0x03<<10)+0x01,  /* non-interlace. */
	(0x04<<10)+0x0F,  /* power enable. */
	(0x05<<10)+0x2B,  /* direction normal, delta, phase sh7. */
	(0x06<<10)+0x18,  /* horizontal center. */
	(0x07<<10)+0x08,  /* vertical center, true color. */
	#endif

	0xFFFFFFFF,
};

static UINT32 dispCmdTblOff[] = {
	(0x04<<10)+0x0A,
	(0x05<<10)+0x5E,

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

