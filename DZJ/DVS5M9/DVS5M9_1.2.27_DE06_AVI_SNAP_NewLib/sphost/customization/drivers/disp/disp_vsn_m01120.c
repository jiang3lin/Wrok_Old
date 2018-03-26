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
#include "api/sp5k_gfx_api.h"
#include "api/sp5k_global_api.h"
#include "api/sp5k_disp_api.h"

#include "api/sp5k_os_api.h"
#include "../customization/hostfw/include/app_pal.h"
#include "../customization/hostfw/include/app_osd_api_lcm.h"

/*******************************************************************************
*                              C O N S T A N T S
*******************************************************************************/

#define LCM_DATA_FMT  1  /* 0: rgb565, 1: rgb666. */

#define WAIT_MS	200
#define LCM_DELAY 0

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

#define DEV_PID  0

/* render dimension. */
#define DEV_NAME     "PANEL - VSN M01120 LCM"
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_LCM
#define WIDTH        96
#define HEIGHT       32
#define HSYNCOS      0
#define VSYNCOS      0
#define HSYNCW       2
#define VSYNCW       2
#define HPIXEL       360/*360*/
#define VLINE        262/*262*/

#define PNLH0        11/*8*/
#define PNLV0        3
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define SCL_CLK      148500
#define CLK_NX       15000	//29700ss

#define CLK_1X       CLK_NX/DATA_LEN /* LCM_DATA_FMT=1,rgb666 */
#define CLK_NX_lcm       1000	//29700ss

#define CLK_1X_lcm       CLK_NX_lcm/DATA_LEN

#define LCM_START_PAGE 0xB0
#define LCM_START_HIGH_BIT 0x11
#define LCM_START_LOW_BIT 0x02
#define LCM_START_LINE 0x40

/* extra control pin. */
#define RST  (0 << 13)   /*DGPIO 13*/
#define BL   (0 << 15)   /*DGPIO 15*/
#define CS   (1 << 14)   /*DGPIO 14*/
#define RD   (1 << 17)   /*DGPIO 17*/
#define RS   (1 << 10)   /*DGPIO 10*/
#define WR   (1 << 16)   /*DGPIO 16*/

/*******************************************************************************
*                                  M A C R O S
*******************************************************************************/
//#define	RST_LO  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RST, 0, 0, 0, 0)
//#define	RST_HI  dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RST, RST, 0, 0, 0)
//#define	BL_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, BL, 0, 0, 0, 0)
//#define	BL_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, BL, BL, 0, 0, 0)
#define	RST_LO  sp5kGpioWrite(SP5K_GPIO_GRP_FMH, 1<<(43-32), 0<<(43-32))	//FMGPIO 43
#define	RST_HI  sp5kGpioWrite(SP5K_GPIO_GRP_FMH, 1<<(43-32), 1<<(43-32))	//FMGPIO 43
#define	BL_LO   sp5kGpioWrite(SP5K_GPIO_GRP_FMH, 1<<(45-32), 0<<(45-32))	//FMGPIO 45
#define	BL_HI   sp5kGpioWrite(SP5K_GPIO_GRP_FMH, 1<<(45-32), 0<<(45-32))	//FMGPIO 45
#define	CS_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, 0, 0, 0, 0)
#define	CS_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, CS, CS, 0, 0, 0)
#define	RD_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RD, 0, 0, 0, 0)
#define	RD_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RD, RD, 0, 0, 0)
#define	RS_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RS, 0, 0, 0, 0)
#define	RS_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, RS, RS, 0, 0, 0)
#define	WR_LO   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, WR, 0, 0, 0, 0)
#define	WR_HI   dispParaSet(DEV_PID, DISP_PARA_GPIO_DATA, WR, WR, 0, 0, 0)

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */

#define dispOpen        dispM01120LcmOpen


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
static UINT8 dispFlipState = 0;

