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
#define DEV_NAME     "PANEL - DJN2.0_ILI9225G LCM"
#define DEV_TYPE     DISP_PANEL_DEVICE_TYPE_LCM
#define WIDTH        220
#define HEIGHT       176
#define HSYNCOS      1
#define VSYNCOS      1
#define HSYNCW       2
#define VSYNCW       2
#define HPIXEL       (220+20)
#define VLINE        (176+20)/*4095*/

#define PNLH0        12
#define PNLV0        5
#define PNLH1        (PNLH0 + WIDTH - 1)
#define PNLV1        (PNLV0 + HEIGHT - 1)
#define CLK_SRC      DISP_CLK_SRC_DRAMPLL
#define SCL_CLK      148500
#define CLK_NX       29700

#define CLK_1X       CLK_NX/DATA_LEN /* LCM_DATA_FMT=1,rgb666 */




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

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
*******************************************************************************/
/* device open. */

#define dispOpen        dispILI9225LcmOpen 


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
	.aspectRatioX = 4,
	.aspectRatioY = 3,
	.logiEn       = 0,
	.frameRate    = 65000,
};

/* parameter table. */
static dispPara_t dispPwrOnParaTbl[] = {
	/* set clk and type. */
	{DISP_PARA_CLK_SRC, CLK_SRC, 0, 0, 0, 0},
	{DISP_PARA_CLK_FREQ, CLK_1X, CLK_NX, 0, 0, 0},
	{DISP_PARA_CLK_PHASE, 0, 0, 0, 0, 0},
	{DISP_PARA_SCALE_CLK_FREQ, SCL_CLK, 0, 0, 0, 0},
	{DISP_PARA_DEVICE_TYPE, DEV_TYPE, 0, 0, 0, 0},
	/*{DISP_PARA_EDGE_ENHANCE, 0, 8, 1, 2, 0},*/

	/* set lcm interface. */
	{DISP_PARA_LCM_BUS_CFG, BUS_TYPE, BUS_WIDTH, CS_ACT, RS_CMD, 0},
	{DISP_PARA_LCM_DATA_FORMAT, DATA_FMT, DATA_LEN, 0, 0, 0},
	{DISP_PARA_LCM_DATA_PACKET, SWITCH_G, SWITCH_G, SWITCH_PACK, DATA_ALIGN, 0}, 
	{DISP_PARA_LCM_CMD_DELAY, LCM_DELAY, 0, 0, 0, 0},

	{DISP_PARA_GPIO_FUNC_CFG, RST | CS | BL , RST | CS | BL, 0, 0, 0},
	{DISP_PARA_GPIO_DIR_CFG, RST | CS | BL , RST | CS | BL, 0, 0, 0},

	{DISP_PARA_GPIO_FUNC_CFG, 0x200, 0x200, 0, 0, 0}, /* panel vsync GPIO singal sel. */
	{DISP_PARA_GPIO_DIR_CFG, 0x200, 0x000, 0, 0, 0},  /* panel vsync GPIO sel input. */
	{DISP_PARA_SIGNAL_POLARITY, 0x01, 0x01, 0, 0, 0}, /* panel vsync active high. */
	{DISP_PARA_LCM_SIGNAL_UPDATE_MODE, 0x01, 0x01, 0, 0, 0}, /* enable panel vsync mode. */


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
};

static UINT32 dispPwrOffCmdTbl[] = {
	/* Display off */
	DISP_LCM_CMD_INST |(0x00),DISP_LCM_CMD_INST |(0x28),/* STB */
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

	tmrUsWait(30000);

	DBG_FUNC_RETURN(SUCCESS, NULL);
}