/* device capability. */
static dispCapabDesc_t dispCapabDesc = {
	.width        = WIDTH,
	.height       = HEIGHT,
	.xoff         = PNLH0,
	.yoff         = PNLV0,
	.aspectRatioX = 3,
	.aspectRatioY = 1,
	.logiEn       = 0,
	.frameRate    = 65000, //65000
};
/* parameter table. */
static dispPara_t dispPwrOnParaTbl_lcm[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0},
	{DISP_PARA_CLK_PHASE, 0, 0, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0},
	/*{DISP_PARA_EDGE_ENHANCE, 0, 8, 1, 2, 0},*/

	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

/* parameter table. */
static dispPara_t dispPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X_lcm, CLK_NX_lcm, 0, 0, 0},
	{DISP_PARA_CLK_PHASE, 0, 0, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0},
	/*{DISP_PARA_EDGE_ENHANCE, 0, 8, 1, 2, 0},*/

	/* set lcm interface. */
	{DISP_PARA_LCM_BUS_CFG, BUS_TYPE, BUS_WIDTH, CS_ACT, RS_CMD, 0},
	{DISP_PARA_LCM_DATA_FORMAT, DATA_FMT, DATA_LEN, 0, 0, 0},
	{DISP_PARA_LCM_DATA_PACKET, SWITCH_G, SWITCH_G, SWITCH_PACK, DATA_ALIGN, 0},
	{DISP_PARA_LCM_CMD_DELAY, LCM_DELAY, 0, 0, 0, 0},

	{DISP_PARA_GPIO_FUNC_CFG, RST | CS | BL | RD, RST | CS | BL | RD, 0, 0, 0},
	{DISP_PARA_GPIO_DIR_CFG, RST | CS | BL | RD, RST | CS | BL | RD, 0, 0, 0},

	{DISP_PARA_GPIO_FUNC_CFG, 0x200, 0x200, 0, 0, 0}, /* panel vsync GPIO singal sel. */
	{DISP_PARA_GPIO_DIR_CFG, 0x200, 0x000, 0, 0, 0},  /* panel vsync GPIO sel input. */
	{DISP_PARA_SIGNAL_POLARITY, 0x01, 0x01, 0, 0, 0}, /* panel vsync active high. */
	{DISP_PARA_LCM_SIGNAL_UPDATE_MODE, 0x00, 0x00, 0, 0, 0}, /* enable panel vsync mode. */


	/* set vh timing and format. */
	{DISP_PARA_DEVICE_TOTAL_RESOLUTION, HPIXEL, VLINE, 0, 0, 0},
	{DISP_PARA_DEVICE_SYNC_TIME, HSYNCOS, VSYNCOS, HSYNCW, VSYNCW, 0},
	{DISP_PARA_DEVICE_ACTIVE_REGION, PNLH0, PNLV0, PNLH1, PNLV1, 0},

	{DISP_PARA_INTERLACE_MODE, DISP_PROGRESSIVE, 0, 0, 0, 0},
	{DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2},

	{DISP_PARA_MAX_ID, 0, 0, 0, 0, 0},
};

/* lcm command table. */
static UINT32 dispPwrOnCmdTbl[] = {

	#if 1

	DISP_LCM_CMD_INST | (0xAE),     //Display Off
	DISP_LCM_CMD_INST | (0xd5),     //Divide Ratio/Oscillator Frequency Mode Set
	DISP_LCM_CMD_INST | (0x51),     //
	DISP_LCM_CMD_INST | (0xA8),     //Multiplex Ration Mode Set
	DISP_LCM_CMD_INST | (0x1F),
	DISP_LCM_CMD_INST | (0xD3),     //Display Offset Mode Set
	DISP_LCM_CMD_INST | (0x10),
	DISP_LCM_CMD_INST | (0x40),     //Set Display Start Line

	DISP_LCM_CMD_WAIT| (200),

	DISP_LCM_CMD_INST | (0xAD),    //DC-DC Control Mode Set
	DISP_LCM_CMD_INST | (0x8B),     //   DISP_LCM_CMD_INST | (0x31),     //
	DISP_LCM_CMD_INST | (0x31),     //Set Pump voltage value
	DISP_LCM_CMD_INST | (0xA1),     //Set Segment Re-map
	DISP_LCM_CMD_INST | (0xC8),     //Set Common Output Scan Direction:
	DISP_LCM_CMD_INST | (0xDA),    //Common Pads Hardware Configuration Mode Set
	DISP_LCM_CMD_INST | (0x12),
	DISP_LCM_CMD_INST | (0x81),     //The Contrast Control Mode Set

	DISP_LCM_CMD_WAIT| (200),

	DISP_LCM_CMD_INST | (0x58),  //
	DISP_LCM_CMD_INST | (0xD9),     //Pre-charge Period Mode Set:
	DISP_LCM_CMD_INST | (0x22),
	DISP_LCM_CMD_INST | (0xDB),     //VCOM Deselect Level Mode Set
	DISP_LCM_CMD_INST | (0x25),     //
	DISP_LCM_CMD_INST | (0xA4),     //Set Entire Display OFF/ON
	DISP_LCM_CMD_INST | (0xA6),     //Set Normal/Reverse Display
	DISP_LCM_CMD_INST | (0xAF),     //Display On

	DISP_LCM_CMD_WAIT| (200),

	DISP_LCM_CMD_STOP,

	#else

	  DISP_LCM_CMD_WAIT |(50),


    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x01),DISP_LCM_CMD_PARA |(0x01),DISP_LCM_CMD_PARA |(0x1C),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x02),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x01),


    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x03),DISP_LCM_CMD_PARA |(0x12),DISP_LCM_CMD_PARA |(0x28),

    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x08),DISP_LCM_CMD_PARA |(0x03),DISP_LCM_CMD_PARA |(0x0D),

    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x0b),DISP_LCM_CMD_PARA |(0x85),DISP_LCM_CMD_PARA |(0x07),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x0C),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x0F),DISP_LCM_CMD_PARA |(0x0c),DISP_LCM_CMD_PARA |(0x01),


    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x20),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0xAF),
    DISP_LCM_CMD_WAIT |(10),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x21),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_WAIT |(50),

    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x10),DISP_LCM_CMD_PARA |(0x0A),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_WAIT |(10),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x11),DISP_LCM_CMD_PARA |(0x10),DISP_LCM_CMD_PARA |(0x38),
    DISP_LCM_CMD_WAIT |(10),
    DISP_LCM_CMD_WAIT |(50),

    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x12),DISP_LCM_CMD_PARA |(0x11),DISP_LCM_CMD_PARA |(0x21),
    DISP_LCM_CMD_WAIT |(20),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x13),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x7A),
    DISP_LCM_CMD_WAIT |(30),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x14),DISP_LCM_CMD_PARA |(0x4f),DISP_LCM_CMD_PARA |(0x45),


    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x30),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x31),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0xDB),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x32),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x33),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x34),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0xDB),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x35),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x36),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0xaf),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x37),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x38),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0xdb),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x39),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),


    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x50),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x51),DISP_LCM_CMD_PARA |(0x07),DISP_LCM_CMD_PARA |(0x04),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x52),DISP_LCM_CMD_PARA |(0x0C),DISP_LCM_CMD_PARA |(0x08),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x53),DISP_LCM_CMD_PARA |(0x05),DISP_LCM_CMD_PARA |(0x02),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x54),DISP_LCM_CMD_PARA |(0x08),DISP_LCM_CMD_PARA |(0x0C),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x55),DISP_LCM_CMD_PARA |(0x04),DISP_LCM_CMD_PARA |(0x07),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x56),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x57),DISP_LCM_CMD_PARA |(0x02),DISP_LCM_CMD_PARA |(0x05),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x58),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x59),DISP_LCM_CMD_PARA |(0x00),DISP_LCM_CMD_PARA |(0x00),
    DISP_LCM_CMD_WAIT |(50),

    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x07),DISP_LCM_CMD_PARA |(0x10),DISP_LCM_CMD_PARA |(0x17),
    DISP_LCM_CMD_WAIT |(30),

    DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x22),

    DISP_LCM_CMD_STOP,

	#endif
};

static UINT32 dispPwrOffCmdTbl[] = {
	/* Display off */
	DISP_LCM_CMD_INST |(0xAE),
	DISP_LCM_CMD_WAIT |(30),
	DISP_LCM_CMD_STOP,
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

void dispWriteCmd(UINT8 cmd)
{
	RS_LO;

	CS_LO;

	WR_LO;
	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, 0xff, cmd);
	tmrUsWait(1);
	WR_HI;
	tmrUsWait(2);	//100
	WR_LO;
	tmrUsWait(1);
	CS_HI;
	tmrUsWait(1);
}

void dispWriteData(UINT8 data)
{
	RS_HI;
	CS_LO;
	WR_LO;
	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, 0xff, data);
	tmrUsWait(1);
	WR_HI;
	tmrUsWait(2);	//100
	WR_LO;
	tmrUsWait(1);
	CS_HI;
	tmrUsWait(1);
}

void dispLcmInit(void)
{
	UINT32 lcmCmdTot, i, cmd;
        //set osd layer format RGB565
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);

	//close unused layer
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_USR_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_OSD_0, 1, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PIP_0, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_VCT_0, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0,0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_IMG_ACTIVE, 0, 0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_PIP_ACTIVE, 0, 0, 0, 0);
	sp5kDispDevAttrSet(SP5K_DISP_OSD_ACTIVE, 1, 0, 0, 0);

        //disable panel HW interface
	dispParaSet(DEV_PID, DISP_PARA_GLOB_ENABLE, 1, 0, 0, 0, 2);

	//set DGPIO[7:0]/RS/WR AS GPIO PIN
	/*sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, 0xff, 0xff);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, RS, RS);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, WR, WR);*/

	//send LCM commond table
	/*lcmCmdTot = sizeof(dispPwrOnCmdTbl)/sizeof(UINT32);
	printf("cmdTot=%d\n", lcmCmdTot);
	for(i=0; i<lcmCmdTot-1; i++)
	{
		cmd = dispPwrOnCmdTbl[i]&0x000000FF;
		printf("cmd = %x, %x, %x\n", dispPwrOnCmdTbl[i], cmd, (UINT8)cmd);
		dispWriteCmd((UINT8)cmd);
	}*/

}

void dispRefresh(void)
{
	UINT32 i = 0;
	UINT32 m = 0;
	UINT32 j = 0;
	UINT32 val = 0;
	UINT32 cnt = 0;
	#ifdef LCM_USE_OSD_FORMAT_IDX8
	UINT8  *pDat = NULL, *pDatFlip = NULL;
	#elif defined LCM_USE_OSD_FORMAT_RGB565
	UINT16 *pDat = NULL, *pDatFlip = NULL;
	#endif

	//Get osd layer buffer
	//return;
	sp5kGfxPageCapabDesc_t osdFrm;
	#ifdef LCM_USE_OSD_FORMAT_IDX8
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,&osdFrm);
	#elif defined LCM_USE_OSD_FORMAT_RGB565
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_USR_0,&osdFrm);
	#endif

	if(osdFrm.pbuf == NULL){
		printf("Get osd Capability false\n");
		return;
	}

	//transform osd layer buffer to W/B Panel show buffer
	#ifdef LCM_USE_OSD_FORMAT_IDX8
	pDat = (UINT8 *)osdFrm.pbuf;
	#elif defined LCM_USE_OSD_FORMAT_RGB565
	pDat = (UINT16 *)osdFrm.pbuf;
	#endif	

	//flip osd
	if(dispFlipState == 1)
	{
		pDatFlip = sp5kMalloc(osdFrm.frmW * osdFrm.frmH * sizeof(pDat[0]));
		for(j = 0; j < osdFrm.frmH; j ++)
		{
			for(i = 0; i < osdFrm.frmW; i ++)
			{
				pDatFlip[j * osdFrm.frmW + i] = pDat[(osdFrm.frmH - 1 - j) * osdFrm.frmW + (osdFrm.frmW - 1 - i)];
			}
		}
		pDat = pDatFlip;
	}
	
	dispWriteCmd(LCM_START_LINE);
	for(j = 0; j < HEIGHT / 8; j ++)
	{
		dispWriteCmd(LCM_START_PAGE + j);
		dispWriteCmd(LCM_START_HIGH_BIT);
		dispWriteCmd(LCM_START_LOW_BIT);

		for(i = 0; i < WIDTH; i ++)
		{
			val = 0;
			cnt = j * osdFrm.frmW * 8 + i;
			for(m = 0; m < 8; m ++)
			{
				val >>= 1;
				//if not 0 ,show black
				//printf("%04x ", pDat[cnt]);
				#ifdef LCM_USE_OSD_FORMAT_IDX8
				if(pDat[cnt] == PAL_WHITE)
				#elif defined LCM_USE_OSD_FORMAT_RGB565
				if(pDat[cnt] == 0xffdf || pDat[cnt] == PAL_NOCOLOR)
				#endif				
				{
					val |= 0x80;
				}
				cnt += osdFrm.frmW;
			}
			val = ~val;
			//printf("\n");
			dispWriteData(val);
		}
	}

	if(dispFlipState == 1)
	{
		sp5kFree(pDatFlip);
	}
}

void dispClean(void)
{
	UINT32 i = 0;
	UINT32 j = 0;
	
	dispWriteCmd(LCM_START_LINE);
	for(j = 0; j < HEIGHT / 8; j ++)
	{
		dispWriteCmd(LCM_START_PAGE + j);
		dispWriteCmd(LCM_START_HIGH_BIT);
		dispWriteCmd(LCM_START_LOW_BIT);

		for(i = 0; i < WIDTH; i ++)
		{
			dispWriteData(0x00);
		}
	}
}

void dispFlip(BOOL bFlip)
{
	if(bFlip == TRUE)
	{
		//dispWriteCmd(0xC8);	//Set Common Output Scan Direction:
		dispFlipState = 1;
	}
	else
	{
		//dispWriteCmd(0xC8);	//Set Common Output Scan Direction:
		dispFlipState = 0;
	}
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
	/*UINT32 lcmCmdTot, i, cmd;*/

	static UINT8 bFisrt = TRUE;	
	return SUCCESS;
	if(bFisrt == FALSE)
	{
		printf("VSN dispOpPowerOn return\n");
		return SUCCESS;
	}
	bFisrt = FALSE;
		
	DBG_FUNC_ENTER(NULL);

	BL_HI;
	tmrUsWait(1000);
	RD_HI;

	dispParaTableSet(DEV_PID, dispPwrOnParaTbl);

	RD_HI;
	/* send lcm reset. */
	RST_HI;
	tmrUsWait(1000);
	RST_LO;
	tmrUsWait(1000);
	RST_HI;
	tmrUsWait(1000);

	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOnCmdTbl);
	dispParaTableSet(DEV_PID, dispPwrOnParaTbl_lcm);

	//set DGPIO[7:0]/RS/WR AS GPIO PIN
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, 0xff, 0xff);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, RS, RS);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, WR, WR);/**/

	#if 0
	//send LCM commond table
	lcmCmdTot = sizeof(dispPwrOnCmdTbl)/sizeof(UINT32);
	printf("cmdTot=%d\n", lcmCmdTot);
	for(i=0; i<lcmCmdTot-1; i++)
	{
		cmd = dispPwrOnCmdTbl[i]&0x000000FF;
		printf("cmd = %x, %x, %x\n", dispPwrOnCmdTbl[i], cmd, (UINT8)cmd);
		dispWriteCmd((UINT8)cmd);
	}/**/
	#endif

	dispLcmInit();
	dispClean();

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

void M01120BlackLightPowerOff()
{
	BL_LO;
	RST_LO;
	printf(" LCM power off\n");
}
 UINT32
dispOpSecondPowerOn(
	void
)
{
	/*UINT32 lcmCmdTot, i, cmd;*/

	static UINT8 bFisrt = TRUE;
	//printf("--dispOpSecondPowerOn\n");
	//return;
	if(bFisrt == FALSE)
	{
		printf("VSN dispOpSecondPowerOn return\n");
		return SUCCESS;
	}
	bFisrt = FALSE;
	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
	DBG_FUNC_ENTER(NULL);

	BL_HI;
	tmrUsWait(1000);
	RD_HI;

	dispParaTableSet(DEV_PID, dispPwrOnParaTbl);

	RD_HI;
	/* send lcm reset. */
	RST_HI;
	tmrUsWait(5000);
	RST_LO;
	tmrUsWait(10000);
	RST_HI;
	tmrUsWait(5000);

	/* send lcm command. */
	dispLcmCmdTableSend(dispPwrOnCmdTbl);
	dispParaTableSet(DEV_PID, dispPwrOnParaTbl_lcm);

	//set DGPIO[7:0]/RS/WR AS GPIO PIN
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, 0xff, 0xff);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, RS, RS);
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, WR, WR);/**/

	#if 0
	//send LCM commond table
	lcmCmdTot = sizeof(dispPwrOnCmdTbl)/sizeof(UINT32);
	printf("cmdTot=%d\n", lcmCmdTot);
	for(i=0; i<lcmCmdTot-1; i++)
	{
		cmd = dispPwrOnCmdTbl[i]&0x000000FF;
		printf("cmd = %x, %x, %x\n", dispPwrOnCmdTbl[i], cmd, (UINT8)cmd);
		dispWriteCmd((UINT8)cmd);
	}/**/
	#endif

	dispLcmInit();
	dispClean();

	DBG_FUNC_RETURN(SUCCESS, NULL);

	sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 100);
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
	//dispWriteCmd(0xAE);

	tmrUsWait(30000);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

UINT8 tab_1[]={
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X30,0XC0,0X80,0X80,0X80,0X80,0X80,0XC0,0X30,0X00,
0X80,0X80,0X80,0X80,0X00,0XE0,0X10,0X10,0X10,0X10,0XE0,0X00,0X00,0X40,0X20,0X10,
0X10,0X10,0X10,0X20,0XC0,0X00,0X00,0X00,0X00,0X00,0XF0,0X00,0X00,0X00,0X00,0X00,
0X80,0X80,0X80,0X80,0X00,0XE0,0X10,0X10,0X10,0X10,0XE0,0X00,0X00,0X10,0X10,0X90,
0X50,0X30,0X10,0X00,0X00,0X00,0X00,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF0,
0X00,0X00,0X00,0X00,0X00,0XE0,0X10,0X10,0X10,0X10,0XE0,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X03,0X1C,0X20,0X1C,0X03,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X1D,0X22,0X22,0X22,0X22,0X1D,0X00,0X00,0X08,0X10,0X20,
0X20,0X20,0X20,0X10,0X0F,0X00,0X00,0X20,0X20,0X20,0X3F,0X20,0X20,0X20,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X1F,0X20,0X20,0X20,0X20,0X1F,0X00,0X00,0X1E,0X21,0X20,
0X20,0X20,0X18,0X00,0X00,0X00,0X00,0X3F,0X10,0X08,0X00,0X00,0X00,0X00,0X00,0X3F,
0X10,0X08,0X00,0X00,0X00,0X1F,0X20,0X20,0X20,0X20,0X1F,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

//
//
//

UINT8 tab_2[]={
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XF0,0XF0,0XF0,
0X70,0X80,0XE0,0XF0,0X70,0X30,0X90,0XC0,0XE0,0XE0,0X70,0XF0,0XF0,0XE0,0XC0,0X00,
0X00,0X80,0XE0,0XF0,0XF0,0X30,0X00,0X80,0XE0,0XF0,0XF0,0X30,0X00,0X80,0XC0,0XC0,
0XE0,0X60,0XF0,0XF0,0XF0,0XE0,0X00,0X00,0X80,0XE0,0XF0,0XF0,0X30,0X00,0XC0,0XC0,
0XE0,0X60,0X70,0X70,0XF0,0XF0,0XE0,0X00,0X00,0X80,0XE0,0XF0,0X70,0X30,0X00,0X80,
0XE0,0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0XC0,0XE0,0X70,0X38,0X1F,0XFF,0XFF,
0XFF,0X03,0X01,0X78,0XFE,0XFF,0XFF,0XE3,0X60,0X70,0X3C,0X1F,0X0F,0X03,0XF0,0XFC,
0XFF,0XFF,0X77,0XA1,0XF0,0XF8,0XFE,0X3F,0X0F,0X03,0X70,0XFC,0XFF,0XFF,0XE3,0XE1,
0X70,0X3C,0X1F,0X0F,0X87,0XE1,0XF8,0X7E,0X1F,0X0F,0X43,0XE0,0XE0,0XE7,0XCF,0XCF,
0XEF,0X7E,0X7E,0X3C,0X18,0XC0,0XE0,0XF8,0X7E,0X1F,0X07,0X81,0XE0,0XF8,0XFE,0XFF,
0X7F,0X1F,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X10,0X1C,0X1C,0X1C,0X04,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X10,0X1C,0X1C,0X0C,0X00,0XC0,0XE0,0XF8,0XFE,0XFF,0XFF,0X3F,0X0F,0X03,
0X00,0XF8,0XFF,0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X10,0X1C,0X1F,0X1F,0X1F,0X1F,0X0F,0X03,0X00,0X00,0X00,0X00,
0X00,0X1F,0X1F,0X1F,0X1F,0X1F,0X1F,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};

UINT8 tab_3[]={
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,

0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,

0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,

0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,0X55,
};

void dispTest(UINT8 Mode)
{
	UINT32 i = 0, j = 0;
	UINT32 m = 0;
	UINT32 val = 0;
	UINT32 cnt = 0;
	#ifdef LCM_USE_OSD_FORMAT_IDX8
	UINT8  *pDat = NULL;
	#elif defined LCM_USE_OSD_FORMAT_RGB565
	UINT16  *pDat = NULL;
	#endif
	
	//Get osd layer buffer
	sp5kGfxPageCapabDesc_t osdFrm;
	#ifdef LCM_USE_OSD_FORMAT_IDX8
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_OSD_0,&osdFrm);
	#elif defined LCM_USE_OSD_FORMAT_RGB565
	sp5kGfxPageCapabilityGet(SP5K_GFX_PAGE_USR_0,&osdFrm);
	#endif

	if(osdFrm.pbuf == NULL){
		printf("Get osd Capability false\n");
		return;
	}

	//transform osd layer buffer to W/B Panel show buffer
	#ifdef LCM_USE_OSD_FORMAT_IDX8
	pDat = (UINT8 *)osdFrm.pbuf;
	#elif defined LCM_USE_OSD_FORMAT_RGB565
	pDat = (UINT16 *)osdFrm.pbuf;
	#endif	
	for(j = 0; j < HEIGHT / 8; j ++)
	{
		dispWriteCmd(LCM_START_PAGE + j);
		dispWriteCmd(LCM_START_HIGH_BIT);
		dispWriteCmd(LCM_START_LOW_BIT);

		for(i = 0; i < WIDTH; i ++)
		{
			val = 0;
			cnt = j * osdFrm.frmW * 8 + i;
			for(m = 0; m < 8; m ++)
			{
				val >>= 1;
				//if not 0 ,show black
				printf("%04x", pDat[cnt]);
				#ifdef LCM_USE_OSD_FORMAT_IDX8
				if(pDat[cnt] == PAL_WHITE)
				#elif defined LCM_USE_OSD_FORMAT_RGB565
				if(pDat[cnt] == 0xffdf || pDat[cnt] == PAL_NOCOLOR)
				#endif				
				{
					val |= 0x80;
				}
				cnt += osdFrm.frmW;
			}
			val = ~val;
			printf("\n");

			if(Mode == 0)
			{
				dispWriteData(val);
			}
			else if(Mode == 1)
			{
				dispWriteData(0x55);
			}
			else if(Mode == 2)
			{
				dispWriteData(tab_1[j * 96 + i]);
			}
			else if(Mode == 3)
			{
				dispWriteData(tab_2[j * 96 + i]);
			}
			else if(Mode == 4)
			{
				dispWriteData(tab_3[j * 96 + i]);
			}
			else if(Mode == 5)
			{
				dispWriteData(0x00);
			}
		}
		printf("\n");
	}
}

